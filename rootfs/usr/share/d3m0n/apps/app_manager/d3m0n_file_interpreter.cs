using System;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace d3m0n
{
	class Interpreter
	{
		public static string currentRunningAppPath;

		public static string[] hidden_apps = {};
		public static string[] getHiddenApps()
		{
			return hidden_apps;
		}
		public static string loadApp(string path)
		{
			string foldername = utils.RandomString(20);
			string temp_app_path = utils.getPath()+"/temp/"+foldername;
			string filename = path.Split("/")[path.Split("/").Length-1];
			string mainfile = temp_app_path+"/app";
			

			if(!Directory.Exists(temp_app_path))
			{
				Directory.CreateDirectory(temp_app_path);
			}

			string to_return = "can't load app "+path;
			if(path.EndsWith(".d3m0n"))
			{
				Unzip(path, temp_app_path);

				// open main file
				try
				{
					Console.WriteLine(mainfile);
					if(File.Exists(mainfile))
					{
						string d3m0n_path = utils.getPath();
						string name = utils.getSetting("name", mainfile).Replace("$d3m0n", d3m0n_path);
						string package = utils.getSetting("package", mainfile).Replace("$d3m0n", d3m0n_path);
					    string icon = utils.getSetting("icon", mainfile).Replace("$d3m0n", d3m0n_path);
					    string perms = utils.getSetting("perms", mainfile).Replace("$d3m0n", d3m0n_path);
						string start_path = utils.getSetting("start_path", mainfile).Replace("$d3m0n", d3m0n_path);
						string category = utils.getSetting("category", mainfile).Replace("$d3m0n", d3m0n_path);
						
						string hidden = utils.getSetting("hidden", mainfile).Replace("$d3m0n", d3m0n_path);
						
						if(hidden == "true")
						{
							Array.Resize(ref hidden_apps, hidden_apps.Length + 1);
							hidden_apps[hidden_apps.Length - 1] = name+"//!//"+icon+"//!//"+temp_app_path+"//!//"+perms+"//!//"+start_path+"//!//"+category+"//!//"+package;
							Console.WriteLine("FOUND(hidden): "+name);
						}
						else
						{
							Console.WriteLine("FOUND: "+name);
							to_return = name+"//!//"+icon+"//!//"+temp_app_path+"//!//"+perms+"//!//"+start_path+"//!//"+category+"//!//"+package;
						}
					}
					else
					{
						throw new Exception("error: no main file");
					}
					foreach (string file in Directory.EnumerateFiles(temp_app_path, "*.layout", SearchOption.AllDirectories))
					{
						File.WriteAllText(file, File.ReadAllText(file).Replace("\r\n	", " ").Replace("	", " "));
					}
					foreach (string file in Directory.EnumerateFiles(temp_app_path, "*.src", SearchOption.AllDirectories))
					{
						File.WriteAllText(file, File.ReadAllText(file).Replace("\r\n	", " ").Replace("	", " "));
						File.WriteAllText(file, File.ReadAllText(file).Replace("\r\n}", "}").Replace("\r\n{", "{"));
					}
				}
				catch(Exception e)
				{
					MessageBox.Show("/!\\ file: "+filename+" is incorrect ("+e.ToString()+")");
					Console.WriteLine("error: "+e.ToString());
				}
			}
			return to_return;
		}
		public static void runApp(string temp_path)
		{
			clean(temp_path);
			string ressources_folder = temp_path+"/ressources";
			string sources_folder = temp_path+"/src";
			string layouts_folder = temp_path+"/layouts";
			string icon_file = ressources_folder+"/icon.png";
			string main_layout = layouts_folder+"/main.layout";
			string main_source = sources_folder+"/main.src";

			currentRunningAppPath=temp_path;

			if(File.Exists(main_layout))
			{
				loadSource(main_source);
			}
			if(File.Exists(main_layout))
			{
				loadLayout(main_layout);
			}
			
		}
		public static void clean(string app_path)
		{
			if(Directory.Exists(app_path+"/defs"))
			{
				utils.DeleteDirectory(app_path+"/defs");
			}
			if(Directory.Exists(app_path+"/variable"))
			{
				utils.DeleteDirectory(app_path+"/variable");
			}
		}
		public static void loadLayout(string path)
		{
			string foldername = path.Replace(utils.getPath()+"/temp/", "").Split("/")[0];
			string temp_path = utils.getPath()+"/temp/"+foldername;

			// setting main source file
			string tempname = Path.GetFileName(path).Split(".")[0];
			if(File.Exists(temp_path+"/src/"+tempname+".src"))
			{
				Graphics.src_file = tempname+".src";
			}
			
			// initiating application host window
			Task init = Task.Run(() => Graphics.init(temp_path));
			while(!Graphics.already_init)
			{}

			string[] filelines = File.ReadAllLines(path);
			// check if it's a d3m0n layout file
			if(filelines[0] == "# d3m0n layout")
			{
				foreach(string line in filelines)
				{
					if(line.Trim().Replace(" ", "") != "" && !line.StartsWith("#") && line.Contains(": "))
					{
						Dictionary<string, string> args = new Dictionary<string, string>();

				        string control = line.Split(": ")[0];
				        string[] temp_args = line.Split(": ")[1].Split("\";");

				        foreach(string argument in temp_args)
				        {
				        	string arg="";;
				        	if(!argument.EndsWith('"'))
				        	{
				        		arg=argument+"\"";
				        	}
				        	
				        	// MessageBox.Show("'"+arg+"'");
				        	if(arg.Contains("=\"") && arg.EndsWith('"'))
				        	{
				        		string key = arg.Split("=\"")[0].Replace(" ", "");
				        		string value = utils.RemoveLast(arg.Split("=\"")[1], "\"");
				        		Console.WriteLine("("+control+") "+key+": "+value);
				        		args.Add(key, value);
				        	}

				        }

				        // display d3m0n layout raw text ==> form
				        Graphics.layout_to_form(control, args);
				   	}
			        
				}
			}
			else
			{
				Console.WriteLine("[x] layout must start with '# d3m0n layout'");
			}
		}
		public static void loadEvent(string name, string filename, string app_path)
		{
			if(File.Exists(app_path+"/src/events/"+filename+"/"+name))
			{
				string[] lines = File.ReadAllText((app_path+"/src/events/"+filename+"/"+name)).Split(";");
				foreach(string line in lines)
				{
					script.variable_dir = app_path+"/variable";
					script.execute(line+";");
					// Console.WriteLine(name+": "+line);
				}	
				

			}
			else
			{
				Console.WriteLine("[x] event name '"+name+"' don't exists in "+app_path+"/src/events/"+filename);
			}

		}
		public static void loadSource(string path)
		{
			string foldername = path.Replace(utils.getPath()+"/temp/", "").Split("/")[0];
			string app_path = utils.getPath()+"/temp/"+foldername;
			// Console.WriteLine(app_path);

			string[] filelines = File.ReadAllLines(path);
			// check if it's a d3m0n src file
			if(filelines[0] == "# d3m0n source")
			{
				foreach(string line in filelines)
				{
					if(line.Trim().Replace(" ", "") != "" && !line.StartsWith("#") && line.Contains(" =>"))
					{
				        string eventName = line.Split(" =>")[0];
				        string rawscript = line.Split(" =>")[1];

				        if(!Directory.Exists(app_path+"/src/events/"+ Path.GetFileName(path)))
				        {
				        	Directory.CreateDirectory(app_path+"/src/events/"+ Path.GetFileName(path));
				        }

				        File.WriteAllText(app_path+"/src/events/"+ Path.GetFileName(path)+"/"+eventName, rawscript);

				        string element = eventName.Split(".")[0];
				        string evName = eventName.Split(".")[1];



				        // script.executeScript(eventName, rawscript);
				        // Graphics.source_to_form(control, args);
				   	}
			        
				}
			}
			else
			{
				Console.WriteLine("[x] layout must start with '# d3m0n layout'");
			}
		}
		public static void Unzip(string input, string output)
		{
			Process process = new Process();

			ProcessStartInfo processStartInfo = new ProcessStartInfo();
			processStartInfo.WindowStyle = ProcessWindowStyle.Hidden;
			processStartInfo.FileName = "unzip";
			processStartInfo.Arguments = "'"+input+"' -d '"+output+"'";
			processStartInfo.RedirectStandardOutput = true;
			processStartInfo.RedirectStandardError = true;
			processStartInfo.UseShellExecute = false;

			process.StartInfo = processStartInfo;
			process.Start();

			string error = process.StandardError.ReadToEnd();
			string output2 = process.StandardOutput.ReadToEnd();
		}
		public static void unloadApps()
		{
			string temp_path = utils.getPath()+"/temp";
			

			if(Directory.Exists(temp_path))
			{
				utils.DeleteDirectory(temp_path);
			}
			Console.WriteLine("[o] Temp directory cleaned!");
		}
	}
}