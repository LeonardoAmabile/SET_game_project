#include "Find_SETs_brute.h"
#include "Table.h"
#include "Find_SETs.h"

#include <vector>
#include <iostream>

using namespace std;

// Brute force function to find all combinations of 3 rows that satisfy the SET condition.
vector<vector<int>> brute_force_find_SETs(Table table, bool print, bool bool_version) {
    vector<int> firstColumn = table.getColumn(0);
    int numRows = firstColumn.size();
    vector<vector<int>> validSets;

    // Loop through all combinations of three rows
    for (int i = 0; i < numRows - 2; ++i) {
        for (int j = i + 1; j < numRows - 1; ++j) {
            for (int k = j + 1; k < numRows; ++k) {
                // Get the rows from the table
                vector<int> row1 = table.getRow(i);
                vector<int> row2 = table.getRow(j);
                vector<int> row3 = table.getRow(k);

                // Check if they form a SET
                if (is_valid_set(row1, row2, row3)) {
                    validSets.push_back({i, j, k});
                    
                    if (print) {
                        cout << "Brute Valid Set:\n";
                        cout << "Card " << i << ": "; table.print_row(i);
                        cout << "Card " << j << ": "; table.print_row(j);
                        cout << "Card " << k << ": "; table.print_row(k);
                        cout << endl;
                    }

                    // Stop immediately if bool_version is true and we found a valid set
                    if (bool_version) {
                        return validSets;
                    }
                }
            }
        }
    }

    // If print is true, print the total number of valid sets found
    if (print) {
        cout << "Total Valid Sets Found with brute force: " << validSets.size() << endl;
    }

    return validSets;
}


