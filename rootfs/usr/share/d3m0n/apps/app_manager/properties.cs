using System;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Collections.Generic;

namespace d3m0n
{
    class properties
    {
        public static Control parseProperties(Control control, string property, string value)
        {
            Dictionary<string, string> temp = new Dictionary<string, string>();
            temp.Add(property, value);
            temp.Add("name", control.Name);
            temp.Add("disabled", "true");

            // need to get d3m0n control name from C# control
            
            return Graphics.layout_to_form("Image", temp, control);
        }
    }
}