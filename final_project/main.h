#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Plan.h"
#include "CycleTimer.h"

using namespace std;

double R_lowerbound = 0.0;
double R_upperbound = 0.0;

double Partition_Constraint;

int Total_area = 0.0;
int Bound_area = 0.0;

// int Iterations = 10;
int Iterations = 1;

double START;
double END;

void Case_File(string filename, vector<Module> &Module_List);

#endif