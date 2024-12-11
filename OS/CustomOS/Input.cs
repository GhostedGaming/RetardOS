using System;
using Cosmos.System;

namespace MySimpleOS
{
    public class Kernel : Sys.Kernel
    {
        [System.Runtime.InteropServices.DllImport("Bridge")]
        private static extern void handle_command(string command);

        protected override void BeforeRun()
        {
            Console.WriteLine("Initializing kernel...");
            // Initialize hardware components
            Cosmos.HAL.Global.Init();
            Console.WriteLine("Welcome to My Simple OS!");
        }

        protected override void Run()
        {
            Console.Write("> ");
            var command = Console.ReadLine();
            handle_command(command);
        }
    }
}
