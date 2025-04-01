#include "Write_txt.h"
#include "Table.h"
#include "Find_SETs.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

using namespace std;

// Function to write data in a txt file
void write_data(const string& filename, 
    const vector<int>& SETs_count, 
    const vector<Table>& matrices) {
    
        ofstream file(filename);

    if (!file) {
        cerr << "Error opening file " << filename << "!" << endl;
        return;
    }

    // Extract parameters from matrices
    int N_tables = matrices.size();
    int n_cards = N_tables > 0 ? matrices[0].getColumn(0).size() : 0;
    int n_att = (n_cards > 0) ? matrices[0].getRow(0).size() : 0;

    // Write the parameters
    file << "# Parameters:" << "\n";
    file << "Number of cards: " << n_cards << "\n";
    file << "Number of attributes: " << n_att << "\n";
    file << "Number of tables: " << N_tables << "\n\n";

    // Write the number of SETs for each matrix in the first row from SETs_count
    vector<int> Zeros_count;
    for (const auto& count : SETs_count) {
        if (count == 0) {
            Zeros_count.push_back(count);
        }
    }

    file << "# Number of SETs in each Table\n\n";
    for (const auto& count : SETs_count) {
        file <<  count << " ";
    }
    
    file << "\n\n";

    // Compute average number of SETs
    double sum = accumulate(SETs_count.begin(), SETs_count.end(), 0.0);
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
