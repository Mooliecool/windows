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
using System.IO;
using System;
using System.Collections;
class Co1659ctor_ifII
{
    public const String s_strActiveBugNums = "";
    public const String s_strDtTmVer       = "";
    public const String s_strClassMethod   = "Constructor( int, float, IHashCodeProvider, IComparer )";
    public const String s_strTFName        = "Co1659ctor_ifII.cs";
    public const String s_strTFAbbrev      = "Co1659";
    public const String s_strTFPath        = "";
    public Boolean runTest()
    {
        Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
        String strLoc = "Loc_000oo";
        Hashtable hash = null;
        int iCountErrors    = 0;                      
        int iCountTestcases = 0;                      
        strLoc = "Loc_001oo";
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( Int32.MaxValue, .1f, null, null );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0001!  hashtable should have thrown ArgumentException here" );
        }
        catch( ArgumentException )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1001!  we expected ArgumentException but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( 100, .1f, null, null );
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1002!  we expected no exception but got exception " + ex.ToString() );
        }
        Int32 iNumberOfElements = 100;
        for(int i=0; i<iNumberOfElements; i++)
        {
            hash.Add("Key_" + i, "Value_" + i);
        }
        iCountTestcases++;
        if(hash.Count != iNumberOfElements) 
        {
            iCountErrors++;
            Console.WriteLine("Err_742dsf! Expected value not returned, " + hash.Count);
        }				
        DictionaryEntry[] strValueArr = new DictionaryEntry[hash.Count];
        hash.CopyTo(strValueArr, 0);
        Hashtable hsh3 = new Hashtable();
        for(int i=0; i<iNumberOfElements; i++)
        {
            if(!hash.Contains("Key_" + i)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_742ds8f! Expected value not returned, " + hash.Contains("Key_" + i));
            }				
            if(!hash.ContainsKey("Key_" + i)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_742389dsaf! Expected value not returned, " + hash.ContainsKey("Key_" + i));
            }				
            if(!hash.ContainsValue("Value_" + i)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_563fgd! Expected value not returned, " + hash.ContainsValue("Value_" + i));
            }				
            if(!hash.ContainsValue(((DictionaryEntry)strValueArr[i]).Value)) 
            {
                iCountErrors++;
                Console.WriteLine("Err_87429dsfd! Expected value not returned, " + ((DictionaryEntry)strValueArr[i]).Value);
            }				
            try
            {
                hsh3.Add(((DictionaryEntry)strValueArr[i]).Value, null);
            }
            catch(Exception)
            {
                iCountErrors++;
                Console.WriteLine("Err_74298dsd! Exception thrown for  " + ((DictionaryEntry)strValueArr[i]).Value);
            }
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( 5, .01f, null, null );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0003!  hashtable should have thrown ArgumentOutOfRangeException  here" );
        }
        catch( ArgumentOutOfRangeException  )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1003!  we expected ArgumentOutOfRangeException  but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( 5, 100.1f, null, null );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0004!  hashtable should have thrown ArgumentOutOfRangeException  here" );
        }
        catch( ArgumentOutOfRangeException  )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1004!  we expected ArgumentOutOfRangeException  but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( Int32.MaxValue, 100.1f, null, null );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0004!  hashtable should have thrown ArgumentOutOfRangeException  here" );
        }
        catch( ArgumentOutOfRangeException  )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1004!  we expected ArgumentOutOfRangeException  but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( Int32.MinValue, 10.1f, null, null );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0005!  hashtable should have thrown ArgumentOutOfRangeException  here" );
        }
        catch( ArgumentOutOfRangeException  )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1005!  we expected ArgumentOutOfRangeException  but got exception strLoc= " + strLoc + ex.ToString() );
        }
        if ( iCountErrors == 0 )
        {
            Console.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases="+ iCountTestcases.ToString() );
            return true;
        }
        else
        {
            Console.WriteLine( "ACTIVE BUGS: " + s_strActiveBugNums );
            Console.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors="+ iCountErrors.ToString() +" ,BugNums?: "+ s_strActiveBugNums );
            return false;
        }
    }
    public static void printUsage()
    {
        Console.WriteLine( "USAGE: Co1659ctor_ifII [option option ....]" );
        Console.WriteLine( "      options:" );
        Console.WriteLine( "               /?            - prints usage and exits" );
    }
    public static void Main( String [] args )
    {
        Boolean bolResult = false;                                  
        Co1659ctor_ifII cbX = new Co1659ctor_ifII();
        if ( Common.argContains( args, "/?" ) )
        {
            printUsage();
            Environment.ExitCode =  0 ;
            return;
        }
        try
        {
            bolResult = cbX.runTest();
        }
        catch ( Exception exc_main )
        {
            bolResult = false;
            Console.WriteLine( s_strTFAbbrev +"  FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main()" );
            Console.WriteLine( "exc_main=="+ exc_main.ToString() );
        }
        if ( ! bolResult )
        {
            Console.WriteLine( "FullerPath="+ s_strTFPath +" "+ s_strTFName );
            Console.WriteLine( " " );
            Console.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
            Console.WriteLine( " " );
        }
        if ( bolResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    } 
} 
class Common
{
    public static Boolean argContains( String [] args, String str )
    {
        for ( int i = 0; i < args.Length; i += 1 )
        {
            if ( args[i].Equals( str ) )
            {
                return true;
            }
        }
        return false;
    }
    public static String argGetArg( String [] args, String str )
    {
        for ( int i = 0; i < args.Length; i += 1 )
        {
            if ( args[i].Equals( str ) )
            {
                if ( i == (args.Length - 1) )
                {
                    return null;
                }
                else
                {
                    return args[i+1];
                }
            }
        }
        return null;
    }
}
