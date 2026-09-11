#include <vector>
#include <iostream>


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
	return -1;
    }
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



int main(){
   std::vector numbers = { 2, 4, 7, 10, 11, 32, 45, 87 };
    
   int answer = iterativeBinarySearch(numbers, 32); 
   int answer2 = recursiveBinarySearch(numbers, numbers.size()-1, 0, 32);
   std::cout << "Iterative: " << answer << "\n" << "Recursive: " << answer2 << "\n";
}




















