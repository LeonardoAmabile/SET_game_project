#include "Table.h"
#include "Find_SETs.h"
#include "Write_txt.h"
#include "Find_SETs_brute.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <thread>

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

// Function to print a progress bar
void printProgressBar(int current, int total) {
    int barWidth = 50;  // Width of the progress bar
    float progress = (float)current / total;
    int pos = barWidth * progress;  // Number of '#' symbols

    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "#";  // Completed part
        else cout << "=";  // Remaining part
    }
    cout << "] " << int(progress * 100.0) << " %\r";  // \r to overwrite
    cout.flush();
}

// Function to generate tables (N_tables tables with n_cards and n_att attributes)
vector<Table> generate_Tables(int N_tables, int n_cards, int n_att) {
    vector<Table> Tables;
    Tables.reserve(N_tables);  // Preallocate space for N_tables tables
    cout << "Generating tables...\n";
    for (int i = 0; i < N_tables; ++i) {
        Tables.emplace_back(n_cards, n_att);  // Create a Table object with the given parameters
        printProgressBar(i, N_tables); // Print a progress bar for the table generation
    }
    cout << "\n";
    cout << "Tables generated!" << "\n\n";  // Indicate that the tables are generated
    return Tables;
}

// Function to run the optimized process
void run_optimized_process(vector<Table> Matrices, bool print_bool) {
    vector<int> SETs_count;
    int total = Matrices.size();

    // Extract n_cards and n_att from the first matrix (assuming all matrices have the same structure)
    int n_cards = Matrices[0].getColumn(0).size();
    int n_att = Matrices[0].getRow(0).size();

    // Loop through each table and find the SETs
    cout << "Finding SETs...\n";
    for (int i = 0; i < total; ++i) {
        Table table = Matrices[i];  // Get the current table
        vector<vector<int>> SET = find_SETs(table, false);  // Find the SETs using the optimized method
        SETs_count.push_back(SET.size());  // Store the number of SETs found
        printProgressBar(i, total); // Print a progress bar for the SET evaluation
    }
    cout << "\n";
    // Save results in a text file with the user-provided values
    write_data("Data.txt", SETs_count, Matrices, print_bool);
}

// Function to run the brute-force process
void run_brute_force_process(vector<Table> Matrices, bool print_bool) {
    vector<int> SETs_count;
    int total = Matrices.size();

    // Extract n_cards and n_att from the first matrix (assuming all matrices have the same structure)
    int n_cards = Matrices[0].getColumn(0).size();
    int n_att = Matrices[0].getRow(0).size();

    // Loop through each table and find the SETs in brute force
    cout << "Finding SETs in brute force...\n";
    for (int i = 0; i < total; ++i) {
        Table table = Matrices[i];  // Get the current table
        vector<vector<int>> SET = brute_force_find_SETs(table, false);  // Find the SETs using the brute-force method
        SETs_count.push_back(SET.size());  // Store the number of SETs found
        printProgressBar(i, total); // Print a progress bar for the SET evaluation
    }
    cout << "\n";
    // Save results in a text file with the user-provided values
    write_data("Data_brute_force.txt", SETs_count, Matrices, print_bool);
}

// Function to print the execution time for both processes
void print_execution_times(chrono::duration<double>& execution_time, const string& process_name) {
    cout << process_name << " execution time: " << execution_time.count() << " seconds\n";
}

int main() {
    int n_cards, n_att, N_tables;

    // Collect data from the user
    get_input(n_cards, n_att, N_tables);

    // Generate the tables (N_tables tables with n_cards and n_att attributes)
    vector<Table> Matrices = generate_Tables(N_tables, n_cards, n_att);

    // Measure time for the optimized process
    auto start_opt = chrono::high_resolution_clock::now();
    run_optimized_process(Matrices, true);
    auto end_opt = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_opt = end_opt - start_opt;
    print_execution_times(elapsed_opt, "Optimized");

    cout << endl << "Done!" << endl; // Indicate that the process is complete

    // Measure time for the brute-force process
    //auto start_brute = chrono::high_resolution_clock::now();
    //run_brute_force_process(Matrices, true);
    //auto end_brute = chrono::high_resolution_clock::now();
    //chrono::duration<double> elapsed_brute = end_brute - start_brute;
    

    //print_execution_times(elapsed_brute, "Brute-force");

    return 0;  // Return 0 to indicate successful execution
}
