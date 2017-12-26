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
public class Co4307ctor
{
    internal static String strName = "Hashtable.Contructor";
    internal static String strTest = "Co4307ctor";
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
        int nAttempts = 100;
        int [] iIntArray = new int[nAttempts];
        int nIndex = 0;
        int nFindIndex = 40;
        int nMultiplier = 5;
        do
        {
            Console.Out.WriteLine( "Create Hashtable using default settings" );
            iCountTestcases++;
            hash = new Hashtable();
            if ( hash == null )
            {
                Console.WriteLine( strTest+ "E_101" );
                Console.WriteLine( strTest+ "Hashtable creation failure" );
                ++iCountErrors;
                break;
            }
            iCountTestcases++;
            if ( hash.Count != 0 )
            {
                Console.WriteLine( strTest+ "E_102" );
                Console.WriteLine( strTest+ "New hash table is not empty" );
                ++iCountErrors;
            }
        }
        while (false);
        Console.Error.Write( strName );
        Console.Error.Write( ": " );
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( strTest );
            Console.Error.WriteLine( " paSs, iCountTestcases==" + iCountTestcases++);
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
        Co4307ctor oCbTest = new Co4307ctor();
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
