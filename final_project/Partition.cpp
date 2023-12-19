#include "Partition.h"

Partition::Partition(/* args */)
{
}

Partition::Partition(int i)
{
    index = i;
}

void Partition::Set_Constraint(double R_lowerbound, double R_upperbound)
{
    this->R_lowerbound = R_lowerbound;
    this->R_upperbound = R_upperbound;
}

void Partition::Set_Module(Module module)
{
    Module_List.emplace_back(module);
}

void Partition::Init_Module_Tree()
{
    Module_num = Module_List.size();

    Tree_left = vector<int>(Module_num, -1);
    Tree_right = vector<int>(Module_num, -1);
    Tree_prev = vector<int>(Module_num, -1);
    Rotate = vector<int>(Module_num, 0);

    int bound = sqrt(Module_num);

    int id = 0, temp = 0;
    bool begin = true;

    while (id < Module_num - 1)
    {
        if (id % bound == 0)
        {
            if (begin == false)
            {
                Tree_left[id - 1] = -1;
                Tree_right[temp] = id;
                Tree_prev[id] = temp;
            }
            else
                begin = false;

            temp = id;
        }

        Tree_left[id] = id + 1;
        Tree_prev[id + 1] = id;
        ++id;
    }

    return;
}

void Partition::Simulated_Annealing()
{
    SA sa(Module_List, Root, Tree_left, Tree_right, Tree_prev, Rotate, R_lowerbound, R_upperbound);
    sa.Fast_Simulated_Annealing();

    Root = sa.get_Root();
    Tree_left = sa.get_Tree_left();
    Tree_right = sa.get_Tree_right();
    Tree_prev = sa.get_Tree_prev();
    Rotate = sa.get_Rotate();
}

void Partition::Output()
{
    Contour contour(Module_List);
    contour.set_type(1);
    contour.Update(Root, Tree_left, Tree_right, Tree_prev, Rotate);
    Module_List = contour.get_Module_List();

    ofstream ofs;
    ofs.open("floorplan/partition_" + to_string(index) + ".out");

    ofs << "A = " << (int)contour.get_Area() << "\n";
    cout << "A = " << (int)contour.get_Area() << "\n";
    ofs << "R = " << contour.get_W_H_ratio() << "\n";

    for (int i = 0; i < Module_num; i++)
    {
        Module item = Module_List[i];
        ofs << item.get_name() << " " << item.get_x() << " " << item.get_y();

        if (Rotate[i] == 1)
            ofs << " R";

        if (i != Module_num - 1)
            ofs << "\n";
    }

    ofs.close();
}

Partition::~Partition()
{
}