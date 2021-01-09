using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace CMakeHelper
{
    class CMakeListsEditor
    {
        private string SourceVariable;

        public Regex SourceMather;

        public CMakeListsEditor(string SourceVariable)
        {
            this.SourceVariable = SourceVariable;
            string pattern = @"set\(\s*"+SourceVariable+ @"\s[\S\s]*?\)";
            SourceMather = new Regex(pattern);

        }

        public void Edit(string CMakeListsDirectory, List<string> SourceFiles)
        {

            string cmake_path = CMakeListsDirectory + Path.DirectorySeparatorChar + "CMakeLists.txt";

            string content = "";
            using (StreamReader reader = new StreamReader(cmake_path))
            {
                content = reader.ReadToEnd();
                reader.Close();
            }
            string SourceSetterString = CreateSourceList(CMakeListsDirectory,SourceFiles);
            content = SourceMather.Replace(content, SourceSetterString);

            using (StreamWriter writer = new StreamWriter(cmake_path))
            {
                writer.Write(content);
                writer.Close();
            }

        }

        public string CreateSourceList(string CMakeListsDirectory, List<string> SourceFiles)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("set(");
            sb.Append(SourceVariable);
            sb.Append("\n");
            foreach(string source_file in SourceFiles)
            {
                sb.Append("\"");
                
                string relativ_path = Path.GetRelativePath(CMakeListsDirectory, source_file);
                relativ_path = relativ_path.Replace(Path.DirectorySeparatorChar.ToString(), "/");
                sb.Append(relativ_path);
                sb.Append("\"");
                sb.Append("\n");
            }
            sb.Append(")");
            return sb.ToString();
        }

    }
}
