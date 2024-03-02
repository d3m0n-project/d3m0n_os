#ifndef CTRL_TYPE
#define CTRL_TYPE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../../utils.h"
#include "../../../display.hpp"
using namespace std;

struct Window {
    Size size;
    ushort BackColor = Colors::Black; 
    ushort ForeColor = Colors::White;

};

struct BitmapPoint {
    int x;
    int y;
    ushort color;
};
struct Bitmap {
    vector<BitmapPoint> points;
    Size size;
};


struct Control {
    int type;
    int id;
    string name;
    Point Location;
    Size size;
    int Width;
    int Height;
    bool Visible=true;
    bool Enabled=true;
    ushort BackColor;
    ushort ForeColor;

    Control() {
        
    }

    Rect getRect()
    {
        int x1 = 240-this->Location.X;
        int y1 = this->Location.Y;

        int x2 = x1-this->size.Width;
        int y2 = y1+this->size.Height;

        x1-=this->size.Width;
        x2-=this->size.Width;

        return Rect(x2, y1, x1, y2);
    }
};

#endif