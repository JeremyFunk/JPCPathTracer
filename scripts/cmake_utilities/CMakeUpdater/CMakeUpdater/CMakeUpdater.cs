using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace CMakeHelper
{
    class CMakeUpdater
    {
        public string SourceVariable = "AUTOSRC";

        public DirectoryScanner scanner;

        public CMakeUpdater(DirectoryScanner scanner)
        {
            this.scanner = scanner;
        }
        public void UpdateForSource(string root_directory,bool verbose = false)
        {
            CMakeListsEditor editor = new CMakeListsEditor(SourceVariable);

            
            List<string> filelist = scanner.Scan(root_directory,verbose);
            //Assumption: CMakeLists of a directory always the first elment in the list 
            //Assumption: Subfolders of a directory always the last element in the list
            for (int index = 0; index < filelist.Count; index++)
            {
                if(filelist[index].EndsWith("CMakeLists.txt"))
                {
                    
                    string cmake_filepath = filelist[index];
                    string cmake_dir = Path.GetDirectoryName(cmake_filepath);
                    List<string> source = new List<string>();
                    index++;
                    for (; index < filelist.Count; index++)
                    {
                        string file = filelist[index];
                        if (file.StartsWith(cmake_dir) && (!file.EndsWith("CMakeLists.txt")))
                        {

                            source.Add(file);
                        }
                        else
                        {
                            index--;
                            break;
                        }
                    }
                    editor.Edit(cmake_dir, source);


                }
                
                
            }

        }
    }
}
