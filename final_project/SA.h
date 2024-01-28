#ifndef SA_H_
#define SA_H_

#include <iostream>
#include <vector>
#include <random>

#include "Contour.h"
#define E 2.718281

using namespace std;

class SA
{
private:
    int seed_0 = time(NULL);
    int seed_1 = time(NULL) << 1;
    int seed_2 = time(NULL) << 2;

    int Module_num;
    vector<Module> Module_List;

    double R_lowerbound;
    double R_upperbound;

    int Root;
    vector<int> Tree_left;
    vector<int> Tree_right;
    vector<int> Tree_prev;
    vector<int> Rotate;

    int Root_copy;
    vector<int> Tree_left_copy;
    vector<int> Tree_right_copy;
    vector<int> Tree_prev_copy;
    vector<int> Rotate_copy;

    // Simulated Annealing Parameters
    int init_times = 100, c = 100, k = 8, s = 10000;
    double T = 0.0001, alpha = 0.8, anneal_rate = 0.9;

public:
    SA(/* args */);
    SA(vector<Module> Module_List, int Root, vector<int> Tree_left, vector<int> Tree_right, vector<int> Tree_prev, vector<int> Rotate,
       double R_lowerbound, double R_upperbound);

    void Fast_Simulated_Annealing();
    void Disturb();

    int get_Root();
    vector<int> get_Tree_left();
    vector<int> get_Tree_right();
    vector<int> get_Tree_prev();
    vector<int> get_Rotate();

    ~SA();
};

#endif