using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace display
{
    
    public partial class utils
    {
        public static bool checkPosition(DataTypes.Rect hitbox, DataTypes.Point pos)
        {
            // origin of rect
            int right = hitbox.X;
            int top = hitbox.Y;

            int left = hitbox.X2;
            int bottom = hitbox.Y2;

            // check if in rect
            if(pos.X >= right && pos.X <= left && pos.Y >= top && pos.Y <= bottom )
            {
                return true;
            }
            return false;
        }
    }
}