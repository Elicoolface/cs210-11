#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

// Assignment 05: Stack, Queue, and Deque Implementations
// C++17.  The six ADTs below use only raw arrays or user-defined linked nodes.
// No standard container directly implementing these ADTs is used.

// -----------------------------------------------------------------------------
// Array-based stack
// -----------------------------------------------------------------------------

template <typename T>
class ArrayStack {
private:
    T* data_;
    std::size_t capacity_;
    std::size_t size_;

    void grow() {
        const std::size_t new_capacity = capacity_ * 2;
        T* new_data = new T[new_capacity];

        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[i];
        }

        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

public:
    explicit ArrayStack(std::size_t initial_capacity = 4)
        : data_(nullptr), capacity_(initial_capacity), size_(0) {
        if (capacity_ == 0) {
            throw std::invalid_argument("ArrayStack capacity must be positive");
        }
        data_ = new T[capacity_];
    }

    ~ArrayStack() {
        delete[] data_;
    }

    ArrayStack(const ArrayStack&) = delete;
    ArrayStack& operator=(const ArrayStack&) = delete;

    void push(const T& value) {
        if (size_ == capacity_) {
            grow();
        }
        data_[size_] = value;
        ++size_;
    }

    T pop() {
        if (empty()) {
            throw std::underflow_error("ArrayStack::pop on an empty stack");
        }
        --size_;
        return data_[size_];
    }

    const T& top() const {
        if (empty()) {
            throw std::underflow_error("ArrayStack::top on an empty stack");
        }
        return data_[size_ - 1];
    }

    bool empty() const {
        return size_ == 0;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }
};

// -----------------------------------------------------------------------------
// Linked-list stack
// -----------------------------------------------------------------------------

template <typename T>
class LinkedStack {
private:
    struct Node {
        T value;
        Node* next;

        Node(const T& node_value, Node* next_node)
            : value(node_value), next(next_node) {}
    };

    Node* head_;
    std::size_t size_;

public:
    LinkedStack() : head_(nullptr), size_(0) {}

    ~LinkedStack() {
        while (head_ != nullptr) {
            Node* old_head = head_;
            head_ = head_->next;
            delete old_head;
        }
    }

    LinkedStack(const LinkedStack&) = delete;
    LinkedStack& operator=(const LinkedStack&) = delete;

    void push(const T& value) {
        head_ = new Node(value, head_);
        ++size_;
    }

    T pop() {
        if (empty()) {
            throw std::underflow_error("LinkedStack::pop on an empty stack");
        }

        Node* old_head = head_;
        T value = old_head->value;
        head_ = old_head->next;
        delete old_head;
        --size_;
        return value;
    }

    const T& top() const {
        if (empty()) {
            throw std::underflow_error("LinkedStack::top on an empty stack");
        }
        return head_->value;
    }

    bool empty() const {
        return head_ == nullptr;
    }

    std::size_t size() const {
        return size_;
    }
};

// -----------------------------------------------------------------------------
// Array-based queue using a circular buffer
// -----------------------------------------------------------------------------

template <typename T>
class ArrayQueue {
private:
    T* data_;
    std::size_t capacity_;
    std::size_t front_index_;
    std::size_t size_;

    void grow() {
        const std::size_t new_capacity = capacity_ * 2;
        T* new_data = new T[new_capacity];

        // Copy in logical queue order rather than physical array order.
        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[(front_index_ + i) % capacity_];
        }

        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
        front_index_ = 0;
    }

public:
    explicit ArrayQueue(std::size_t initial_capacity = 4)
        : data_(nullptr),
          capacity_(initial_capacity),
          front_index_(0),
          size_(0) {
        if (capacity_ == 0) {
            throw std::invalid_argument("ArrayQueue capacity must be positive");
        }
        data_ = new T[capacity_];
    }

    ~ArrayQueue() {
        delete[] data_;
    }

    ArrayQueue(const ArrayQueue&) = delete;
    ArrayQueue& operator=(const ArrayQueue&) = delete;

    void enqueue(const T& value) {
        if (size_ == capacity_) {
            grow();
        }

        const std::size_t insertion_index =
            (front_index_ + size_) % capacity_;
        data_[insertion_index] = value;
        ++size_;
    }

    T dequeue() {
        if (empty()) {
            throw std::underflow_error("ArrayQueue::dequeue on an empty queue");
        }

        T value = data_[front_index_];
        front_index_ = (front_index_ + 1) % capacity_;
        --size_;

        if (size_ == 0) {
            front_index_ = 0;
        }
        return value;
    }

    const T& front() const {
        if (empty()) {
            throw std::underflow_error("ArrayQueue::front on an empty queue");
        }
        return data_[front_index_];
    }

    bool empty() const {
        return size_ == 0;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }
};

