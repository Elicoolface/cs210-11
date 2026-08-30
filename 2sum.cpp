#include <iostream>
#include <vector>
#include <unordered_map>



std::pair<int,int> twoSumBruteForce(const std::vector<int>& nums, int target)
{

    for(int i = 0; i < nums.size(); i++){
	for(int j = i + 1; j < nums.size(); j++)
	{
	    if ((nums[i]+nums[j])==target){
		return std::pair<int,int>(i, j);
	    }
	}
    }
    return std::pair<int,int>(-1,-1);
};

std::pair<int,int> twoSumHash(const std::vector<int>& nums, int target){
    std::unordered_map<int, int> theMap;

    for (int i = 0; i < nums.size(); i++){
	theMap[nums[i]] = i;
    }
    
    for (int i = 0; i < nums.size(); i++){
	int candidate = target - nums[i];
	if(theMap.count(candidate) && theMap[candidate] != i){
	    return std::pair(i,theMap[candidate]);
	}
	
    }

    return std::pair<int,int>(-1,-1);


};


int main(){

    int testArrays[4][15] = {
	{15, 4, 18, 8, 19, 22, 24, 59, 59, 20, 18, 12, 36, 42, 9},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,2,3,5,8,12,2,9,11,22,13,14,15,18,20},
	{0,1,5,6,7,8,9,4,2,9,11,17,18,32,60},
    };

    int testTargets[4] = {24,210,42,3};
    
    std::vector<int> nums;
    int target;


    for(int i = 0; i < 4; i++){
	target = testTargets[i];
	for(int j = 0; j < 15; j++){
	    nums.push_back(testArrays[i][j]);
	}
	auto [first,second] = twoSumBruteForce(nums, target); 
	std::cout << "Brute Force Test Case " << i + 1 << ": " << first << " " << second << "\n";
	nums.clear();
    }

    for(int i = 0; i < 4; i++){
	target = testTargets[i];
	for(int j = 0; j < 15; j++){
	    nums.push_back(testArrays[i][j]);
	}
	auto [first,second] = twoSumHash(nums, target); 
	std::cout << "Hash-like Test Case " << i + 1 << ": " << first << " " << second << "\n";
	nums.clear();
    }



}














