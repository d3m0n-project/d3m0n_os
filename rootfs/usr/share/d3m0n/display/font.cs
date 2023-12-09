using System;
using System.IO;
using System.Drawing;

namespace display
{
    class font
    {
        public static void WriteString(string text, int fontSize, int horizontal_padding, ushort x, ushort y, ushort color, bool center, ushort width, ushort height)
        {
            int max_chars_per_line = width/(fontSize+horizontal_padding);
            int max_lines = height/(2*fontSize);

            // center text horizontaly
            if(center)
            {
                x-=(ushort)(width/2-(text.Length*fontSize+(text.Length-1)*horizontal_padding)/2);
            }
            

            Console.WriteLine("Max chars par line: "+max_chars_per_line.ToString());
            // x  fontSize  horizontal_padding
            // <> <->       <----->
            //    ...
            //    . .
            //    ...
            //    
            string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


            int pos=0;
            foreach (char letter in text)
            {
                pos++;
                if(pos>max_chars_per_line)
                {
                    y+=(ushort)(2*fontSize);
                    x+=width;
                    pos=1;
                }

                string bitmap_path="";
                bool needYCalibration=false;
                // check if letter is uppercase
                if(uppercase.Contains(letter.ToString()))
                {
                    needYCalibration=true;
                    // bitmap_path = d3m0n.utils.getPath()+"/display/letters/"+letter.ToString()+"_upper.bmp";
                    bitmap_path = "./letters/"+letter.ToString()+"_upper.bmp";
                }
                else
                {
                    // bitmap_path = d3m0n.utils.getPath()+"/display/letters/"+letter.ToString()+".bmp";
                    bitmap_path = "./letters/"+letter.ToString()+".bmp";
                }
                
                
                if(File.Exists(bitmap_path))
                {
                    Bitmap bmp2 = new Bitmap(bitmap_path);
                    // resize from 100px to fontSize 
                    Bitmap bmp = utils.ResizeBitmap(bmp2, fontSize, 2*fontSize);
                    for (int ii = 0; ii < bmp.Width; ii++)
                    {
                        for (int jj = 0; jj < bmp.Height; jj++)
                        {
                            Color pixelColor = bmp.GetPixel(ii, jj);
                            // check 
                            if(pixelColor.Name != "ffffffff")
                            {
                                int x1 = (int)x-ii;
                                int y1 = (int)y+jj;
                                // move down uppercase letters
                                if(needYCalibration) { y1+=10; }
                                ili9340.lcdDrawPixel((ushort)x1, (ushort)y1, color);
                            }
                        }
                    }
                    x-=(ushort)(bmp.Width+horizontal_padding);
                }
                else
                {
                    Console.WriteLine("ERROR: font.cs => can't find char '"+letter.ToString()+"'");
                }
            }
        }
    }
}