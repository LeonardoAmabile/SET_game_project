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

// Funzione per raccogliere i dati dall'utente
void get_input(int& n_cards, int& n_att, int& N_tables) {
    cout << "Enter the number of cards: ";
    cin >> n_cards;

    cout << "Enter the number of attributes: ";
    cin >> n_att;

    cout << "Enter the number of tables: ";
    cin >> N_tables;
}

// Funzione per eseguire il test dell'ottimizzazione
chrono::duration<double> run_optimized_process(int N_tables, int n_cards, int n_att) {
    auto start = chrono::high_resolution_clock::now();

    // Save results in a txt file with the user-provided values
    write_data("Data.txt", N_tables, n_cards, n_att, true);

    auto end = chrono::high_resolution_clock::now();
    return end - start;
}

// Funzione per eseguire il test della versione brute force
chrono::duration<double> run_brute_force_process(int n_cards, int n_att) {
    auto start = chrono::high_resolution_clock::now();

    // Test with the brute force version
    Table table(n_cards, n_att);
    vector<vector<int>> brute_valid_SETs = brute_force_find_SETs(table, true);

    auto end = chrono::high_resolution_clock::now();
    return end - start;
}

// Funzione per stampare il risultato
void print_execution_times(const chrono::duration<double>& elapsed_opt, const chrono::duration<double>& elapsed_brute) {
    cout << "Execution time optimized: " << elapsed_opt.count() << " seconds\n";
    cout << "Execution time brute: " << elapsed_brute.count() << " seconds\n";
}

int main() {
    int n_cards, n_att, N_tables;

    // Raccogliere i dati dall'utente
    get_input(n_cards, n_att, N_tables);

    // Eseguire il processo ottimizzato e misurare il tempo
    auto elapsed_opt = run_optimized_process(N_tables, n_cards, n_att);

    // Eseguire il processo brute force e misurare il tempo
    auto elapsed_brute = run_brute_force_process(n_cards, n_att);

    // Stampare i tempi di esecuzione
    print_execution_times(elapsed_opt, elapsed_brute);

    return 0; // Return 0 to indicate successful execution
}
