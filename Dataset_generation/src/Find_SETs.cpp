#include "Table.h"  
#include <vector>    
#include <algorithm> 
#include <iostream> 

using namespace std; 

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

//Sorts the rows of a table by the first element of each row
void sort_table_by_first_element(Table& table) {
    int numRows = table.getColumn(0).size();  // Numero di righe nella tabella
    vector<vector<int>> sortedTable(numRows); // Nuova matrice per memorizzare le righe ordinate
    
    vector<int> counts = count_elements(table.getColumn(0));

    // Determiniamo gli indici di inserimento per -1, 0 e 1
    int indexNeg = 0;
    int indexZero = counts[0];
    int indexPos = counts[0] + counts[1];

    // Riordiniamo le righe nella nuova matrice
    for (int i = 0; i < numRows; ++i) {
        int firstElement = table.getRow(i)[0];
        if (firstElement == -1) sortedTable[indexNeg++] = table.getRow(i);
        else if (firstElement == 0) sortedTable[indexZero++] = table.getRow(i);
        else sortedTable[indexPos++] = table.getRow(i);
    }

    // Copiamo il contenuto della matrice ordinata nella tabella originale
    for (int i = 0; i < numRows; ++i) {
        table.setRow(i, sortedTable[i]);
    }
}


vector<vector<int>> find_SETs(Table& table, bool print = false) {
    // Ordina la tabella basandosi solo sui primi elementi delle righe
    sort_table_by_first_element(table); 
    
    vector<vector<int>> validSets; // Vector per memorizzare i SET validi
    int numRows = table.getMatrix().size(); // Numero di righe nella tabella
    vector<int> numCounts = count_elements(table.getColumn(0));

    // Nested loops to find all valid SETs
    for (int i = 0; i < numRows - 2; ++i) {
        for (int j = i + 1; j < numRows - 1; ++j) {
            int pair_sum = table.getColumn(0)[i] + table.getColumn(0)[j]; // Sum of the values of row i and row j
            int k_start;
            int k_end; // Default range for the third element (row k)
            
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
                if (table.getColumn(0)[i] == -1 && table.getColumn(0)[j] == 1) {
                    continue;
                }

                k_start = j + 1;                  // Any value could work for `k`
                k_end = numRows; // `k` can range over all `-1` and `0`
            }

            else if (pair_sum == 1) {
                i++;  // skips to the next row
                j = i; // Ensures that the cycle starts correctly
                continue; // Avoid to run the code for useless part
            }
            
            
            // Trova combinazioni valide
            for (int k = k_start; k < k_end; ++k) {
                if (is_valid_set(table.getRow(i), table.getRow(j), table.getRow(k))){

                    vector<int> combination = {
                        VectorHash()(table.getRow(i)), // Usa l'hash della riga invece del numero di riga
                        VectorHash()(table.getRow(j)),
                        VectorHash()(table.getRow(k))
                    };

                    validSets.push_back(combination);

                    // If print flag is true, print the valid set
                    if (print) {
                        cout << "\nValid Set:\n";
                        for (int card : combination) { // Loop through each row index in the combination
                            cout << "Card " << card << ":\n";
                        }
                    }
                }
            }
        }
    }
    
    if (print) cout << "Total Valid Sets Found: " << validSets.size() << endl;

    return validSets;
}

