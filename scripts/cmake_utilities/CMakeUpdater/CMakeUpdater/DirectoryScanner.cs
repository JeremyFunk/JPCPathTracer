using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;


namespace CMakeHelper
{
    class DirectoryScanner
    {
        private List<String> ExcludedFolders;
        private List<String> ExcludedFileNames;
        private List<string> SearchOptions;

        public DirectoryScanner(List<string> ExcludedFolders,
            List<string> SearchOptions, List<string> ExcludedFileNames)
        {
            this.ExcludedFolders = ExcludedFolders;
            this.SearchOptions = SearchOptions;
            this.ExcludedFileNames = ExcludedFileNames;
        }

        public List<string> Scan(string directory,bool verbose = false)
        {
            List<string> files = new List<string>();
            foreach(string searchoption in SearchOptions)
            { 
                files.AddRange(Directory.GetFiles(directory, searchoption));
            }
            
            foreach(string subdir in Directory.GetDirectories(directory))
            {

                

                if (! ExcludedFolders.Exists(foldername => subdir.EndsWith(foldername) || subdir.Contains(Path.DirectorySeparatorChar+".")))
                {
                    if (verbose) Console.WriteLine($"Search: {subdir}");
                    files.AddRange(Scan(subdir));
                }
            }
            return files;
        }

        public List<string> ScanForExcluded(string directory)
        {
            List<string> files = new List<string>();
            foreach (string filename in ExcludedFileNames)
            {
                files.AddRange(Directory.GetFiles(directory, filename));
            }

            foreach (string subdir in Directory.GetDirectories(directory))
            {
                if (ExcludedFolders.Exists(foldername => subdir.EndsWith(foldername) ))
                {
                    files.Add(subdir);
                }else if(! subdir.Contains(Path.DirectorySeparatorChar+ "."))
                {
                    files.AddRange(ScanForExcluded(subdir));
                }
            }
            return files;
        }
    }
}
