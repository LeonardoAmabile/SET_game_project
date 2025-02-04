#ifndef WINNING_HANDS_H
#define WINNING_HANDS_H

#include "Table.h"
#include <vector>
#include <algorithm>
#include <iostream>

// Function to efficiently sort an array of -1, 0, and 1 in O(n) time
std::vector<int> efficient_sort(const std::vector<int>& input) {
    int countMinusOne = 0, countZero = 0, countOne = 0;

    for (int num : input) {
        if (num == -1) ++countMinusOne;
        else if (num == 0) ++countZero;
        else if (num == 1) ++countOne;
    }

    std::vector<int> sortedArray;
    sortedArray.reserve(input.size());

    sortedArray.insert(sortedArray.end(), countMinusOne, -1);
    sortedArray.insert(sortedArray.end(), countZero, 0);
    sortedArray.insert(sortedArray.end(), countOne, 1);

    return sortedArray;
}

// Function to find all possible valid SET combinations (based on the fact that the sum of the first column has to be 3, 0 -3 for each SET)
std::vector<std::vector<int>> find_possible_SETs(Table table, bool print = false) {
    std::vector<int> firstColumn = table.getColumn(0);  
    std::vector<std::pair<int, int>> indexedColumn;  
    int numRows = firstColumn.size();  

    // Create pairs (value, index) to maintain the original mapping
    for (int i = 0; i < numRows; ++i) {
        indexedColumn.push_back({firstColumn[i], i});
    }

    // Sort `indexedColumn` based on values using counting sort
    std::vector<int> sortedValues = efficient_sort(firstColumn);
    
    // Reconstruct indexedColumn with the same indices but sorted values
    for (int i = 0; i < numRows; ++i) {
        indexedColumn[i].first = sortedValues[i];
    }

    std::vector<std::vector<int>> validCombinations;

    // Search for valid combinations
    for (int i = 0; i < numRows - 2; ++i) {
        int j = i + 1, k = numRows - 1;

        while (j < k) {
            int sum = indexedColumn[i].first + indexedColumn[j].first + indexedColumn[k].first;

            if (sum == -3 || sum == 0 || sum == 3) {
                validCombinations.push_back({indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second});

                if (print) {
                    std::cout << "Combination of row indices: " << indexedColumn[i].second << " " 
                              << indexedColumn[j].second << " " << indexedColumn[k].second << std::endl;
                }
                ++j;
                --k;
            } 
            else if (sum < -3) ++j;
            else --k;
        }
    }

    return validCombinations;
}

// Function to filter valid SETs based on the sum condition for each column
std::vector<std::vector<int>> find_SETs(Table table, bool print = false) {
    std::vector<std::vector<int>> possibleSets = find_possible_SETs(table, false);
    std::vector<std::vector<int>> validSets;  

    for (const auto& combination : possibleSets) {
        std::vector<int> columnSums(table.getRow(0).size(), 0);

        // Sum the values of the rows in the combination
        for (int rowIdx : combination) {
            std::vector<int> row = table.getRow(rowIdx);
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
                std::cout << "Valid Set:\n";
                for (int rowIdx : combination) {
                    std::cout << "Card " << rowIdx << ":\n";
                    table.print_row(rowIdx);
                }
                std::cout << "Column sums: ";
                for (int sum : columnSums) {
                    std::cout << sum << " ";
                }
                std::cout << "\n\n";
            }
        }
    }

    return validSets;
}

#endif // WINNING_HANDS_H
