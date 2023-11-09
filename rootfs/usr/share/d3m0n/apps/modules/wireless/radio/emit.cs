using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using d3m0n;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace d3m0n_modules
{
    public partial class wireless
    {
        public static void radio_emit_stop()
        {
            utils.logn("[o] stopped emiting radio waves", ConsoleColor.Green);
            execute("sudo pkill -f pi_fm_rds");
        }
        public static void radio_emit(float freq, string soundFile, string radioName, string radioText)
        {
            // check if PiFmRds is installed
            if(File.Exists(utils.getPath()+"/lib/PiFmRds"))
            {
                utils.logn("[~] Playing file '"+soundFile+"' on "+freq.ToString(), ConsoleColor.Magenta);
                // play audio file
                execute("sudo '"+utils.getPath()+"/lib/pi_fm_rds' -freq "+freq.ToString()+" -audio "+soundFile+" -ps "+radioName+" -rt "+radioText);
                
            }
            else
            {
                utils.logn("[x] PiFmRds isn't installed", ConsoleColor.Red);
                // downloads PiFmRds
                execute("cd '"+utils.getPath()+"/lib' && git clone https://github.com/ChristopheJacquet/PiFmRds.git && cd PiFmRds/src && make clean && make");

                // create shotcut to /lib/PiFmRds folder
                execute("ln -s '"+utils.getPath()+"/lib/pi_fm_rds' '"+utils.getPath()+"/lib/PiFmRds/src/pi_fm_rds'");
            }
        }

        private static void execute(string command)
        {
            Process process = new Process();

            string filename = d3m0n.script.getString(command.Split(" ")[0]);
			string commandname = d3m0n.script.getString(command.Replace(filename, ""));


			ProcessStartInfo processStartInfo = new ProcessStartInfo();
			processStartInfo.WindowStyle = ProcessWindowStyle.Hidden;
			processStartInfo.FileName = filename;
			processStartInfo.Arguments = commandname;
			processStartInfo.RedirectStandardOutput = true;
			processStartInfo.RedirectStandardError = true;
			processStartInfo.UseShellExecute = false;

			process.StartInfo = processStartInfo;
			process.Start();

			string error = process.StandardError.ReadToEnd();
			string output2 = process.StandardOutput.ReadToEnd();
        }
    }
}