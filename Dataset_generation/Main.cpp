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
#include <fstream>

#ifdef _WIN32  // If compiled on Windows platform
#define byte win_byte_override  // Define a macro to override 'byte' with 'win_byte_override'
#include <windows.h>  // Include the Windows API headers (for Windows-specific functionality)
#undef byte  // Undefine the 'byte' macro to avoid conflicts after including windows.h
#else  // For non-Windows platforms (e.g., Linux/macOS)
#include <getopt.h>  // Include getopt for argument parsing on Linux/macOS
#endif


using namespace std;


//Print help message
void print_help() {
    cout << "Usage: program_name [options]\n"
         << "Options:\n"
         << "  --help, -h               Show this help message\n"
         << "  --tables, -t             Show information about Table section\n"
         << "  --sets, -s               Show information about find_SETs section\n"
         << "  --brute, -b              Show information about brute_force_find_SETs section\n"
         << "  --write, -w              Show information about Write_txt.cpp Section\n"
         << "  --optimized-only, -o     Run only the optimized algorithm\n"
         << "  --brute-only, -B         Run only the brute-force algorithm\n"
         << "  --full, -f               Run the full process (default)\n"
         << endl;
}


//Logic to manage the parsing variales
void parse_arguments(int argc, char* argv[], bool& run_full, bool& show_tables, bool& show_sets, bool& show_brute, bool& show_write, bool& run_optimized_only, bool& run_brute_only) {
    #ifdef _WIN32  // If compiled on Windows, parse arguments this way
        for (int i = 1; i < argc; ++i) {  // Iterate through the command-line arguments
            string arg = argv[i];  // Store each argument as a string
            if (arg == "--help" || arg == "-h") {  // If user asks for help
                print_help();  // Show help message
                exit(0);  // Exit the program
            } else if (arg == "--tables" || arg == "-t") {  // If user asks to show information about table generation
                show_tables = true;
                run_full = false;  // Disable full execution (this option overrides it)
            } else if (arg == "--sets" || arg == "-s") {  // If user asks for info about optimized SETs search
                show_sets = true;
                run_full = false;
            } else if (arg == "--brute" || arg == "-b") {  // If user asks for info about brute-force SETs search
                show_brute = true;
                run_full = false;
            } else if (arg == "--full" || arg == "-f") {  // Fix the missing closing brace here
    	        run_full = true;
	        } else if (arg == "--write" || arg == "-w") {  
	    	show_write = true; 
	    	run_full = false;
            } else if (arg == "--optimized-only" || arg == "-o") {  // If user wants to run only the optimized algorithm
                run_optimized_only = true;
                run_full = false;
            } else if (arg == "--brute-only" || arg == "-B") {  // If user wants to run only the brute-force algorithm
                run_brute_only = true;
                run_full = false;
            } else {  // If an unknown argument is found
                cerr << "Unknown option: " << arg << "\n";  // Print error message
                print_help();  // Show help message
                exit(1);  // Exit the program
            }
        }
    #else  // If compiled on a non-Windows platform (e.g., Linux/macOS)
        struct option long_options[] = {  // Define long options for the command-line arguments
            {"help", no_argument, 0, 'h'},
            {"tables", no_argument, 0, 't'},
            {"sets", no_argument, 0, 's'},
            {"brute", no_argument, 0, 'b'},
            {"full", no_argument, 0, 'f'},
            {"write", no_argument, 0, 'w'},
            {"optimized-only", no_argument, 0, 'o'},
            {"brute-only", no_argument, 0, 'B'},
            {0, 0, 0, 0}  // End of the options array
        };
        int option_index = 0;  // Initialize the option index for getopt_long
        int c;
        while ((c = getopt_long(argc, argv, "htsbfoBw", long_options, &option_index)) != -1) {  // Parse the arguments
            switch (c) {  // Handle different options
                case 'h': print_help(); exit(0); break;  // Help option
                case 't': show_tables = true; run_full = false; break;  // Show table generation info
                case 's': show_sets = true; run_full = false; break;  // Show optimized SET search info
                case 'b': show_brute = true; run_full = false; break;  // Show brute-force SET search info
                case 'f': run_full = true; break;  // Run the full process
                case 'w': show_write = true; run_full = false; break;  // Show write_txt info
                case 'o': run_optimized_only = true; run_full = false; break;  // Run only the optimized algorithm
                case 'B': run_brute_only = true; run_full = false; break;  // Run only the brute-force algorithm
                default: print_help(); exit(1); break;  // Unknown option
            }
        }
    #endif
}
    

// Function to display detailed information about specific sections of the program
void print_file_info(const string& filename) {
    ifstream file("info/" + filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << "\n";
        }
        file.close();
    } else {
        cout << "Error: Unable to open info/" << filename << "\n";
    }
}

// Function to collect user input for the program configuration
void get_input(int& n_cards, int& n_att, int& N_tables, bool& bool_version) {
    char choice;

    cout << "Enter the number of cards: ";
    cin >> n_cards;  // Collect the number of cards

    cout << "Enter the number of attributes: ";
    cin >> n_att;  // Collect the number of attributes

    cout << "Enter the number of tables: ";
    cin >> N_tables;  // Collect the number of tables to generate

    cout << "Boolean version (Y/n): ";
    cin >> choice;  // Collect the user's choice for boolean version

    // Convert the user's response into a boolean value
    bool_version = (choice == 'Y' || choice == 'y');
}

