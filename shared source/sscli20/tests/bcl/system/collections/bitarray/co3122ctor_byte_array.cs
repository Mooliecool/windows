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
public class Co3122ctor_byte_array
{
    public virtual bool runTest()
    {
        Console.Out.WriteLine( "BitArray\\Co3122ctor_byte_array.cs runTest() started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strError = null;
        BitArray ba1 = null;
        byte[] bytes = null;
        int i;
        bytes = null;
        iCountTestcases++;
        try 
        {
            ba1 = new BitArray(bytes);
            iCountErrors++;
            print("E_81ww");
        } 
        catch (ArgumentException ) {}
        catch (Exception exc2)
        {
            iCountErrors++;
            print("E_875i");
            strError = "EXTENDEDINFO: ";
            strError = strError + exc2.ToString();
            Console.Error.WriteLine(strError);
        }
        bytes = new byte[10];
        for(i = 0;i<10;i++)
            bytes[i] = (byte)0xff;
        ba1 = new BitArray(bytes);
        iCountTestcases++;
        if(ba1.Length != 80)
        {
            iCountErrors++;
            print("E_390d");
        }
        iCountTestcases++;
        for(i = 0;i<80;i++)
            if(!ba1.Get(i))
            {
                iCountErrors++;
                print("E_83ia");
                break;
            }
        bytes = new byte[10];
        for(i = 0;i<10;i++)
            bytes[i] = (byte)0;
        ba1 = new BitArray(bytes);
        iCountTestcases++;
        if(ba1.Length != 80)
        {
            iCountErrors++;
            print("E_84aa");
        }
        iCountTestcases++;
        for(i = 0;i<80;i++)
            if(ba1.Get(i))
            {
                iCountErrors++;
                print("E_94av");
                break;
            }
        bytes = new byte[10];
        for(i = 0;i<10;i++)
            bytes[i] = (byte)0xaa;
        ba1 = new BitArray(bytes);
        iCountTestcases++;
        if(ba1.Length != 80)
        {
            iCountErrors++;
            print("E_39wk");
        }
        for(i=0;i<80;i++)
        {
            if((i%2 == 1) && (!ba1.Get(i)))
            {
                Console.Out.WriteLine(i);
                iCountErrors++;
                print("E_93ay");
                break;
            }
            else if((i%2 == 0) && (ba1.Get(i)))
            {
                Console.Out.WriteLine(i);
                iCountErrors++;
                print("E_985i");
                break;
            }
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "BitArray\\Co3122ctor_byte_array.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine(  "Co3122ctor_byte_array.cs"  );
            Console.Error.Write( "Co3122ctor_byte_array.cs iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "BitArray\\Co3122ctor_byte_array.cs: FAiL!" );
            return false;
        }
    }
    private void print(String error)
    {
        StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
        output.Append(error);
        output.Append(" (Co3122ctor_byte_array.cs)");
        Console.Out.WriteLine(output.ToString());
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        Co3122ctor_byte_array cb0 = new Co3122ctor_byte_array();
        try
        {
            bResult = cb0.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine(  "Co3122ctor_byte_array.cs")  ;
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
