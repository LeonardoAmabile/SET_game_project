#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <stdexcept>  // Include per invalid_argument

using namespace std;

// Custom hash function for vector<int> to make it compatible with unordered_set (O(1) lookups, no duplicate rows)
struct VectorHash {
    size_t operator()(const vector<int>& vec) const {
        size_t hash = 0;
        
        // Loop through each element in the vector and update the hash value
        for (size_t i = 0; i < vec.size(); ++i) {
            // Transform -1 to 2 to avoid any issues with hashing negative numbers
            int transformedValue = (vec[i] == -1) ? 2 : vec[i];
            
            // Update the hash using multiplication and addition
            hash = hash * 31 + transformedValue;
        }
        
        return hash; // Return the computed hash value
    }
};


// Class representing a table of cards (2D array)
class Table {
private:
    vector<vector<int>> tableData; // 2D vector to store card data
    int numRows, numColumns; // Number of rows (cards) and columns (attributes)

public:
    // Constructor to generate a table. Without the constructor we have to insert manually the rows when we define an instance
    //of Table, and this is inefficient. So we use the constructor to define an instance of table by using only the numCards and numAttributes. 
    Table(int numCards, int numAttributes)  {

        numRows= numCards;
        numColumns=numAttributes;

        // Check if numCards is within valid bounds (for the hash table)
        if (numCards > pow(3, numAttributes)) {
            throw invalid_argument(
                "Number of cards exceeds the maximum possible unique combinations "
                "for the hashing algorithm. (3^(n_attributes)) = " 
                + to_string(pow(3, numAttributes))
                );
}


        unordered_set<vector<int>, VectorHash> uniqueRows; // Set to store unique rows

        // Random number generator setup
        mt19937 generator(31);
        uniform_int_distribution<int> distribution(-1, 1); // Random values between -1 and 1
        
        // Keep generating unique rows until we have the required number of rows
        while (uniqueRows.size() < numRows) {
            vector<int> newRow(numColumns); // Create a new row (card)
            
            // Fill the row with random values (-1, 0, or 1)
            for (int& value : newRow) {
                value = distribution(generator);
            }
            
            // Insert the new row into the set (duplicates are automatically rejected)
            uniqueRows.insert(newRow);
        }
        
        // Copy the unique rows from the set into the tableData vector
        tableData.assign(uniqueRows.begin(), uniqueRows.end());
    }

    // Method to get the value at a specific row and column
    int getValue(int row, int column) const {
        if (row >= 0 && row < numRows && column >= 0 && column < numColumns) {
            return tableData[row][column]; // Return the value at the specified position
        }
        throw out_of_range("Index out of bounds in getValue!"); // Exception if indices are out of range
    }

    // Method to print the table (2D array)
    void printTable() const {
        for (const auto& row : tableData) {
            for (int value : row) {
                cout << value << " "; // Print each value in the row
            }
            cout << "\n"; // Move to the next line after printing a row
        }
    }
    
    // Method to print a row 
    void print_row(int rowidx) const {
        if (rowidx >= 0 && rowidx < numRows) {  
            for (int value : tableData[rowidx]) {  
                cout << value << " ";  
            }
            cout << "\n";  
        } else {
            throw out_of_range("Indice fuori limite in print_row!"); 
        }
    }
    
    // Method to get the a column as an array
    vector<int> getColumn(int columnIdx) const {
        vector<int> columnData;
        for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) {
            if (columnIdx < numColumns) {
                columnData.push_back(tableData[rowIdx][columnIdx]);  // Add the k-th element of each row
            } else {
                throw out_of_range("Index out of bounds in getColumn!"); // Exception if column index is out of range
            }
        }
        return columnData;
    }

    // Method to get a row as an array
    vector<int> getRow(int rowIdx) const {
        vector<int> rowData;
        if (rowIdx < numRows) {  // Check if the row index is valid
            for (int colIdx = 0; colIdx < numColumns; ++colIdx) {
                rowData.push_back(tableData[rowIdx][colIdx]);  // Add the c-th element of the j-th row
            }
        } else {
            throw out_of_range("Index out of bounds in getRow!");  // Exception if row index is out of range
        }
        return rowData;
    }
};

#endif // TABLE_H
