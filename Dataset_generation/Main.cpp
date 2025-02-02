#include "Table.h"
#include "Winning_hands.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>

int main() {

        auto start = std::chrono::high_resolution_clock::now(); // Start timing

        // Create a 'Table' object
        Table table(10000, 4); 

        //search for possible winning hands
        //find_possible_SETs(table,true);
        find_SETs(table, true);
        
        auto end = std::chrono::high_resolution_clock::now(); // End timing
        
        // Calculate the duration (time taken to create the Hand object)
        std::chrono::duration<double> elapsed = end - start; 
        


        // Output the elapsed time to the console
        std::cout << "Execution time: " << elapsed.count() << " seconds\n";


    return 0; // Return 0 to indicate successful execution
}