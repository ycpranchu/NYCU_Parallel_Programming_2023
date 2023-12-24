#ifndef FM_H_
#define FM_H_

#include <vector>
#include <set>
#include <unordered_map>
#include <random>
#include <algorithm>

#include "Partition.h"
#include "Module.h"

using namespace std;

struct ListNode
{
    int uid;
    int val = 0;
    ListNode *pev;
    ListNode *next;
    ListNode() : uid(0), pev(nullptr), next(nullptr) {}
    ListNode(int x) : uid(x), pev(nullptr), next(nullptr) {}
    ListNode(int x, ListNode *pev, ListNode *next) : uid(x), pev(pev), next(next) {}
};

class FM
{
private:
    vector<Module> Module_List;
    string connect_filename;

    Partition partition;
    Partition partition1;
    Partition partition2;

    vector<vector<int>> Net;
    vector<vector<int>> Cell;

    vector<int> temp_partition;
    vector<int> final_partition;

    vector<vector<ListNode *>> Bucket_List;
    vector<ListNode *> cell_record;

    double balance_factor;
    double constraint_down;
    double constraint_up;

    int cell_nums = 0;
    int P_max = 0;

    int max_gain_index0;
    int max_gain_index1;

    int global_min_cutsize = INT32_MAX;
    int group_size[2];
    int Epochs = 30;

public:
    FM(/* args */);
    FM(Partition partition, string connect_filename);
    ~FM();

    void Input_File();
    void Output_File();

    void Initial();
    void Initial_Gain();

    int Find_Element();
    int Update_Gain(int max_node);

    void FM_Partitioning();

    Partition get_Partition1();
    Partition get_Partition2();
};

#endif