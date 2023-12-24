#include "SA.h"

SA::SA(/* args */)
{
}

SA::SA(vector<Module> Module_List, int Root, vector<int> Tree_left, vector<int> Tree_right, vector<int> Tree_prev, vector<int> Rotate,
       double R_lowerbound, double R_upperbound)
{
    this->Module_List = Module_List;
    Module_num = Module_List.size();

    this->R_lowerbound = R_lowerbound;
    this->R_upperbound = R_upperbound;

    this->Root = Root;
    this->Tree_left = Tree_left;
    this->Tree_right = Tree_right;
    this->Tree_prev = Tree_prev;
    this->Rotate = Rotate;

    Root_copy = Root;
    Tree_left_copy = Tree_left;
    Tree_right_copy = Tree_right;
    Tree_prev_copy = Tree_prev;
    Rotate_copy = Rotate;
}

void SA::Fast_Simulated_Annealing()
{
    Contour contour(Module_List);
    contour.Update(Root, Tree_left, Tree_right, Tree_prev, Rotate);

    double previous_area = contour.get_Area();
    double average_area = contour.get_Area();
    double previous_wh = contour.get_W_H_ratio();
    double cost = 0.0;

    double previous_cost = 1;
    double previous_cost_init = previous_cost;
    double delta_cost = previous_cost;

    for (int times = 1; times <= init_times; ++times)
    {
        Disturb();
        contour.Update(Root, Tree_left, Tree_right, Tree_prev, Rotate);

        cost = contour.get_Area() / (average_area / times);
        average_area += contour.get_Area();

        if (cost > previous_cost_init)
            delta_cost += cost - previous_cost_init;

        previous_cost_init = cost;
    }

    average_area /= init_times; // normalize average area

    Tree_left = Tree_left_copy;
    Tree_right = Tree_right_copy;
    Tree_prev = Tree_prev_copy;
    Rotate = Rotate_copy;
    Root = Root_copy;

    int accept = 0;
    double Temprature_init = 0.0, Temprature = 0.0, P = 0.99;

    Temprature_init = -1 * delta_cost / init_times / log(P);
    Temprature = Temprature_init;

    cout << "Temprature: " << Temprature << endl;

    while (Temprature > T)
    {
        double Probability = Temprature / Temprature_init;
        delta_cost = 0.0;

        for (int i = 0; i < s; ++i)
        {
            Disturb();
            contour.Update(Root, Tree_left, Tree_right, Tree_prev, Rotate);
            cost = contour.get_Area() / average_area;

            if (cost > previous_cost && contour.get_W_H_ratio() < R_upperbound && contour.get_W_H_ratio() > R_lowerbound) // Delta(cost) > 0
            {
                delta_cost += cost - previous_cost;
                seed_2 = rand_r((unsigned int *)&seed_2);

                if ((double)seed_2 / RAND_MAX <= Probability) // Up-hill accepted {}
                    accept = 1;
            }
            else if (contour.get_W_H_ratio() < R_upperbound && contour.get_W_H_ratio() > R_lowerbound) // Delta(cost) < 0
            {
                accept = 1;
            }

            if (accept == 1)
            {
                Tree_left_copy = Tree_left;
                Tree_right_copy = Tree_right;
                Tree_prev_copy = Tree_prev;
                Rotate_copy = Rotate;
                Root_copy = Root;

                previous_cost = cost;
            }
            else
            {
                Tree_left = Tree_left_copy;
                Tree_right = Tree_right_copy;
                Tree_prev = Tree_prev_copy;
                Rotate = Rotate_copy;
                Root = Root_copy;
            }

            accept = 0;
        }

        Temprature = Temprature * anneal_rate;
    }

    contour.Update(Root, Tree_left, Tree_right, Tree_prev, Rotate);
}

inline void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void SA::Disturb()
{
    seed_0 = rand_r((unsigned int *)&seed_0);
    int type = seed_0 % 2;
    int node_1 = seed_0 % Module_num;

    if (type == 0) // rotate a macro
    {
        Rotate[node_1] = (Rotate[node_1] == 0) ? 1 : 0;
    }
    else if (type == 1) // swap 2 nodes
    {
        int node_2 = node_1;

        while (node_2 == node_1)
        {
            seed_1 = rand_r((unsigned int *)&seed_1);
            node_2 = seed_1 % Module_num;
        }

        if (Tree_prev[node_1] == -1)
            Root = node_2;
        else if (Tree_prev[node_2] == -1)
            Root = node_1;

        // Swap Tree_left and Tree_right of parent nodes

        if (Tree_prev[node_1] != -1)
        {
            if (Tree_left[Tree_prev[node_1]] == node_1)
                Tree_left[Tree_prev[node_1]] = node_2;
            else
                Tree_right[Tree_prev[node_1]] = node_2;
        }

        if (Tree_prev[node_2] != -1)
        {
            if (Tree_left[Tree_prev[node_2]] == node_2)
                Tree_left[Tree_prev[node_2]] = node_1;
            else
                Tree_right[Tree_prev[node_2]] = node_1;
        }

        swap(&Tree_prev[node_1], &Tree_prev[node_2]);
        swap(&Tree_left[node_1], &Tree_left[node_2]);
        swap(&Tree_right[node_1], &Tree_right[node_2]);

        // Update Tree_prev of child nodes

        if (Tree_left[node_1] != -1)
            Tree_prev[Tree_left[node_1]] = node_1;
        if (Tree_right[node_1] != -1)
            Tree_prev[Tree_right[node_1]] = node_1;
        if (Tree_left[node_2] != -1)
            Tree_prev[Tree_left[node_2]] = node_2;
        if (Tree_right[node_2] != -1)
            Tree_prev[Tree_right[node_2]] = node_2;

        if (Root != -1)
        {
            Tree_prev[Root] = -1;
        }
    }
}

int SA::get_Root() { return Root; }
vector<int> SA::get_Tree_left() { return Tree_left; }
vector<int> SA::get_Tree_right() { return Tree_right; }
vector<int> SA::get_Tree_prev() { return Tree_prev; }
vector<int> SA::get_Rotate() { return Rotate; }

SA::~SA()
{
}