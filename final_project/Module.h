#ifndef MODULE_H_
#define MODULE_H_

#include <string>

using namespace std;

class Module
{
private:
    string name;
    int width;
    int height;
    int x, y;

public:
    Module();
    Module(string n, int w, int h);

    void set_x(int x);
    void set_y(int y);

    string get_name();

    int get_x();
    int get_y();

    int get_width();
    int get_height();

    ~Module();
};

#endif