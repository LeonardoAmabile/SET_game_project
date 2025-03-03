#ifndef FIND_SETS_H
#define FIND_SETS_H

#include "Table.h"  

#include <vector>    
#include <algorithm> 
#include <iostream> 

using namespace std;


// Function to check if three rows form a valid SET
bool is_valid_set(const vector<int>& row1, const vector<int>& row2, const vector<int>& row3);

// Function to count the occurrences of -1, 0, and 1 in a vector
vector<int> count_elements(const vector<int>& input);

//Sorts table instance's row by their first element
void sort_table_by_first_element(Table& table);

// Function to find all valid SETs in a table
vector<vector<int>> find_SETs(Table& table, bool print = false);

#endif // Find_SETS_H
