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
public class Co3118ctor_int
{
    public virtual bool runTest()
    {
        Console.Out.WriteLine( "BitArray\\Co3118ctor_int.cs runTest() started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        String strError = null;
        BitArray ba1 = null;
        int size = 0;
        size = -1;
        iCountTestcases++;
        try 
        {
            ba1 = new BitArray(size);
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
        size = 40;
        int i = 0;
        ba1 = new BitArray(size);
        for(i=0;i<40;i++)
            if(ba1.Get(i))
                break;
        iCountTestcases++;
        if(i != size)
        {
            iCountErrors++;
            print("E_943s");
        }
        iCountTestcases++;
        if(ba1.Length != size)
        {
            iCountErrors++;
            print("E_38sa");
        }
        size = 200;
        ba1 = new BitArray(size);
        for(i=0;i<200;i++)
            if(ba1.Get(i))
                break;
        iCountTestcases++;
        if(i != size)
        {
            iCountErrors++;
            print("E_84do");
        }
        iCountTestcases++;
        if(ba1.Length != size)
        {
            iCountErrors++;
            print("E_39te");
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "BitArray\\Co3118ctor_int.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine(  "Co3118ctor_int.cs"  );
            Console.Error.Write( "Co3118ctor_int.cs iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "BitArray\\Co3118ctor_int.cs: FAiL!" );
            return false;
        }
    }
    private void print(String error)
    {
        StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
        output.Append(error);
        output.Append(" (Co3118ctor_int.cs)");
        Console.Out.WriteLine(output.ToString());
    }
    public static void Main( System.String[] args )
    {
        bool bResult = false; 
        Co3118ctor_int cb0 = new Co3118ctor_int();
        try
        {
            bResult = cb0.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine( "Co3118ctor_int.cs") ;
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
