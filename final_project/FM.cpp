#include "FM.h"

FM::FM(/* args */) {}

FM::FM(Partition partition, string connect_filename)
{
    this->partition = partition;
    this->connect_filename = connect_filename;

    Module_List = partition.get_Module_List();
    cell_nums = Module_List.size();
}

void FM::Initial_Dataset()
{
    ifstream ifs;
    ifs.open(connect_filename);

    string temp;
    ifs >> temp;
    balance_factor = stod(temp);

    vector<int> item;

    while (getline(ifs, temp, ' ')) // Net array;
    {
        if (temp[0] == 'm')
        {
            int i = stoi(temp.substr(1));
            item.emplace_back(i);
        }
        else if (temp[0] == ';')
        {
            Net.emplace_back(item);
            item.clear();
        }
    }

    Cell.resize(cell_nums);
    for (int i = 0; i < (int)Net.size(); ++i) // Cell array;
    {
        for (auto &index : Net[i])
        {
            Cell[index].emplace_back(i);
        }
    }

    for (auto &it : Cell) // Calculate P_max;
        P_max = max(P_max, (int)it.size());

    ifs.close();
}

void FM::Output_File()
{
    ofstream ofs;
    ofs.open("connect.out");

    int n = final_partition.size();
    int group2_size = reduce(final_partition.begin(), final_partition.end());
    vector<int> partition_a, partition_b;

    for (int i = 0; i < n; ++i)
    {
        if (final_partition[i] == 0)
            partition_a.emplace_back(i);
        else
            partition_b.emplace_back(i);
    }

    ofs << "Cutsize = " << global_min_cutsize << "\n";
    ofs << "G1 " << final_partition.size() - group2_size << "\n";
    for (auto &i : partition_a)
        ofs << "m" << i << " ";
    ofs << ";"
        << "\n"
        << "G2 " << group2_size << "\n";
    for (auto &i : partition_b)
        ofs << "m" << i << " ";
    ofs << ";"
        << "\n";

    cout << "G1: " << final_partition.size() - group2_size << " G2: " << group2_size << "\n";

    ofs.close();
}

void FM::Initial()
{
    temp_partition = vector<int>(cell_nums, 0);

    for (int i = 0; i < cell_nums / 2; ++i)
        temp_partition[i] = 1;

    auto rd = random_device{};
    auto rng = default_random_engine{rd()};
    shuffle(temp_partition.begin(), temp_partition.end(), rng);

    // ----------------------------------------------------------------

    Bucket_List = vector<vector<ListNode *>>(2);
    cell_record = vector<ListNode *>(cell_nums);

    for (int i = 0; i < cell_nums; i++)
    {
        ListNode *node = new ListNode(i);
        node->val = -1;
        cell_record[i] = node;
    }

    Initial_Gain();
    group_size[1] = reduce(temp_partition.begin(), temp_partition.end());
    group_size[0] = cell_nums - group_size[1];
}

void FM::Initial_Gain()
{
    vector<int> gain(cell_nums, 0);

    Bucket_List[0].clear();
    Bucket_List[1].clear();

    for (auto &it : Cell)
        P_max = max(P_max, (int)it.size());

    for (int i = 0; i < P_max * 2 + 1; ++i)
    {
        Bucket_List[0].emplace_back(new ListNode(i - P_max, nullptr, nullptr));
        Bucket_List[1].emplace_back(new ListNode(i - P_max, nullptr, nullptr));
    }

    for (auto &item : Net)
    {
        int checker = 0; // number of cells in partition B;
        for (auto &i : item)
            checker += temp_partition[i];

        if (checker == 0 || checker == (int)item.size()) // All cells in the same partition;
        {
            for (auto &i : item)
                --gain[i];
        }

        if (checker == 1) // Single cell in the partition B;
        {
            for (auto &i : item)
                if (temp_partition[i] == 1)
                {
                    ++gain[i];
                    break;
                }
        }

        if (checker == (int)item.size() - 1) // Single cell in partition A;
        {
            for (auto &i : item)
                if (temp_partition[i] == 0)
                {
                    ++gain[i];
                    break;
                }
        }
    }

    for (int i = 0; i < cell_nums; ++i) // Initialize Bucket_List;
    {
        ListNode *node = cell_record[i];
        ListNode *head = Bucket_List[temp_partition[i]][gain[i] + P_max];

        node->val = gain[i];
        node->next = head->next;
        node->pev = head;
        head->next = node;
        if (node->next != nullptr)
            node->next->pev = node;
    }
}

int FM::Find_Element()
{
    int index = -1;
    ListNode *node;

    if (group_size[0] >= group_size[1])
    {
        --group_size[0];
        ++group_size[1];

        node = Bucket_List[0][max_gain_index0];
        index = node->next->uid;
    }
    else
    {
        ++group_size[0];
        --group_size[1];

        node = Bucket_List[1][max_gain_index1];
        index = node->next->uid;
    }

    return index;
}

