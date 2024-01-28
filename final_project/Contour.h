#ifndef CONTOUR_H
#define CONTOUR_H

#include <iostream>
#include <vector>
#include <stack>

#include "Module.h"

using namespace std;

struct ContourNode
{
    int x, y;
    ContourNode *prev;
    ContourNode *next;
    ContourNode() : x(-1), y(-1), prev(nullptr), next(nullptr) {}
    ContourNode(int x, int y) : x(x), y(y), prev(nullptr), next(nullptr) {}
    ContourNode(int x, int y, ContourNode *prev, ContourNode *next) : x(x), y(y), prev(prev), next(next) {}
};

class Contour
{
private:
    vector<Module> Module_List;

    int Root;
    vector<int> Tree_left;
    vector<int> Tree_right;
    vector<int> Tree_prev;
    vector<int> Rotate;

    int max_x = 0;
    int max_y = 0;

    double Area, W_H_ratio;

    vector<int> Block_x;
    vector<int> Block_y;
    int type = 0;

public:
    Contour(/* args */);
    Contour(vector<Module> Module_List);

    void Update(int Root, vector<int> Tree_left, vector<int> Tree_right, vector<int> Tree_prev, vector<int> Rotate);
    void Generate_Graph();

    void set_type(int type);
    double get_Area();
    double get_W_H_ratio();

    vector<Module> get_Module_List();

    ~Contour();
};

#endif