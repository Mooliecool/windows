using System;
using System.Text;
using System.IO;

namespace PrivilegedExe
{
    class Program
    {
        static int Main(string[] args)
        {
            int retVal = 1;

            try
            {
                string folderPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles), "UACDemo");
                Directory.CreateDirectory(folderPath);
                string filePath = Path.Combine(folderPath, "log.txt");

                using (TextWriter tw = new StreamWriter(filePath, true))
                {
                    tw.WriteLine("entry added from privileged executable at " + DateTime.Now);
                }

                retVal = 0;
            }
            catch (System.Security.SecurityException secEx)
            {
                retVal = 1;
            }
            catch (UnauthorizedAccessException authEx)
            {
                retVal = 2;
            }
            catch (Exception ex)
            {
                retVal = 3;
            }
            Console.WriteLine("PrivelegedExe completed with return code: {0}", retVal.ToString());

            return (retVal);
        }
    }
}