// -----------------------------------------------------------------------------
// Linked-list queue
// -----------------------------------------------------------------------------

template <typename T>
class LinkedQueue {
private:
    struct Node {
        T value;
        Node* next;

        explicit Node(const T& node_value)
            : value(node_value), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    std::size_t size_;

public:
    LinkedQueue() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~LinkedQueue() {
        while (head_ != nullptr) {
            Node* old_head = head_;
            head_ = head_->next;
            delete old_head;
        }
        tail_ = nullptr;
    }

    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;

    void enqueue(const T& value) {
        Node* new_node = new Node(value);

        if (tail_ == nullptr) {
            head_ = new_node;
            tail_ = new_node;
        } else {
            tail_->next = new_node;
            tail_ = new_node;
        }
        ++size_;
    }

    T dequeue() {
        if (empty()) {
            throw std::underflow_error("LinkedQueue::dequeue on an empty queue");
        }

        Node* old_head = head_;
        T value = old_head->value;
        head_ = old_head->next;
        delete old_head;
        --size_;

        if (head_ == nullptr) {
            tail_ = nullptr;
        }
        return value;
    }

    const T& front() const {
        if (empty()) {
            throw std::underflow_error("LinkedQueue::front on an empty queue");
        }
        return head_->value;
    }

    bool empty() const {
        return head_ == nullptr;
    }

    std::size_t size() const {
        return size_;
    }
};

// -----------------------------------------------------------------------------
// Array-based deque using a circular buffer
// -----------------------------------------------------------------------------

template <typename T>
class ArrayDeque {
private:
    T* data_;
    std::size_t capacity_;
    std::size_t front_index_;
    std::size_t size_;

    void grow() {
        const std::size_t new_capacity = capacity_ * 2;
        T* new_data = new T[new_capacity];

        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[(front_index_ + i) % capacity_];
        }

        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
        front_index_ = 0;
    }

    void ensure_space() {
        if (size_ == capacity_) {
            grow();
        }
    }

public:
    explicit ArrayDeque(std::size_t initial_capacity = 4)
        : data_(nullptr),
          capacity_(initial_capacity),
          front_index_(0),
          size_(0) {
        if (capacity_ == 0) {
            throw std::invalid_argument("ArrayDeque capacity must be positive");
        }
        data_ = new T[capacity_];
    }

    ~ArrayDeque() {
        delete[] data_;
    }

    ArrayDeque(const ArrayDeque&) = delete;
    ArrayDeque& operator=(const ArrayDeque&) = delete;

    void push_front(const T& value) {
        ensure_space();
        front_index_ = (front_index_ + capacity_ - 1) % capacity_;
        data_[front_index_] = value;
        ++size_;
    }

    void push_back(const T& value) {
        ensure_space();
        const std::size_t insertion_index =
            (front_index_ + size_) % capacity_;
        data_[insertion_index] = value;
        ++size_;
    }

    T pop_front() {
        if (empty()) {
            throw std::underflow_error("ArrayDeque::pop_front on an empty deque");
        }

        T value = data_[front_index_];
        front_index_ = (front_index_ + 1) % capacity_;
        --size_;
        if (size_ == 0) {
            front_index_ = 0;
        }
        return value;
    }

    T pop_back() {
        if (empty()) {
            throw std::underflow_error("ArrayDeque::pop_back on an empty deque");
        }

        const std::size_t removal_index =
            (front_index_ + size_ - 1) % capacity_;
        T value = data_[removal_index];
        --size_;
        if (size_ == 0) {
            front_index_ = 0;
        }
        return value;
    }

    const T& front() const {
        if (empty()) {
            throw std::underflow_error("ArrayDeque::front on an empty deque");
        }
        return data_[front_index_];
    }

    const T& back() const {
        if (empty()) {
            throw std::underflow_error("ArrayDeque::back on an empty deque");
        }
        const std::size_t back_index =
            (front_index_ + size_ - 1) % capacity_;
        return data_[back_index];
    }

