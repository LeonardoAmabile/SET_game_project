#include "Table.h"                // Custom class for handling a matrix of integers
#include "Find_SETs.h"           // Header for optimized SET-finding logic
#include "Find_SETs_brute.h"     // Header for brute-force SET-finding logic

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>               // For assert() to test conditions
#include <set>

using namespace std;

// Test function for Table::getValue()
void testGetValue() {
    cout << "===== Testing getValue() Method =====" << endl;

    // Sample 3x3 table
    vector<vector<int>> Data = {
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, 1}
    };
    Table table(Data);

    // Check a valid value
    assert(table.getValue(1, 1) == 1);

    // Check that out-of-bounds access throws an exception
    try {
        table.getValue(5, 5);
        assert(false); // Should not reach here if exception is thrown
    } catch (const exception& e) {
        // Passes if exception is caught
    }

    cout << "getValue() passed all tests\n" << endl;
}

// Test function for Table::getRow() and Table::getColumn()
void testGetRowAndColumn() {
    cout << "===== Testing getRow() and getColumn() =====" << endl;
    vector<vector<int>> Data = {
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, 1}
    };
    Table table(Data);

    // Expected values for validation
    vector<int> row1 = {0, 1, -1};
    vector<int> col1 = {1, 0, -1};

    // Check values match
    assert(table.getRow(1) == row1);
    assert(table.getColumn(0) == col1);

    cout << "getRow() and getColumn() passed all tests\n" << endl;
}

// Test function for Table::setRow()
void testSetRow() {
    cout << "===== Testing setRow() Method =====" << endl;
    vector<vector<int>> Data = {
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, 1}
    };
    Table table(Data);

    vector<int> newRow = {1, -1, 0};
    table.setRow(2, newRow); // Replace row 2

    assert(table.getRow(2) == newRow);

    // Out-of-bounds row index
    try {
        table.setRow(5, newRow);
        assert(false); // Should throw
    } catch (const exception& e) {}

    // Incorrect row size
    try {
        vector<int> wrongRow = {1, 0}; // Only 2 elements instead of 3
        table.setRow(1, wrongRow);
        assert(false); // Should throw
    } catch (const exception& e) {}

    cout << "setRow() passed all tests\n" << endl;
}

// Test function for validating if three rows form a valid SET
void testIsValidSet() {
    cout << "===== Testing is_valid_set() =====" << endl;

    // Valid set example
    vector<int> row1 = {1, 0, -1};
    vector<int> row2 = {0, 1, -1};
    vector<int> row3 = {-1, -1, -1};
    assert(is_valid_set(row1, row2, row3));

    // Invalid set
    vector<int> row4 = {1, 1, 1};
    assert(!is_valid_set(row1, row2, row4));

    cout << "is_valid_set() tests passed" << endl;
}

// Test function for count_elements() utility
void testCountElements() {
    cout << "===== Testing count_elements() =====" << endl;

    vector<int> input = {-1, 0, 1, -1, 0, 1};
    vector<int> counts = count_elements(input);

    // Expected counts for -1, 0, 1
    assert(counts[0] == 2);
    assert(counts[1] == 2);
    assert(counts[2] == 2);

    cout << "count_elements() tests passed" << endl;
}

// Test function for counting_sort()
void test_counting_sort() {
    // Test case 1: Balanced sequence
    vector<pair<int, int>> input1 = {{-1, 0}, {0, 1}, {1, 2}, {-1, 3}, {0, 4}, {1, 5}};
    vector<pair<int, int>> expected1 = {{-1, 0}, {-1, 3}, {0, 1}, {0, 4}, {1, 2}, {1, 5}};
    assert(counting_sort(input1) == expected1);

    // Test case 2: Only -1
    vector<pair<int, int>> input2 = {{-1, 0}, {-1, 1}, {-1, 2}};
    vector<pair<int, int>> expected2 = {{-1, 0}, {-1, 1}, {-1, 2}};
    assert(counting_sort(input2) == expected2);

    // Test case 3: Only 0
    vector<pair<int, int>> input3 = {{0, 0}, {0, 1}, {0, 2}};
    vector<pair<int, int>> expected3 = {{0, 0}, {0, 1}, {0, 2}};
    assert(counting_sort(input3) == expected3);

    // Test case 4: Only 1
    vector<pair<int, int>> input4 = {{1, 0}, {1, 1}, {1, 2}};
    vector<pair<int, int>> expected4 = {{1, 0}, {1, 1}, {1, 2}};
    assert(counting_sort(input4) == expected4);

    // Test case 5: Mixed values
    vector<pair<int, int>> input5 = {{1, 0}, {-1, 1}, {0, 2}, {1, 3}, {0, 4}, {-1, 5}};
    vector<pair<int, int>> expected5 = {{-1, 1}, {-1, 5}, {0, 2}, {0, 4}, {1, 0}, {1, 3}};
    assert(counting_sort(input5) == expected5);
}

// Test optimized version of find_SETs()
void testFindSETs() {
    cout << "===== Testing find_SETs() =====" << endl;

    // Sample input
    Table table({
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, -1},
        {1, 0, 1}
    });

    // Call find_SETs (optimized version)
    vector<vector<int>> validSets = find_SETs(table, false);

    // Expect 1 valid set
    assert(validSets.size() == 1);

    cout << "find_SETs() tests passed" << endl;
}

// Test brute-force version of find_SETs()
void testFindSETsBrute() {
    cout << "===== Testing brute_force_find_SETs() =====" << endl;

    Table table({
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, -1},
        {1, 0, 1}
    });

    // Brute-force find
    vector<vector<int>> validSets = brute_force_find_SETs(table, false);

    assert(validSets.size() == 1);

    cout << "brute_force_find_SETs() tests passed" << endl;
}

// Main function that runs all test cases
int main() {
    // Table class tests
    testGetValue();
    cout << endl;
    testGetRowAndColumn();
    cout << endl;
    testSetRow();
    cout << endl << endl;

    // Utility and SET-finding tests
    testIsValidSet();
    cout << endl;
    testCountElements();
    cout << endl;
    test_counting_sort();
    cout << endl;
    testFindSETs();
    cout << endl << endl;

    // Brute-force SET finder test
    testFindSETsBrute();

    return 0;
}
