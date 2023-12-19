#ifndef PLAN_H_
#define PLAN_H_

#include <vector>
#include <deque>

#include "Partition.h"

using namespace std;

class Plan
{
private:
    deque<Partition> Partition_List;
    int list_size;

public:
    Plan(/* args */);
    ~Plan();

    Partition get_Partition();
    void save_Partition(Partition partition);
};

#endif