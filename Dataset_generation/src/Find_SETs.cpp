#include "Table.h"  
#include <vector>    
#include <algorithm> 
#include <iostream> 

using namespace std; 

// Function to count the occurrences of -1, 0, and 1 in a vector
vector<int> count_elements(const vector<int>& input) {
    vector<int> counts(3, 0);  // Create a vector `counts` initialized to [0, 0, 0]. 
                               // counts[0] for -1, counts[1] for 0, counts[2] for 1.

    // Loop over each number in the input vector and update the respective count
    for (int num : input) {
        if (num == -1) ++counts[0]; // If the number is -1, increment counts[0]
        else if (num == 0) ++counts[1]; // If the number is 0, increment counts[1]
        else if (num == 1) ++counts[2]; // If the number is 1, increment counts[2]
    }

    return counts; // Return the counts of -1, 0, and 1 in a vector
}

// Function to check if three rows form a valid SET (according to specific conditions)
bool is_valid_set(const vector<int>& row1, const vector<int>& row2, const vector<int>& row3) {
    // Iterate through each element of the rows
    for (size_t i = 0; i < row1.size(); ++i) {
        int sum = row1[i] + row2[i] + row3[i]; // Sum the corresponding elements from all three rows
        // Check if the sum is neither 0, 3, nor -3. If it's any other value, return false.
        if (sum != 0 && sum != 3 && sum != -3) return false;
    }
    return true; // If all conditions pass, the set is valid
}

// Function to add a valid SET combination to the list and optionally print it
void add_valid_set(Table& table, const vector<int>& combination, vector<vector<int>>& validSets, bool print) {
    // Check if the combination of rows forms a valid SET
    if (is_valid_set(table.getRow(combination[0]), table.getRow(combination[1]), table.getRow(combination[2]))) {
        validSets.push_back(combination); // Add the combination to the list of valid sets

        // If print flag is true, print the valid set
        if (print) {
            cout << "\nValid Set:\n";
            for (int rowIdx : combination) { // Loop through each row index in the combination
                cout << "Card " << rowIdx << ":\n"; 
                table.print_row(rowIdx); // Print the corresponding row of the table
            }
        }
    }
}

vector<pair<int, int>> counting_sort(const vector<pair<int, int>>& indexedColumn) {
    vector<pair<int, int>> sortedColumn(indexedColumn.size()); // Vettore per il risultato

    // definition of the vector of -1, 0 and 1s in indexed column
    vector<int> values;
    for (const auto& p : indexedColumn) {
        values.push_back(p.first);
    }
    vector<int> counts = count_elements(values);

    // Start indices
    int countMinusOnes = 0;             // -1 from position 0
    int countZeros = counts[0];         // 0 after the 1s
    int countOnes = counts[0] + counts[1]; // 1 after the 0s

    // Sort elements in the correct order
    for (const auto& p : indexedColumn) {
        if (p.first == -1) {
            sortedColumn[countMinusOnes++] = p;
        } else if (p.first == 0) {
            sortedColumn[countZeros++] = p;
        } else { // p.first == 1
            sortedColumn[countOnes++] = p;
        }
    }

    return sortedColumn;
}


// Function to find all valid SETs in a table
vector<vector<int>> find_SETs(Table& table, bool print, bool bool_version) {

    vector<int> firstColumn = table.getColumn(0);  // Get the first column of the table
    vector<int> numCounts = count_elements(firstColumn); // Count occurrences of -1, 0, and 1 in the first column
    int numRows = firstColumn.size(); // Get the number of rows in the table

    // Create a vector of pairs, where each pair holds an element of the first column and its index
    vector<pair<int, int>> indexedColumn;
    for (int i = 0; i < numRows; ++i) {
        indexedColumn.emplace_back(firstColumn[i], i); // Store the value and its original index
    }

    // Sort the indexedColumn using counting sort, which organizes the elements as -1, 0, and 1
    indexedColumn = counting_sort(indexedColumn);

    vector<vector<int>> validSets; // Vector to store valid SET combinations

    // Nested loops to find all valid SETs
    for (int i = 0; i < numRows - 2; ++i) {
        for (int j = i + 1; j < numRows - 1; ++j) {
            int pair_sum = indexedColumn[i].first + indexedColumn[j].first; // Sum of the values of row i and row j
            int k_start = j + 1, k_end = numRows; // Default range for the third element (row k)
    
            // Determine the range for k based on the sum of row i and row j
            if (pair_sum == -2) { 
                k_start = j + 1;                  // Searching for another `-1`
                k_end = numCounts[0];             // All `-1`s end at numCounts[0]
            } 
            else if (pair_sum == 2) { 
                k_start = j + 1;                  // Searching for another `1`
                k_end = numRows;                   // All `1`s end at numRows
            } 
            else if (pair_sum == -1) { 
                k_start = numCounts[0] + numCounts[1]; // Searching for the first `1`
                k_end = numRows; // All `0`s end at numRows
            } 
            else if (pair_sum == 0) {
                // Special case: avoid combinations where i = -1 and j = 1
                if (indexedColumn[i].first == -1 && indexedColumn[j].first == 1) {
                    continue;
                }
                k_start = j + 1;                  // Any value could work for `k`
                k_end = numCounts[0] + numCounts[1]; // `k` can range over all `-1` and `0`
            }
            else if (pair_sum == 1) {
                i++;  // skips to the next row
                j = i; // Ensures that the cycle starts correctly
                continue; // Avoid to run the code for useless part
            }
    
            // Iterate over the valid range for `k` and check for valid sets
            for (int k = k_start; k < k_end; ++k) {
                vector<int> combination = {indexedColumn[i].second, indexedColumn[j].second, indexedColumn[k].second};
                add_valid_set(table, combination, validSets, false); // Add valid set if it forms one

                // Stop immediately if bool_version is true and we found a valid set
                if (bool_version && !validSets.empty()) {
                    return validSets;
                }
            }
        }
    }
    
    // Optionally print the total number of valid sets found
    if (print) cout << "Total Valid Sets Found: " << validSets.size() << endl;

    return validSets; // Return the list of valid sets
}

