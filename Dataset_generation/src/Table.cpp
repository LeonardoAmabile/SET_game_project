#include "Table.h"

#include <random>
#include <unordered_set>
#include <stdexcept>
#include <iostream>

using namespace std;

// Implementation of the VectorHash function
int VectorHash::operator()(const vector<int>& vec) const {
    int hash = 0;
    int base = 1;

    for (int i = vec.size(); i > 0; --i) {
        int mapped_value = vec[i - 1] + 1; // Map -1 -> 0, 0 -> 1, 1 -> 2
        hash += mapped_value * base;
        base *= 3;
    }

    return hash; // Ensure a range from 1 to 3^N
}

// Constructor I: Initialize with number of rows and columns
Table::Table(int numCards, int numAttributes) {
    numRows = numCards;
    numColumns = numAttributes;

    int Max_cards = pow(3, numAttributes);
    if (numCards > Max_cards) {
        throw invalid_argument("Number of cards exceeds the max possible unique combinations.");
    }

    unordered_set<vector<int>, VectorHash> uniqueRows;

    random_device randomDevice;
    mt19937 generator(randomDevice());
    uniform_int_distribution<int> distribution(-1, 1);

    while (uniqueRows.size() < numRows) {
        vector<int> newRow(numColumns);
        for (int& value : newRow) {
            value = distribution(generator);
        }
        uniqueRows.insert(newRow);
    }

    tableData.assign(uniqueRows.begin(), uniqueRows.end());
}

// Constructor II: Initialize with a given matrix
Table::Table(const vector<vector<int>>& inputTableData) {
    numRows = inputTableData.size();
    numColumns = inputTableData.empty() ? 0 : inputTableData[0].size();

    // Validate that all elements are -1, 0, or 1
    for (const auto& row : inputTableData) {
        for (int value : row) {
            if (value != -1 && value != 0 && value != 1) {
                throw invalid_argument("All elements in the table must be -1, 0, or 1.");
            }
        }
    }

    tableData = inputTableData;
}

// Other method implementations...

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
        throw out_of_range("Index out of bounds in print_row!");
    }
}

vector<int> Table::getColumn(int columnIdx) const {
    if (columnIdx < 0 || columnIdx >= numColumns) {
        throw out_of_range("Index out of bounds in getColumn!");
    }

    vector<int> columnData;
    for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) {
        columnData.push_back(tableData[rowIdx][columnIdx]);
    }
    return columnData;
}

vector<int> Table::getRow(int rowIdx) const {
    if (rowIdx < 0 || rowIdx >= numRows) {
        throw out_of_range("Index out of bounds in getRow!");
    }
    return tableData[rowIdx];
}

void Table::setRow(int rowIdx, const vector<int>& newRow) {
    if (rowIdx < 0 || rowIdx >= numRows) {
        throw out_of_range("Index out of bounds in setRow!");
    }
    if (newRow.size() != numColumns) {
        throw invalid_argument("New row must have the same number of columns as the table.");
    }
    tableData[rowIdx] = newRow;
}