    bool empty() const {
        return size_ == 0;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }
};

// -----------------------------------------------------------------------------
// Linked-list deque using doubly linked nodes
// -----------------------------------------------------------------------------

template <typename T>
class LinkedDeque {
private:
    struct Node {
        T value;
        Node* previous;
        Node* next;

        explicit Node(const T& node_value)
            : value(node_value), previous(nullptr), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    std::size_t size_;

public:
    LinkedDeque() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~LinkedDeque() {
        while (head_ != nullptr) {
            Node* old_head = head_;
            head_ = head_->next;
            delete old_head;
        }
        tail_ = nullptr;
    }

    LinkedDeque(const LinkedDeque&) = delete;
    LinkedDeque& operator=(const LinkedDeque&) = delete;

    void push_front(const T& value) {
        Node* new_node = new Node(value);
        new_node->next = head_;

        if (head_ != nullptr) {
            head_->previous = new_node;
        } else {
            tail_ = new_node;
        }

        head_ = new_node;
        ++size_;
    }

    void push_back(const T& value) {
        Node* new_node = new Node(value);
        new_node->previous = tail_;

        if (tail_ != nullptr) {
            tail_->next = new_node;
        } else {
            head_ = new_node;
        }

        tail_ = new_node;
        ++size_;
    }

    T pop_front() {
        if (empty()) {
            throw std::underflow_error("LinkedDeque::pop_front on an empty deque");
        }

        Node* old_head = head_;
        T value = old_head->value;
        head_ = old_head->next;

        if (head_ != nullptr) {
            head_->previous = nullptr;
        } else {
            tail_ = nullptr;
        }

        delete old_head;
        --size_;
        return value;
    }

    T pop_back() {
        if (empty()) {
            throw std::underflow_error("LinkedDeque::pop_back on an empty deque");
        }

        Node* old_tail = tail_;
        T value = old_tail->value;
        tail_ = old_tail->previous;

        if (tail_ != nullptr) {
            tail_->next = nullptr;
        } else {
            head_ = nullptr;
        }

        delete old_tail;
        --size_;
        return value;
    }

    const T& front() const {
        if (empty()) {
            throw std::underflow_error("LinkedDeque::front on an empty deque");
        }
        return head_->value;
    }

    const T& back() const {
        if (empty()) {
            throw std::underflow_error("LinkedDeque::back on an empty deque");
        }
        return tail_->value;
    }

    bool empty() const {
        return head_ == nullptr;
    }

    std::size_t size() const {
        return size_;
    }
};

// -----------------------------------------------------------------------------
// Small test harness and demonstrations
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
            std::cout << "  [FAIL] " << label
                      << " (expected " << expected
                      << ", got " << actual << ")\n";
        }
    }

    template <typename Action>
    void expect_underflow(Action action, const std::string& label) {
        try {
            action();
            ++failed;
            std::cout << "  [FAIL] " << label
                      << " (no underflow_error was thrown)\n";
        } catch (const std::underflow_error&) {
            ++passed;
            std::cout << "  [PASS] " << label
                      << " (underflow handled)\n";
        } catch (const std::exception& exception) {
            ++failed;
            std::cout << "  [FAIL] " << label
                      << " (unexpected exception: " << exception.what()
                      << ")\n";
        }
    }
};

void print_operation(int& operation_number, const std::string& description) {
    ++operation_number;
    std::cout << "    op " << operation_number << ": " << description << '\n';
}

