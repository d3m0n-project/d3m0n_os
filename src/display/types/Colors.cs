using System;
using System.Drawing;

namespace display
{
    public class Colors
    {
        public static ushort Red = 0xf800;
        public static ushort Green = 0x07e0;
        public static ushort Blue  = 0x001f;
        public static ushort Black = 0x0000;
        public static ushort White = 0xffff;
        public static ushort Gray  = 0x8c51;
        public static ushort Yellow = 0xFFE0;
        public static ushort Cyan  = 0x07FF;
        public static ushort Magenta = 0xF81F;
        public static ushort Transparent = 0x0;

        public static ushort getRGB565(int red, int green, int blue)
        {
            // Ensure that the color values are within the valid range (0-255)
            red = Math.Clamp(red, 0, 255);
            green = Math.Clamp(green, 0, 255);
            blue = Math.Clamp(blue, 0, 255);

            // Convert the RGB values to 5-bit red, 6-bit green, and 5-bit blue
            ushort r = (ushort)((red >> 3) & 0x1F);
            ushort g = (ushort)((green >> 2) & 0x3F);
            ushort b = (ushort)((blue >> 3) & 0x1F);

            // Combine the bits to form the RGB565 color code
            ushort rgb565 = (ushort)((r << 11) | (g << 5) | b);

            return rgb565;
        }

        public static ushort getColor(Color color)
        {
            return (ushort)getRGB565(color.R, color.G, color.B);
        }
    }
}