#include <cstddef>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

// Assignment 06: Passwords and Hash Tables
// C++17.  This is an educational data-structures implementation.
// The password hash is intentionally the demo hash required by the assignment
// and is NOT suitable for production password security.

// -----------------------------------------------------------------------------
// Required educational hash function
// -----------------------------------------------------------------------------

uint64_t demoHash(const std::string& s) {
    uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : s) {
        h = (h ^ c) * 1099511628211ULL;
    }
    return h;
}

// -----------------------------------------------------------------------------
// Supporting types and prime-capacity helpers
// -----------------------------------------------------------------------------

struct UserRecord {
    std::string salt;
    uint64_t password_hash = 0;
    std::size_t password_rehashes = 0;
};

enum class CollisionStrategy {
    SeparateChaining,
    LinearProbing,
    QuadraticProbing,
    DoubleHashing
};

const char* strategyName(CollisionStrategy strategy) {
    switch (strategy) {
    case CollisionStrategy::SeparateChaining:
        return "Separate chaining";
    case CollisionStrategy::LinearProbing:
        return "Linear probing";
    case CollisionStrategy::QuadraticProbing:
        return "Quadratic probing";
    case CollisionStrategy::DoubleHashing:
        return "Double hashing";
    }
    return "Unknown strategy";
}

bool isPrime(std::size_t value) {
    if (value < 2) {
        return false;
    }
    if (value == 2) {
        return true;
    }
    if (value % 2 == 0) {
        return false;
    }

    for (std::size_t divisor = 3; divisor * divisor <= value; divisor += 2) {
        if (value % divisor == 0) {
            return false;
        }
    }
    return true;
}

std::size_t nextPrime(std::size_t value) {
    if (value < 5) {
        value = 5;
    }
    while (!isPrime(value)) {
        ++value;
    }
    return value;
}

// -----------------------------------------------------------------------------
// Hash table with four collision mitigation strategies
// -----------------------------------------------------------------------------

class HashTable {
private:
    enum class SlotState {
        Empty,
        Occupied,
        Deleted
    };

    struct Slot {
        std::string key;
        UserRecord value;
        SlotState state = SlotState::Empty;
    };

    struct ChainNode {
        std::string key;
        UserRecord value;
        ChainNode* next;

        ChainNode(
            const std::string& node_key,
            const UserRecord& node_value,
            ChainNode* next_node)
            : key(node_key), value(node_value), next(next_node) {}
    };

    CollisionStrategy strategy_;
    std::size_t capacity_;
    std::size_t size_;
    std::size_t tombstones_;
    std::size_t collision_count_;
    std::size_t rehash_count_;
    Slot* slots_;
    ChainNode** buckets_;

    double maximumLoadFactor() const {
        switch (strategy_) {
        case CollisionStrategy::SeparateChaining:
            return 0.75;
        case CollisionStrategy::LinearProbing:
        case CollisionStrategy::DoubleHashing:
            return 0.70;
        case CollisionStrategy::QuadraticProbing:
            // A conservative threshold helps quadratic probing find a slot
            // without needing to visit every position in the table.
            return 0.45;
        }
        return 0.70;
    }

    std::size_t primaryIndex(const std::string& key) const {
        return static_cast<std::size_t>(demoHash(key) % capacity_);
    }

    std::size_t secondaryStep(const std::string& key) const {
        // capacity_ is prime and at least 5, so this produces a nonzero step
        // that is relatively prime to the capacity.
        return 1 + static_cast<std::size_t>(
            demoHash("secondary:" + key) % (capacity_ - 1));
    }

    std::size_t probeIndex(const std::string& key, std::size_t attempt) const {
        const std::size_t base = primaryIndex(key);

        switch (strategy_) {
        case CollisionStrategy::LinearProbing:
            return (base + attempt) % capacity_;
        case CollisionStrategy::QuadraticProbing:
            return (base + attempt * attempt) % capacity_;
        case CollisionStrategy::DoubleHashing:
            return (base + attempt * secondaryStep(key)) % capacity_;
        case CollisionStrategy::SeparateChaining:
            break;
        }

        return base;
    }

