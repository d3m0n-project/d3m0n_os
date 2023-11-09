using System;
using System.IO;
using System.Text;
using System.Threading;
// using Mono.Unix.Native;
using System.Threading.Tasks;
using d3m0n;

namespace d3m0n_modules
{
    public partial class IR
    {
        public static void parse(string command, string[] args)
        {
            switch(command)
            {
                case "IR.play":
                    //play(args[0]);
                    break;
                case "IR.read":
                    // read();
                    break;
            }
        }
    }
}