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
public class Co3110ToArray
{
    public virtual bool runTest()
    {
        Console.Out.WriteLine( "Queue\\Co3110ToArray.cs runTest() started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Queue myQueue = null;
        String str1 = null;
        String str2 = null;
        Boolean mrBool = false;
        byte mrByte = Byte.MaxValue;
        Int16 mrInt2 = -2;
        Int32 mrInt4 = -2;
        Int64 mrInt8 = -2;
        Single mrSingle = -2;
        Double mrDouble = -2;
        Char mrChar = ' ';
        Object[] @object = null;
        myQueue = new Queue();
        str1 = "test1";
        iCountTestcases++;
        if(myQueue.ToArray().Length != 0 )
        {
            iCountErrors++;
            print("E_7jvs");
        }
        str1 = "test1";
        str2 = "test2";
        mrBool = true;
        mrByte = Byte.MaxValue;
        mrInt2 = Int16.MaxValue;
        mrInt4 = Int32.MaxValue;
        mrInt8 = Int64.MinValue;
        mrSingle = Single.MaxValue;
        mrDouble = Double.MinValue;
        mrChar = '\0';
        @object = new Object[10];
        myQueue.Enqueue(str1);
        myQueue.Enqueue(str2);
        myQueue.Enqueue(mrBool);
        myQueue.Enqueue(mrByte);
        myQueue.Enqueue(mrInt2);
        myQueue.Enqueue(mrInt4);
        myQueue.Enqueue(mrInt8);
        myQueue.Enqueue(mrSingle);
        myQueue.Enqueue(mrDouble);
        myQueue.Enqueue(mrChar);
        @object = myQueue.ToArray();
        iCountTestcases++;
        if(myQueue.Count != 10)
        {
            iCountErrors++;
            print("E_38is");
        }
        iCountTestcases++;
        if(@object.Length != 10)
        {
            iCountErrors++;
            print("E_93id");
        }
        iCountTestcases++;
        if(@object[0] != str1)
        {
            iCountErrors++;
            print("E_83sd");
        }
        iCountTestcases++;
        if(@object[1] != str2)
        {
            iCountErrors++;
            print("E_3dsj");
        }
        iCountTestcases++;
        if((Boolean)@object[2] != mrBool)
        {
            iCountErrors++;
            print("E_9usd");
        }
        iCountTestcases++;
        if((byte)@object[3] != mrByte)
        {
            iCountErrors++;
            print("E_439s");
        }
        iCountTestcases++;
        if((Int16)@object[4] != mrInt2)
        {
            iCountErrors++;
            print("E_39id");
        }
        iCountTestcases++;
        if((Int32)@object[5] != mrInt4)
        {
            iCountErrors++;
            print("E_93ie");
        }
        iCountTestcases++;
        if((Int64)@object[6] != mrInt8)
        {
            iCountErrors++;
            print("E_93dk");
        }
        iCountTestcases++;
        if((Single)@object[7] != mrSingle)
        {
            iCountErrors++;
            print("E_94rj");
        }
        iCountTestcases++;
        if((Double)@object[8] != mrDouble)
        {
            iCountErrors++;
            print("E_82ks");
        }
        iCountTestcases++;
        if((Char)@object[9] != mrChar)
        {
            iCountErrors++;
            print("E_95kd");
        }
        myQueue = new Queue();
        str1 = "test1";
        str2 = "test2";
        mrBool = true;
        mrByte = Byte.MaxValue;
        mrInt2 = Int16.MaxValue;
        mrInt4 = Int32.MaxValue;
        mrInt8 = Int64.MinValue;
        mrSingle = Single.MaxValue;
        mrDouble = Double.MinValue;
        mrChar = '\0';
        @object = new Object[10];
        myQueue.Enqueue(str1);
        myQueue.Enqueue(str2);
        myQueue.Enqueue(mrBool);
        myQueue.Enqueue(mrByte);
        myQueue.Enqueue(mrInt2);
        myQueue.Enqueue(mrInt4);
        myQueue.Enqueue(mrInt8);
        myQueue.Enqueue(mrSingle);
        myQueue.Enqueue(mrDouble);
        myQueue.Enqueue(mrChar);
        @object = myQueue.ToArray();
        iCountTestcases++;
        if(@object.Length != 10)
        {
            iCountErrors++;
            print("E_39im");
        }
        if(myQueue.Count != 10)
        {
            iCountErrors++;
            print("E_80ff");
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "Queue\\Co3110ToArray.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine(  "Co3110ToArray.cs"  );
            Console.Error.Write( "Co3110ToArray.cs iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "Queue\\Co3110ToArray.cs: FAiL!" );
            return false;
        }
    }
    private void print(String error)
    {
        StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
        output.Append(error);
        output.Append(" (Co3110ToArray.cs)");
        Console.Out.WriteLine(output.ToString());
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        Co3110ToArray cb0 = new Co3110ToArray();
        try
        {
            bResult = cb0.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine(  "Co3110ToArray.cs")  ;
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
