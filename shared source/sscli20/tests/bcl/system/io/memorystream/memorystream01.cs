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
            MemoryStream hackyTest = new MemoryStream();
            WriteBeyondEndTest(hackyTest);
            hackyTest.Close();
        } 
        catch( Exception )
        {
            iCountErrors++ ;
            Console.WriteLine("Error_1212!!!! Unexpected exception occured....");
        }
        if( iCountErrors == 0 ) Environment.ExitCode = 0; else Environment.ExitCode = 1;                
    }
    public static bool WriteBeyondEndTest(Stream s)
    {
        Console.WriteLine("Write Beyond End test on "+s.GetType().Name);
        FileStream fs = s as FileStream;
        if (fs != null)
            Console.WriteLine("FileStream type is: "+(fs.IsAsync ? "asynchronous" : "synchronous"));
        long origLength = s.Length;        
        byte[] bytes = new byte[10];
        for(int i=0; i<bytes.Length; i++)
            bytes[i] = (byte) i;
        int spanPastEnd = 5;
        s.Seek(spanPastEnd, SeekOrigin.End);
        if (s.Position != s.Length + spanPastEnd)
            throw new Exception("Position is incorrect!  Seek(5, SeekOrigin.End) should leave us at s.Length + spanPastEnd ("+(s.Length + spanPastEnd)+"), but got: "+s.Position);
        Console.WriteLine("Original Length: "+origLength);
        s.Write(bytes, 0, bytes.Length);
        long pos = s.Position;
        if (pos != origLength + spanPastEnd + bytes.Length)
            throw new Exception(String.Format("After asynchronously writing beyond end of the stream, position is now incorrect!  origLength: {0}  pos: {1}", origLength, pos));
        if (s.Length != origLength + spanPastEnd + bytes.Length)
            throw new Exception(String.Format("After asynchronously writing beyond end of the stream, Length is now incorrect!  origLength: {0}  pos: {1}", origLength, pos));
        WritePastEndHelper(s, bytes, origLength, spanPastEnd, false);
        origLength = s.Length;
        s.Position = s.Length + spanPastEnd;
        s.WriteByte(0x42);
        long expected = origLength + spanPastEnd + 1;
        if (s.Position != expected)
        {
            iCountErrors++ ;
            throw new Exception("After WriteByte, Position was wrong!  got: "+s.Position+"  expected: "+expected);
        }
        if (s.Length != expected)
        {
            iCountErrors++ ;
            throw new Exception("After WriteByte, Length was wrong!  got: "+s.Length+"  expected: "+expected);
        }
        origLength = s.Length;
        s.Position = s.Length + spanPastEnd;
        IAsyncResult ar = s.BeginWrite(bytes, 0, bytes.Length, null, null);
        s.EndWrite(ar);
        pos = s.Position;
        if (pos != origLength + spanPastEnd + bytes.Length) 
        {
            iCountErrors++ ;
            throw new Exception(String.Format("After writing beyond end of the stream, position is now incorrect!  origLength: {0}  pos: {1}", origLength, pos));
        }
        if (s.Length != origLength + spanPastEnd + bytes.Length) 
        {
            iCountErrors++;
            throw new Exception(String.Format("After writing beyond end of the stream, Length is now incorrect!  origLength: {0}  pos: {1}", origLength, pos));
        }
        WritePastEndHelper(s, bytes, origLength, spanPastEnd, true);
        return true;
    }
    private static bool WritePastEndHelper(Stream s, byte[] bytes, long origLength, int spanPastEnd, bool afterAsync)
    {
        if (s.CanRead) 
        {
            s.Position = origLength;
            byte[] newData = new byte[bytes.Length + spanPastEnd];
            int n = s.Read(newData, 0, newData.Length);
            if (n != newData.Length)
                throw new Exception("Hmmm, maybe a bug in the stream.  Asked to read "+newData.Length+", but got back "+n+" bytes.");
            for(int i=0; i<spanPastEnd; i++)
                if (newData[i] != 0)
                    throw new Exception(String.Format("New data in the middle of the stream should have been all 0's, but at position {0} I got a wrong byte: {1} [0x{1:x}]!  Was call to BeginWrite: {2}", i, newData[i], afterAsync));
            for(int i=0; i<bytes.Length; i++)
                if (newData[i+spanPastEnd] != bytes[i])
                    throw new Exception(String.Format("New data at the end of the stream should have been equal to our byte[], but the {0}'th new byte was a wrong byte: {1} [0x{1:x}]!  Was call to BeginWrite: {2}", i, newData[i+spanPastEnd], afterAsync));
        }
        return true;
    }
}