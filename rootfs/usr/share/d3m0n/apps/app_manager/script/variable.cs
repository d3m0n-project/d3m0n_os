using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;

namespace d3m0n
{
	public partial class script
	{
		public static string variable_dir = utils.getPath()+"/temp/null/variable";

		public static void setVariable(string name, string value)
		{
			init();
			value=value.TrimEnd(';');
			utils.logn(name+" => "+value, ConsoleColor.Magenta);
            if(value.StartsWith("\"") && value.EndsWith("\""))
            {
            	File.WriteAllText(variable_dir+"/"+name, value);
            	Console.WriteLine("var ("+name+") is new equal to "+value);
            }	
            else
            {
            	File.WriteAllText(variable_dir+"/"+name, getVariable(value));
            	Console.WriteLine("var ("+name+") is new equal to "+getVariable(value));
            }
            	
		}
		public static void delVariable(string name)
		{
			init();
            if(File.Exists(variable_dir+"/"+name))
            {
            	File.Delete(variable_dir+"/"+name);
            }
		}
		public static bool varExists(string name)
		{
			init();
			if(File.Exists(variable_dir+"/"+name))
            {
            	return true;
            }
            else
            {
            	return false;
            }
		}
		public static string getVariable(string name)
		{
			init();
            if(File.Exists(variable_dir+"/"+name))
            {
            	return File.ReadAllText(variable_dir+"/"+name);
            }
            else
            {
            	return "var not declared";
            }
		}
		private static void init()
		{
			if(!Directory.Exists(variable_dir))
			{
				Directory.CreateDirectory(variable_dir);
			}
		}
	}
}