void test_array_stack(TestStats& stats) {
    std::cout << "\n[1] Array-based stack\n";
    ArrayStack<int> stack(2);

    stats.expect_underflow(
        [&stack] { (void)stack.pop(); },
        "pop on an empty array stack");
    stats.expect_underflow(
        [&stack] { (void)stack.top(); },
        "top on an empty array stack");

    const std::size_t initial_capacity = stack.capacity();
    int operation_number = 0;
    std::cout << "  Operation sequence (12 operations):\n";

    print_operation(operation_number, "push(10)");
    stack.push(10);
    print_operation(operation_number, "push(20)");
    stack.push(20);
    print_operation(operation_number, "push(30) [triggers resize]");
    stack.push(30);
    print_operation(operation_number, "top() -> 30");
    stats.expect_equal(stack.top(), 30, "array stack top is 30");
    print_operation(operation_number, "pop() -> 30");
    stats.expect_equal(stack.pop(), 30, "array stack pop returns 30");
    print_operation(operation_number, "push(40)");
    stack.push(40);
    print_operation(operation_number, "push(50)");
    stack.push(50);
    print_operation(operation_number, "top() -> 50");
    stats.expect_equal(stack.top(), 50, "array stack top is 50");
    print_operation(operation_number, "pop() -> 50");
    stats.expect_equal(stack.pop(), 50, "array stack pop returns 50");
    print_operation(operation_number, "pop() -> 40");
    stats.expect_equal(stack.pop(), 40, "array stack pop returns 40");
    print_operation(operation_number, "push(60)");
    stack.push(60);
    print_operation(operation_number, "pop() -> 60");
    stats.expect_equal(stack.pop(), 60, "array stack pop returns 60");

    stats.expect_true(
        stack.capacity() > initial_capacity,
        "array stack doubled its capacity when full");
    stats.expect_equal(stack.size(), static_cast<std::size_t>(2),
                       "array stack retains 2 values");
}

void test_linked_stack(TestStats& stats) {
    std::cout << "\n[2] Linked-list stack\n";
    LinkedStack<int> stack;

    stats.expect_underflow(
        [&stack] { (void)stack.pop(); },
        "pop on an empty linked stack");
    stats.expect_underflow(
        [&stack] { (void)stack.top(); },
        "top on an empty linked stack");

    int operation_number = 0;
    std::cout << "  Operation sequence (12 operations):\n";

    print_operation(operation_number, "push(10)");
    stack.push(10);
    print_operation(operation_number, "push(20)");
    stack.push(20);
    print_operation(operation_number, "push(30)");
    stack.push(30);
    print_operation(operation_number, "top() -> 30");
    stats.expect_equal(stack.top(), 30, "linked stack top is 30");
    print_operation(operation_number, "pop() -> 30");
    stats.expect_equal(stack.pop(), 30, "linked stack pop returns 30");
    print_operation(operation_number, "push(40)");
    stack.push(40);
    print_operation(operation_number, "push(50)");
    stack.push(50);
    print_operation(operation_number, "top() -> 50");
    stats.expect_equal(stack.top(), 50, "linked stack top is 50");
    print_operation(operation_number, "pop() -> 50");
    stats.expect_equal(stack.pop(), 50, "linked stack pop returns 50");
    print_operation(operation_number, "pop() -> 40");
    stats.expect_equal(stack.pop(), 40, "linked stack pop returns 40");
    print_operation(operation_number, "push(60)");
    stack.push(60);
    print_operation(operation_number, "pop() -> 60");
    stats.expect_equal(stack.pop(), 60, "linked stack pop returns 60");

    stats.expect_equal(stack.size(), static_cast<std::size_t>(2),
                       "linked stack retains 2 values");
}

void test_array_queue(TestStats& stats) {
    std::cout << "\n[3] Array-based queue (circular buffer)\n";
    ArrayQueue<int> queue(2);

    stats.expect_underflow(
        [&queue] { (void)queue.dequeue(); },
        "dequeue on an empty array queue");
    stats.expect_underflow(
        [&queue] { (void)queue.front(); },
        "front on an empty array queue");

    const std::size_t initial_capacity = queue.capacity();
    int operation_number = 0;
    std::cout << "  Operation sequence (12 operations):\n";

    print_operation(operation_number, "enqueue(1)");
    queue.enqueue(1);
    print_operation(operation_number, "enqueue(2)");
    queue.enqueue(2);
    print_operation(operation_number, "enqueue(3) [triggers resize]");
    queue.enqueue(3);
    print_operation(operation_number, "front() -> 1");
    stats.expect_equal(queue.front(), 1, "array queue front is 1");
    print_operation(operation_number, "dequeue() -> 1");
    stats.expect_equal(queue.dequeue(), 1, "array queue dequeue returns 1");
    print_operation(operation_number, "enqueue(4)");
    queue.enqueue(4);
    print_operation(operation_number, "enqueue(5)");
    queue.enqueue(5);
    print_operation(operation_number, "dequeue() -> 2");
    stats.expect_equal(queue.dequeue(), 2, "array queue dequeue returns 2");
    print_operation(operation_number, "front() -> 3");
    stats.expect_equal(queue.front(), 3, "array queue front is 3");
    print_operation(operation_number, "enqueue(6)");
    queue.enqueue(6);
    print_operation(operation_number, "dequeue() -> 3");
    stats.expect_equal(queue.dequeue(), 3, "array queue dequeue returns 3");
    print_operation(operation_number, "dequeue() -> 4");
    stats.expect_equal(queue.dequeue(), 4, "array queue dequeue returns 4");

    stats.expect_true(
        queue.capacity() > initial_capacity,
        "array queue doubled its capacity when full");
    stats.expect_equal(queue.size(), static_cast<std::size_t>(2),
                       "array queue retains 2 values");
}

