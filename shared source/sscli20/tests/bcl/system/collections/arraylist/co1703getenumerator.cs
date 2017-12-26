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
public class Co1703GetEnumerator
{
    public static String strName = "ArrayList.GetEnumerator";
    public static String strTest = "Cb2468GetEnumerator";
    public static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
        ArrayList arrList = null;
        int ii = 0;
        bool bGetNext = false;
        Object o1;
        String [] strHeroes =
            {
                "Aquaman",
                "Atom",
                "Batman",
                "Black Canary",
                "Captain America",
                "Captain Atom",
                "Catwoman",
                "Cyborg",
                "Flash",
                "Green Arrow",
                "Green Lantern",
                "Hawkman",
                "Huntress",
                "Ironman",
                "Nightwing",
                "Robin",
                "SpiderMan",
                "Steel",
                "Superman",
                "Thor",
                "Wildcat",
                "Wonder Woman",
        };
        ArrayList list;
        IEnumerator ien1;
        do
        {
            ++iCountTestcases;
            Console.Error.WriteLine( "[]  Construct ArrayList" );
            try
            {
                arrList = new ArrayList( (ICollection) strHeroes );
                if ( arrList == null )
                {
                    Console.WriteLine( strTest+ "E_101: Failed to construct new ArrayList" );
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
            Console.Error.WriteLine( "[]  Obtain and verify enumerator" );
            try
            {
                IEnumerator enu = arrList.GetEnumerator();
                if ( enu == null )
                {
                    Console.WriteLine( strTest+ "E_303: Failed to construct enumerator" );
                    ++iCountErrors;
                    break;
                }
                for ( ii = 0; ;ii++ )
                {
                    bGetNext = enu.MoveNext();
                    if ( bGetNext == false )
                        break;
                    if ( strHeroes[ii].CompareTo( (String)enu.Current ) != 0 )
                    {
                        String strInfo = strTest + " error: ";
                        strInfo = strInfo + "Expected Hero <"+ strHeroes[ii] + "> ";
                        strInfo = strInfo + "Returned Hero <"+ (String)enu.Current + "> ";
                        Console.WriteLine( strTest+ "E_404: " + strInfo );
                        ++iCountErrors;
                        break;
                    }
                }
                if ( ii != strHeroes.Length )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Enumeration discrepancy\n";
                    strInfo = strInfo + "Expected Loop count==" + strHeroes.Length;
                    strInfo = strInfo + "Returned Loop count==" + ii;
                    Console.WriteLine( strInfo );
                    Console.WriteLine( strTest+ "E_303: Enumeration discrepancy.Failed to construct enumerator " );
                    ++iCountErrors;
                    break;
                }
                bGetNext = enu.MoveNext();
                if ( bGetNext != false )
                {
                    String strInfo = strTest + " error: ";
                    strInfo = strInfo + "Enumeration discrepancy\n";
                    strInfo = strInfo + "Expected Enumeration GetNext()==false ";
                    strInfo = strInfo + "Returned Enumeration GetNext()==" + bGetNext;
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
            iCountTestcases++;
            list = new ArrayList();
            for(int i=0; i<100; i++)
                list.Add(i);
            ien1= list.GetEnumerator();
            ien1.MoveNext();
            ien1.MoveNext();
            list[10]=1000;
            if((int)ien1.Current!=1)
            {
                ++iCountErrors;
                Console.WriteLine("Err_3857tsgfg! Wrong value returned! " + ien1.Current);
            }
            try
            {
                ien1.MoveNext();
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_357dg! Unexpected exception thrown, " + ex.GetType().Name);
            }
            iCountTestcases++;
            list = new ArrayList();
            for(int i=0; i<100; i++)
                list.Add(i);
            ien1= list.GetEnumerator();
            try
            {
                o1 = ien1.Current;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_357dg! Unexpected exception thrown, " + ex.GetType().Name);
            }
        while(ien1.MoveNext());
            try
            {
                o1 = ien1.Current;
            }
            catch(InvalidOperationException)
            {
            }
            catch(Exception ex)
            {
                ++iCountErrors;
                Console.WriteLine( "Err_357dg! Unexpected exception thrown, " + ex.GetType().Name);
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
        Co1703GetEnumerator oCbTest = new Co1703GetEnumerator();
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
