using System;

namespace display
{
    public class DataTypes
    {
        public struct Point
        {
            public int X;
            public int Y;

            public Point(int x, int y)
            {
                X = x;
                Y = y;
            }
        }
        public struct Size
        {
            public int Width;
            public int Height;

            public Size(int x, int y)
            {
                Width = x;
                Height = y;
            }
        }
        public struct Rect
        {
            public int X;
            public int Y;
            public int X2;
            public int Y2;

            public Rect(int x, int y, int x2, int y2)
            {
                X=x;
                Y=y;
                X2 = x2;
                Y2 = y2;
            }
        }
    }
}