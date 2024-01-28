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

void Plan::Set_Constraint(double R_lowerbound, double R_upperbound)
{
    this->R_lowerbound = R_lowerbound;
    this->R_upperbound = R_upperbound;
}

void Plan::Initial_Partition()
{
    Partition Init_partition;
    Init_partition.set_Constraint(R_lowerbound, R_upperbound);
    Init_partition.set_Module_List(Module_List);

    Partition_List.emplace_back(Init_partition);
}

void Plan::Start_Planning() // Main function of this system
{
    Initial_Partition();
    vector<Partition> temp_list;

    bool stop = 1;
    while (stop)
    {
        stop = 0;

        for (auto &partition : Partition_List)
        {
            if (partition.get_Area() > Bound_area)
            {
                FM FM_system(partition, connect_filename);
                FM_system.Initial_Dataset();
                FM_system.Initial();
                FM_system.FM_Partitioning();

                vector<Module> list_1 = FM_system.get_list_1();
                partition.set_Module_List(list_1);
                temp_list.emplace_back(partition);

                vector<Module> list_2 = FM_system.get_list_2();
                partition.set_Module_List(list_2);
                temp_list.emplace_back(partition);

                stop = 1;
            }
            else
            {
                temp_list.emplace_back(partition);
            }
        }

        Partition_List = temp_list;
        temp_list.clear();
    }
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

void Plan::OMP_Start_Planning()
{
    Initial_Partition();

    vector<Partition> temp_list;
    bool stop = 1;

    while (stop)
    {
        stop = 0;

#pragma omp parallel for num_threads(Num_Threads)
        for (auto &partition : Partition_List)
        {
            if (partition.get_Area() > Bound_area)
            {
                FM FM_system(partition, connect_filename);
                FM_system.Initial_Dataset();
                FM_system.Initial();
                FM_system.FM_Partitioning();

                vector<Module> list_1 = FM_system.get_list_1();
                partition.set_Module_List(list_1);

#pragma omp critical
                temp_list.emplace_back(partition);

                vector<Module> list_2 = FM_system.get_list_2();
                partition.set_Module_List(list_2);

#pragma omp critical
                temp_list.emplace_back(partition);

                stop = 1;
            }
            else
            {
#pragma omp critical
                temp_list.emplace_back(partition);
            }
        }

        Partition_List = temp_list;
        temp_list.clear();
    }
}

void Plan::OMP_Output_Planning()
{
    int i = 0;
    for (auto &partition : Partition_List)
    {
        partition.set_Index(i);
        i += 1;
    }

#pragma omp parallel for num_threads(Num_Threads)
    for (auto &partition : Partition_List)
    {
        partition.Init_Module_Tree();
        partition.Simulated_Annealing();
        partition.Output();
    }
}

void Input_Thread_Function(Input_args *thread_data)
{
    vector<Partition> temp_list;

    for (auto &partition : thread_data->Partition_List)
    {
        if (partition.get_Area() > thread_data->Bound_area)
        {
            FM FM_system(partition, thread_data->connect_filename);
            FM_system.Initial_Dataset();
            FM_system.Initial();
            FM_system.FM_Partitioning();

            vector<Module> list_1 = FM_system.get_list_1();
            partition.set_Module_List(list_1);
            temp_list.emplace_back(partition);

            vector<Module> list_2 = FM_system.get_list_2();
            partition.set_Module_List(list_2);
            temp_list.emplace_back(partition);

            thread_data->stop = 1;
        }
        else
        {
            temp_list.emplace_back(partition);
        }
    }

    thread_data->Partition_List = temp_list;
}

void Plan::Pthread_Start_Planning()
{
    Initial_Partition();

    thread threads[Num_Threads - 1];
    Input_args thread_data[Num_Threads];

    for (int i = 0; i < Num_Threads; ++i)
    {
        thread_data[i].connect_filename = connect_filename;
        thread_data[i].Bound_area = Bound_area;
        thread_data[i].Partition_List.clear();
        thread_data[i].stop = 0;
    }

    bool stop = 1;
    while (stop)
    {
        stop = 0;

        for (int i = 0; i < Partition_List.size(); ++i)
        {
            thread_data[i % Num_Threads].Partition_List.emplace_back(Partition_List[i]);
        }

        for (int i = 0; i < Num_Threads - 1; ++i)
        {
            threads[i] = thread(Input_Thread_Function, &thread_data[i]);
        }

        Input_Thread_Function(&thread_data[Num_Threads - 1]);

        for (int i = 0; i < Num_Threads - 1; ++i)
        {
            threads[i].join();
        }

        Partition_List.clear();

        for (int i = 0; i < Num_Threads; ++i)
        {
            Partition_List.insert(Partition_List.end(), thread_data[i].Partition_List.begin(), thread_data[i].Partition_List.end());

            if (thread_data[i].stop == 1)
                stop = 1;

            thread_data[i].Partition_List.clear();
            thread_data[i].stop = 0;
        }
    }
}

void Output_Thread_Function(Output_args *thread_data)
{
    for (auto &partition : thread_data->Partition_List)
    {
        partition.Init_Module_Tree();
        partition.Simulated_Annealing();
        partition.Output();
    }
}

void Plan::Pthread_Output_Planning()
{
    thread threads[Num_Threads - 1];
    Output_args thread_data[Num_Threads];

    for (int i = 0; i < Partition_List.size(); ++i)
    {
        Partition_List[i].set_Index(i);
        thread_data[i % Num_Threads].Partition_List.emplace_back(Partition_List[i]);
    }

    for (int i = 0; i < Num_Threads - 1; ++i)
    {
        threads[i] = thread(Output_Thread_Function, &thread_data[i]);
    }

    Output_Thread_Function(&thread_data[Num_Threads - 1]);

    for (int i = 0; i < Num_Threads - 1; ++i)
    {
        threads[i].join();
    }
}

Plan::~Plan()
{
}