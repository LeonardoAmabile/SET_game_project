#ifndef FIND_SETS_BRUTE_H
#define FIND_SETS_BRUTE_H

#include "Find_SETs_brute.h"
#include "Table.h"
#include "Find_SETs.h"

#include <vector>
#include <iostream>

using namespace std;

//Brute-force algorithm to find all the possible SETs combinations in a Table
vector<vector<int>> brute_force_find_SETs(Table table, bool print = false, bool bool_version = false);



#endif