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
using System.IO;
using System.Collections;
using System.Globalization;
public class Co9050ChangeExtension_str_str
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.ChangeExtension(String,String)";
    public static String s_strTFName        = "Co9050ChangeExtension_str_str.cs";
    public static String s_strTFAbbrev      = "Co9053";
    public static String s_strTFPath        = Environment.CurrentDirectory;
    public bool runTest()
    {
        Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strLoc = "Loc_000oo";
        String strValue = String.Empty;
        try
        {
            String strExtension;
            String extension ;
            String strPathName;
            strLoc = "Loc_985yb";
            iCountTestcases++;
            try 
            {
                strExtension = Path.ChangeExtension(null , ".exe");
                if( strExtension != null )
                {
                    iCountErrors++;
                    printerr( "Error_4443! Unexpected return value:" + strExtension);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_019ux! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_00443";
            iCountTestcases++;
            try 
            {
                strExtension = Path.ChangeExtension("" , ".exe");
                if( strExtension.Equals(".exe") )
                {
                    iCountErrors++;
                    printerr( "Error_4432! Unexpected return value:" + strExtension);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_432443! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_985yb";
            iCountTestcases++;
            try 
            {
                strExtension = Path.ChangeExtension("        " , ".exe");
                if( strExtension.Equals(".exe") )
                {
                    iCountErrors++;
                    printerr( "Error_9000! Unexpected return value:" + strExtension);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_545! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_985yb";
            iCountTestcases++;
            try 
            {
                Path.ChangeExtension(@"><" , ".exe");
                iCountErrors++;
                printerr( "Error_34543! Expected Exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_66645! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_7879! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_985yb";
            iCountTestcases++;
            try 
            {
                Path.ChangeExtension("\"|" , ".exe");
                iCountErrors++;
                printerr( "Error_4234! Expected Exception not thrown");
            } 
            catch (ArgumentException aexc) 
            {
                printinfo("Info_42646! Caught expected exception, aexc=="+aexc.Message);
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_80456! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_0000";
            extension = ".exe";
            iCountTestcases++;
            try 
            {
                strExtension = Path.ChangeExtension(String.Empty , extension);
                Console.WriteLine ( strExtension );
                if ( strExtension != String.Empty )
                {
                    iCountErrors++;
                    printerr( "Error_0001! Expected Exception not thrown");
                }        
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_0002! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_89y9t";
            strPathName = "jabba\\de\\hutt" ;
            strExtension = Path.ChangeExtension( strPathName, extension);
            iCountTestcases++;
            if(strExtension !=  ExpectedExtension(strPathName , extension)) 
            {
                iCountErrors++;
                printerr( "Error_9t58y! String.Empty not returned for no extension" + strExtension);
            }
            strLoc = "Loc_298yv";
            strPathName = "luke..........sky..." ;
            strExtension = Path.ChangeExtension( strPathName, extension );
            iCountTestcases++;
            if( strExtension != ExpectedExtension(strPathName , extension) ) 
            {
                iCountErrors++;
                printerr( "Error_298hc! Extension=="+strExtension);
            }
            strLoc = "loc_48yvy";
            strPathName =  "star/wars/han.solo" ;                                                
            strExtension = Path.ChangeExtension( strPathName, extension);
            iCountTestcases++;
            if( strExtension != ExpectedExtension(strPathName , extension) ) 
            {
                iCountErrors++;
                printerr( "Error_298ch! Extension=="+strExtension);
            }
            strLoc = "loc_48yvy";
            strPathName =  "star/wars/han.solo" ;                                                
            strExtension = Path.ChangeExtension( strPathName, null);
            iCountTestcases++;
            if( strExtension != ExpectedExtension(strPathName , null) ) 
            {
                iCountErrors++;
                printerr( "Error_12133! Extension=="+strExtension);
            }
            strLoc = "Err_1003";
            iCountTestcases++;
            strPathName =  "foo.bar.fkl;fkds92-509450-4359.213213213@*?2-3203-=210";
            extension = "";
            strExtension = Path.ChangeExtension( strPathName, extension);
            if( strExtension != ExpectedExtension(strPathName , extension) ) 
            {
                iCountErrors++;
                printerr( "Error_1004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_2003";
            iCountTestcases++;
            extension = ".bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
            strPathName =  "AAAAAAAAAAAAAAAAAAAAAA" ;
            strExtension = Path.ChangeExtension(strPathName , extension);
            if( strExtension != ExpectedExtension(strPathName , extension) ) 
            {
                iCountErrors++;
                printerr( "Error_2004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_3003";
            iCountTestcases++;
            strPathName =  "foo.blah";
            extension = "" ;
            strExtension = Path.ChangeExtension(strPathName , extension);
            if( strExtension != ExpectedExtension(strPathName , extension) ) 
            {
                iCountErrors++;
                printerr( "Error_3004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_4003";
            iCountTestcases++;
            extension = ".$#@$_)+_)!@@!!@##&_$)#_";
            strPathName =  "foo" + extension ;
            strExtension = Path.ChangeExtension(strPathName , extension);
            if( strExtension != ExpectedExtension(strPathName , extension)) 
            {
                iCountErrors++;
                printerr( "Error_4004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_6003";
            iCountTestcases++;
            extension = ".z";
            strPathName =  "foo" + extension;
            strExtension = Path.ChangeExtension(strPathName , extension);
            if( strExtension != ExpectedExtension(strPathName , extension) ) 
            {
                iCountErrors++;
                printerr( "Error_6004! Incorrect extension , file=="+strExtension);
            }     
        } 
        catch (Exception exc_general ) 
        {
            ++iCountErrors;
            Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
            return true;
        }
        else
        {
            Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
            return false;
        }
    }
    private String ExpectedExtension(String str1 ,String str2)
    {
        int iIndex = str1.LastIndexOf(".") ;
        if ( iIndex > -1 )
        {
            if ( str2 == null )
                return str1.Substring( 0 , iIndex ) ;
            if ( str2 == "" )                                 
                return str1.Substring( 0 , iIndex + 1 ) ;                                
            return str1.Substring( 0 , iIndex ) + str2 ;
        }        
        else
            return str1 + str2 ;         
    }
    public void printerr ( String err )
    {
        Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
    }
    public void printinfo ( String info )
    {
        Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
    }
    public static void Main(String[] args)
    {
        bool bResult = false;
        Co9050ChangeExtension_str_str cbA = new Co9050ChangeExtension_str_str();
        try 
        {
            bResult = cbA.runTest();
        } 
        catch (Exception exc_main)
        {
            bResult = false;
            Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
        }
        if (!bResult)
        {
            Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
            Console.WriteLine( " " );
        }
        if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
