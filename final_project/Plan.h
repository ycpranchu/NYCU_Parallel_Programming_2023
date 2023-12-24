#ifndef PLAN_H_
#define PLAN_H_

#include <vector>
#include <unordered_map>
#include <deque>

#include "Partition.h"
#include "Module.h"
#include "FM.h"

using namespace std;

class Plan
{
private:
    vector<Module> Module_List;
    string connect_filename;

    deque<Partition> Partition_List;
    int list_size;

    int Total_area;
    int Bound_area;

    double R_lowerbound;
    double R_upperbound;
    double Partition_Constraint;

public:
    Plan(/* args */);
    Plan(vector<Module> &Module_List, string connect_filename);
    ~Plan();

    void Set_Bound(int Total_area, int Bound_area);
    void Set_Constraint(double R_lowerbound, double R_upperbound, double Partition_Constraint);
    void Initial_Partition();
    void Start_Planning();
    void Output_Planning();
};

#endif