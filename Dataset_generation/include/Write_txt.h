#ifndef WRITE_TXT_H
#define WRITE_TXT_H

#include "Table.h"  // Aggiungi questa riga!
#include <string>
#include <vector>

using namespace std;

//Function to write properly data in a file txt
void write_data(const string& filename, const vector<int>& SETs_count, const vector<Table>& matrices,int N_tables, int n_cards, int n_att);

#endif