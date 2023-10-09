using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Reflection;

namespace d3m0n
{
	public partial class script
	{
        public static void variable_property(string name, string value)
        {
            if(name.StartsWith("app.layout."))
			{
                try
                {

                    string property_name = name.Replace("app.layout.", "").Split('.')[name.Replace("app.layout.", "").Split('.').Length-1];
                    string element_name = name.Replace("app.layout.", "").Replace(property_name, "").TrimEnd('.');

                    utils.logn(element_name+"  =>  "+property_name, ConsoleColor.Magenta);
                    utils.logn("       = '"+value+"'", ConsoleColor.Magenta);
                    

                    app_display temp = Graphics.into as app_display;
                    temp.setControlProperty(element_name, property_name, value);
                }
                catch(Exception)
                {
                    utils.logn("[x] error app.layout.", ConsoleColor.Red);
                }
			}
        }
    }
}