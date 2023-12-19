#include "Module.h"

Module::Module()
{
    name = "";
    width = 0;
    height = 0;
}

Module::Module(string n, int w, int h)
{
    name = n;
    width = w;
    height = h;
}

void Module::set_x(int x) { this->x = x; }
void Module::set_y(int y) { this->y = y; }

string Module::get_name() { return name; };

int Module::get_x() { return x; }
int Module::get_y() { return y; }

int Module::get_width() { return width; }
int Module::get_height() { return height; }

Module::~Module()
{
}
