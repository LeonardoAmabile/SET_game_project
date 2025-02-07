#include "Table.h"
#include "Find_SETs.h"
#include "Write_txt.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>

using namespace std;

int main() {


    auto start = chrono::high_resolution_clock::now(); // Start timing

        //Definition of the number of cards, attributes and tables
        int n_cards = 100;
        int n_att = 5;
        int N_tables = 100;
        //Save results in a txt file
        write_data("Data.txt", N_tables, n_cards, n_att);

        auto end = chrono::high_resolution_clock::now(); // End timing
        
    // Calculate the duration (time taken to create the Hand object)
    chrono::duration<double> elapsed = end - start; 

    // Output the elapsed time to the console
    cout << "Execution time: " << elapsed.count() << " seconds\n";


    return 0; // Return 0 to indicate successful execution
}