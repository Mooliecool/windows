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
public class Co2458AddRange_I
{
    public static String strName = "ArrayList.AddRange";
    public static String strTest = "Co2458AddRange_I";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList1 = null;
        ArrayList arrList2 = null;
        int nItems = 100;
        ArrayList olst1;
        Queue que;
        String [] strHeroes =
            {
                "Batman",
                "Superman",
                "SpiderMan",
                "Wonder Woman",
                "Green Lantern",
                "Flash",
                "Steel"
            };
        String [] strIdentities =
            {
                "Bruce Wayne",
                "Clark Kent",
                "Peter Parker",
                "Diana Prince",
                "Kyle Rayner",
                "Wally West",
                "John Henry Irons"
            };
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Construct ArrayList" );
            try
            {
                arrList1 = new ArrayList();
                arrList2 = new ArrayList();
                if ( arrList1 == null || arrList2 == null )
                {
                    Console.WriteLine( strTest+ "E_101: Failed to construct new ArrayList" );
                    ++iCountErrors;
                    break;
                }
                for ( int ii = 0; ii < strHeroes.Length; ++ii )
                {
                    arrList1.Add( strHeroes[ii] );
                    arrList2.Add( strIdentities[ii] );
                }
                if ( arrList1.Count != strHeroes.Length )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected size <"+ nItems + "> ";
                    strInfo = strInfo + "Returned size <"+ arrList1.Count + "> ";
                    Console.WriteLine( strTest+ "E_202: " + strInfo );
                    ++iCountErrors;
                    break;
                }
                if ( arrList2.Count != strIdentities.Length )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected size <"+ nItems + "> ";
                    strInfo = strInfo + "Returned size <"+ arrList2.Count + "> ";
                    Console.WriteLine( strTest+ "E_303: " + strInfo );
                    ++iCountErrors;
                    break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10001: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Append the second list to the first list" );
            try
            {
                arrList1.AddRange( arrList2 );
                if ( arrList1.Count != (strHeroes.Length + strIdentities.Length) )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Expected size <"+ (strHeroes.Length + strIdentities.Length) + "> ";
                    strInfo = strInfo + "Returned size <"+ arrList1.Count + "> ";
                    Console.WriteLine( strTest+ "E_404: " + strInfo );
                    ++iCountErrors;
                    break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10002: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Attempt bogus AddRange" );
            try
            {
                arrList1.AddRange( null );
                Console.WriteLine( strTest+ "E_505: Expected NullReferenceException" );
                ++iCountErrors;
                break;
            }
            catch (ArgumentException)
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine( strTest+ "E_10003: Unexpected Exception: " + ex.ToString() );
                ++iCountErrors;
                break;
            }
            iCountTestcases++;
            arrList1 = new ArrayList();
            for(int i=0; i<10; i++)
                arrList1.Add(i);
            olst1 = new ArrayList();
            for(int i=0; i<10; i++)
                olst1.Add(i+10);
            arrList1.AddRange(olst1);
            for(int i=0; i<arrList1.Count; i++)
            {
                if ( (Int32)arrList1[i] != i )
                {
                    iCountErrors++;
                    Console.WriteLine("Err_742sf! incorrect value returned");
                }
            }
            que = new Queue();
            for(int i=0; i<10; i++)
                que.Enqueue(i+20);
            arrList1.AddRange(que);
            for(int i=0; i<arrList1.Count; i++)
            {
                if ( (Int32)arrList1[i] != i )
                {
                    iCountErrors++;
                    Console.WriteLine("Err_742sf! incorrect value returned");
                }
            }
            ++iCountTestcases;
            arrList1.AddRange(arrList1);
            for(int i=0; i<arrList1.Count/2; i++)
            {
                if ( (Int32)arrList1[i] != i )
                {
                    iCountErrors++;
                    Console.WriteLine("Err_742sf! incorrect value returned");
                }
            }
            for(int i=arrList1.Count/2; i<arrList1.Count; i++)
            {
                if ( (Int32)arrList1[i] != (i-arrList1.Count/2) )
                {
                    iCountErrors++;
                    Console.WriteLine("Err_742sf! incorrect value returned");
                }
            }
            arrList1 = new ArrayList();
            for(int i=0; i<10; i++)
                arrList1.Add(i);
            que = new Queue();
            for(int i=10; i<20; i++)
                que.Enqueue("String_" + i);
            arrList1.AddRange(que);
            for(int i=0; i<10; i++)
            {
                if ( (Int32)arrList1[i] != i )
                {
                    iCountErrors++;
                    Console.WriteLine("Err_742sf! incorrect value returned");
                }
            }
            for(int i=10; i<20; i++)
            {
                if ( !((String)arrList1[i]).Equals("String_" +i) )
                {
                    iCountErrors++;
                    Console.WriteLine("Err_84752dsfdf! incorrect value returned, " + (String)arrList1[i]);
                }
            }
            ++iCountTestcases;
            arrList1 = new ArrayList();
            que = new Queue();
            arrList1.AddRange(que);
            if(arrList1.Count != 0)
            {
                iCountErrors++;
                Console.WriteLine("Err_742sf! incorrect value returned");
            }
        }
        while ( false );
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.WriteLine( strTest + " iCountTestcases==" + iCountTestcases + " paSs" );
            return true;
        }
        else
        {
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "FAiL" );
            Console.Error.WriteLine( strTest + " iCountErrors==" + iCountErrors );
            return false;
        }
    }
    public static void Main( String[] args )
    {
        bool bResult = false;	
        Co2458AddRange_I oCbTest = new Co2458AddRange_I();
        try
        {
            bResult = oCbTest.runTest();
        }
        catch( Exception ex )
        {
            bResult = false;
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "E_1000000" );
            Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
            Console.WriteLine( strTest+ ex.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
    }
}
