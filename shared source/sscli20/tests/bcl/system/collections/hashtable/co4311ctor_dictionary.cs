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
public class Co4311ctor_dictionary
{
    internal static String strName = "Hashtable.Contructor_dictionary";
    internal static String strTest = "Co4311ctor_dictionary";
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
        Hashtable hash2 = null;
        Int32 i4a;
        do
        {
            iCountTestcases++;
            try 
            {
                hash = new Hashtable((IDictionary)null);
                Console.WriteLine( strTest+ "E_101" );
                Console.WriteLine( strTest+ "Hashtable creation failure" );
                ++iCountErrors;
            }
            catch (ArgumentNullException )
            {}
            catch (Exception exc) 
            {
                Console.WriteLine( strTest+ "E_102" );
                Console.WriteLine( strTest+ "NullRefExc thrown instead of ArgExc " );
                Console.WriteLine( strTest+ exc.ToString());
                ++iCountErrors;
            }
            iCountTestcases++;
            hash2 = new Hashtable(); 
            try 
            {
                hash = new Hashtable(hash2);
            }
            catch (Exception exc) 
            { 
                Console.WriteLine( strTest+ "E_12gf9" );
                Console.WriteLine( strTest+ exc.ToString());
                ++iCountErrors;
            }
            iCountTestcases++;
            hash2 = new Hashtable();
            for (int i=0; i<100; i++) 
            {
                hash2.Add ("key_" +i, "val_" +i);
            }
            try 
            {
                hash = new Hashtable(hash2);
            }
            catch (Exception exc) 
            { 
                Console.WriteLine( strTest+ "E_329" );
                Console.WriteLine( strTest+ exc.ToString());
                ++iCountErrors;
            }
            iCountTestcases++;
            Hashtable hash3 = new Hashtable(200);
            for(int ii=0; ii<100;ii++)
            {
                i4a = ii;
                hash3.Add("key_" + ii, i4a);
            }
            hash = new Hashtable(hash3);
            if (hash.Count != 100)
            {
                Console.WriteLine( strTest+ "Err_673gs" );
                Console.WriteLine( strTest+ "wrong hash table Count" );
            }
            for(int ii=0; ii<100;ii++)
            {
                i4a = ii;
                iCountTestcases++;
                if ((int)hash3["key_" + ii] != ii)
                {
                    Console.WriteLine( strTest+ "Err_735xa_" + ii );
                    Console.WriteLine( strTest+ "wrong value returned" );
                }
                if(hash3.ContainsKey("key_" + ii)!= true)
                {
                    Console.WriteLine( strTest+ "Err_736aa_" + ii );
                    Console.WriteLine( strTest+ "key not found" );
                }
            }
            iCountTestcases++;
            if(hash3.ContainsKey("key_100")!= false)
            {
                Console.WriteLine( strTest+ "Err_463zd" );
                Console.WriteLine( strTest+ "bogus key found" );
            }
        }
        while (false);
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
        Co4311ctor_dictionary oCbTest = new Co4311ctor_dictionary();
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