    UserRecord* findMutable(const std::string& key) {
        if (strategy_ == CollisionStrategy::SeparateChaining) {
            ChainNode* node = buckets_[primaryIndex(key)];
            while (node != nullptr) {
                if (node->key == key) {
                    return &node->value;
                }
                node = node->next;
            }
            return nullptr;
        }

        for (std::size_t attempt = 0; attempt < capacity_; ++attempt) {
            const std::size_t index = probeIndex(key, attempt);
            Slot& slot = slots_[index];

            if (slot.state == SlotState::Empty) {
                return nullptr;
            }
            if (slot.state == SlotState::Occupied && slot.key == key) {
                return &slot.value;
            }
        }
        return nullptr;
    }

    const UserRecord* findConst(const std::string& key) const {
        if (strategy_ == CollisionStrategy::SeparateChaining) {
            ChainNode* node = buckets_[primaryIndex(key)];
            while (node != nullptr) {
                if (node->key == key) {
                    return &node->value;
                }
                node = node->next;
            }
            return nullptr;
        }

        for (std::size_t attempt = 0; attempt < capacity_; ++attempt) {
            const std::size_t index = probeIndex(key, attempt);
            const Slot& slot = slots_[index];

            if (slot.state == SlotState::Empty) {
                return nullptr;
            }
            if (slot.state == SlotState::Occupied && slot.key == key) {
                return &slot.value;
            }
        }
        return nullptr;
    }

    bool insertChainNoResize(
        const std::string& key,
        const UserRecord& record,
        bool count_collision) {
        const std::size_t index = primaryIndex(key);
        ChainNode* bucket_head = buckets_[index];

        if (count_collision && bucket_head != nullptr) {
            ++collision_count_;
        }

        buckets_[index] = new ChainNode(key, record, bucket_head);
        ++size_;
        return true;
    }

    bool insertOpenNoResize(
        const std::string& key,
        const UserRecord& record,
        bool count_collision) {
        std::size_t first_deleted = capacity_;

        for (std::size_t attempt = 0; attempt < capacity_; ++attempt) {
            const std::size_t index = probeIndex(key, attempt);
            Slot& slot = slots_[index];

            if (slot.state == SlotState::Occupied) {
                if (slot.key == key) {
                    slot.value = record;
                    return false;
                }
                if (count_collision) {
                    ++collision_count_;
                }
            } else if (slot.state == SlotState::Deleted) {
                if (first_deleted == capacity_) {
                    first_deleted = index;
                }
            } else {
                const std::size_t destination =
                    first_deleted == capacity_ ? index : first_deleted;
                Slot& destination_slot = slots_[destination];
                destination_slot.key = key;
                destination_slot.value = record;
                destination_slot.state = SlotState::Occupied;
                if (first_deleted != capacity_) {
                    --tombstones_;
                }
                ++size_;
                return true;
            }
        }

        if (first_deleted != capacity_) {
            Slot& destination_slot = slots_[first_deleted];
            destination_slot.key = key;
            destination_slot.value = record;
            destination_slot.state = SlotState::Occupied;
            --tombstones_;
            ++size_;
            return true;
        }

        throw std::runtime_error("HashTable could not find an open slot");
    }

    void ensureCapacityForInsert() {
        const double projected_load =
            static_cast<double>(size_ + 1) / static_cast<double>(capacity_);

        if (projected_load > maximumLoadFactor()) {
            rehash(capacity_ * 2);
        } else if (
            strategy_ != CollisionStrategy::SeparateChaining
            && tombstones_ > capacity_ / 4) {
            // Rebuilding at the same capacity removes deleted-slot markers.
            rehash(capacity_);
        }
    }

