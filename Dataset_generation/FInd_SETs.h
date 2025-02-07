#ifndef FIND_SETS_H
#define FIND_SETS_H

#include "Table.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

// Function to efficiently sort an array of -1, 0, and 1 in O(n) time
vector<int> efficient_sort(const vector<int>& input) {
    int countMinusOne = 0, countZero = 0, countOne = 0;

    for (int num : input) {
        if (num == -1) ++countMinusOne;
        else if (num == 0) ++countZero;
        else if (num == 1) ++countOne;
    }

    vector<int> sortedArray;
    sortedArray.reserve(input.size());

    sortedArray.insert(sortedArray.end(), countMinusOne, -1);
    sortedArray.insert(sortedArray.end(), countZero, 0);
    sortedArray.insert(sortedArray.end(), countOne, 1);

    return sortedArray;
}

//function to find all the possible SETs in a Table using the fact that a winning combo of 3 card's single attribute gives a sum of 3, 0 or -3.
//First of all it checks if a combination of three cards has a winning combination of the only first attributes. If so it checks for all the attributes.
//The algorithm to check over all the possible combinations is an optimyzed algorithm based on the fact that the only possible values are -1, 0 and 1
//for all the columns. It has a complexity of O(n^2).
vector<vector<int>> find_SETs(Table table, bool print = false) {
    vector<int> firstColumn = table.getColumn(0);
    vector<pair<int, int>> indexedColumn;
    int numRows = firstColumn.size();

    // Create pairs (value, index) to maintain the original mapping
    for (int i = 0; i < numRows; ++i) {
        indexedColumn.push_back({firstColumn[i], i});
    }

    // Sort `indexedColumn` based on values using counting sort
    vector<int> sortedValues = efficient_sort(firstColumn);
    
    // Reconstruct indexedColumn with the same indices but sorted values
    for (int i = 0; i < numRows; ++i) {
        indexedColumn[i].first = sortedValues[i];
    }

    vector<vector<int>> validSets;

    // Search for valid combinations
    for (int i = 0; i < numRows - 2; ++i) {
        int j = i + 1, k = numRows - 1;

        while (j < k) {
            int sum = indexedColumn[i].first + indexedColumn[j].first + indexedColumn[k].first;

            if (sum == -3 || sum == 0 || sum == 3) {
                // Sum the values of the rows in the combination
                vector<int> columnSums(table.getRow(0).size(), 0);
                vector<int> combination = {indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second};

                for (int rowIdx : combination) {
                    vector<int> row = table.getRow(rowIdx);
                    for (int colIdx = 0; colIdx < row.size(); ++colIdx) {
                        columnSums[colIdx] += row[colIdx];
                    }
                }

                // Check if the sum condition for each column is met (sum must be 3, 0, or -3)
                bool isValid = true;
                for (int sum : columnSums) {
                    if (sum != 3 && sum != 0 && sum != -3) {
                        isValid = false;
                        break;
                    }
                }

                // If the combination is valid, add it to validSets
                if (isValid) {
                    validSets.push_back(combination);
                    if (print) {
                        cout << "Valid Set:\n";
                        for (int rowIdx : combination) {
                            cout << "Card " << rowIdx << ":\n";
                            table.print_row(rowIdx);
                        }
                        cout << "Column sums: ";
                        for (int sum : columnSums) {
                            cout << sum << " ";
                        }
                        cout << "\n\n";
                    }
                }

                ++j;
                --k;
            }
            else if (sum < -3) ++j;
            else --k;
        }
    }

    return validSets;
}


#endif // FIND_SETS_H
