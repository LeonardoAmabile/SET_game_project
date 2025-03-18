#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <unordered_set>

using namespace std;

// Hash function for vector<int> to allow unordered_set to store unique rows
class VectorHash {
public:
    size_t operator()(const vector<int>& vec) const;
};

// Table class definition
class Table {
public:
    // Constructor to initialize the table with random data
    Table(int numCards, int numAttributes);

    // Constructor to initialize the table with manually provided data
    Table(const vector<vector<int>>& manualData);

    // Method to get the value at a specific row and column
    int getValue(int row, int column) const;

    // Method to print the entire table
    void printTable() const;

    // Method to return the table as a matrix (2D vector)
    vector<vector<int>> getMatrix() const;

    // Method to print a specific row by its index
    void print_row(int rowidx) const;

    // Method to get a specific column by its index
    vector<int> getColumn(int columnIdx) const;

    // Method to get a specific row by its index
    vector<int> getRow(int rowIdx) const;
    
    // Method to set a specific row at a given index
    void setRow(int rowIdx, const vector<int>& newRow);

private:
    vector<vector<int>> tableData;  // Stores the table data
    int numRows;                   // Number of rows in the table
    int numColumns;                // Number of columns in the table
};

#endif  // TABLE_H
