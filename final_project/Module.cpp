#include "Module.h"

Module::Module()
{
}

Module::Module(int n, int w, int h)
{
    index = n;
    width = w;
    height = h;
}

void Module::set_x(int x) { this->x = x; }
void Module::set_y(int y) { this->y = y; }

int Module::get_index() { return index; };

int Module::get_x() { return x; }
int Module::get_y() { return y; }

int Module::get_width() { return width; }
int Module::get_height() { return height; }

Module::~Module()
{
}
