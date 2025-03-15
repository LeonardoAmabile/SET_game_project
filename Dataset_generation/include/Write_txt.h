#ifndef WRITE_TXT_H
#define WRITE_TXT_H

#include <string>
#include <vector>
#include "Table.h" 

// Function to write data in a txt file
void write_data(const std::string& filename, 
                const std::vector<int>& SETs_count, 
                const std::vector<Table>& matrices, 
                bool print_boolean = false);

#endif // WRITE_TXT_H