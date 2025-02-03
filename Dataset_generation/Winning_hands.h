#ifndef IS_WINNING_H
#define IS_WINNING_H

#include "Table.h"
#include <vector>
#include <algorithm>

// Function to find all possible valid combinations of sets (based on the sum of only the first elements)
std::vector<std::vector<int>> find_possible_SETs(Table table, bool print = false) {
    std::vector<int> firstColumn = table.getColumn(0);  // Get the first column of the table
    std::vector<std::pair<int, int>> indexedColumn;  // Vector to store (value, index) pairs for sorting
    int numRows = firstColumn.size();  // Total number of rows

    // Create pairs of (value, index) to maintain original index mapping
    for (int i = 0; i < numRows; ++i) {
        indexedColumn.push_back({firstColumn[i], i});
    }

    // Sort based on values in the first column while maintaining original indices
    std::sort(indexedColumn.begin(), indexedColumn.end());

    std::vector<std::vector<int>> validCombinations;  // Vector to store valid row combinations

    // Find all valid triplets (i, j, k) where the sum of the values is -3, 0, or 3
    for (int i = 0; i < numRows - 2; ++i) {
        int j = i + 1;
        int k = numRows - 1;

        while (j < k) {
            int sum = indexedColumn[i].first + indexedColumn[j].first + indexedColumn[k].first;

            // If the sum is -3, 0, or 3, store the row indices (i, j, k)
            if (sum == -3 || sum == 0 || sum == 3) {
                validCombinations.push_back({indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second});  // Store the combination

                // If print is true, print the combination and its corresponding values
                if (print) {
                    std::cout << "Combination of row indices: ";
                    std::cout << indexedColumn[i].second << " " << indexedColumn[j].second << " " << indexedColumn[k].second << std::endl;
                    std::cout << "Values: ";
                    std::cout << indexedColumn[i].first << " " << indexedColumn[j].first << " " << indexedColumn[k].first << std::endl;
                }

                // Move the pointers
                ++j;
                --k;
            } 
            // If the sum is too small, move the j pointer
            else if (sum < -3) {
                ++j;
            } 
            // If the sum is too large, move the k pointer
            else {
                --k;
            }
        }
    }

    return validCombinations;  // Return the valid combinations of row indices
}

// Function to filter valid SETS based on the sum condition for each column
std::vector<std::vector<int>> find_SETs(Table table, bool print = false) {
    std::vector<std::vector<int>> possibleSets = find_possible_SETs(table, false);  // Get all possible valid sets
    std::vector<std::vector<int>> validSets;  // Vector to store the valid sets after checking column sum conditions

    // Check each combination for validity
    for (const auto& combination : possibleSets) {
        // Initialize columnSums as a vector of zeros, with the same size as the number of columns in the table
        std::vector<int> columnSums(table.getRow(0).size(), 0);
    
        // Sum the values of the rows in the combination
        for (int rowIdx = 0; rowIdx < combination.size(); ++rowIdx) {
            std::vector<int> row = table.getRow(combination[rowIdx]);  // Get the row by its index
            for (int colIdx = 0; colIdx < row.size(); ++colIdx) {
                columnSums[colIdx] += row[colIdx];  // Add each column's value to the corresponding sum
            }
        }

        // Check if the sum condition for each column is met (sum must be 3, 0, or -3)
        bool isValid = true;
        for (int sum : columnSums) {
            if (sum != 3 && sum != 0 && sum != -3) {
                isValid = false;  // Mark the combination as invalid if the sum does not meet the condition
                break;
            }
        }

        // If the combination is valid, add it to validSets
        if (isValid) {
            validSets.push_back(combination);

            // If print is true, print the valid set and the corresponding row indices
            if (print) {
                std::cout << "Valid Set: "<<"\n";
                for (const int& rowIdx : combination) {
                    std::cout << "Card "<<rowIdx << ":\n";  // Print the row index
                    table.print_row(rowIdx);
                }
                std::cout << "\nSum of columns: ";
                for (int sum : columnSums) {
                    std::cout << sum << " ";  // Print the sum of each column
                }
                std::cout << "\n\n";
            }
        }
    }

    return validSets;  // Return the valid sets (as row indices)
}

#endif // IS_WINNING_H