int FM::Update_Gain(int max_node)
{
    ListNode *node = cell_record[max_node];

    // Delete the moved node;
    node->pev->next = node->next;
    if (node->next != nullptr)
        node->next->pev = node->pev;

    node->val = -P_max - 1;
    node->pev = nullptr;
    node->next = nullptr;

    int partition_index = temp_partition[max_node];
    int gain_offset[cell_nums] = {0};
    int cutsize_offset = 0;
    int checker = 0, F = 0, T = 0;
    set<int> gain_changed;

    for (auto &net_element : Cell[max_node])
    {
        vector<int> cell_element = Net[net_element];

        checker = 0; // number of cells in partition B;

        for (auto &i : cell_element)
        {
            checker += temp_partition[i];
        }

        F = 0, T = 0; // calculate F(n) and T(n);

        if (partition_index == 0)
        {
            F = cell_element.size() - checker;
            T = checker;
        }
        else
        {
            F = checker;
            T = cell_element.size() - checker;
        }

        // Update gains before the move;
        if (T == 0)
        {
            for (auto &i : cell_element)
            {
                ++gain_offset[i];
                gain_changed.emplace(i);
            }
            ++cutsize_offset;
        }
        else if (T == 1)
        {
            for (auto &i : cell_element) // Find the only T cell on n;
            {
                if (temp_partition[i] != partition_index)
                {
                    --gain_offset[i];
                    gain_changed.emplace(i);
                    break;
                }
            }
        }

        F -= 1;

        // Update gains after the move;
        if (F == 0)
        {
            for (auto &i : cell_element)
            {
                --gain_offset[i];
                gain_changed.emplace(i);
            }
            --cutsize_offset;
        }
        else if (F == 1)
        {
            for (auto &i : cell_element) // Find the only T cell on n;
            {
                if (i != max_node && temp_partition[i] == partition_index)
                {
                    ++gain_offset[i];
                    gain_changed.emplace(i);
                    break;
                }
            }
        }
    }

    temp_partition[max_node] = 1 - partition_index;

    // Update the Bucket_List;
    for (auto &i : gain_changed)
    {
        ListNode *node = cell_record[i];

        if (gain_offset[i] != 0 && node->val != -P_max - 1)
        {
            ListNode *node = cell_record[i];
            node->val = node->val + gain_offset[i];

            node->pev->next = node->next;
            if (node->next != nullptr)
                node->next->pev = node->pev;

            ListNode *head = Bucket_List[temp_partition[i]][node->val + P_max];

            if (temp_partition[i] == 0)
                max_gain_index0 = max(max_gain_index0, node->val + P_max);
            else
                max_gain_index1 = max(max_gain_index1, node->val + P_max);

            node->next = head->next;
            node->pev = head;
            head->next = node;
            if (node->next != nullptr)
                node->next->pev = node;
        }
    }

    return cutsize_offset;
}

void FM::FM_Partitioning()
{
    for (int epoch = 0; epoch < Epochs; ++epoch)
    {
        cout << "[Running the algorithm, epoch " << epoch << "]" << endl;

        int cutsize = 0;
        for (int i = 0; i < (int)Net.size(); ++i)
        {
            vector<int> cell_element = Net[i];
            int checker = 0;

            for (auto &i : cell_element)
                checker += temp_partition[i];

            if (checker != 0 && checker != (int)cell_element.size())
                ++cutsize;
        }

        max_gain_index0 = P_max * 2, max_gain_index1 = P_max * 2;
        int count = 0, max_node = -1;

        while (count < cell_nums)
        {
            // Update the max_gain index; ----------------------------------------------------------------
            for (int j = max_gain_index0; j >= 0; --j)
            {
                max_gain_index0 = j;
                if (Bucket_List[0][j]->next != nullptr)
                {
                    break;
                }
            }

            for (int j = max_gain_index1; j >= 0; --j)
            {
                max_gain_index1 = j;
                if (Bucket_List[1][j]->next != nullptr)
                {
                    break;
                }
            }
            // ----------------------------------------------------------------

            max_node = Find_Element();

            if (max_node == -1)
            {
                cout << "Can't make a partition!!" << endl;
                break;
            }

            cutsize += Update_Gain(max_node);

            if (cutsize < global_min_cutsize)
            {
                final_partition.assign(temp_partition.begin(), temp_partition.end());
                global_min_cutsize = cutsize;
            }

            ++count;
        }

        if (epoch < Epochs - 1)
        {
            Initial_Gain();
        }
    }

    cout << "[Found the Solution]" << endl;
    cout << "Min Cutsize: " << global_min_cutsize << endl;
    int group2_size = reduce(final_partition.begin(), final_partition.end());
    cout << "Group 1: " << final_partition.size() - group2_size << " Group 2: " << group2_size << "\n";

    list_1.clear();
    list_2.clear();

    for (int i = 0; i < cell_nums; i++)
    {
        if (final_partition[i] == 0)
            list_1.emplace_back(Module_List[i]);
        else
            list_2.emplace_back(Module_List[i]);
    }
}

vector<Module> FM::get_list_1() { return list_1; }

vector<Module> FM::get_list_2() { return list_2; }

FM::~FM()
{
}