#ifndef PLAN_H_
#define PLAN_H_

#include <vector>
#include <unordered_map>
#include <deque>
#include <list>

#include <omp.h>
#include <thread>

#include "Partition.h"
#include "Module.h"
#include "FM.h"

using namespace std;

static const int Num_Threads = 16;

struct Input_args
{
    string connect_filename;
    int Bound_area;

    vector<Partition> Partition_List;
    int stop = 0;
};

struct Output_args
{
    vector<Partition> Partition_List;
};

class Plan
{
private:
    vector<Module> Module_List;
    string connect_filename;

    vector<Partition> Partition_List;

    int Total_area;
    int Bound_area;

    double R_lowerbound;
    double R_upperbound;

public:
    Plan(/* args */);
    Plan(vector<Module> &Module_List, string connect_filename);
    ~Plan();

    void Set_Bound(int Total_area, int Bound_area);
    void Set_Constraint(double R_lowerbound, double R_upperbound);

    void Initial_Partition();

    void Start_Planning();
    void Output_Planning();

    void OMP_Start_Planning();
    void OMP_Output_Planning();

    void Pthread_Start_Planning();
    void Pthread_Output_Planning();
};

#endif