void test_linked_queue(TestStats& stats) {
    std::cout << "\n[4] Linked-list queue\n";
    LinkedQueue<int> queue;

    stats.expect_underflow(
        [&queue] { (void)queue.dequeue(); },
        "dequeue on an empty linked queue");
    stats.expect_underflow(
        [&queue] { (void)queue.front(); },
        "front on an empty linked queue");

    int operation_number = 0;
    std::cout << "  Operation sequence (12 operations):\n";

    print_operation(operation_number, "enqueue(1)");
    queue.enqueue(1);
    print_operation(operation_number, "enqueue(2)");
    queue.enqueue(2);
    print_operation(operation_number, "enqueue(3)");
    queue.enqueue(3);
    print_operation(operation_number, "front() -> 1");
    stats.expect_equal(queue.front(), 1, "linked queue front is 1");
    print_operation(operation_number, "dequeue() -> 1");
    stats.expect_equal(queue.dequeue(), 1, "linked queue dequeue returns 1");
    print_operation(operation_number, "enqueue(4)");
    queue.enqueue(4);
    print_operation(operation_number, "enqueue(5)");
    queue.enqueue(5);
    print_operation(operation_number, "dequeue() -> 2");
    stats.expect_equal(queue.dequeue(), 2, "linked queue dequeue returns 2");
    print_operation(operation_number, "front() -> 3");
    stats.expect_equal(queue.front(), 3, "linked queue front is 3");
    print_operation(operation_number, "enqueue(6)");
    queue.enqueue(6);
    print_operation(operation_number, "dequeue() -> 3");
    stats.expect_equal(queue.dequeue(), 3, "linked queue dequeue returns 3");
    print_operation(operation_number, "dequeue() -> 4");
    stats.expect_equal(queue.dequeue(), 4, "linked queue dequeue returns 4");

    stats.expect_equal(queue.size(), static_cast<std::size_t>(2),
                       "linked queue retains 2 values");
}

void test_array_deque(TestStats& stats) {
    std::cout << "\n[5] Array-based deque (circular buffer)\n";
    ArrayDeque<std::string> deque(2);

    stats.expect_underflow(
        [&deque] { (void)deque.pop_front(); },
        "pop_front on an empty array deque");
    stats.expect_underflow(
        [&deque] { (void)deque.pop_back(); },
        "pop_back on an empty array deque");
    stats.expect_underflow(
        [&deque] { (void)deque.front(); },
        "front on an empty array deque");
    stats.expect_underflow(
        [&deque] { (void)deque.back(); },
        "back on an empty array deque");

    const std::size_t initial_capacity = deque.capacity();
    int operation_number = 0;
    std::cout << "  Operation sequence (12 operations):\n";

    print_operation(operation_number, "push_back(\"B\")");
    deque.push_back("B");
    print_operation(operation_number, "push_front(\"A\")");
    deque.push_front("A");
    print_operation(operation_number, "push_back(\"C\") [triggers resize]");
    deque.push_back("C");
    print_operation(operation_number, "push_front(\"X\")");
    deque.push_front("X");
    print_operation(operation_number, "front() -> X");
    stats.expect_equal(deque.front(), std::string("X"),
                       "array deque front is X");
    print_operation(operation_number, "back() -> C");
    stats.expect_equal(deque.back(), std::string("C"),
                       "array deque back is C");
    print_operation(operation_number, "pop_front() -> X");
    stats.expect_equal(deque.pop_front(), std::string("X"),
                       "array deque pop_front returns X");
    print_operation(operation_number, "push_front(\"Y\")");
    deque.push_front("Y");
    print_operation(operation_number, "push_back(\"D\")");
    deque.push_back("D");
    print_operation(operation_number, "pop_back() -> D");
    stats.expect_equal(deque.pop_back(), std::string("D"),
                       "array deque pop_back returns D");
    print_operation(operation_number, "pop_front() -> Y");
    stats.expect_equal(deque.pop_front(), std::string("Y"),
                       "array deque pop_front returns Y");
    print_operation(operation_number, "pop_back() -> C");
    stats.expect_equal(deque.pop_back(), std::string("C"),
                       "array deque pop_back returns C");

    stats.expect_true(
        deque.capacity() > initial_capacity,
        "array deque doubled its capacity when full");
    stats.expect_equal(deque.front(), std::string("A"),
                       "array deque remaining front is A");
    stats.expect_equal(deque.back(), std::string("B"),
                       "array deque remaining back is B");
    stats.expect_equal(deque.size(), static_cast<std::size_t>(2),
                       "array deque retains 2 values");
}

