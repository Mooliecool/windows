//------------------------------------------------------------------------------
// <copyright file="shared.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;

namespace RemotingSamples 
{
  public enum InvokeKind {Async, Sequential}
  public enum ChannelKind {Both, Http, TCP}
  public enum CallingConvention {ByRef, ByVal}


  public class ForwardByRef : MarshalByRefObject 
  {
    public void CallMe(string text)
    {
      Console.WriteLine(text);
    }//CallMe
  }//class ForwardByRef

  public class HelloServerByRef : MarshalByRefObject 
  {
    private int m_CallCount = 0;
    public HelloServerByRef()
    {
      Console.WriteLine("HelloServer activated");
    }//constructor

    public string HelloMethod(string text, ForwardByRef fbr)
    {
      lock(this)
      {
        m_CallCount++;

        fbr.CallMe("Regards from the server");
        Console.WriteLine("\nHello.HelloMethod : {0}", text);
        Console.WriteLine("Number of calls on this instance: {0}\n", m_CallCount);
        return "Hi there " + text;
      }//lock
    }//HelloMethod()

  }//class HelloServerByRef

  [Serializable]
  public class ForwardByVal 
  {
    private string m_Message = null;

    public string Message
    {
      get 
      {
        return m_Message;
      }//get

      set 
      {
        m_Message = value;
      }//set
    }//property Count
  }//class ForwardByVal


  public class HelloServerByVal : MarshalByRefObject 
  {
    private int m_CallCount = 0;

    public HelloServerByVal()
    {
      Console.WriteLine("HelloServer activated");
    }//constructor

    public ForwardByVal HelloMethod(string text, ForwardByVal fbv)
    {
      lock(this)
      {
        m_CallCount++;

        fbv.Message = "Regards from the server " + text;
        Console.WriteLine("\nHello.HelloMethod in HelloServerByVal: {0}", text);
        Console.WriteLine("Number of calls on this instance: {0}\n", m_CallCount);
        return fbv;
      }//lock
    }//HelloMethod()
  }//class ForwardByVal
}//namespace RemotingSamples
