using System;
using System.Collections.Generic;

namespace display
{
    public class Control
    {
        public string Name;
        public DataTypes.Point Location = new DataTypes.Point(0, 0);
        public static DataTypes.Size defaultSize = new DataTypes.Size(50, 25);
        public DataTypes.Size Size = defaultSize;

        public DataTypes.Rect getRect()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;

            return new DataTypes.Rect(x2, y1, x1, y2);
        }
    }
    public class Window
    {
        public List<Control> Controls = new List<Control>();


        public Window()
        {
            this.Controls = new List<Control>();
        }

        public Control Get(string name)
        {
            foreach (Control control in Controls)
            {
                if(control.Name == name)
                {
                    return control;
                }
            }
            return new Control();
        }

        public void Add(Control control)
        {
            this.Controls.Add(control);
        }
    }
}