#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "app.hpp"

class MainWindow : public Window
{
private:
    Rect ControlFllpti0;
    Text ControlUwk1sa1;
    Switch ControlIiw08r2;
public:
    MainWindow(void) : Window("My App", Size(320), Size(480))
    {
        ControlFllpti0.name = string("control_fllpti");
        ControlFllpti0.visible = true;
        ControlFllpti0.enabled = true;
        ControlFllpti0.location = Point(0, 0);
        ControlFllpti0.width = Size(320);
        ControlFllpti0.height = Size(59);
        ControlFllpti0.color = Color(0xFFFFFFFF);
        ControlFllpti0.bg_color = Color(0xFFCE1212);
        ControlFllpti0.radius = 0;
        ControlUwk1sa1.name = string("control_uwk1sa");
        ControlUwk1sa1.visible = true;
        ControlUwk1sa1.enabled = true;
        ControlUwk1sa1.location = Point(0, 17);
        ControlUwk1sa1.width = Size(100, true);
        ControlUwk1sa1.height = Size(24);
        ControlUwk1sa1.color = Color(0xFFFFFFFF);
        ControlUwk1sa1.bg_color = Color(0x00000000);
        ControlUwk1sa1.content = string("Hello World!");
        ControlUwk1sa1.font_size = 14;
        ControlUwk1sa1.text_align = 0;
        ControlIiw08r2.name = string("control_iiw08r");
        ControlIiw08r2.visible = true;
        ControlIiw08r2.enabled = true;
        ControlIiw08r2.location = Point(92, 139);
        ControlIiw08r2.width = Size(95);
        ControlIiw08r2.height = Size(28);
        ControlIiw08r2.color = Color(0xFFFFFFFF);
        ControlIiw08r2.bg_color = Color(0x00000000);
        ControlIiw08r2.content = string("Switch");
        ControlIiw08r2.font_size = 14;
        ControlIiw08r2.text_align = ANCHOR_LEFT;
        ControlIiw08r2.checked = true;
        this->add_control(&ControlFllpti0);
        this->add_control(&ControlUwk1sa1);
        this->add_control(&ControlIiw08r2);
    }
};

#endif
