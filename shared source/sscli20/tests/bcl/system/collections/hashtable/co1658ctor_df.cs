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
class Co1658ctor_Df
{
    public const String s_strActiveBugNums = "";
    public const String s_strDtTmVer       = "";
    public const String s_strClassMethod   = "Constructor( Dictionary, float)";
    public const String s_strTFName        = "Co1658ctor_Df.cs";
    public const String s_strTFAbbrev      = "Co1658";
    public const String s_strTFPath        = "";
    public Boolean runTest()
    {
        Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
        Hashtable hash = null;
        int iCountErrors    = 0;                      
        int iCountTestcases = 0;                      
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( null, 1 );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0001!  hashtable should have thrown ArgumentNullException here" );
        }
        catch( ArgumentNullException )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1001!  we expected ArgumentNullException but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( new Hashtable(), Int32.MinValue );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0002!  hashtable should have thrown ArgumentOutOfRangeException here" );
        }
        catch( ArgumentOutOfRangeException )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1002!  we expected ArgumentOutOfRangeException but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( new Hashtable(), Single.NaN );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0003!  hashtable should have thrown ArgumentOutOfRangeException here" );
        }
        catch( ArgumentOutOfRangeException )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1003!  we expected ArgumentOutOfRangeException but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( new Hashtable(), 100.1f );
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_0004!  hashtable should have thrown ArgumentOutOfRangeException here" );
        }
        catch( ArgumentOutOfRangeException )
        {
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1004!  we expected ArgumentOutOfRangeException but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( new Hashtable(), 1f );
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1005!  we expected no exception but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            hash = new Hashtable( new Hashtable(new Hashtable(new Hashtable(new Hashtable(new Hashtable()), 1f), 1f), 1f), 1f );
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1006!  we expected no exception but got exception " + ex.ToString() );
        }
        try
        {
            ++iCountTestcases;
            Hashtable tempHash = new Hashtable();
            for ( long i = Int64.MinValue; i < Int64.MinValue + 100; i++ )
            {
                tempHash.Add( i, i );
            }
            hash = new Hashtable( tempHash, 1f );
            for ( long i = Int64.MinValue; i < Int64.MinValue + 100; i++ )
            {
                if ( ! hash.ContainsKey( i ) )
                {
                    Console.WriteLine( s_strTFAbbrev + " Error_007a!  new Hashtable does not contain KEY " + i.ToString() );
                }
                else if ( ! hash.ContainsValue( i ) )
                {
                    Console.WriteLine( s_strTFAbbrev + " Error_007b!  new Hashtable does not contain VALUE " + i.ToString() );
                }
            }
            ++iCountTestcases;
            tempHash.Clear();
            for ( long i = Int64.MinValue; i < Int64.MinValue + 100; i++ )
            {
                if ( ! hash.ContainsKey( i ) )
                {
                    Console.WriteLine( s_strTFAbbrev + " Error_007a!  new Hashtable does not contain KEY " + i.ToString() );
                }
                else if ( ! hash.ContainsValue( i ) )
                {
                    Console.WriteLine( s_strTFAbbrev + " Error_007b!  new Hashtable does not contain VALUE " + i.ToString() );
                }
            }
        }
        catch( Exception ex )
        {
            ++iCountErrors;
            Console.WriteLine(  s_strTFAbbrev +" Error_1007!  we expected no exception but got exception " + ex.ToString() );
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
        Console.WriteLine( "USAGE: Co1658ctor_Df [option option ....]" );
        Console.WriteLine( "      options:" );
        Console.WriteLine( "               /?            - prints usage and exits" );
    }
    public static void Main( String [] args )
    {
        Boolean bolResult = false;                                  
        Co1658ctor_Df cbX = new Co1658ctor_Df();
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
