#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <stdexcept>  // Include per std::invalid_argument



// Custom hash function for vector<int> to make it compatible with unordered_set (O(1) lookups, no duplicate rows)
struct VectorHash {
    std::size_t operator()(const std::vector<int>& vec) const {
        std::size_t hash = 0;
        
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
    std::vector<std::vector<int>> cardData; // 2D vector to store card data
    int numRows, numColumns; // Number of rows (cards) and columns (attributes)

public:
    // Constructor to generate a table with random, unique rows (cards)
    Table(int numCards, int numAttributes) : numRows(numCards), numColumns(numAttributes) {
        // Check if numCards is within valid bounds
        if (numCards > std::pow(3, numAttributes)) {
            throw std::invalid_argument(
                "Number of cards exceeds the maximum possible unique combinations "
                "for the hashing algorithm. (3^(n_attributes)) = " 
                + std::to_string(std::pow(3, numAttributes))
                );
}


        std::unordered_set<std::vector<int>, VectorHash> uniqueRows; // Set to store unique rows

        // Random number generator setup
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<int> distribution(-1, 1); // Random values between -1 and 1
        
        // Keep generating unique rows until we have the required number of rows
        while (uniqueRows.size() < numRows) {
            std::vector<int> newRow(numColumns); // Create a new row (card)
            
            // Fill the row with random values (-1, 0, or 1)
            for (int& value : newRow) {
                value = distribution(generator);
            }
            
            // Insert the new row into the set (duplicates are automatically rejected)
            uniqueRows.insert(newRow);
        }
        
        // Copy the unique rows from the set into the cardData vector
        cardData.assign(uniqueRows.begin(), uniqueRows.end());
    }

    // Method to get the value at a specific row and column
    int getValue(int row, int column) const {
        if (row >= 0 && row < numRows && column >= 0 && column < numColumns) {
            return cardData[row][column]; // Return the value at the specified position
        }
        throw std::out_of_range("Index out of bounds in getValue!"); // Exception if indices are out of range
    }

    // Method to print the table (2D array)
    void printTable() const {
        for (const auto& row : cardData) {
            for (int value : row) {
                std::cout << value << " "; // Print each value in the row
            }
            std::cout << "\n"; // Move to the next line after printing a row
        }
    }
    
    // Method to print a row 
    void print_row(int rowidx) const {
        if (rowidx >= 0 && rowidx < numRows) {  
            for (int value : cardData[rowidx]) {  
                std::cout << value << " ";  
            }
            std::cout << "\n";  
        } else {
            throw std::out_of_range("Indice fuori limite in print_row!"); 
        }
    }
    
    // Method to get the k-th column as an array
    std::vector<int> getColumn(int columnIdx) const {
        std::vector<int> columnData;
        for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) {
            if (columnIdx < numColumns) {
                columnData.push_back(cardData[rowIdx][columnIdx]);  // Add the k-th element of each row
            } else {
                throw std::out_of_range("Index out of bounds in getColumn!"); // Exception if column index is out of range
            }
        }
        return columnData;
    }

    // Method to get the j-th row as an array
    std::vector<int> getRow(int rowIdx) const {
        std::vector<int> rowData;
        if (rowIdx < numRows) {  // Check if the row index is valid
            for (int colIdx = 0; colIdx < numColumns; ++colIdx) {
                rowData.push_back(cardData[rowIdx][colIdx]);  // Add the c-th element of the j-th row
            }
        } else {
            throw std::out_of_range("Index out of bounds in getRow!");  // Exception if row index is out of range
        }
        return rowData;
    }
};

#endif // TABLE_H
