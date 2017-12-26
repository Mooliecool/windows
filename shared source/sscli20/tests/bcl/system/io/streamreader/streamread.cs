// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
using System;
using System.Security;
using System.Security.Policy;
using System.IO;
using System.Collections;
using System.Text;
class StringTest
{
	public static void Main( String[] args )
	{
        int iCountErrors = 0 , iCountTestcases = 0 ;
		try{
            iCountTestcases++ ;
            String strTemp = CreatePolicyLevel( "webhightrust.config", "c:\\foo\\bar", "c:\\bar\\foo", "http://www.microsoft.com");
            FileStream file = new FileStream( "webhightrust.config", FileMode.Open, FileAccess.Read );
            StreamReader reader = new StreamReader( file, Encoding.UTF8 );
            String strFileContent = reader.ReadToEnd();
            strFileContent = ReplaceWithFilePaths( strFileContent, "c:\\foo\\bar", "c:\\bar\\foo", "http://www.microsoft.com");
            if ( String.Compare(strFileContent, 0, strTemp, 0,(int) file.Length) != 0) {
                iCountErrors++;
                Console.WriteLine("Error_1111!!!! StreamReader.Read method returned unexpected buffer");
            }
            reader.Close();
        } catch(Exception e){
            iCountErrors++;
            Console.WriteLine("Error_2222!!!! Unexpected exception occured....e.Message:"+ e.Message);
        }
        CreateLogFile( iCountTestcases, iCountErrors );
        if( iCountErrors == 0 ) Console.WriteLine("Test Passed"); else Console.WriteLine("Test FAILED");
	}
    public static void CreateLogFile (Int32 iCountTestcases, Int32 iCountErrors)
    {
        FileStream fs = new FileStream("results.txt", FileMode.Create, FileAccess.Write);
        StreamWriter w = new StreamWriter(fs);
        w.WriteLine ("<Testcase>");
        if (iCountErrors > 0)
        {
            w.WriteLine ("	<FinalResults type=\"Fail\" total=\"{0}\" fail=\"{1}\"/>", iCountTestcases.ToString(), iCountErrors.ToString());
        }
        else
        {
            w.WriteLine ("\t<FinalResults type=\"Pass\" total=\"{0}\" fail=\"{1}\"/>", iCountTestcases.ToString(), iCountErrors.ToString());
        }
        w.WriteLine ("</Testcase>");
        w.Close();
    }
    internal static String CreatePolicyLevel( String configFile, String appDir, String binDir, String strOriginUrl) {
        FileStream file = new FileStream( configFile, FileMode.Open, FileAccess.Read );
        StreamReader reader = new StreamReader( file, Encoding.UTF8 );
        char[] cFileData = new char[(int)file.Length];
        reader.Read( cFileData, 0, (int)file.Length );
        String strFileData = new String( cFileData );
        if (appDir.EndsWith("\\"))
            appDir = appDir.Substring(0, appDir.Length - 1);
        if (binDir.EndsWith("\\"))
            binDir = binDir.Substring(0, binDir.Length - 1);
        strFileData = ReplaceWithFilePaths( strFileData, appDir, binDir, strOriginUrl);
        return strFileData;
	}
    private static String ReplaceWithFilePaths( String strFileData,  String appDir, String binDir, String strOriginUrl){
        strFileData = strFileData.Replace("$AppDir$", appDir);
        strFileData = strFileData.Replace("$AppDirUrl$", MakeFileUrl(appDir));
        strFileData = strFileData.Replace("$CodeGen$", MakeFileUrl(binDir));
        if (strOriginUrl != null)
            strFileData = strFileData.Replace("$OriginUrl$", strOriginUrl);
        return strFileData ;
    }
    private static String MakeFileUrl(String path) {
        return "file://" + path.Replace('\\', '/');
    }
}