void test_linked_deque(TestStats& stats) {
    std::cout << "\n[6] Linked-list deque (doubly linked)\n";
    LinkedDeque<std::string> deque;

    stats.expect_underflow(
        [&deque] { (void)deque.pop_front(); },
        "pop_front on an empty linked deque");
    stats.expect_underflow(
        [&deque] { (void)deque.pop_back(); },
        "pop_back on an empty linked deque");
    stats.expect_underflow(
        [&deque] { (void)deque.front(); },
        "front on an empty linked deque");
    stats.expect_underflow(
        [&deque] { (void)deque.back(); },
        "back on an empty linked deque");

    int operation_number = 0;
    std::cout << "  Operation sequence (12 operations):\n";

    print_operation(operation_number, "push_back(\"B\")");
    deque.push_back("B");
    print_operation(operation_number, "push_front(\"A\")");
    deque.push_front("A");
    print_operation(operation_number, "push_back(\"C\")");
    deque.push_back("C");
    print_operation(operation_number, "push_front(\"X\")");
    deque.push_front("X");
    print_operation(operation_number, "front() -> X");
    stats.expect_equal(deque.front(), std::string("X"),
                       "linked deque front is X");
    print_operation(operation_number, "back() -> C");
    stats.expect_equal(deque.back(), std::string("C"),
                       "linked deque back is C");
    print_operation(operation_number, "pop_front() -> X");
    stats.expect_equal(deque.pop_front(), std::string("X"),
                       "linked deque pop_front returns X");
    print_operation(operation_number, "push_front(\"Y\")");
    deque.push_front("Y");
    print_operation(operation_number, "push_back(\"D\")");
    deque.push_back("D");
    print_operation(operation_number, "pop_back() -> D");
    stats.expect_equal(deque.pop_back(), std::string("D"),
                       "linked deque pop_back returns D");
    print_operation(operation_number, "pop_front() -> Y");
    stats.expect_equal(deque.pop_front(), std::string("Y"),
                       "linked deque pop_front returns Y");
    print_operation(operation_number, "pop_back() -> C");
    stats.expect_equal(deque.pop_back(), std::string("C"),
                       "linked deque pop_back returns C");

    stats.expect_equal(deque.front(), std::string("A"),
                       "linked deque remaining front is A");
    stats.expect_equal(deque.back(), std::string("B"),
                       "linked deque remaining back is B");
    stats.expect_equal(deque.size(), static_cast<std::size_t>(2),
                       "linked deque retains 2 values");
}

int main() {
    std::cout << "Assignment 05: Stack, Queue, and Deque Demonstration\n";
    std::cout << "C++17 custom implementations; no <stack>, <queue>, or <deque>.\n";

    TestStats stats;
    test_array_stack(stats);
    test_linked_stack(stats);
    test_array_queue(stats);
    test_linked_queue(stats);
    test_array_deque(stats);
    test_linked_deque(stats);

    std::cout << "\n=== TEST SUMMARY ===\n";
    std::cout << "Passed: " << stats.passed << '\n';
    std::cout << "Failed: " << stats.failed << '\n';
    std::cout << (stats.failed == 0 ? "Result: ALL TESTS PASSED\n"
                                    : "Result: TESTS FAILED\n");

    return stats.failed == 0 ? 0 : 1;
}
