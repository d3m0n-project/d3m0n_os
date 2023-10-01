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
		public static void execute(string line)
		{
			string app_path = Interpreter.currentRunningAppPath;

			while(line.StartsWith(' ') || line.EndsWith(' '))
			{
				line=line.TrimStart(' ').TrimEnd(' ');
			}


            // Console.WriteLine("'"+line+"'");



			if(line.StartsWith("#"))
            {}
            else if(line.StartsWith("def "))
            {
            	string def_name = line.Replace("def ", "").TrimEnd('}').Split("{")[0];
            	string def_script = line.Replace("def ", "").TrimEnd('}').Split("{")[1].Replace("  ", "\r\n");
            	if(!Directory.Exists(app_path+"/defs"))
            	{
            		Directory.CreateDirectory(app_path+"/defs");
            	}
            	File.WriteAllText(app_path+"/defs/"+def_name, def_script);
            	utils.logn("[o] created def "+def_name, ConsoleColor.Green);
            }
            else if(line.StartsWith("use"))
            {
            	string path = line.Split(">")[1];
            	// MessageBox.Show("using "+path);
            	script.importLib(getString(path).Replace(";", ""));
            }
            else if(line.Contains("="))
            {
            	string name = line.Split("=")[0].Replace(" ", "");
            	string value = line.Split("=")[1];
            	script.setVariable(name, value);
            }
            else if(line.Contains("(") && line.EndsWith(");"))
            {
            	string name = line.Split("(")[0];
            	string[] value = line.Split("(")[1].Replace(");", "").Split(", ");
            	// MessageBox.Show(name+": "+value[0].ToString());
            	script.callFunction(name, value);
            }
            	
		}
		public static string getString(string raw)
		{
			string app_path = Interpreter.currentRunningAppPath;

			string ressources = app_path+"/ressources";
			string source = app_path+"/src";
			string layouts = app_path+"/layouts";

			string to_return=raw.TrimStart(' ').TrimStart('"').TrimEnd('"').Replace("$d3m0n", utils.getPath()).Replace("$source", source).Replace("$layouts", layouts).Replace("$ressources", ressources);


			// utils.logn(to_return, ConsoleColor.Magenta);
			foreach(string word in to_return.Split(" "))
			{
				if(word.StartsWith('$') && varExists(word.TrimStart('$')))
				{
					// MessageBox.Show(word);
					to_return=to_return.Replace(word, getVariable(word.TrimStart('$')));
				}
			}
			
			return to_return;
		}
	}
}