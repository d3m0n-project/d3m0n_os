#ifndef CTRL_TYPE
#define CTRL_TYPE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <ili9340.h>
#include <vector>
using namespace std;


namespace DataTypes
{
    struct Rect {
        int X;
        int Y;
        int X2;
        int Y2;
        Rect(int x, int y, int x2, int y2) {
            this->X = x;
            this->Y = y;
            this->X2 = x2;
            this->Y2 = y2;
        }
    };
    struct Point {
        int X;
        int Y;
        Point(int x, int y) {
            this->X = x;
            this->Y = y;
        }
        Point() : X(0), Y(0) {}
    };
    struct Size {
        int Width;
        int Height;
        Size(int x, int y) {
            this->Width = x;
            this->Height = y;
        }
        Size() : Width(0), Height(0) {}
    };
    struct Control {
        string name;
        DataTypes::Point Location = DataTypes::Point(0, 0);
        DataTypes::Size size = DataTypes::Size(100, 50);
        bool Visible=true;
        bool Enabled=true;
        ushort BackColor = 0x0000; 
        ushort ForeColor = 0xffff;

        Control() {}

        virtual void draw() {}; // Draws the control on screen.

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
    struct Window {
        DataTypes::Size size;
        ushort BackColor = 0x0000; 
        ushort ForeColor = 0xffff;

        vector<Control*> Controls;

        void Add(Control* ctrl) {
            Controls.push_back(ctrl);
        }

        void Update() {
            printf("Updating window\n");
            // lcdFillScreen(BackColor);
            size_t size = Controls.size();
            printf("Size of Window's Controls vector: %zu\n", size);
            
            for (Control* control : Controls) {
                if(control != nullptr) {
                    printf(("name: "+control->name+"\n").c_str());
                    control->draw();
                }
            }
        }

    };

    struct BitmapPoint {
        int x;
        int y;
        ushort color;
    };
    struct Bitmap {
        vector<BitmapPoint> points;
        DataTypes::Size size;
    };
};
#endif