    void rehash(std::size_t requested_capacity) {
        const std::size_t old_capacity = capacity_;
        const std::size_t new_capacity = nextPrime(requested_capacity);

        if (strategy_ == CollisionStrategy::SeparateChaining) {
            ChainNode** old_buckets = buckets_;
            buckets_ = new ChainNode*[new_capacity];
            for (std::size_t index = 0; index < new_capacity; ++index) {
                buckets_[index] = nullptr;
            }
            capacity_ = new_capacity;
            size_ = 0;
            tombstones_ = 0;

            for (std::size_t index = 0; index < old_capacity; ++index) {
                ChainNode* node = old_buckets[index];
                while (node != nullptr) {
                    ChainNode* next_node = node->next;
                    insertChainNoResize(node->key, node->value, false);
                    delete node;
                    node = next_node;
                }
            }
            delete[] old_buckets;
        } else {
            Slot* old_slots = slots_;
            slots_ = new Slot[new_capacity];
            capacity_ = new_capacity;
            size_ = 0;
            tombstones_ = 0;

            for (std::size_t index = 0; index < old_capacity; ++index) {
                if (old_slots[index].state == SlotState::Occupied) {
                    insertOpenNoResize(
                        old_slots[index].key,
                        old_slots[index].value,
                        false);
                }
            }
            delete[] old_slots;
        }

        ++rehash_count_;
    }

    void destroyStorage() {
        if (strategy_ == CollisionStrategy::SeparateChaining) {
            for (std::size_t index = 0; index < capacity_; ++index) {
                ChainNode* node = buckets_[index];
                while (node != nullptr) {
                    ChainNode* next_node = node->next;
                    delete node;
                    node = next_node;
                }
            }
            delete[] buckets_;
            buckets_ = nullptr;
        } else {
            delete[] slots_;
            slots_ = nullptr;
        }
    }

public:
    explicit HashTable(
        CollisionStrategy strategy,
        std::size_t initial_capacity = 11)
        : strategy_(strategy),
          capacity_(nextPrime(initial_capacity)),
          size_(0),
          tombstones_(0),
          collision_count_(0),
          rehash_count_(0),
          slots_(nullptr),
          buckets_(nullptr) {
        if (strategy_ == CollisionStrategy::SeparateChaining) {
            buckets_ = new ChainNode*[capacity_];
            for (std::size_t index = 0; index < capacity_; ++index) {
                buckets_[index] = nullptr;
            }
        } else {
            slots_ = new Slot[capacity_];
        }
    }

