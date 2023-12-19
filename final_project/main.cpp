#pragma GCC optimize(3, "Ofast", "inline")
#include "main.h"

using namespace std;

void Input_File(string filename, Partition *partition)
{
    ifstream ifs;
    ifs.open(filename);

    if (!ifs.is_open())
    {
        cout << "Failed to open the input file.\n";
        return;
    }

    string temp, a, b, c;

    ifs >> temp;
    R_lowerbound = stod(temp); // Read the lowerbound
    ifs >> temp;
    R_upperbound = stod(temp); // Read the upperbound

    partition->Set_Constraint(R_lowerbound, R_upperbound);

    int count = 0;
    while (ifs >> a >> b >> c) // Read width and height of each block
    {
        string name = a;
        int width = stoi(b);
        int height = stoi(c);

        Module module(name, width, height);
        partition->Set_Module(module);

        Total_area += width * height; // Calculate the total area
        count += 1;
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

    string input_filename = argv[1];

    cout << "----------------------------------------------------------------" << endl;
    cout << "[Initialize]" << endl;

    Partition Init_partition(0);

    cout << "Reading the input file..." << endl;
    Input_File(input_filename, &Init_partition);

    cout << "Total area: " << Total_area << endl;
    Bound_area /= Total_area;

    cout << "Initialized the first B* Tree..." << endl;

    Init_partition.Init_Module_Tree();
    Init_partition.Simulated_Annealing();
    Init_partition.Output();

    return 0;
}