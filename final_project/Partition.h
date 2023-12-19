#ifndef PARTITION_H_
#define PARTITION_H_

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include "Module.h"
#include "SA.h"

using namespace std;

class Partition
{
private:
    int index;
    int height;
    int width;

    vector<Module> Module_List;
    int Module_num;

    double R_lowerbound;
    double R_upperbound;

    int Root = 0;

    vector<int> Tree_left;
    vector<int> Tree_right;
    vector<int> Tree_prev;
    vector<int> Rotate;

public:
    Partition(/* args */);
    Partition(int i);
    ~Partition();

    void Set_Constraint(double R_lowerbound, double R_upperbound);
    void Set_Module(Module module);
    void Init_Module_Tree();
    void Simulated_Annealing();
    void Output();
};

#endif