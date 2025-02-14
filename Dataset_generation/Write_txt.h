#ifndef WRITE_TXT_H
#define WRITE_TXT_H

#include "Table.h"
#include "Find_SETs.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

using namespace std;

// Function to write data in a txt file
void write_data(const string& filename, const vector<int>& SETs_count, const vector<Table>& matrices) {
    ofstream file(filename);

    if (!file) {
        cerr << "Error opening file " << filename << "!" << endl;
        return;
    }

    // Write description of the first row
    file << "# Number of SETs in each Table" << "\n\n";

    // Write the number of SETs for each matrix in the first row from SETs_count
    vector<int> Zeros_count;
    for (const auto& count : SETs_count) {
        file << count << " ";  // Each count corresponds to a table
        if (count == 0) {
            Zeros_count.push_back(count);
        }
    }
    file << "\n\n";  // New line after the number of SETs

    // Compute average number of SETs
    double sum = 0.0;
        for (const auto& count : SETs_count) {
            sum += count;
        }
        
double average_SETs = SETs_count.empty() ? 0 : sum / SETs_count.size();

    file << "# Number of Tables with 0 SETs: " << Zeros_count.size() << "/" << SETs_count.size() << "\n\n";
    file << "# Probability of no SETs: " << (double)100 * Zeros_count.size() / SETs_count.size() << "% \n\n";
    file << "# Average number of SETs: " << average_SETs << "\n\n";
    
    file << "# Tables considered" << "\n\n";
    
    // Write all matrices with a blank line between them
    for (const auto& matrix : matrices) {
        for (const auto& row : matrix.getMatrix()) {
            for (int num : row) {
                file << num << " ";
            }
            file << "\n";
        }
        file << "\n";  // Blank line between matrices
    }

    file.close();
    cout << "Results saved to " << filename << endl;
}

#endif // WRITE_TXT_H
