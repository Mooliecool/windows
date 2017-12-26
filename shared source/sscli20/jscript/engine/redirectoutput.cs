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

namespace Microsoft.JScript {
   
    using System;
    using System.Text;
    using System.IO;

  //*************************************************************************************
  // IRedirectOutput
  //
  //  Implemented by VSAEngine; give a host a way to catch output that would otherwise
  //  go to console or nowhere when no console is up
  //*************************************************************************************
  [System.Runtime.InteropServices.GuidAttribute("5B807FA1-00CD-46ee-A493-FD80AC944715")]
  [System.Runtime.InteropServices.ComVisible(true)]
  public interface IRedirectOutput {
    void SetOutputStream(IMessageReceiver output);
  }

  //*************************************************************************************
  // IMessageReceiver
  //
  //  Implemented by a host that wants to receive output from an engine. It is 
  //  essentially a callback
  //*************************************************************************************
  [System.Runtime.InteropServices.GuidAttribute("F062C7FB-53BF-4f0d-B0F6-D66C5948E63F")]
  [System.Runtime.InteropServices.ComVisible(true)]
  public interface IMessageReceiver {
    void Message(String strValue);
  }

  //*************************************************************************************
  // COMCharStream
  //
  //  Define a stream over an IMessageReceiver. The stream is then used to set
  //  the Out and Err static variable in ScriptStream
  //*************************************************************************************
  public class COMCharStream : Stream{ 
    
    private IMessageReceiver messageReceiver; 
    private StringBuilder buffer;
    
    public COMCharStream(IMessageReceiver messageReceiver) {
      this.messageReceiver = messageReceiver;
      this.buffer = new StringBuilder(128);
    }
  
    public override bool CanWrite {
      get { return true; }
    }

    public override bool CanRead {
      get { return false; }
    }

    public override bool CanSeek {
      get { return false; }
    }

    public override long Length {
      get { return this.buffer.Length; }
    }

    public override long Position {
      get { return this.buffer.Length; }
      set {}
    }
      
    public override void Close(){
      Flush();
    }
      
    public override void Flush() {
      this.messageReceiver.Message(this.buffer.ToString());
      this.buffer = new StringBuilder(128);
    }

    public override int Read(byte[] buffer, int offset, int count){
      throw new NotSupportedException();
    }
    
    public override long Seek(long offset, SeekOrigin origin){
      return 0;
    }
    
    public override void SetLength(long value){
      this.buffer.Length = (int)value;
    }

    public override void Write(byte[] buffer, int offset, int count) {
      for (int i = count; i > 0; i--)
        this.buffer.Append((char)buffer[offset++]);
    }
     
  }
}
