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

// Function to collect data from the user
void get_input(int& n_cards, int& n_att, int& N_tables) {
    cout << "Enter the number of cards: ";
    cin >> n_cards;

    cout << "Enter the number of attributes: ";
    cin >> n_att;

    cout << "Enter the number of tables: ";
    cin >> N_tables;
}

// Function to generate tables (N_tables tables with n_cards and n_att attributes)
vector<Table> generate_Tables(int N_tables, int n_cards, int n_att) {
    vector<Table> Tables;
    Tables.reserve(N_tables);  // Preallocate space for N_tables tables

    for (int i = 0; i < N_tables; ++i) {
        Tables.emplace_back(n_cards, n_att);  // Create a Table object with the given parameters
    }

    return Tables;
}

// Function to run the optimized process and measure the execution time
chrono::duration<double> run_optimized_process(vector<Table> Matrices) {
    auto start = chrono::high_resolution_clock::now();

    vector<int> SETs_count;

    // Loop through each table and find the SETs
    for (Table table : Matrices) {
        vector<vector<int>> SET = find_SETs(table, true);  // Find the SETs using the optimized method
        SETs_count.push_back(SET.size());  // Store the number of SETs found
    }

    // Save results in a text file with the user-provided values
    write_data("Data.txt", SETs_count, Matrices);

    auto end = chrono::high_resolution_clock::now();
    return end - start;  // Return the duration of the execution
}

// Function to run the brute-force process and measure the execution time
chrono::duration<double> run_brute_force_process(vector<Table> Matrices) {
    auto start = chrono::high_resolution_clock::now();

    vector<int> SETs_count;

    // Loop through each table and find the SETs using brute force
    for (const Table& table : Matrices) {
        vector<vector<int>> brute_valid_SETs = brute_force_find_SETs(table, true);  // Find SETs using brute force
        SETs_count.push_back(brute_valid_SETs.size());  // Store the number of SETs found
    }

    // Save the brute-force results in a text file
    write_data("Brute_Force_Data.txt", SETs_count, Matrices);

    auto end = chrono::high_resolution_clock::now();
    return end - start;  // Return the duration of the execution
}


// Function to print the execution times for both processes
void print_execution_times(const chrono::duration<double>& execution_time) {
    cout << "Execution time: " << execution_time.count() << " seconds\n";

}

int main() {
    int n_cards, n_att, N_tables;

    // Collect data from the user
    get_input(n_cards, n_att, N_tables);

    // Generate the tables (N_tables tables with n_cards and n_att attributes)
    vector<Table> Matrices = generate_Tables(N_tables, n_cards, n_att);

    // Run the optimized process and measure the time
    auto elapsed_opt = run_optimized_process(Matrices);

    // Run the brute-force process and measure the time
    //auto elapsed_brute = run_brute_force_process(Matrices);

    // Print the execution times
    print_execution_times(elapsed_opt);
    //print_execution_times(elapsed_brute);

    return 0;  // Return 0 to indicate successful execution
}
