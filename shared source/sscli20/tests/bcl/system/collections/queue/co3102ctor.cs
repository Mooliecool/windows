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
public class Co3102ctor
{
    public virtual bool runTest()
    {
        Console.Out.WriteLine( "Queue\\Co3102ctor.cs runTest() started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Queue myQueue = null;
        String str1 = null;
        myQueue = new Queue();
        iCountTestcases++;
        if(myQueue == null)
        {
            iCountErrors++;
            print("E_839k");
        }
        int defaultCap = 32;
        myQueue = new Queue();
        str1 = "test";
        for(int i=0;i <= defaultCap;i++)
        {
            myQueue.Enqueue(str1);
        }
        iCountTestcases++;
        if(myQueue.Count != defaultCap+1)
        {
            iCountErrors++;
            print("E_34aj");
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "Queue\\Co3102ctor.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine(  "Co3102ctor.cs"  );
            Console.Error.Write( "Co3102ctor.cs iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "Queue\\Co3102ctor.cs: FAiL!" );
            return false;
        }
    }
    private void print(String error)
    {
        StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
        output.Append(error);
        output.Append(" (Co3102ctor.cs)");
        Console.Out.WriteLine(output.ToString());
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        Co3102ctor cb0 = new Co3102ctor();
        try
        {
            bResult = cb0.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine(  "Co3102ctor.cs")  ;
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
