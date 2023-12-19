#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Plan.h"

double R_lowerbound = 0.0;
double R_upperbound = 0.0;
double Total_area = 0.0;
double Bound_area = 0.0;

void Input_File(string filename, Partition *partition);

#endif