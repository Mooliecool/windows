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
using System.Threading;
class MyStream : Stream {
 public bool UseWaitHandle() {
 try { 
 WaitHandle wh1 = this.CreateWaitHandle();
 WaitHandle wh2 = this.CreateWaitHandle();
 WaitHandle wh3 = this.CreateWaitHandle();
 WaitHandle wh4 = this.CreateWaitHandle();
 if(wh3.Equals(wh4)) {
 return false;
 }
 return true;
 }
 catch(Exception e) {  return false;}
 }
 public override bool CanRead {  get { return true;} }
 public  override bool CanWrite {get { return true;} }
 public override long Length { get { return (long)0;} }
 public  override long Position {get { return (long)0;}  set  {} }
 public override bool CanSeek {  get {return true;} }
 public override IAsyncResult BeginRead(byte[] buffer, int offset, int count, AsyncCallback callback, object state) {return null;}
 public override IAsyncResult BeginWrite(byte[] buffer, int offset, int count, AsyncCallback callback, object state) {return null;}
 public override void Close()  { }
 public override System.Runtime.Remoting.ObjRef CreateObjRef(Type requestedType)  { return null;}
 public override int EndRead(IAsyncResult asyncResult)  { return 0; }
 public override void EndWrite(IAsyncResult asyncResult)  { }
 public override bool Equals(object obj)  { return false;}
 public override void Flush()  { }
 public override int GetHashCode()  { return 0;}
 public override object InitializeLifetimeService()  {return null; }
 public override int Read(byte[] buffer, int offset, int count)  { return 0;}
 public override int ReadByte()  {return 0; }
 public override long Seek(long offset, System.IO.SeekOrigin origin)  { return (long)0;}
 public override void SetLength(long value)  { }
 public override string ToString()  {return null; }
 public override void Write(byte[] buffer, int offset, int count)  { }
 public override void WriteByte(byte value)  { }
}
class Test {
 public static void Main() {
 int errors = 0;
 int testcases = 0;
 testcases++;
 MyStream myStream = new MyStream();
 if(! myStream.UseWaitHandle())
   errors++;
 Environment.ExitCode = errors;
 }
}
