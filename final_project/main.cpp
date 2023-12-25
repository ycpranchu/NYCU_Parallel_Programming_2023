#pragma GCC optimize(3, "Ofast", "inline")
#include "main.h"

using namespace std;

void Case_File(string filename, vector<Module> &Module_List)
{
    ifstream ifs;
    ifs.open(filename);

    if (!ifs.is_open())
    {
        cout << "Failed to open the case file.\n";
        return;
    }

    string temp, a, b, c;

    ifs >> temp;
    R_lowerbound = stod(temp); // Read the lowerbound
    ifs >> temp;
    R_upperbound = stod(temp); // Read the upperbound

    while (ifs >> a >> b >> c) // Read width and height of each block
    {
        int index = stoi(a.substr(1));
        int width = stoi(b);
        int height = stoi(c);

        Module module(index, width, height);
        Module_List.emplace_back(module);

        Total_area += width * height; // Calculate the total area
    }

    ifs.close();
    return;
}

inline void Output_File(string filename, Partition *partition)
{
    ofstream ofs;
    ofs.open(filename);
}

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    string case_filename = argv[1];
    string connect_filename = argv[2];

    cout << "----------------------------------------------------------------" << endl;
    cout << "[Initialize]" << endl;

    vector<Module> Module_List;
    vector<vector<int>> Net;
    vector<vector<int>> Cell;

    cout << "Reading the input file..." << endl;

    Case_File(case_filename, Module_List);
    Bound_area = Total_area / 20;

    cout << "----------------------------------------------------------------" << endl;
    cout << "[Serial version]" << endl;

    START = currentSeconds();
    for (int i = 0; i < Iterations; i++)
    {
        Plan Main_Plan(Module_List, connect_filename);
        Main_Plan.Set_Bound(Total_area, Bound_area);
        Main_Plan.Set_Constraint(R_lowerbound, R_upperbound, Partition_Constraint);
        Main_Plan.Start_Planning();
        Main_Plan.Output_Planning();
    }
    END = currentSeconds();

    double serial_time = END - START;
    cout << serial_time << " seconds" << endl;

    cout << "----------------------------------------------------------------" << endl;
    cout << "[OpenMP version]" << endl;

    START = currentSeconds();
    for (int i = 0; i < Iterations; i++)
    {
        Plan Main_Plan(Module_List, connect_filename);
        Main_Plan.Set_Bound(Total_area, Bound_area);
        Main_Plan.Set_Constraint(R_lowerbound, R_upperbound, Partition_Constraint);
        Main_Plan.OMP_Start_Planning();
        Main_Plan.OMP_Output_Planning();
    }
    END = currentSeconds();

    double openmp_time = END - START;
    cout << openmp_time << " seconds - " << serial_time / openmp_time << " x faster than serial version" << endl;

    return 0;
}