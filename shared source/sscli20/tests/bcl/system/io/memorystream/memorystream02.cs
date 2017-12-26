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
using System;
using System.IO;
using System.IO.IsolatedStorage;
using System.Reflection;
using System.Security.Principal;
class bug
{	
    static int iCountTestcases = 0 , iCountErrors = 0 ;
    public static void Main(String[] args) 
    {
        try
        {
            iCountTestcases++ ;
            MemStreamClearWriteByteTest();
        } 
        catch( Exception )
        {
            iCountErrors++ ;
            Console.WriteLine("Error_1212!!!! Unexpected exception occured....");
        }
        if( iCountErrors == 0 ) Environment.ExitCode = 0; else Environment.ExitCode = 1;                
    }
    public static bool MemStreamClearWriteByteTest()
    {
        Console.WriteLine("Ensuring that we clear data > Length in a MemoryStream when we write past the end via WriteByte");
        const int len = 10;
        const int spanPastEnd = 5;
        MemoryStream ms = new MemoryStream(3*len);
        byte[] bytes = new byte[len];
        for(int i=0; i<bytes.Length; i++)
            bytes[i] = (byte) i;
        ms.Write(bytes, 0, bytes.Length);
        for(int i=0; i<2*len; i++)
            ms.WriteByte((byte)255);
        ms.SetLength(len);
        ms.Seek(spanPastEnd, SeekOrigin.End);
        for(int i=0; i<bytes.Length; i++)
            ms.WriteByte(bytes[i]);
        ms.Position = bytes.Length;
        byte[] newData = new byte[bytes.Length + spanPastEnd];
        int n = ms.Read(newData, 0, newData.Length);
        if (n != newData.Length) 
        {
            iCountErrors++ ;
            throw new Exception("Hmmm, maybe a bug in the stream.  Asked to read "+newData.Length+", but got back "+n+" bytes.");
        }
        for(int i=0; i<spanPastEnd; i++)
        {
            if (newData[i] != 0)
            {
                iCountErrors++ ;
                throw new Exception(String.Format("New data in the middle of the stream should have been all 0's, but at position {0} I got a wrong byte: {1} [0x{1:x}]!", i+bytes.Length, newData[i]));
            }
        }
        for(int i=0; i<bytes.Length; i++)
        {
            if (newData[i+spanPastEnd] != bytes[i])
            {
                iCountErrors++ ;
                throw new Exception(String.Format("New data at the end of the stream should have been equal to our byte[], but the {0}'th new byte was a wrong byte: {1} [0x{1:x}]!", i, newData[i+spanPastEnd]));
            }
        }
        ms.Flush();
        ms.Close();
        return true;
    }    
}