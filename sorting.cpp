#include <vector>
#include <iostream>
#include <chrono>

bool isSorted(const std::vector<int>& values){
    
    for(int i = 0; i < values.size() - 1; i++){
	if(values[i] > values[i+1]){
	    return false;
	}
    }

    return true;
}

std::vector<int> bubble(const std::vector<int>& values){
    std::vector<int> sortedValues = values;

    for(int i = 0; i < sortedValues.size(); i++){
       for(int j = 0; j < sortedValues.size() - i - 1; j++){
	   if (sortedValues[j] > sortedValues[j+1]){
	       int temp = sortedValues[j];
	       sortedValues[j] = sortedValues[j+1];
	       sortedValues[j+1] = temp;
	       
	   }
       }

   }
    return sortedValues;
}
std::vector<int> selection(const std::vector<int>& values){
    std::vector sortedValues = values;
    for(int i = 0; i < sortedValues.size() -1; i++){
	int min = i;

	for(int j = i + 1; j < sortedValues.size(); j++ ){
	    if(sortedValues[j] < sortedValues[min]){
		min = j;
	    }
	}

	if (min != i){
	    int temp = sortedValues[i];
	    sortedValues[i] = sortedValues[min];
	    sortedValues[min] = temp;
	}
    }
    return sortedValues;
    
}
std::vector<int> quick(const std::vector<int>& values, int low, int high) {
    std::vector<int> sortedValues = values;

    if (low >= high) {
        return sortedValues;
    }

    int pivot = sortedValues[high];
    int i = low;

    for (int j = low; j < high; j++) {
        if (sortedValues[j] < pivot) {
            int temp = sortedValues[i];
            sortedValues[i] = sortedValues[j];
            sortedValues[j] = temp;

            i++;
        }
    }

    int temp = sortedValues[i];
    sortedValues[i] = sortedValues[high];
    sortedValues[high] = temp;

    sortedValues = quick(sortedValues, low, i - 1);

    sortedValues = quick(sortedValues, i + 1, high);

    return sortedValues;
}
int main(){
    std::vector<std::vector<int>> testCases = {
    {8, 3, 7, 4, 2, 9, 1, 6},
    {5, 1, 9, 3, 7, 2, 8, 4},
    {1, 2, 3, 4, 5, 6, 7, 8},
    {8, 7, 6, 5, 4, 3, 2, 1},
    {10, 4, 6, 1, 9, 3, 8, 2, 7, 5}
    };
    
    std::vector<int> answer1{};
    std::vector<int> answer2{};
    std::vector<int> answer3{};
    std::vector<int> currVec; 
    
    for (int i = 0; i < testCases.size(); i++){
	for (int j = 0; j < testCases[i].size(); j++){
		currVec.push_back(testCases[i][j]);
        }


	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i< 10000; i++){
	answer1 = bubble(currVec);
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto bubbleDuration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );
	
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i< 10000; i++){
	answer2 = selection(currVec);
	}
	end = std::chrono::high_resolution_clock::now();
	auto selectionDuration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );

	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i< 10000; i++){
	answer3 = quick(currVec,0, currVec.size() - 1);
	}
	end = std::chrono::high_resolution_clock::now();
	auto quickDuration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );


	currVec.clear();


	std::cout << "\nBubble: ";
	for(int num : answer1){
	    std::cout << num;
	}
	std::cout << "\n Runtime:" << bubbleDuration.count() << "ms";
	
	std::cout << "\nSelection: ";
	for(int num : answer2){
	    std::cout << num;
	}
	std::cout << "\n Runtime:" << selectionDuration.count() << "ms";

	std::cout << "\nQuick: ";
	for(int num : answer3){
	    std::cout << num;
	}
	std::cout << "\n Runtime:" << quickDuration.count() << "ms";






	    
    }

}

