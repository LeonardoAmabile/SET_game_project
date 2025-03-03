#include "Table.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>  // Include assert()

using namespace std;

void testGetValue() {
    cout << "===== Testing getValue() Method =====" << endl;

    vector<vector<int>> Data = {
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, 1}
    };
    Table table(Data);

    assert(table.getValue(1, 1) == 1);

    // Test out-of-bounds access (should throw an exception)
    try {
        table.getValue(5, 5);
        assert(false);  // If no exception is thrown, fail the test
    } catch (const exception& e) {
        // Exception correctly caught, test passes
    }

    cout << "✅ getValue() passed all tests\n" << endl;
}

void testGetRowAndColumn() {
    cout << "===== Testing getRow() and getColumn() =====" << endl;
    vector<vector<int>> Data = {
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, 1}
    };
    Table table(Data);

    vector<int> row1 = {0, 1, -1};
    vector<int> col1 = {1, 0, -1};

    assert(table.getRow(1) == row1);
    assert(table.getColumn(0) == col1);

    cout << "✅ getRow() and getColumn() passed all tests\n" << endl;
}

void testSetRow() {
    cout << "===== Testing setRow() Method =====" << endl;
    vector<vector<int>> Data = {
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, 1}
    };
    Table table(Data);

    vector<int> newRow = {1, -1, 0};
    table.setRow(2, newRow);

    assert(table.getRow(2) == newRow);

    // Test out-of-bounds row (should throw an exception)
    try {
        table.setRow(5, newRow);
        assert(false);  // If no exception is thrown, fail the test
    } catch (const exception& e) {
        // Exception correctly caught, test passes
    }

    // Test wrong column size (should throw an exception)
    try {
        vector<int> wrongRow = {1, 0};  // Only 2 elements, should be 3
        table.setRow(1, wrongRow);
        assert(false);
    } catch (const exception& e) {
        // Exception correctly caught, test passes
    }

    cout << "✅ setRow() passed all tests\n" << endl;
}

int main() {
    testGetValue();
    testGetRowAndColumn();
    testSetRow();

    cout << "✅ All tests completed successfully!" << endl;
    return 0;
}
