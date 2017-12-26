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
using System.Text;
using System;
using System.Collections;
public class Co2350Remove
{
    internal static String strName = "Hashtable.Remove";
    internal static String strTest = "Co2350Remove";
    internal static String strPath = "";
    public virtual bool runTest()
    {
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Console.Out.Write( strPath );
        Console.Out.Write( strTest );
        Console.Out.Write( ".cs" );
        Console.Out.WriteLine( " runTest() started..." );
        Hashtable hash = null;
        bool bRet = false;
        int ii;
        String strSecretID = null;
        Object objBogus = null;
        HashConfuse hshcnf1;
        HashConfuse hshcnf2;
        String strValue;
        ArrayList alst;
        Boolean fRetValue;
        Int32 iCount;
        Random rnd1;
        Int32 iElement;
        String [] strSuperHeroes =
        {
            "Captain Marvel" ,		
            "Batgirl" ,			
            "Nightwing" ,			
            "Green Lantern" ,		
            "Robin" ,				
            "Superman" ,			
            "Black Canary" ,		
            "Spiderman" ,			
            "Iron Man" ,			
            "Wonder Girl" ,		
            "Batman" ,				
            "Flash" ,				
            "Green Arrow" ,		
            "Atom" ,				
            "Steel" ,				
            "Powerman" ,			
        };
        String [] strSecretIdentities =
        {
            "Batson, Billy" ,		
            "Gordan, Barbara" ,	
            "Grayson, Dick" ,		
            "Jordan, Hal" ,		
            "Drake, Tim" ,			
            "Kent, Clark" ,		
            "Lance, Dinah" ,		
            "Parker, Peter" ,		
            "Stark, Tony" ,		
            "Troy, Donna" ,		
            "Wayne, Bruce" ,		
            "West, Wally" ,		
            "Queen, Oliver" ,		
            "Palmer, Ray" ,		
            "Irons, John Henry" ,	
            "Cage, Luke" ,			
        };
        do
        {
            iCountTestcases++;
            hash = new Hashtable();
            if ( hash == null )
            {
                Console.WriteLine( strTest+ "E_101" );
                Console.WriteLine( strTest+ "Hashtable creation error" );
                ++iCountErrors;
                break;
            }
            for ( ii = 0; ii < strSuperHeroes.Length; ++ii )
            {
                iCountTestcases++;
                try
                {
                    hash.Add( strSuperHeroes[ii], strSecretIdentities[ii] );
                }
                catch ( Exception ex )
                {
                    StringBuilder strInfo = new StringBuilder( "Remove: Add error " );
                    strInfo = strInfo.Append( "Unexpected exception on Item <" );
                    strInfo = strInfo.Append( "> " );
                    Console.WriteLine( strTest+ "E_303" );
                    Console.WriteLine( strTest+ strInfo.ToString() );
                    Console.WriteLine( strTest+ ex.ToString() );
                    ++iCountErrors;
                    break;
                }
            }
            iCountTestcases++;
            if ( hash.Count != strSuperHeroes.Length )
            {
                StringBuilder strInfo = new StringBuilder( "Remove: add error " );
                strInfo = strInfo.Append( "On Item <" + ii );
                strInfo = strInfo.Append( "> " );
                strInfo = strInfo.Append( "Expected Result = <" );
                strInfo = strInfo.Append( strSuperHeroes.Length.ToString() );
                strInfo = strInfo.Append( "> " );
                strInfo = strInfo.Append( "Returned Result = <" );
                strInfo = strInfo.Append( hash.Count.ToString() );
                strInfo = strInfo.Append( "> " );
                Console.WriteLine( strTest+ "E_404" );
                Console.WriteLine( strTest+ strInfo.ToString() );
                ++iCountErrors;
            }
            Console.Out.WriteLine( "Remove: Attempt to remove a bogus key entry from table" );
            try
            {
                iCountTestcases++;
                hash.Remove( "THIS IS A BOGUS KEY"  );
                if ( hash.Count != strSuperHeroes.Length )
                {
                    StringBuilder strInfo = new StringBuilder( "Remove error " );
                    strInfo = strInfo.Append( "Expected Result = <" );
                    strInfo = strInfo.Append( strSuperHeroes.Length.ToString() );
                    strInfo = strInfo.Append( "> " );
                    strInfo = strInfo.Append( "Returned Result = <" );
                    strInfo = strInfo.Append( hash.Count.ToString() );
                    strInfo = strInfo.Append( "> " );
                    Console.WriteLine( strTest+ "E_505" );
                    Console.WriteLine( strTest+ strInfo.ToString() );
                    ++iCountErrors;
                }
            }
            catch ( Exception ex )
            {
                StringBuilder strInfo = new StringBuilder( "Remove error " );
                strInfo = strInfo.Append( "Unexpected exception <" );
                strInfo = strInfo.Append( "> " );
                Console.WriteLine( strTest+ "E_606" );
                Console.WriteLine( strTest+ strInfo.ToString() );
                Console.WriteLine( strTest+ ex.ToString() );
                ++iCountErrors;
            }
            iCountTestcases++;
            Console.Out.WriteLine( "Remove: Attempt to remove a null key entry from table" );
            try
            {
                hash.Remove( null );
                Console.WriteLine( strTest+ "E_707" );
                Console.WriteLine( strTest+ "Remove error: ArgumentException expected" );
                ++iCountErrors;
            }
            catch ( ArgumentException ex )
            {
                Console.Out.WriteLine( "ArgumentException caught" );
            }
            catch ( Exception ex )
            {
                StringBuilder strInfo = new StringBuilder( "Remove error " );
                strInfo = strInfo.Append( "Generic exception caught:" );
                strInfo = strInfo.Append( ex.ToString() );
                Console.WriteLine( strTest+ "E_808" );
                Console.WriteLine( strTest+ strInfo.ToString() );
                ++iCountErrors;
            }
            Console.Out.WriteLine( "Remove: Add key/value pair to Hashtable and remove items" );
            for ( ii = 0; ii < strSuperHeroes.Length; ++ii )
            {
                iCountTestcases++;
                try
                {
                    hash.Remove( strSuperHeroes[ii] );
                    if ( hash.Count != strSuperHeroes.Length - ii - 1 )
                    {
                        StringBuilder strInfo = new StringBuilder( "Remove error " );
                        strInfo = strInfo.Append( "On Item <" + ii );
                        strInfo = strInfo.Append( "> " );
                        strInfo = strInfo.Append( "Expected Result = <" );
                        strInfo = strInfo.Append( (strSuperHeroes.Length - ii - 1).ToString() );
                        strInfo = strInfo.Append( "> " );
                        strInfo = strInfo.Append( "Returned Result = <" );
                        strInfo = strInfo.Append( hash.Count.ToString() );
                        strInfo = strInfo.Append( "> " );
                        Console.WriteLine( strTest+ "E_1010" );
                        Console.WriteLine( strTest+ strInfo.ToString() );
                        ++iCountErrors;
                        break;
                    }
                }
                catch ( Exception ex )
                {
                    StringBuilder strInfo = new StringBuilder( "Remove error " );
                    strInfo = strInfo.Append( "Unexpected exception on Item <" );
                    strInfo = strInfo.Append( "> " );
                    Console.WriteLine( strTest+ "E_1111" );
                    Console.WriteLine( strTest+ strInfo.ToString() );
                    Console.WriteLine( strTest+ ex.ToString() );
                    ++iCountErrors;
                    break;
                }
            }
            hash = new Hashtable();
            alst = new ArrayList();
            for(int i=0; i<7; i++)
            {
                strValue = "Test_" + i;
                hshcnf1 = new HashConfuse(strValue);
                alst.Add(hshcnf1);
                hash.Add(hshcnf1, strValue);
            }
            fRetValue = true;
            iCount = 7;
            rnd1 = new Random();
            iCountTestcases++;
            for(int i=0; i<100; i++)
            {
                for(int j=0; j<7; j++)
                {
                    if(!((String)hash[alst[j]]).Equals(((HashConfuse)alst[j]).Word))
                    {
                        fRetValue = false;
                    }
                }
                for(int j=0; j<3; j++)
                {
                    iElement = rnd1.Next(6);
                    hash.Remove(alst[iElement]);
                    alst.RemoveAt(iElement);
                    strValue = "Test_" + iCount++;
                    hshcnf1 = new HashConfuse(strValue);
                    alst.Add(hshcnf1);
                    hash.Add(hshcnf1, strValue);
                }
            }
            if(!fRetValue)
            {
                Console.WriteLine("Something wrong somewhere");
                iCountErrors++;
            }
        }
        while ( false );
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine( " paSs, iCountTestcases==" + iCountTestcases);
            return true;
        }
        else
        {
            System.String strFailMsg = null;
            Console.WriteLine( strTest+ strPath );
            Console.WriteLine( strTest+ "FAiL" );
            Console.Error.Write( strTest );
            Console.Error.Write( " iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            return false;
        }
    }
    public static void Main(System.String[] args)
    {
        bool bResult = false;	
        Co2350Remove oCbTest = new Co2350Remove();
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
class HashConfuse
{
    private String strValue;
    public HashConfuse(String val)
    {
        strValue = val;
    }
    public String Word
    {
        get{return strValue;}
        set{strValue = value;}
    }
    public override int GetHashCode()
    {
        return 5;
    }
}
