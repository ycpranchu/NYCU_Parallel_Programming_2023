#include "Contour.h"

Contour::Contour(/* args */)
{
}

Contour::Contour(vector<Module> Module_List)
{
    this->Module_List = Module_List;
}

void Contour::Update(int Root, vector<int> Tree_left, vector<int> Tree_right, vector<int> Tree_prev, vector<int> Rotate)
{
    this->Root = Root;
    this->Tree_left = Tree_left;
    this->Tree_right = Tree_right;
    this->Rotate = Rotate;

    Generate_Graph();
}

void Contour::Generate_Graph()
{
    ContourNode *node_1 = new ContourNode(0, 0);
    ContourNode *node_2 = new ContourNode(INT32_MAX, 0);
    ContourNode *start = node_1;

    node_1->next = node_2;
    node_2->prev = node_1;

    int base_x = 0, next_x = 0;
    int base_y = 0, next_y = 0;
    int root = Root, update = 0;

    stack<int> stk;
    stack<ContourNode *> stk_contour;

    max_x = 0, max_y = 0;

    while (!stk.empty() || root != -1)
    {
        if (root == -1)
        {
            root = stk.top();
            stk.pop();
            start = stk_contour.top();
            stk_contour.pop();

            while (start->y != 0 && max_y >= start->prev->y)
            {
                start = start->prev;
                update = 1;
            }

            start = node_1;
        }

        if (Tree_right[root] != -1)
            stk.emplace(Tree_right[root]);

        if (start->y > start->next->y)
        {
            start->y = start->next->y;
        }

        int width = (Rotate[root] == 0) ? Module_List[root].get_width() : Module_List[root].get_height();
        int height = (Rotate[root] == 0) ? Module_List[root].get_height() : Module_List[root].get_width();

        base_x = start->x;

        if (type == 1)
        {
            Module_List[root].set_x(base_x);
        }

        next_x = base_x + width;
        base_y = start->y;

        while (next_x > start->next->x)
        {
            base_y = max(base_y, start->next->y);
            start->next->next->prev = start;
            delete (start->next);
            start->next = start->next->next;
        }

        if (type == 1)
        {
            Module_List[root].set_y(base_y);
        }

        next_y = base_y + height;

        ContourNode *new_node1 = new ContourNode(base_x, next_y);
        ContourNode *new_node2 = new ContourNode(next_x, next_y);
        ContourNode *new_node3 = new ContourNode(next_x, base_y);
        max_x = max(max_x, next_x);
        max_y = max(max_y, next_y);

        new_node3->next = start->next;
        if (start != node_1)
        {
            start->prev->next = new_node1;
            new_node1->prev = start->prev;
            delete (start);
        }
        else
        {
            start->next = new_node1;
            new_node1->prev = start;
        }

        new_node1->next = new_node2;
        new_node2->next = new_node3;

        new_node2->prev = new_node1;
        new_node3->prev = new_node2;
        new_node3->next->prev = new_node3;

        // Check front
        if (new_node1->x == new_node1->prev->x && new_node1->y == new_node1->prev->y)
        {
            new_node1->prev->prev->next = new_node1;
            new_node1->prev = new_node1->prev->prev;
        }

        // Check back
        if (new_node3->x == new_node3->next->x && new_node3->y == new_node3->next->y)
        {
            new_node3->next->next->prev = new_node3;
            new_node3->next = new_node3->next->next;
        }

        if (update == 1 && !stk_contour.empty())
        {
            stk_contour.pop();
            stk_contour.emplace(new_node1);
            update = 0;
        }

        if (Tree_right[root] != -1)
            stk_contour.emplace(new_node1);

        start = new_node3;
        root = Tree_left[root];
    }

    ContourNode *tmp;
    while (node_1 != nullptr)
    {
        tmp = node_1;
        node_1 = node_1->next;
        delete (tmp);
    }

    Area = (double)max_x * max_y;
    W_H_ratio = (double)max_x / max_y;
}

void Contour::set_type(int type) { this->type = type; }

double Contour::get_Area() { return Area; }

double Contour::get_W_H_ratio() { return W_H_ratio; }

vector<Module> Contour::get_Module_List() { return Module_List; }

Contour::~Contour()
{
}