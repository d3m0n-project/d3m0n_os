using System;
using System.Runtime.InteropServices;

namespace display
{
    public static class xpt
    {
        [DllImport("./lib.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void xptGetxy(int pin, ref int x, ref int y);
    }
}