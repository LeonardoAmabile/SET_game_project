#ifndef SETFUNCTIONS_H
#define SETFUNCTIONS_H

#include "Table.h"  
#include <vector>    
#include <algorithm> 
#include <iostream> 

using namespace std;

// Function to count the occurrences of -1, 0, and 1 in a vector
vector<int> count_elements(const vector<int>& input);

// Function to check if three rows form a valid SET
bool is_valid_set(const vector<int>& row1, const vector<int>& row2, const vector<int>& row3);

// Function to add a valid SET combination to the list and optionally print it
void add_valid_set(Table& table, const vector<int>& combination, vector<vector<int>>& validSets, bool print);

// Function to perform counting sort on the indexed column based on the counts of -1, 0, and 1
vector<pair<int, int>> counting_sort(const vector<pair<int, int>>& indexedColumn);

// Function to find all valid SETs in a table
vector<vector<int>> find_SETs(Table& table, bool print = false);

#endif // SETFUNCTIONS_H
