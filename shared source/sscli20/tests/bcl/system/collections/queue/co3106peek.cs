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
public class Co3106Peek
{
    public virtual bool runTest()
    {
        Console.Out.WriteLine( "Queue\\Co3106Peek.cs runTest() started." );
        int iCountErrors = 0;
        int iCountTestcases = 0;
        Queue myQueue = null;
        String str1 = null;
        String str2 = null;
        Boolean mrBool;
        byte mrByte = (byte)0;
        Int16 mrInt2 = 0;
        Int32 mrInt4 = 0;
        Int64 mrInt8 = 0L;
        Single mrSingle = (float)0.0;
        Double mrDouble = (double)0.0;
        Char mrChar = (Char)('\0');
        myQueue = new Queue();
        iCountTestcases++;
        try
        {
            myQueue.Peek();
            Console.WriteLine( "Err_001,  Should have thrown InvalidOperationException" );
            ++iCountErrors;
        }
        catch ( InvalidOperationException  )
        {}
        catch ( Exception ex )
        {
            Console.WriteLine( "Err_002,  Should have thrown InvalidOperationException but ex thrown=" + ex.ToString() );
            ++iCountErrors;
        }
        str1 = "test1";
        str2 = "test2";
        mrBool = (Boolean)(true);
        mrByte = (byte)(127);
        mrInt2 = (Int16)(Int16.MaxValue);
        mrInt4 = (Int32)(Int32.MaxValue);
        mrInt8 = (Int64)(Int64.MinValue);
        mrSingle = (Single)(Single.MaxValue);
        mrDouble = (Double)(Double.MinValue);
        myQueue.Enqueue(str1);
        myQueue.Enqueue(str2);
        myQueue.Enqueue(mrBool);
        myQueue.Enqueue(mrByte);
        myQueue.Enqueue(mrInt2);
        myQueue.Enqueue(mrInt4);
        myQueue.Enqueue(mrInt8);
        myQueue.Enqueue(mrSingle);
        myQueue.Enqueue(mrDouble);
        iCountTestcases++;
        if(myQueue.Peek() != str1)
        {
            iCountErrors++;
            print("E_32ap");
        }
        iCountTestcases++;
        myQueue.Dequeue();
        if(myQueue.Peek() != str2)
        {
            iCountErrors++;
            print("E_239s");
        }
        iCountTestcases++;
        myQueue.Dequeue();
        if(Convert.ToBoolean(myQueue.Peek()) != mrBool)
        {
            iCountErrors++;
            print("E_3qwu");
        }
        iCountTestcases++;
        myQueue.Dequeue();
        if(Convert.ToByte(myQueue.Peek()) != mrByte)
        {
            iCountErrors++;
            print("E_1wia");
        }
        iCountTestcases++;
        myQueue.Dequeue();
        if(Convert.ToInt16(myQueue.Peek()) != mrInt2)
        {
            iCountErrors++;
            print("E_29ds");
        }
        iCountTestcases++;
        myQueue.Dequeue();
        if(Convert.ToInt32(myQueue.Peek()) != mrInt4)
        {
            iCountErrors++;
            print("E_9sid");
        }
        iCountTestcases++;
        myQueue.Dequeue();
        if(Convert.ToInt64(myQueue.Peek()) != mrInt8)
        {
            iCountErrors++;
            print("E_57di");
        }
        myQueue.Dequeue();
        iCountTestcases++;
        if(Convert.ToSingle(myQueue.Peek()) != mrSingle)
        {
            iCountErrors++;
            print("E_39ss");
        }
        myQueue.Dequeue();
        iCountTestcases++;
        if(Convert.ToDouble(myQueue.Peek()) != mrDouble)
        {
            iCountErrors++;
            print("E_5sdf");
        }
        myQueue.Enqueue(mrChar);
        iCountTestcases++;
        if(Convert.ToDouble(myQueue.Peek()) != mrDouble)
        {
            iCountErrors++;
            print("E_39bv");
        }
        myQueue.Dequeue();
        iCountTestcases++;
        if(Convert.ToChar(myQueue.Dequeue()) != mrChar)
        {
            iCountTestcases++;
            print("E_382i");
        }
        if ( iCountErrors == 0 )
        {
            Console.Error.Write( "String\\Co3106Peek.cs: paSs.  iCountTestcases==" );
            Console.Error.WriteLine( iCountTestcases );
            return true;
        }
        else
        {
            Console.Error.WriteLine(  "Co3106Peek.cs"  );
            Console.Error.Write( "Co3106Peek.cs iCountErrors==" );
            Console.Error.WriteLine( iCountErrors );
            Console.Error.WriteLine( "String\\Co3106Peek.cs: FAiL!" );
            return false;
        }
    }
    private void print(String error)
    {
        StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
        output.Append(error);
        output.Append(" (Co3106Peek.cs)");
        Console.Out.WriteLine(output.ToString());
    }
    public static void Main( String[] args )
    {
        bool bResult = false; 
        Co3106Peek cb0 = new Co3106Peek();
        try
        {
            bResult = cb0.runTest();
        }
        catch ( System.Exception exc )
        {
            bResult = false;
            System.Console.Error.WriteLine( "Co3106Peek.cs")  ;
            System.Console.Error.WriteLine( exc.ToString() );
        }
        if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
    }
}
