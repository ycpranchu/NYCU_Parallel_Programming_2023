#include "Plan.h"

Plan::Plan(/* args */)
{
}

Partition Plan::get_Partition()
{
    Partition partition;

    if (list_size > 0)
    {
        partition = Partition_List.front();
        Partition_List.pop_front();
        list_size -= 1;
    }

    return partition;
}

void Plan::save_Partition(Partition partition)
{
    Partition_List.emplace_back(partition);
    list_size += 1;
}

Plan::~Plan()
{
}