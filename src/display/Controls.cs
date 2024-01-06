using System;
using System.Collections.Generic;

namespace display
{
    public delegate void ClickEventHandler(object sender, EventArgs e);

    public class Control
    {
        // init events
        public event ClickEventHandler Click;
        public event ClickEventHandler MouseDown;
        public event ClickEventHandler MouseUp;
        public event ClickEventHandler MouseMove;

        public string Name;
        public ushort ForeColor;
        public ushort BackColor;
        public DataTypes.Point Location = new DataTypes.Point(0, 0);
        public static DataTypes.Size defaultSize = new DataTypes.Size(50, 25);
        public DataTypes.Size Size = defaultSize;
        public bool Visible = true;
        public List<Control> Controls = new List<Control>();


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

        public DataTypes.Rect getRect()
        {
            int x1 = 240-this.Location.X;
            int y1 = this.Location.Y;

            int x2 = x1-this.Size.Width;
            int y2 = y1+this.Size.Height;

            return new DataTypes.Rect(x2, y1, x1, y2);
        }

        public virtual void Clicked(int x, int y)
        {
            OnClick(EventArgs.Empty);
        }
        public virtual void MouseIsDown(int x, int y)
        {
            onMouseDown(EventArgs.Empty);
        }
        public virtual void MouseIsUp(int x, int y)
        {
            onMouseUp(EventArgs.Empty);
        }
        public virtual void MouseIsMoved(int x, int y)
        {
            onMouseMove(EventArgs.Empty);
        }

        // Method to invoke the Click event
        protected virtual void OnClick(EventArgs e)
        {
           Click?.Invoke(this, e);
        }
        // Method to invoke the MouseDown event
        protected virtual void onMouseDown(EventArgs e)
        {
           MouseDown?.Invoke(this, e);
        }
        // Method to invoke the MouseMove event
        protected virtual void onMouseMove(EventArgs e)
        {
           MouseMove?.Invoke(this, e);
        }
        // Method to invoke the MouseUp event
        protected virtual void onMouseUp(EventArgs e)
        {
           MouseUp?.Invoke(this, e);
        }
    }
    public class Window : Control
    {
        public ushort BackColor = Colors.White;
        public List<Control> Controls = new List<Control>();
        public string Title = "";
        public DataTypes.Size Size = new DataTypes.Size(240, 320);


        public Window()
        {
            this.Controls = new List<Control>();
            ili9340.lcdFillScreen(this.BackColor);
        }


        public void processClick(int x, int y)
        {
            Console.WriteLine("x=" + x.ToString() + " y=" + y.ToString());

            // check if clicked rect
            foreach (Control item in this.Controls)
            {
                if (utils.checkPosition(item.getRect(), new DataTypes.Point(x, y)))
                {
                    Console.WriteLine($"CLICKED222 {item.Name}!!!!");
                    item.Clicked(x, y);
                    break; // Exit the loop after handling the click
                }
            }
        }
    }
}