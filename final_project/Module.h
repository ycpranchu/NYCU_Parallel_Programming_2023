#ifndef MODULE_H_
#define MODULE_H_

#include <string>
#include <vector>

using namespace std;

class Module
{
private:
    int index;
    int width;
    int height;
    int x, y;

public:
    Module();
    Module(int n, int w, int h);

    void set_x(int x);
    void set_y(int y);

    int get_index();

    int get_x();
    int get_y();

    int get_width();
    int get_height();

    ~Module();
};

#endif