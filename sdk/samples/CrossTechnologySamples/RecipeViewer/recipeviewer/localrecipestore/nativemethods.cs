using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.Samples.RecipeCatalog.LocalStore
{
    public class NativeMethods
    {
        const int MAX_PATH = 260;

        // BOOL PathYetAnotherMakeUniqueName( LPWSTR pszUniqueName,
        //                      LPCWSTR pszPath,
        //                      LPCWSTR pszShort,
        //                      LPCWSTR pszFileSpec);
        [DllImport("shell32.dll", CharSet = CharSet.Unicode)]
        static extern int PathYetAnotherMakeUniqueName(StringBuilder pszUniqueName, string pszPath, string pszShort, string pszFileSpec);

        static public string GetUniqueFilename(string dirPath, string fileSpec)
        {
            StringBuilder buffer = new StringBuilder(MAX_PATH);
            int result = PathYetAnotherMakeUniqueName(buffer, dirPath + @"\" + fileSpec, null, fileSpec);

            return buffer.ToString();
        }
    }
}
