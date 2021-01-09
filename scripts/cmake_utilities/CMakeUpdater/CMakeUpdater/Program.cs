using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace CMakeHelper
{
    class Program
    {
        static void Main(string[] args)
        {
            string directory_input = ".";
            if (args.Length > 0)
            {
                directory_input = args[0];
            }
            if(args.Length > 0 && args[0] == "-h")
            {
                Console.WriteLine("CMakeUpdater <directory to CMakeLists.txt>");
                return;
            }
            bool verbose = false;
            if(args.Length > 1 && args[1] == "-v")
            {
                verbose = true;
            }
            //directory_input = @"H:\dev Übung\repos\CMakeCpp\SubModulesTest2";

           
            string root_directory = Path.GetFullPath(directory_input);

            List<string> excludedirs = new List<string> { "cmake", "CMakeFiles", "build", "extern","Release","libs", "Debug", "_deps", ".dir" };
            List<string> extensions = new List<string> { "CMakeLists.txt","*cu","*cuh", "*.cpp", "*.h" };
            List<string> excludefiles = new List<string> { "*.dir", "cmake_install.cmake", "CTestTestfile.cmake", "CMakeCache.txt" };

            DirectoryScanner scanner = new DirectoryScanner(excludedirs, extensions, excludefiles);

            CMakeUpdater updater = new CMakeUpdater(scanner);
            updater.UpdateForSource(root_directory, verbose);

            ////In Source Build
            //var processInfo = new ProcessStartInfo
            //{
            //    FileName = "cmake",
            //    Arguments = $" -S \"{root_directory}\" -b \"{root_directory}\" {cmake_args}"
            //};
            //
            //Console.WriteLine("Starting cmake...");
            //using (var process = Process.Start(processInfo))
            //{
            //    process.WaitForExit();
            //}
            ////Hide
            //foreach (string path in scanner.ScanForExcluded(root_directory))
            //{
            //    File.SetAttributes(path, FileAttributes.Hidden);
            //}


        }
    }
}
