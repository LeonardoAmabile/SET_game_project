#ifndef FIND_SETS_H
#define FIND_SETS_H

#include "Table.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

// This function efficiently sorts an input vector containing -1, 0, 1 values
// and returns both the sorted array and the counts of -1, 0, and 1 elements.
pair<vector<int>, vector<int>> efficient_sort(const vector<int>& input) {
    int countMinusOne = 0, countZero = 0, countOne = 0;

    // Count the occurrences of -1, 0, and 1
    for (int num : input) {
        if (num == -1) ++countMinusOne;
        else if (num == 0) ++countZero;
        else if (num == 1) ++countOne;
    }

    // Prepare the sorted array by inserting counts of -1, 0, and 1
    vector<int> sortedArray;
    vector<int> numCounts = {countMinusOne, countZero, countOne};
    sortedArray.reserve(input.size());

    sortedArray.insert(sortedArray.end(), countMinusOne, -1);
    sortedArray.insert(sortedArray.end(), countZero, 0);
    sortedArray.insert(sortedArray.end(), countOne, 1);

    return {sortedArray, numCounts}; // Return both the sorted array and counts of elements
}

// Given three rows in a table, this function checks if they form a SET.
// A SET is valid if for each attribute, the sum of the three elements is either 0, 3, or -3.
bool check_combination(const vector<int>& arr1, const vector<int>& arr2, const vector<int>& arr3) {
    // Assume that the vectors have the same size
    int size = arr1.size();
    
    // Check the sum of each attribute across the three rows
    for (int i = 0; i < size; ++i) {
        int sum = arr1[i] + arr2[i] + arr3[i];
        if (sum != 0 && sum != 3 && sum != -3) {
            return false; // If any sum is not 0, 3, or -3, return false
        }
    }
    
    return true; // If all attributes satisfy the condition, return true
}

// This function finds all the possible SETs in a table.
// It checks combinations of three cards using the fact that a valid SET for each attribute
// must result in a sum of 0, 3, or -3.
// The algorithm is optimized based on the fact that the only possible values for each attribute are -1, 0, and 1.
vector<vector<int>> find_SETs(Table table, bool print = false) {
    vector<int> firstColumn = table.getColumn(0);
    auto sortedResult = efficient_sort(firstColumn);
    vector<int> sortedValues = sortedResult.first;
    vector<int> numCounts = sortedResult.second;
    int numRows = firstColumn.size();

    // Prepare a vector of pairs containing the values from the first column and their row indices
    vector<pair<int, int>> indexedColumn;
    for (int i = 0; i < numRows; ++i) {
        indexedColumn.push_back({firstColumn[i], i});  // Store the value and the original row index
    }
    
    // Sort the indexed column based on the values (first element of each pair)
    sort(indexedColumn.begin(), indexedColumn.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.first < b.first; // Sort in ascending order based on the first element (value)
    });

    // Optionally print the sorted values and their corresponding row indices
    for (const auto& p : indexedColumn) {
        cout << "Value: " << p.first << ", Original Row Index: " << p.second << "\n";
    }
    

    vector<vector<int>> validSets;

    // Counter for the number of valid sets found
    int validSetCount = 0;

    // Loop through all pairs of rows to check for valid SETs
    for (int i = 0; i < numRows - 2; ++i) {
        for (int j = i + 1; j < numRows - 1; ++j) {
            int pair_sum = indexedColumn[i].first + indexedColumn[j].first;
            int k = j + 1;

            // Case where the sum of the first two rows is -2
            if (pair_sum == -2) {
                k = j + 1;
                while (k < numCounts[0]) {
                    vector<int> combination = {indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second};
                    if (check_combination(table.getRow(combination[0]), table.getRow(combination[1]), table.getRow(combination[2]))) {
                        validSets.push_back(combination);
                        validSetCount++;  // Increment the counter for valid sets
                        if (print) {
                            cout << "i,j,k =" << i << " " << j << " " << k << "\n";
                            cout << "\n" << "Valid Set:\n";
                            for (int rowIdx : combination) {
                                cout << "Card " << rowIdx << ":\n";
                                table.print_row(rowIdx);
                            }
                        }
                    }
                    k++;
                }
            } 
            // Case where the sum of the first two rows is 2
            else if (pair_sum == 2) {
                k = j + 1;
                while (k < numRows) {
                    vector<int> combination = {indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second};
                    if (check_combination(table.getRow(combination[0]), table.getRow(combination[1]), table.getRow(combination[2]))) {
                        validSets.push_back(combination);
                        validSetCount++;  // Increment the counter for valid sets
                        if (print) {
                            cout << "\n" << "Valid Set:\n";
                            for (int rowIdx : combination) {
                                cout << "Card " << rowIdx << ":\n";
                                table.print_row(rowIdx);
                            }
                        }
                    }
                    k++;
                }
            }
            // Case where the sum of the first two rows is -1
            else if (pair_sum == -1) {
                k = max(j + 1, numCounts[0] + numCounts[1] - 1);
                while (k < numRows) {
                    vector<int> combination = {indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second};
                    if (check_combination(table.getRow(combination[0]), table.getRow(combination[1]), table.getRow(combination[2]))) {
                        validSets.push_back(combination);
                        validSetCount++;  // Increment the counter for valid sets
                        if (print) {
                            cout << "\n" << "Valid Set:\n";
                            for (int rowIdx : combination) {
                                cout << "Card " << rowIdx << ":\n";
                                table.print_row(rowIdx);
                            }
                        }
                    }
                    k++;
                }
            }
            
            // Case where the sum of the first two rows is 0
            else if (pair_sum == 0) {
                k = max(j + 1, numCounts[0] - 1);  // Correctly initialize k
            
                while (k < numCounts[0] + numCounts[1]) {
                    // Check the conditions to skip certain values of k
                    if ((indexedColumn[i].first == -1 || indexedColumn[i].first == 1) ||
                        (indexedColumn[j].first == -1 || indexedColumn[j].first == 1)) {
                        k++;  // Skip k if the condition is true
                        continue;  // Immediately continue to the next iteration
                    }
            
                    vector<int> combination = {indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second};
                    if (check_combination(table.getRow(combination[0]), table.getRow(combination[1]), table.getRow(combination[2]))) {
                        validSets.push_back(combination);
                        validSetCount++;  // Increment the counter for valid sets
                        if (print) {
                            cout << i << " " << j << " " << k << "\n";
                            cout << "\n" << "Valid Set:\n";
                            for (int rowIdx : combination) {
                                cout << "Card " << rowIdx << ":\n";
                                table.print_row(rowIdx);
                            }
                        }
                    }
                    
                    k++;  // Increment k after checking all conditions
                }
            }
        }
    }

    // If print is true, print the total number of valid sets found
    if (print) {
        cout << "Total Valid Sets Found: " << validSetCount << endl;
    }

    return validSets;
}

#endif // FIND_SETS_H
