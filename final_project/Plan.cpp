#include "Plan.h"

Plan::Plan(/* args */)
{
}

Plan::Plan(vector<Module> &Module_List, string connect_filename)
{
    this->Module_List = Module_List;
    this->connect_filename = connect_filename;
}

void Plan::Set_Bound(int Total_area, int Bound_area)
{
    this->Total_area = Total_area;
    this->Bound_area = Bound_area;
}

void Plan::Set_Constraint(double R_lowerbound, double R_upperbound, double Partition_Constraint)
{
    this->R_lowerbound = R_lowerbound;
    this->R_upperbound = R_upperbound;
    this->Partition_Constraint = Partition_Constraint;
}

void Plan::Initial_Partition()
{
    Partition Init_partition;
    Init_partition.set_Constraint(R_lowerbound, R_upperbound);
    Init_partition.set_Module_List(Module_List);

    Partition_List.emplace_back(Init_partition);
    list_size = 1;
}

void Plan::Start_Planning() // Main function of this system
{
    Initial_Partition();

    // bool stop = 1;

    // while (stop)
    // {
    //     stop = 0;

    //     for (int i = 0; i < list_size; i++)
    //     {
    //         Partition partition = Partition_List.front();
    //         Partition_List.pop_front();

    //         if (partition.get_Area() > Bound_area)
    //         {
    //             FM FM_system(partition, Connect_To, Connect_From);
    //             FM_system.Set_Balance(Partition_Constraint);
    //             FM_system.Set_Dataset();
    //             FM_system.FM_Partitioning();

    //             Partition_List.emplace_back(FM_system.get_Partition1());
    //             Partition_List.emplace_back(FM_system.get_Partition2());
    //             list_size += 1;
    //             stop = 1;
    //         }
    //         else
    //         {
    //             Partition_List.emplace_back(partition);
    //         }
    //     }
    // }

    Partition partition = Partition_List.front();
    Partition_List.pop_front();

    FM FM_system(partition, connect_filename);
    FM_system.Initial_Dataset();
    FM_system.Initial();
    FM_system.FM_Partitioning();

    vector<Module> list_1 = FM_system.get_list_1();
    partition.set_Module_List(list_1);
    Partition_List.emplace_back(partition);

    vector<Module> list_2 = FM_system.get_list_2();
    partition.set_Module_List(list_2);
    Partition_List.emplace_back(partition);
}

void Plan::Output_Planning()
{
    int i = 0;
    for (auto &partition : Partition_List)
    {
        partition.set_Index(i);
        partition.Init_Module_Tree();
        partition.Simulated_Annealing();
        partition.Output();
        i += 1;
    }
}

Plan::~Plan()
{
}