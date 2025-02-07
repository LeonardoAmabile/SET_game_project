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

//First attempts with "Table.h" file and "Winning_hands.h" file 
/*
        // Create a 'Table' object
        Table table(50, 4); 

        //search for possible winning hands (only the sum of the first attribute is -3, 0, 3)
        find_possible_SETs(table,true);

        //search winning hands
        find_SETs(table, true);

*/ 

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