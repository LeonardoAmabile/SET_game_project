#include "Table.h"

#include <iostream>
#include <unordered_set>
#include <random>
#include <chrono>
#include <stdexcept>  // Include per invalid_argument
#include <cstddef>

using namespace std;

// Definizione della struttura VectorHash
size_t VectorHash::operator()(const vector<int>& vec) const {
    size_t hash = 0;
    
    for (size_t i = 0; i < vec.size(); ++i) {
        int transformedValue = (vec[i] == -1) ? 2 : vec[i];
        hash = hash * 31 + transformedValue;
    }
    
    return hash;
}

// Definizione della classe Table
Table::Table(int numCards, int numAttributes) {
    numRows = numCards;
    numColumns = numAttributes;

    // Logica per la creazione della tabella, come già definito nel tuo codice
    int Max_cards = pow(3, numAttributes);
    if (numCards > Max_cards) {
        throw invalid_argument(
            "Number of cards exceeds the maximum possible unique combinations "
            "for the hashing algorithm. (3^(n_attributes)) = " 
            + to_string(Max_cards)
        );
    }

    unordered_set<vector<int>, VectorHash> uniqueRows; // Set per righe uniche

    // Generazione delle righe uniche
    random_device randomDevice;
    mt19937 generator(randomDevice());
    uniform_int_distribution<int> distribution(-1, 1); // Valori casuali tra -1 e 1

    while (uniqueRows.size() < numRows) {
        vector<int> newRow(numColumns);
        for (int& value : newRow) {
            value = distribution(generator);
        }
        uniqueRows.insert(newRow);
    }

    tableData.assign(uniqueRows.begin(), uniqueRows.end());
}

// Altri metodi della classe Table (definizioni)
int Table::getValue(int row, int column) const {
    if (row >= 0 && row < numRows && column >= 0 && column < numColumns) {
        return tableData[row][column];
    }
    throw out_of_range("Index out of bounds in getValue!");
}

void Table::printTable() const {
    for (const auto& row : tableData) {
        for (int value : row) {
            cout << value << " ";
        }
        cout << "\n";
    }
}

vector<vector<int>> Table::getMatrix() const {
    return tableData;
}

void Table::print_row(int rowidx) const {
    if (rowidx >= 0 && rowidx < numRows) {  
        for (int value : tableData[rowidx]) {  
            cout << value << " ";  
        }
        cout << "\n";  
    } else {
        throw out_of_range("Indice fuori limite in print_row!"); 
    }
}

vector<int> Table::getColumn(int columnIdx) const {
    vector<int> columnData;
    for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) {
        if (columnIdx < numColumns) {
            columnData.push_back(tableData[rowIdx][columnIdx]);
        } else {
            throw out_of_range("Index out of bounds in getColumn!");
        }
    }
    return columnData;
}

vector<int> Table::getRow(int rowIdx) const {
    vector<int> rowData;
    if (rowIdx < numRows) {
        for (int colIdx = 0; colIdx < numColumns; ++colIdx) {
            rowData.push_back(tableData[rowIdx][colIdx]);
        }
    } else {
        throw out_of_range("Index out of bounds in getRow!");
    }
    return rowData;
}
