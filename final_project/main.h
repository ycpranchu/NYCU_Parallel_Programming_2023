#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Plan.h"

double R_lowerbound = 0.0;
double R_upperbound = 0.0;

double Partition_Constraint;

int Total_area = 0.0;
int Bound_area = 0.0;

void Case_File(string filename, vector<Module> &Module_List);

#endif