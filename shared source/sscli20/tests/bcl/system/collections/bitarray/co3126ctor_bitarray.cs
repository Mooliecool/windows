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
public class Co3126ctor_BitArray
{
    public virtual bool runTest()
    {
        Console.Out.WriteLine( "BitArray\\Co3126ctor_BitArray.cs runTest() started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strError = null;
        BitArray ba1 = null;
        BitArray bits = null;
        int[] values = null;
        int i;
        iCountTestcases++;
        try 
        {
            ba1 = new BitArray(bits);
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
        values = new int[10];
        for(i = 0;i<10;i++)
            values[i] = unchecked((int) 0xffffffff);
        bits = new BitArray(values);
        ba1 = new BitArray(bits);
        iCountTestcases++;
        if(ba1.Length != 320)
        {
            iCountErrors++;
            print("E_390d");
        }
        iCountTestcases++;
        for(i = 0;i<320;i++)
            if(!ba1.Get(i))
            {
                iCountErrors++;
                print("E_83ia");
                break;
            }
        values = new int[10];
        for(i = 0;i<10;i++)
            values[i] = 0;
        bits = new BitArray(values);
        ba1 = new BitArray(bits);
        iCountTestcases++;
        if(ba1.Length != 320)
        {
            iCountErrors++;
            print("E_84aa");
        }
        iCountTestcases++;
        for(i = 0;i<320;i++)
            if(ba1.Get(i))
            {
                iCountErrors++;
                print("E_94av");
                break;
            }
        values = new int[10];
        for(i = 0;i<10;i++)
            values[i] = unchecked((int) 0xaaaaaaaa);
        bits = new BitArray(values);
        ba1 = new BitArray(bits);
        iCountTestcases++;
        if(ba1.Length != 320)
        {
            iCountErrors++;
            print("E_39wk");
        }
        for(i=0;i<320;i++)
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
            Console.Error.Write( "BitArray\\Co3126ctor_BitArray.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine(  "Co3126ctor_BitArray.cs"  );
            Console.Error.Write( "Co3126ctor_BitArray.cs iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "BitArray\\Co3126ctor_BitArray.cs: FAiL!" );
            return false;
        }
    }
    private void print(String error)
    {
        StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
        output.Append(error);
        output.Append(" (Co3126ctor_BitArray.cs)");
        Console.Out.WriteLine(output.ToString());
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        Co3126ctor_BitArray cb0 = new Co3126ctor_BitArray();
        try
        {
            bResult = cb0.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine(  "Co3126ctor_BitArray.cs")  ;
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
