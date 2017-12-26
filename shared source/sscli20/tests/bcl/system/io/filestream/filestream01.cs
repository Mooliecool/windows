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
            FileStream hackyTest = new FileStream("hackyTest.junk", FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite, 4096, true);
            ReadBeyondEndTest(hackyTest);
            hackyTest.Close();
        } 
        catch( Exception )
        {
            iCountErrors++ ;
            Console.WriteLine("Error_1212!!!! Unexpected exception occured....");
        }        
        if( iCountErrors == 0 ) Environment.ExitCode = 0; else Environment.ExitCode = 1;        
    }
    public static bool ReadBeyondEndTest(Stream s)
    {
        Console.WriteLine("Read Beyond End test on "+s.GetType().Name);
        byte[] bytes = new byte[10];
        for(int i=0; i<bytes.Length; i++)
            bytes[i] = (byte) i;
        s.Seek(5, SeekOrigin.End);
        if (s.Position != s.Length + 5)
        {
            iCountErrors++;
            throw new Exception("Position is incorrect!  Seek(5, SeekOrigin.End) should leave us at s.Length + 5, but got: "+s.Position);
        }
        int numRead = s.Read(bytes, 0, bytes.Length);
        if (numRead != 0)
        {
            iCountErrors++ ;
            throw new Exception("Reading from past end of stream is broken!  Expected 0, got: "+numRead);
        }
        for(int i=0; i<bytes.Length; i++)
        {
            if (bytes[i] != (byte) i)
            {
                iCountErrors++ ;
                throw new Exception("Error in byte[] - Read overwrote it!  pos: "+i+"  got: "+bytes[i]);
            }
        }
        numRead = s.ReadByte();
        if (numRead != -1)
        {
            iCountErrors++ ;
            throw new Exception("ReadByte didn't return -1!  got: "+numRead);
        }
        IAsyncResult ar = s.BeginRead(bytes, 0, bytes.Length, null, null);
        numRead = s.EndRead(ar);
        if (numRead != 0)
        {
            iCountErrors++ ;
            throw new Exception("Reading from past end of stream with BeginRead is broken!  Expected 0, got: "+numRead);
        }
        for(int i=0; i<bytes.Length; i++)
            if (bytes[i] != (byte) i)
            {
                iCountErrors++ ;
                throw new Exception("Error in byte[] - BeginRead overwrote it!  pos: "+i+"  got: "+bytes[i]);
            }
        return true;
    }
}
