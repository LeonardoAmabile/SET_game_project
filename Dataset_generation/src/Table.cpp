#include "Table.h"

#include <iostream>
#include <unordered_set>
#include <random>
#include <chrono>
#include <stdexcept>  // Include for invalid_argument exception handling
#include <cstddef>

using namespace std;

// Definition of the VectorHash structure
size_t VectorHash::operator()(const vector<int>& vec) const {
    size_t hash = 0;
    
    // Loop through each element in the vector and transform it if it's -1
    for (size_t i = 0; i < vec.size(); ++i) {
        int transformedValue = (vec[i] == -1) ? 2 : vec[i];  // Transform -1 to 2
        hash = hash * 31 + transformedValue;  // Hashing logic
    }
    
    return hash;
}

// Definition of the Table class constructor
Table::Table(int numCards, int numAttributes) {
    numRows = numCards;
    numColumns = numAttributes;

    // Calculate the maximum possible unique combinations for the table
    int Max_cards = pow(3, numAttributes);
    if (numCards > Max_cards) {
        throw invalid_argument(
            "Number of cards exceeds the maximum possible unique combinations "
            "for the hashing algorithm. (3^(n_attributes)) = " 
            + to_string(Max_cards)
        );
    }

    unordered_set<vector<int>, VectorHash> uniqueRows; // Set to store unique rows

    // Logic to generate unique rows for the table
    random_device randomDevice;
    mt19937 generator(randomDevice());
    uniform_int_distribution<int> distribution(-1, 1); // Random values between -1 and 1

    while (uniqueRows.size() < numRows) {
        vector<int> newRow(numColumns);
        for (int& value : newRow) {
            value = distribution(generator);  // Assign random values to the row
        }
        uniqueRows.insert(newRow);  // Add the new row to the set
    }

    // Convert the set of unique rows into the table data
    tableData.assign(uniqueRows.begin(), uniqueRows.end());
}

// Other methods of the Table class (definitions)

// Method to get the value at a specific row and column
int Table::getValue(int row, int column) const {
    if (row >= 0 && row < numRows && column >= 0 && column < numColumns) {
        return tableData[row][column];  // Return the value if indices are valid
    }
    throw out_of_range("Index out of bounds in getValue!");  // Exception for invalid indices
}

// Method to print the entire table
void Table::printTable() const {
    for (const auto& row : tableData) {
        for (int value : row) {
            cout << value << " ";  // Print each value in the row
        }
        cout << "\n";  // New line after each row
    }
}

// Method to return the table as a matrix (2D vector)
vector<vector<int>> Table::getMatrix() const {
    return tableData;  // Return the table data
}

// Method to print a specific row by its index
void Table::print_row(int rowidx) const {
    if (rowidx >= 0 && rowidx < numRows) {  // Check if the row index is valid
        for (int value : tableData[rowidx]) {  
            cout << value << " ";  // Print each value in the specified row
        }
        cout << "\n";  // New line after the row
    } else {
        throw out_of_range("Index out of bounds in print_row!");  // Exception for invalid index
    }
}

// Method to get a specific column by its index
vector<int> Table::getColumn(int columnIdx) const {
    vector<int> columnData;
    for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) {
        if (columnIdx < numColumns) {
            columnData.push_back(tableData[rowIdx][columnIdx]);  // Collect values from the specified column
        } else {
            throw out_of_range("Index out of bounds in getColumn!");  // Exception for invalid column index
        }
    }
    return columnData;  // Return the collected column data
}

// Method to get a specific row by its index
vector<int> Table::getRow(int rowIdx) const {
    vector<int> rowData;
    if (rowIdx < numRows) {  // Check if the row index is valid
        for (int colIdx = 0; colIdx < numColumns; ++colIdx) {
            rowData.push_back(tableData[rowIdx][colIdx]);  // Collect values from the specified row
        }
    } else {
        throw out_of_range("Index out of bounds in getRow!");  // Exception for invalid row index
    }
    return rowData;  // Return the collected row data
}
