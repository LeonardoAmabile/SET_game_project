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

// Function to run the optimized process and measure the execution time
chrono::duration<double> run_optimized_process(vector<Table> Matrices) {
    auto start = chrono::high_resolution_clock::now();

    vector<int> SETs_count;
    int total = Matrices.size();

    // Loop through each table and find the SETs
    cout << "Finding SETs...\n";
    for (int i=0; i < total; ++i) {
        Table table = Matrices[i];  // Get the current table
        vector<vector<int>> SET = find_SETs(table, false);  // Find the SETs using the optimized method
        SETs_count.push_back(SET.size());  // Store the number of SETs found
        printProgressBar(i, total); // Print a progress bar for the SET evalutation
    }
    cout << "\n";
    // Save results in a text file with the user-provided values
    write_data("Data.txt", SETs_count, Matrices);

    auto end = chrono::high_resolution_clock::now();
    return end - start;  // Return the duration of the execution
}

// Function to run the brute-force process and measure the execution time
chrono::duration<double> run_brute_force_process(vector<Table> Matrices) {
    auto start = chrono::high_resolution_clock::now();

    vector<int> SETs_count;
    int total = Matrices.size();

    // Loop through each table and find the SETs
    cout << "Finding SETs in brute force...\n";
    for (int i=0; i < total; ++i) {
        Table table = Matrices[i];  // Get the current table
        vector<vector<int>> SET = brute_force_find_SETs(table, false);  // Find the SETs using the optimized method
        SETs_count.push_back(SET.size());  // Store the number of SETs found
        printProgressBar(i, total); // Print a progress bar for the SET evalutation
    }
    cout << "\n";
    // Save results in a text file with the user-provided values
    write_data("Data.txt", SETs_count, Matrices);

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

    cout << std::endl << "Done!" << std::endl; // Indicate that the process is complete

    // Run the brute-force process and measure the time
    auto elapsed_brute = run_brute_force_process(Matrices);

    // Print the execution times
    print_execution_times(elapsed_opt);
    print_execution_times(elapsed_brute);

    return 0;  // Return 0 to indicate successful execution
}
