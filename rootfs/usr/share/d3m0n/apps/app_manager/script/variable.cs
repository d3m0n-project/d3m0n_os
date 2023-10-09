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
		public static string variable_dir = utils.getPath()+"/temp/null/variable";

		public static void setVariable(string name, string value)
		{
			init();
			value=value.TrimEnd(';');
			utils.logn(name+" => "+value, ConsoleColor.Magenta);

			if(name.Contains("."))
			{
				variable_property(name, value.TrimStart('"').TrimEnd('"'));
			}
            else if(value.StartsWith("\"") && value.EndsWith("\""))
            {
            	File.WriteAllText(variable_dir+"/"+name, value);
            	Console.WriteLine("var ("+name+") is new equal to "+value);
            }
			// external dll function
			else if(value.StartsWith("[") && value.EndsWith("];"))
            {
				// [/path/to/a.dll,myNamespace,myclass,myFunct(a, b)];

            	string content             = getString(value.Split("[")[1].Split("]")[0]);
	            // MessageBox.Show(content);
	            string file_name           = getString(content.Split(",")[0]);
            	string namespace_name      = content.Split(",")[1];
	            string class_name          = content.Split(",")[2];
	            string raw_function_name   = content.Split(",")[3];
	            string function_name       = raw_function_name.Split("(")[0];
		        string[] function_args     = raw_function_name.Split("(")[1].Replace(")", "").Split(",");

				int i=0;
				foreach(string arg in function_args)
				{
					function_args[i]=function_args[i].TrimStart(' ').TrimEnd(' ');
					Console.WriteLine("argument: "+arg);
					i++;
				}



            	try
            	{
            		Assembly asm = Assembly.LoadFile(file_name);

		            object obj = asm.CreateInstance(namespace_name+"."+class_name);
					MethodBase method = obj.GetType().GetMethod(function_name);



					ParameterInfo[] parameters = method.GetParameters();

					List<object> args = new List<object>();

					//Get and display the ParameterInfo of each parameter.
					int j=0;
					foreach (ParameterInfo param in parameters)
					{
						args.Add(getType(param.ParameterType, function_args[j]));
						// MessageBox.Show("\nFor parameter '" + "x" + "', the ParameterType is - " + param.ParameterType);
						j++;
					}
					
					object[] final_arguments = args.ToArray();

		            switch(function_args.Length)
					{
						case 0:
							method.Invoke(null, null);
							break;
						case 1:
							if(function_args[0] == "")
							{
								method.Invoke(null, null);
							}
							else
							{
								method.Invoke(null, final_arguments);
							}
							break;
						// case 2:
						// 	obj.GetType().GetMethod(function_name).Invoke(null, new object[]{function_args[0]}, new object[]{function_args[1]});
						// 	break;
						// case 3:
						// 	obj.GetType().GetMethod(function_name).Invoke(null, null);
						// 	break;
						// case 4:
						// 	obj.GetType().GetMethod(function_name).Invoke(null, null);
						// 	break;
						// case 5:
						// 	obj.GetType().GetMethod(function_name).Invoke(null, null);
						// 	break;
					}

            	}
            	catch(Exception e)
            	{
            		utils.logn("[x] error while loading asm => '"+file_name+"' ("+namespace_name+"."+class_name+"."+function_name+")", ConsoleColor.Red);
            		Console.WriteLine(e.ToString());
            	}
            	
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