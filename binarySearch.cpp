#include <vector>
#include <iostream>
#include <chrono>


int iterativeBinarySearch(std::vector<int> &nums, int target){
    int low{};
    int high = nums.size() - 1;
    
    while (high >= low){
	int mid = (high + low) / 2;
	if (nums[mid] < target) {
	    low = mid + 1; 
	}
	else if(nums[mid] > target) {
	    high = mid - 1;
	}
	else{
	    return mid;
	}
    }
    
    return -1;
};

int recursiveBinarySearch(std::vector<int> &nums, int high, int low, int target){
    if(low > high){
	return -1; }
    int mid = (high + low) / 2;
    if (nums[mid] < target){
	return recursiveBinarySearch(nums, high, mid + 1, target);
    }
    else if(nums[mid] > target){
	return recursiveBinarySearch(nums, mid - 1, low, target);
    }
    else {
	return mid;
    }
    return -1;
};

int linearSearch(std::vector<int> &nums, int target){
    for(int i = 0; i < nums.size(); i++){
	if (nums[i] == target){
	    return i;
	}
    }
    return -1;
}



int main(){

   std::vector numbers = { 2, 4, 7, 10, 11, 32, 45, 87 };

   int testNumbers[5][8] = {
   { 2, 4, 7, 10, 11, 32, 45, 87 },
   { 1, 4, 5, 6, 8, 30, 38, 80 },
   { 5, 7, 8, 13, 15, 16, 60, 90},
   { 2, 4, 12, 15, 18, 32, 55, 87 },
   { 2, 4, 7, 10, 11, 32, 45, 87 }
   };

   int targets[5] = {2,80,13,1,50};
   int currTarget{};
   int answer1{};
   int answer2{};
   int answer3{};
   std::vector<int> currVec; 
   for (int i = 0; i < 5; i++){

	currTarget = targets[i];
	for (int j = 0; j < 8; j++){
	   currVec.push_back(testNumbers[i][j]);
	}
	
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i< 1000000; i++){
	answer1 = iterativeBinarySearch(currVec, currTarget);
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto iterativeDuration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );
	
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i< 1000000; i++){
	answer2 = recursiveBinarySearch(currVec, currVec.size()-1,0,currTarget);
	}
	end = std::chrono::high_resolution_clock::now();
	auto recursiveDuration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );

	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i< 1000000; i++){
	answer3 = linearSearch(currVec, currTarget);
	}
	end = std::chrono::high_resolution_clock::now();
	auto linearDuration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );


	currVec.clear();

	std::cout << 
	    "\nCase " << i << ":" << 
	    "\nIterative: " << answer1 << " Runtime: " << iterativeDuration.count() << "ms"
	    "\nRecursive: " << answer2 << " Runtime: "<< recursiveDuration.count() << "ms"
	    "\nLinear: " << answer3 << " Runtime: "<< linearDuration.count() << "ms\n";
    }
   }




















