#ifndef WRITE_TXT_H
#define WRITE_TXT_H

#include "Table.h"
#include "Find_SETs.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

//Generates N_tables Tables of dimensionality n_cards X n_att and writes in a txt file the number of winning SETs for each Table and the Table considered
void write_data(const string& filename, int N_tables, int n_cards, int n_att) {
    ofstream file(filename);

    if (!file) {
        cerr << "Error opening file " << filename << "!" << endl;
        return;
    }

    vector<int> validSetsCounts;  // Store the number of winning sets for each matrix
    vector<vector<vector<int>>> matrices; // Store the matrices

    // Generate and analyze matrices
    for (int t = 0; t < N_tables; ++t) {
        Table table(n_cards, n_att);  // Create a random table with n_cards x n_att

        // Find valid sets
        vector<vector<int>> validSets = find_SETs(table);
        validSetsCounts.push_back(validSets.size()); // Store the number of valid sets

        // Store the matrix for later writing
        vector<vector<int>> matrix;
        for (int i = 0; i < n_cards; ++i) {
            matrix.push_back(table.getRow(i));
        }
        matrices.push_back(matrix);
    }

    //Write description of the first row
    file<< "# Number of SETs in each Table"<<"\n\n";

    // Write the number of valid sets for each matrix in the first row
    for (int count : validSetsCounts) {
        file << count << " ";
    }
    file << "\n\n";  // New line after the number of valid sets
    
    //Write description of the Tables:
    file<< "# Tables considered"<<"\n\n";
    // Write all matrices with a blank line between them
    for (const auto& matrix : matrices) {
        for (const auto& row : matrix) {
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

#endif
