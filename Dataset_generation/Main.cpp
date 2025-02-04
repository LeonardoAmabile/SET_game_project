#include "Table.h"
#include "Winning_hands.h"
#include "Write_txt.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>

int main() {

//First attempts with "Table.h" file and "Winning_hands.h" file 
/*
        // Create a 'Table' object
        Table table(50, 4); 

        //search for possible winning hands (only the sum of the first attribute is -3, 0, 3)
        find_possible_SETs(table,true);

        //search winning hands
        find_SETs(table, true);

*/ 

    auto start = std::chrono::high_resolution_clock::now(); // Start timing

        //Definition of the number of cards, attributes and tables
        int n_cards = 200;
        int n_att = 7;
        int N_tables = 1000;
        //Save results in a txt file
        save_results_to_file("Results.txt", N_tables, n_cards, n_att);

        auto end = std::chrono::high_resolution_clock::now(); // End timing
        
    // Calculate the duration (time taken to create the Hand object)
    std::chrono::duration<double> elapsed = end - start; 

    // Output the elapsed time to the console
    std::cout << "Execution time: " << elapsed.count() << " seconds\n";


    return 0; // Return 0 to indicate successful execution
}