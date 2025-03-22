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

#ifdef _WIN32
#include <windows.h>
#else
#include <getopt.h> // Per Linux/macOS
#endif

using namespace std;

void print_help() {
    cout << "Usage: program_name [options]\n"
         << "Options:\n"
         << "  --help, -h         Show this help message\n"
         << "  --tables, -t       Show information about table generation\n"
         << "  --sets, -s         Show information about optimized SETs search\n"
         << "  --brute, -b        Show information about brute-force SETs search\n"
         << "  --optimized-only, -o Run only the optimized algorithm\n"
         << "  --brute-only, -B   Run only the brute-force algorithm\n"
         << "  --full, -f         Run the full process (default)\n"
         << "  --info, -i         Show info.txt for more details\n"
         << endl;
}

void parse_arguments(int argc, char* argv[], bool& run_full, bool& show_tables, bool& show_sets, bool& show_brute, bool& show_info, bool& run_optimized_only, bool& run_brute_only) {
#ifdef _WIN32
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            print_help();
            exit(0);
        } else if (arg == "--tables" || arg == "-t") {
            show_tables = true;
            run_full = false;
        } else if (arg == "--sets" || arg == "-s") {
            show_sets = true;
            run_full = false;
        } else if (arg == "--brute" || arg == "-b") {
            show_brute = true;
            run_full = false;
        } else if (arg == "--full" || arg == "-f") {
            run_full = true;
        } else if (arg == "--info" || arg == "-i") {
            show_info = true;
            run_full = false;
        } else if (arg == "--optimized-only" || arg == "-o") {
            run_optimized_only = true;
            run_full = false;
        } else if (arg == "--brute-only" || arg == "-B") {
            run_brute_only = true;
            run_full = false;
        } else {
            cerr << "Unknown option: " << arg << "\n";
            print_help();
            exit(1);
        }
    }
#else
    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"tables", no_argument, 0, 't'},
        {"sets", no_argument, 0, 's'},
        {"brute", no_argument, 0, 'b'},
        {"full", no_argument, 0, 'f'},
        {"info", no_argument, 0, 'i'},
        {"optimized-only", no_argument, 0, 'o'},
        {"brute-only", no_argument, 0, 'B'},
        {0, 0, 0, 0}
    };
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "htsbfioB", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h': print_help(); exit(0);
            case 't': show_tables = true; run_full = false; break;
            case 's': show_sets = true; run_full = false; break;
            case 'b': show_brute = true; run_full = false; break;
            case 'f': run_full = true; break;
            case 'i': show_info = true; run_full = false; break;
            case 'o': run_optimized_only = true; run_full = false; break;
            case 'B': run_brute_only = true; run_full = false; break;
            default: print_help(); exit(1);
        }
    }
#endif
}

// Function to display detailed information about specific sections of the program
void print_info(const string& section) {
    if (section == "tables") {
        // Description about table generation
        cout << "Table generation:\n"
             << "  - Generates N tables with n_cards and n_att attributes.\n"
             << "  - Uses random values for initialization.\n"
             << "  - Shows a progress bar during generation.\n"
             << endl;
    } else if (section == "sets") {
        // Description about the optimized SET search
        cout << "Optimized SETs search:\n"
             << "  - Uses an efficient method to find SETs in each table.\n"
             << "  - Results are saved in Data.txt.\n"
             << "  - Displays a progress bar.\n"
             << endl;
    } else if (section == "brute") {
        // Description about brute-force SET search
        cout << "Brute-force SETs search:\n"
             << "  - Uses an exhaustive search to find SETs.\n"
             << "  - Results are saved in Data_brute_force.txt.\n"
             << "  - Displays a progress bar.\n"
             << endl;
    } else if (section == "info") {
        // If the "info" section is requested, read and display the content of info.txt
        ifstream file("info.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                cout << line << "\n";
            }
            file.close();
        } else {
            cout << "Error: Unable to open Info.txt\n";
        }
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

// Function to print the execution time of each process
void print_execution_times(chrono::duration<double>& execution_time, const string& process_name) {
    cout << process_name << " execution time: " << execution_time.count() << " seconds\n";
}


int main(int argc, char* argv[]) {
    int n_cards, n_att, N_tables;
    bool bool_version;
    
    bool run_full = true;
    bool show_tables = false, show_sets = false, show_brute = false, show_info = false;
    bool run_optimized_only = false, run_brute_only = false;

    parse_arguments(argc, argv, run_full, show_tables, show_sets, show_brute, show_info, run_optimized_only, run_brute_only);
    
    if (show_tables) print_info("tables");
    if (show_sets) print_info("sets");
    if (show_brute) print_info("brute");
    if (show_info) print_info("info");
    
    if (!run_full && !run_optimized_only && !run_brute_only) return 0;

    get_input(n_cards, n_att, N_tables, bool_version);
    vector<Table> Matrices = generate_Tables(N_tables, n_cards, n_att);
    
    if (run_full || run_optimized_only) {
        run_optimized_process(Matrices, bool_version);
    }
    
    if (run_full || run_brute_only) {
        run_brute_force_process(Matrices, bool_version);
    }
    
    return 0;
}
