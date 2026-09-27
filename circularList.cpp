#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

class boardSpace {
    public:
	string propertyName;
	string ownerName;
	int purchaseCost;
	bool owned;
	boardSpace *next;

	boardSpace(){
	    propertyName = "No Name";
	    ownerName = "No Owner Name";
	    purchaseCost= -1;
	    owned = 0;
	    next = nullptr;
	}

	boardSpace(string propertyName, string ownerName, int purchaseCost, bool owned){
	    this->propertyName = propertyName;
	    this->ownerName = ownerName;
	    this->purchaseCost = purchaseCost;
	    this->owned = owned;
	    this->next = nullptr;
	}
};

class MonopolyBoard {
    boardSpace *head;
    boardSpace *tail;

    public:
	MonopolyBoard(){
	    head = nullptr;
	    tail = nullptr;
	}

	MonopolyBoard(boardSpace* initialNode){
	    head = initialNode;
	    tail = initialNode;
	}


    void insertSpace(boardSpace* space){
	space->next = head->next;
	head->next = space;
    }

    void print(boardSpace* head){
	boardSpace* temp = head;
	do{
	    std::string s = temp->owned ? "Owned by " + temp->ownerName : "Unowned";
	    std::cout << temp->propertyName << ", " << temp->purchaseCost << ", " << s << "\n";
	    
	    temp = temp->next;
	    
	}while(temp != head);
    }

    boardSpace* search(string name){
	boardSpace* curr = head;
	do{
	    if(curr->propertyName == name){
		return curr;
	    }
	    curr = curr->next;
	}while(curr != head);
	return nullptr;
    }

    boardSpace* traversal(boardSpace* curr, int spaces){
	for (int i = 0; i < spaces; i++){
	    curr = curr->next;
	}
	return curr;
    }

    void purchase(boardSpace* space, string name){
	if(space->owned == 1){
	    if(space->ownerName == name){
		std::cout << space->ownerName << " attempted to purchase their own property" << "\n";
	    }
	    std::cout << "Sorry, " << space->propertyName << " is owned by " << space->ownerName << "\n";
	}
	else{
	    space->owned = 1;
	    space->ownerName = name;
	    std::cout << name << " has purchased " << space->propertyName << " for $" << space->purchaseCost << "\n";
	}

    }

    void generate(int numSpaces, boardSpace* head){
	head->propertyName = "Property1";
	head->purchaseCost = 10;
	this->head = head;
	boardSpace* prev = head;
	for(int i = 0; i < numSpaces-1; i++){
	    boardSpace* temp = new boardSpace();
	    temp->propertyName = "Property" + std::to_string(i+2);
	    temp->ownerName = "No Owner Name";
	    temp->purchaseCost = 20 + ((i+12)*5 *i) - (((i+12)*5 * i) % 10);
	    temp->owned = 0;

	    prev->next = temp;
	    prev = temp;
	}
	    prev->next = head;
	    this->tail = prev;

    }

    void removeNext(boardSpace* curr){
	if (curr == nullptr) {
	    boardSpace* nextSpace = head;
	    head = head->next;
	    delete nextSpace;
	    
	    if (head == nullptr) {
		tail = nullptr;
	    }
	}
	else if (curr->next){
	    boardSpace* nextSpace = curr->next;
	    boardSpace* nextNextSpace = nextSpace->next;
	    curr->next = nextNextSpace;
	    delete nextSpace;

	    if (nextNextSpace == nullptr){
		tail = curr;
	    }
	}
    }
    void simulate(int spaces, int turns){
	std::cout << "\033[2J\033[H" << std::flush;
	boardSpace* head = new boardSpace();
	boardSpace* Eli = head;
	boardSpace* Alejandro = head;
	this->generate(spaces,head);
	auto startClock = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < turns; i++)
	{	

	    
	    this->print(head);
	    
	    auto rawTick = startClock.time_since_epoch();
	    auto eliTick = std::chrono::duration_cast<std::chrono::milliseconds>(rawTick).count() % (i+1) + 1 ;
	    auto alejandroTick =  eliTick % (i+1);

	    Eli = this->traversal(Eli,eliTick);
	    if(i != 0){
	    this->purchase(Eli, "Eli");
	    }

	    Alejandro = this->traversal(Alejandro,alejandroTick);
	    this->purchase(Alejandro, "Alejandro");
	    std::cout << "End of turn " << i + 1;

	    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	    std::cout << "\033[2J\033[H" << std::flush;

	}
    }
};

int main(){
    boardSpace* space1 = new boardSpace();
    MonopolyBoard Board = MonopolyBoard(space1);
    Board.simulate(20, 10);

}

