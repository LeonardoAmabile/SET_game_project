#ifndef WRITE_TXT_H
#define WRITE_TXT_H

#include "Table.h"
#include "Find_SETs.h"


#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

//Function to write data in a txt file
void write_data(const string& filename, const vector<int>& SETs_count, const vector<Table>& matrices) {
    ofstream file(filename);

    if (!file) {
        cerr << "Error opening file " << filename << "!" << endl;
        return;
    }

    // Write description of the first row
    file << "# Number of SETs in each Table" << "\n\n";

    // Write the number of SETs for each matrix in the first row from SETs_count
    for (const auto& count : SETs_count) {
        file << count << " ";  // Each count corresponds to a table
    }
    file << "\n\n";  // New line after the number of SETs

    // Write description of the Tables:
    file << "# Tables considered" << "\n\n";
    
    // Write all matrices with a blank line between them
    for (const auto& matrix : matrices) {
        // Write each matrix row by row
        for (const auto& row : matrix.getMatrix()) {
            for (int num : row) {
                file << num << " ";  // Print each element in the row
            }
            file << "\n";  // New line after each row
        }
        file << "\n";  // Blank line between matrices
    }

    file.close();
    cout << "Results saved to " << filename << endl;
}

#endif // WRITE_TXT_H