// Function to print a progress bar during long-running operations
void printProgressBar(int current, int total) {
    int barWidth = 50;  // Width of the progress bar
    float progress = (float)current / total;  // Calculate progress as a percentage
    int pos = barWidth * progress;  // Determine how many "#" symbols to show

    cout << "[";
    // Print the progress bar
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "#";  // Show completed progress
        else cout << "=";  // Show remaining progress
    }
    cout << "] " << int(progress * 100.0) << " %\r";  // Print the percentage of progress and overwrite previous output
    cout.flush();  // Ensure the progress bar is updated on screen
}

// Function to generate tables based on user input (number of tables, cards, attributes)
vector<Table> generate_Tables(int N_tables, int n_cards, int n_att) {
    vector<Table> Tables;
    Tables.reserve(N_tables);  // Preallocate space for N_tables tables
    cout << "Generating tables...\n";
    for (int i = 0; i < N_tables; ++i) {
        Tables.emplace_back(n_cards, n_att);  // Create a new Table object with the specified number of cards and attributes
        printProgressBar(i, N_tables);  // Show progress bar as tables are generated
    }
    cout << "\nTables generated!\n\n";  // Indicate that table generation is complete
    return Tables;  // Return the generated tables
}

// Function to run the optimized process for finding SETs
void run_optimized_process(vector<Table> Matrices, bool bool_version) {
    vector<int> SETs_count;  // To store the number of SETs found in each table
    int total = Matrices.size();  // Total number of tables

    // Extract the dimensions of the first table to assume all tables are of the same size
    int n_cards = Matrices[0].getColumn(0).size();  
    int n_att = Matrices[0].getRow(0).size();

    cout << "Finding SETs...\n";
    for (int i = 0; i < total; ++i) {
        Table table = Matrices[i];  // Get the current table
        vector<vector<int>> SET = find_SETs(table, false, bool_version);  // Find the SETs using the optimized method
        SETs_count.push_back(SET.size());  // Store the number of SETs found in the current table
        printProgressBar(i, total);  // Show the progress of SET evaluation
    }
    cout << "\n";
    // Save the results (SET counts and tables) to a text file
    write_data("Data.txt", SETs_count, Matrices);
}

// Function to run the brute-force process for finding SETs
void run_brute_force_process(vector<Table> Matrices, bool bool_version) {
    vector<int> SETs_count;  // To store the number of SETs found in each table
    int total = Matrices.size();  // Total number of tables

    // Extract the dimensions of the first table to assume all tables are of the same size
    int n_cards = Matrices[0].getColumn(0).size();  
    int n_att = Matrices[0].getRow(0).size();

    cout << "Finding SETs in brute force...\n";
    for (int i = 0; i < total; ++i) {
        Table table = Matrices[i];  // Get the current table
        vector<vector<int>> SET = brute_force_find_SETs(table, false, bool_version);  // Find the SETs using the brute-force method
        SETs_count.push_back(SET.size());  // Store the number of SETs found in the current table
        printProgressBar(i, total);  // Show the progress of SET evaluation
    }
    cout << "\n";
    // Save the results (SET counts and tables) to a text file
    write_data("Data_brute_force.txt", SETs_count, Matrices);
}


int main(int argc, char* argv[]) {
    // Declare variables
    int n_cards, n_att, N_tables;
    bool bool_version;
    
    // Flags for options
    bool run_full = true;
    bool show_tables = false, show_sets = false, show_brute = false, show_write = false;
    bool run_optimized_only = false, run_brute_only = false;

    // Parse the command-line arguments
    parse_arguments(argc, argv, run_full, show_tables, show_sets, show_brute, show_write, run_optimized_only, run_brute_only);
    
    // Show relevant information if requested by the user
    if (show_tables) print_file_info("Table_info.txt");
    if (show_sets) print_file_info("find_SETs_info.txt");
    if (show_brute) print_file_info("find_SETs_brute_info.txt");
    if (show_write) print_file_info("Write_txt_info.txt");
    
    // If no option is selected, exit early
    if (!run_full && !run_optimized_only && !run_brute_only) return 0;

    // Get user input for table generation
    get_input(n_cards, n_att, N_tables, bool_version);
    
    // Generate tables based on user input
    vector<Table> Matrices = generate_Tables(N_tables, n_cards, n_att);

    // Run the optimized process if requested
    if (run_full || run_optimized_only) {
        // Start measuring time
        auto start = std::chrono::high_resolution_clock::now();

        run_optimized_process(Matrices, bool_version);

        // Calculate the time taken
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        // Print the time taken
        std::cout << "Execution time for the optimized process: " 
                  << duration.count() << " seconds." << std::endl;
    }
    
    // Run the brute-force process if requested
    if (run_full || run_brute_only) {
        // Start measuring time
        auto start = std::chrono::high_resolution_clock::now();

        run_brute_force_process(Matrices, bool_version);

        // Calculate the time taken
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        // Print the time taken
        std::cout << "Execution time for the brute-force process: " 
                  << duration.count() << " seconds." << std::endl;
    }
    
    return 0;
}

