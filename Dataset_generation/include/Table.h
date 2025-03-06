#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <stdexcept>  // Include per invalid_argument
#include <cstddef>

using namespace std;

// Dichiarazione della struttura VectorHash
struct VectorHash {
    size_t operator()(const vector<int>& vec) const;
};

// Dichiarazione della classe Table
class Table {
private:
    vector<vector<int>> tableData; // Table data (2D matrix)
    int numRows, numColumns; // Number of rows (cards) and columns (attributes)

public:
    // Costruttore
    Table(int numCards, int numAttributes);

    // Metodi
    int getValue(int row, int column) const;
    void printTable() const;
    vector<vector<int>> getMatrix() const;
    void print_row(int rowidx) const;
    vector<int> getColumn(int columnIdx) const;
    vector<int> getRow(int rowIdx) const;
};

#endif
