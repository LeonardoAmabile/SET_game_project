#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

using namespace std;

// Custom hash function for a vector of integers
struct VectorHash {
    int operator()(const vector<int>& vec) const;
};

class Table {
private:
    int numRows;
    int numColumns;
    vector<vector<int>> tableData;

public:
    // Constructor I: Initialize with number of rows and columns
    Table(int numCards, int numAttributes);

    // Constructor II: Initialize with a vector of vectors
    Table(const vector<vector<int>>& inputTableData);

    // Method to get a specific value from the table
    int getValue(int row, int column) const;

    // Method to print the entire table
    void printTable() const;

    // Method to return the table as a matrix
    vector<vector<int>> getMatrix() const;

    // Method to print a specific row
    void print_row(int rowidx) const;

    // Method to get a specific column
    vector<int> getColumn(int columnIdx) const;

    // Method to get a specific row
    vector<int> getRow(int rowIdx) const;

    // Method to set a specified row
    void setRow(int rowIdx, const vector<int>& newRow);
};

#endif // TABLE_H