    ~HashTable() {
        destroyStorage();
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    // Returns true when a new key is inserted and false when an existing key
    // is updated.
    bool insert(const std::string& key, const UserRecord& record) {
        if (key.empty()) {
            throw std::invalid_argument("HashTable keys cannot be empty");
        }

        if (UserRecord* existing = findMutable(key)) {
            *existing = record;
            return false;
        }

        ensureCapacityForInsert();
        if (strategy_ == CollisionStrategy::SeparateChaining) {
            return insertChainNoResize(key, record, true);
        }
        return insertOpenNoResize(key, record, true);
    }

    UserRecord* find(const std::string& key) {
        return findMutable(key);
    }

    const UserRecord* find(const std::string& key) const {
        return findConst(key);
    }

    bool remove(const std::string& key) {
        if (strategy_ == CollisionStrategy::SeparateChaining) {
            const std::size_t index = primaryIndex(key);
            ChainNode** link = &buckets_[index];

            while (*link != nullptr) {
                if ((*link)->key == key) {
                    ChainNode* old_node = *link;
                    *link = old_node->next;
                    delete old_node;
                    --size_;
                    return true;
                }
                link = &((*link)->next);
            }
            return false;
        }

        for (std::size_t attempt = 0; attempt < capacity_; ++attempt) {
            const std::size_t index = probeIndex(key, attempt);
            Slot& slot = slots_[index];

            if (slot.state == SlotState::Empty) {
                return false;
            }
            if (slot.state == SlotState::Occupied && slot.key == key) {
                slot.key.clear();
                slot.value = UserRecord{};
                slot.state = SlotState::Deleted;
                --size_;
                ++tombstones_;

                if (tombstones_ > capacity_ / 4) {
                    rehash(capacity_);
                }
                return true;
            }
        }
        return false;
    }

    double loadFactor() const {
        return static_cast<double>(size_) / static_cast<double>(capacity_);
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    std::size_t collisionCount() const {
        return collision_count_;
    }

    std::size_t rehashCount() const {
        return rehash_count_;
    }

    std::size_t tombstoneCount() const {
        return tombstones_;
    }

    double maxLoadFactor() const {
        return maximumLoadFactor();
    }

    CollisionStrategy strategy() const {
        return strategy_;
    }
};

// -----------------------------------------------------------------------------
// Collision demonstration helper
// -----------------------------------------------------------------------------

struct CollisionPair {
    std::string first;
    std::string second;
    std::size_t bucket;
};

CollisionPair findCollisionPair(std::size_t capacity) {
    std::string* representatives = new std::string[capacity];
    bool* seen = new bool[capacity]();

    for (std::size_t number = 0; number < 10000; ++number) {
        const std::string candidate =
            "collision_user_" + std::to_string(number);
        const std::size_t bucket =
            static_cast<std::size_t>(demoHash(candidate) % capacity);

        if (!seen[bucket]) {
            representatives[bucket] = candidate;
            seen[bucket] = true;
        } else {
            CollisionPair result{
                representatives[bucket], candidate, bucket};
            delete[] representatives;
            delete[] seen;
            return result;
        }
    }

    delete[] representatives;
    delete[] seen;
    throw std::runtime_error("Could not find a collision pair");
}

// -----------------------------------------------------------------------------
// Password records and login service
// -----------------------------------------------------------------------------

enum class LoginResult {
    Success,
    BadPassword,
    UnknownUser
};

const char* loginResultName(LoginResult result) {
    switch (result) {
    case LoginResult::Success:
        return "success";
    case LoginResult::BadPassword:
        return "failed password";
    case LoginResult::UnknownUser:
        return "unknown user";
    }
    return "unknown result";
}

class PasswordService {
private:
    HashTable users_;
    std::string pepper_;

    uint64_t hashPassword(
        const std::string& salt,
        const std::string& password) const {
        return demoHash(salt + password + pepper_);
    }

public:
    explicit PasswordService(
        CollisionStrategy strategy = CollisionStrategy::SeparateChaining)
        : users_(strategy, 5), pepper_("classroom-pepper") {}

    bool registerUser(
        const std::string& username,
        const std::string& password,
        const std::string& salt) {
        UserRecord record;
        record.salt = salt;
        record.password_hash = hashPassword(salt, password);
        record.password_rehashes = 0;
        return users_.insert(username, record);
    }

    LoginResult login(
        const std::string& username,
        const std::string& submitted_password) const {
        const UserRecord* record = users_.find(username);
        if (record == nullptr) {
            return LoginResult::UnknownUser;
        }

        const uint64_t submitted_hash =
            hashPassword(record->salt, submitted_password);
        if (submitted_hash == record->password_hash) {
            return LoginResult::Success;
        }
        return LoginResult::BadPassword;
    }

    // A password rehash event keeps the password the same, but replaces the
    // salt and recomputes salt + password + pepper.  This models a policy
    // upgrade while keeping the example simple and deterministic.
    bool rehashPassword(
        const std::string& username,
        const std::string& current_password,
        const std::string& new_salt) {
        UserRecord* record = users_.find(username);
        if (record == nullptr) {
            return false;
        }

        if (hashPassword(record->salt, current_password)
            != record->password_hash) {
            return false;
        }

        record->salt = new_salt;
        record->password_hash = hashPassword(new_salt, current_password);
        ++record->password_rehashes;
        return true;
    }

    HashTable& table() {
        return users_;
    }

    const HashTable& table() const {
        return users_;
    }
};

// -----------------------------------------------------------------------------
// Test harness
// -----------------------------------------------------------------------------

struct TestStats {
    int passed = 0;
    int failed = 0;

    void expect_true(bool condition, const std::string& label) {
        if (condition) {
            ++passed;
            std::cout << "  [PASS] " << label << '\n';
        } else {
            ++failed;
            std::cout << "  [FAIL] " << label << '\n';
        }
    }

    template <typename Actual, typename Expected>
    void expect_equal(
        const Actual& actual,
        const Expected& expected,
        const std::string& label) {
        if (actual == expected) {
            ++passed;
            std::cout << "  [PASS] " << label << '\n';
        } else {
            ++failed;
            std::cout << "  [FAIL] " << label << '\n';
        }
    }

    template <typename Action>
    void expect_invalid_argument(Action action, const std::string& label) {
        try {
            action();
            ++failed;
            std::cout << "  [FAIL] " << label
                      << " (no invalid_argument was thrown)\n";
        } catch (const std::invalid_argument&) {
            ++passed;
            std::cout << "  [PASS] " << label
                      << " (invalid input handled)\n";
        } catch (const std::exception&) {
            ++failed;
            std::cout << "  [FAIL] " << label
                      << " (wrong exception type)\n";
        }
    }
};

UserRecord sampleRecord(uint64_t value) {
    UserRecord record;
    record.salt = "sample-salt";
    record.password_hash = value;
    return record;
}

void testCollisionStrategy(
    CollisionStrategy strategy,
    TestStats& stats) {
    std::cout << "\n[1] Strategy: " << strategyName(strategy) << '\n';
    HashTable table(strategy, 5);
    const CollisionPair pair = findCollisionPair(table.capacity());

    std::cout << "  Collision pair: " << pair.first << " and "
              << pair.second << " share initial bucket " << pair.bucket
              << '\n';

    stats.expect_true(
        table.insert(pair.first, sampleRecord(101)),
        "first colliding key inserted");
    stats.expect_true(
        table.insert(pair.second, sampleRecord(202)),
        "second colliding key inserted");
    stats.expect_true(
        table.collisionCount() > 0,
        "collision was recorded and mitigated");
    stats.expect_true(
        table.find(pair.first) != nullptr
            && table.find(pair.second) != nullptr,
        "both colliding keys are findable");

    const std::size_t rehashes_before_bulk = table.rehashCount();
    const std::string extra_keys[] = {
        "alpha", "bravo", "charlie", "delta", "echo", "foxtrot"};
    for (std::size_t index = 0; index < 6; ++index) {
        table.insert(extra_keys[index], sampleRecord(300 + index));
    }

    stats.expect_true(
        table.rehashCount() > rehashes_before_bulk,
        "automatic resize/rehash occurred during bulk insertion");
    stats.expect_true(
        table.loadFactor() <= table.maxLoadFactor(),
        "load factor is within the strategy threshold after rehash");

    UserRecord replacement = sampleRecord(999);
    stats.expect_true(
        !table.insert(pair.first, replacement),
        "duplicate key updates an existing record");
    stats.expect_equal(
        table.find(pair.first)->password_hash,
        static_cast<uint64_t>(999),
        "updated record is returned by find");

    stats.expect_true(
        table.remove(pair.first),
        "remove deletes the first colliding key");
    stats.expect_true(
        table.find(pair.first) == nullptr
            && table.find(pair.second) != nullptr,
        "removal does not delete the other colliding key");
    stats.expect_true(
        !table.remove("missing-key"),
        "removing an unknown key returns false");

    std::cout << "  Final size=" << table.size()
              << ", capacity=" << table.capacity()
              << ", loadFactor=" << table.loadFactor()
              << ", collisions=" << table.collisionCount()
              << ", rehashes=" << table.rehashCount() << '\n';
}

void testPasswordFlow(TestStats& stats) {
    std::cout << "\n[2] Password storage and login flow\n";
    PasswordService service(CollisionStrategy::SeparateChaining);

    stats.expect_true(
        service.registerUser("alice", "correct-horse", "salt-alice-v1"),
        "register alice");
    stats.expect_true(
        service.registerUser("bob", "blue-sky-42", "salt-bob-v1"),
        "register bob");
    stats.expect_true(
        service.registerUser("carol", "maple-leaf", "salt-carol-v1"),
        "register carol");
    stats.expect_true(
        service.registerUser("dave", "river-stone", "salt-dave-v1"),
        "register dave");
    stats.expect_true(
        service.registerUser("erin", "coffee-mug", "salt-erin-v1"),
        "register erin");
    stats.expect_true(
        service.registerUser("frank", "orange-comet", "salt-frank-v1"),
        "register frank");

    const HashTable& users = service.table();
    stats.expect_equal(users.size(), static_cast<std::size_t>(6),
                       "six users are stored");
    stats.expect_true(
        users.find("alice") != nullptr
            && !users.find("alice")->salt.empty()
            && users.find("alice")->password_hash != 0,
        "alice record contains a salt and password hash");
    stats.expect_true(
        users.rehashCount() > 0,
        "user table automatically rehashed while growing");

    std::cout << "  Chosen strategy: " << strategyName(users.strategy())
              << "\n  Table capacity=" << users.capacity()
              << ", loadFactor=" << users.loadFactor()
              << ", automatic rehashes=" << users.rehashCount() << '\n';

    stats.expect_true(
        service.login("alice", "correct-horse") == LoginResult::Success,
        "successful login returns success");
    std::cout << "  alice login: "
              << loginResultName(service.login("alice", "correct-horse"))
              << '\n';

    stats.expect_true(
        service.login("alice", "wrong-password")
            == LoginResult::BadPassword,
        "wrong password is rejected");
    std::cout << "  alice wrong-password login: "
              << loginResultName(service.login("alice", "wrong-password"))
              << '\n';

    stats.expect_true(
        service.login("nobody", "anything") == LoginResult::UnknownUser,
        "unknown username is rejected");
    std::cout << "  nobody login: "
              << loginResultName(service.login("nobody", "anything"))
              << '\n';

    UserRecord* alice_before_rehash = service.table().find("alice");
    const std::string old_salt = alice_before_rehash->salt;
    stats.expect_true(
        service.rehashPassword(
            "alice", "correct-horse", "salt-alice-v2"),
        "password rehash event succeeds after verifying the password");
    UserRecord* alice_after_rehash = service.table().find("alice");
    stats.expect_true(
        alice_after_rehash->salt != old_salt
            && alice_after_rehash->password_rehashes == 1,
        "rehash changes the salt and records one rehash event");
    stats.expect_true(
        service.login("alice", "correct-horse") == LoginResult::Success,
        "login still succeeds after password rehash");

    stats.expect_true(
        service.table().remove("erin"),
        "removal deletes erin from the user table");
    stats.expect_true(
        service.login("erin", "coffee-mug") == LoginResult::UnknownUser,
        "removed user is no longer found during login");
    stats.expect_true(
        !service.table().remove("erin"),
        "removing erin a second time returns false");
}

int main() {
    std::cout << "Assignment 06: Passwords and Hash Tables\n";
    std::cout << "Custom table; no standard hash-map container is used.\n";
    std::cout << std::fixed << std::setprecision(3);

    TestStats stats;
    stats.expect_invalid_argument(
        [] {
            HashTable invalid_table(CollisionStrategy::SeparateChaining, 5);
            invalid_table.insert("", sampleRecord(1));
        },
        "empty username is rejected");

    testCollisionStrategy(CollisionStrategy::SeparateChaining, stats);
    testCollisionStrategy(CollisionStrategy::LinearProbing, stats);
    testCollisionStrategy(CollisionStrategy::QuadraticProbing, stats);
    testCollisionStrategy(CollisionStrategy::DoubleHashing, stats);
    testPasswordFlow(stats);

    std::cout << "\n=== TEST SUMMARY ===\n";
    std::cout << "Passed: " << stats.passed << '\n';
    std::cout << "Failed: " << stats.failed << '\n';
    std::cout << (stats.failed == 0 ? "Result: ALL TESTS PASSED\n"
                                    : "Result: TESTS FAILED\n");
    return stats.failed == 0 ? 0 : 1;
}
