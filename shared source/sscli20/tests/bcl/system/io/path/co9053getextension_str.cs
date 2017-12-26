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
public class Co9053GetExtension_str
{
    public static String s_strDtTmVer       = "";
    public static String s_strClassMethod   = "Path.GetExtension(String)";
    public static String s_strTFName        = "Co9053GetExtension_str.cs";
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
            String str2 = null;
            String strExtension;
            String fileName ;
            strLoc = "Loc_985yb";
            iCountTestcases++;
            try 
            {
                strExtension = Path.GetExtension(null);
                if ( strExtension != null) 
                {
                    iCountErrors++;
                    printerr( "Error_4449! Incorrect return value" + strExtension);
                }
            } 
            catch (Exception exc) 
            {
                iCountErrors++;
                printerr( "Error_019ux! Incorrect exception thrown, exc=="+exc.ToString());
            }
            strLoc = "Loc_0000";
            iCountTestcases++;
            try 
            {
                strExtension = Path.GetExtension(String.Empty);
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
            str2 = Path.GetExtension("jabba\\de\\hutt");
            iCountTestcases++;
            if(! String.Empty.Equals(str2) ) 
            {
                iCountErrors++;
                printerr( "Error_9t58y! String.Empty not returned for no extension");
            }
            strLoc = "Loc_298yv";
            str2 = Path.GetExtension("luke..........sky...");
            iCountTestcases++;
            if(str2.Equals(".")) 
            {
                iCountErrors++;
                printerr( "Error_298hc! Extension=="+str2);
            }
            strLoc = "loc_48yvy";
            str2 = Path.GetExtension("star/wars/han.solo");
            iCountTestcases++;
            if(!str2.Equals(".solo")) 
            {
                iCountErrors++;
                printerr( "Error_298ch! Extension=="+str2);
            }
            strLoc = "Err_1003";
            iCountTestcases++;
            fileName =  "foo.bar.fkl;fkds92-509450-4359.$#%()#%().%#(%)_#(%_).cs";
            strExtension = Path.GetExtension(fileName);
            if( strExtension != ".cs" ) 
            {
                iCountErrors++;
                printerr( "Error_1004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_2003";
            iCountTestcases++;
            String extension = ".bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
            fileName =  "AAAAAAAAAAAAAAAAAAAAAA" + extension ;
            strExtension = Path.GetExtension(fileName);
            if( strExtension != extension ) 
            {
                iCountErrors++;
                printerr( "Error_2004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_3003";
            iCountTestcases++;
            fileName =  "foo.";
            strExtension = Path.GetExtension(fileName);
            if( strExtension == "." ) 
            {
                iCountErrors++;
                printerr( "Error_3004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_4003";
            iCountTestcases++;
            extension = ".$#@$_)+_)!@@!!@##&_$)#_";
            fileName =  "foo" + extension ;
            strExtension = Path.GetExtension(fileName);
            if( strExtension != extension ) 
            {
                iCountErrors++;
                printerr( "Error_4004! Incorrect extension , file=="+strExtension);
            }
            strLoc = "Err_6003";
            iCountTestcases++;
            extension = ".z";
            fileName =  "foo" + extension;
            strExtension = Path.GetExtension(fileName);
            if( strExtension != ".z" ) 
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
        Co9053GetExtension_str cbA = new Co9053GetExtension_str();
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
