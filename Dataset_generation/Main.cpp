#include "Table.h"
#include "Find_SETs.h"
#include "Write_txt.h"

#include "Find_SETs_brute.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>

using namespace std;

int main() {
    // Prompt the user for the number of cards, attributes, and tables
    int n_cards, n_att, N_tables;

    cout << "Enter the number of cards: ";
    cin >> n_cards;

    cout << "Enter the number of attributes: ";
    cin >> n_att;

    cout << "Enter the number of tables: ";
    cin >> N_tables;

    auto start = chrono::high_resolution_clock::now(); // Start timing the optimized version

    // Save results in a txt file with the user-provided values
    write_data("Data.txt", N_tables, n_cards, n_att);

    auto end = chrono::high_resolution_clock::now(); // End timing the optimized version

    // Calculate the duration (time taken for the optimized process)
    chrono::duration<double> elapsed_opt = end - start;

    auto start_brute = chrono::high_resolution_clock::now(); // Start timing the brute force version

    // Test with the brute force version
    Table table(n_cards, n_att);
    vector<vector<int>> brute_valid_SETs = brute_force_find_SETs(table, true);

    auto end_brute = chrono::high_resolution_clock::now(); // End timing the brute force version

    // Calculate the duration (time taken for the brute force process)
    chrono::duration<double> elapsed_brute = end_brute - start_brute;

    // Output the elapsed time to the console
    cout << "Execution time optimized: " << elapsed_opt.count() << " seconds\n";
    cout << "Execution time brute: " << elapsed_brute.count() << " seconds\n";

    return 0; // Return 0 to indicate successful execution
}
