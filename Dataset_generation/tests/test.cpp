#include "Table.h"
#include "Find_SETs.h"
#include "Find_SETs_brute.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>  // Include assert()
#include <set>

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


// Functionto verify if a SET is valid or no
void testIsValidSet() {
    cout << "===== Testing is_valid_set() =====" << endl;

    // Test valid set
    vector<int> row1 = {1, 0, -1};
    vector<int> row2 = {0, 1, -1};
    vector<int> row3 = {-1, -1, -1};
    assert(is_valid_set(row1, row2, row3) && "Test failed: Valid set should return true");

    // Test invalid set
    vector<int> row4 = {1, 1, 1};
    assert(!is_valid_set(row1, row2, row4) && "Test failed: Invalid set should return false");

    cout << "✅ is_valid_set() tests passed" << endl;
}

// Function to test the COuntElements function
void testCountElements() {
    cout << "===== Testing count_elements() =====" << endl;

    vector<int> input = {-1, 0, 1, -1, 0, 1};
    vector<int> counts = count_elements(input);

    // Verify that the counts are correct
    assert(counts[0] == 2 && "Test failed: Count of -1 should be 2");
    assert(counts[1] == 2 && "Test failed: Count of 0 should be 2");
    assert(counts[2] == 2 && "Test failed: Count of 1 should be 2");

    cout << "✅ count_elements() tests passed" << endl;
}

void test_counting_sort() {
    // Test case 1: Sequenza bilanciata di -1, 0 e 1
    vector<pair<int, int>> input1 = {{-1, 0}, {0, 1}, {1, 2}, {-1, 3}, {0, 4}, {1, 5}};
    vector<pair<int, int>> expected1 = {{-1, 0}, {-1, 3}, {0, 1}, {0, 4}, {1, 2}, {1, 5}};
    assert(counting_sort(input1) == expected1);

    // Test case 2: Solo -1
    vector<pair<int, int>> input2 = {{-1, 0}, {-1, 1}, {-1, 2}};
    vector<pair<int, int>> expected2 = {{-1, 0}, {-1, 1}, {-1, 2}};
    assert(counting_sort(input2) == expected2);

    // Test case 3: Solo 0
    vector<pair<int, int>> input3 = {{0, 0}, {0, 1}, {0, 2}};
    vector<pair<int, int>> expected3 = {{0, 0}, {0, 1}, {0, 2}};
    assert(counting_sort(input3) == expected3);

    // Test case 4: Solo 1
    vector<pair<int, int>> input4 = {{1, 0}, {1, 1}, {1, 2}};
    vector<pair<int, int>> expected4 = {{1, 0}, {1, 1}, {1, 2}};
    assert(counting_sort(input4) == expected4);

    // Test case 5: Alternanza di valori
    vector<pair<int, int>> input5 = {{1, 0}, {-1, 1}, {0, 2}, {1, 3}, {0, 4}, {-1, 5}};
    vector<pair<int, int>> expected5 = {{-1, 1}, {-1, 5}, {0, 2}, {0, 4}, {1, 0}, {1, 3}};
    assert(counting_sort(input5) == expected5);

    cout << "Tutti i test sono stati superati con successo!" << endl;
}



// Function to test the correct functioning of find_SETs
void testFindSETs() {
    cout << "===== Testing find_SETs() =====" << endl;

    // Create an example table
    Table table({
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, -1},
        {1, 0, 1}
    });

    // Find SETs
    vector<vector<int>> validSets = find_SETs(table, false);


    // Ensure we found exactly one valid SET
    assert(validSets.size() == 1 && "Test failed: Expected exactly 1 valid SET");

    cout << "✅ find_SETs() tests passed" << endl;
}


// Function to test the correct functioning of FIndSETs_brute
void testFindSETsBrute() {
    cout << "===== Testing brute_force_find_SETs() =====" << endl;

    // Create an example table
    Table table({
        {1, 0, -1},
        {0, 1, -1},
        {-1, -1, -1},
        {1, 0, 1}
    });

    // Find SETs
    vector<vector<int>> validSets = brute_force_find_SETs(table, false);

    // Ensure we found exactly one valid SET
    assert(validSets.size() == 1 && "Test failed: Expected exactly 1 valid SET");

    cout << "✅ brute_force_find_SETs() tests passed" << endl;
}

int main() {
    //Table tests
    testGetValue();
    testGetRowAndColumn();
    testSetRow();
    
    //Find_SETs tests
    testIsValidSet();
    testCountElements();
    test_counting_sort();
    testFindSETs();

    //FInd_SETs_brute tests
    testFindSETsBrute();

    return 0;
}
