//------------------------------------------------------------------------------
// <copyright file="myc.cs" company="Microsoft">
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

namespace MyC
{
 
/*
 * My simple C compiler
 *
 * Brad Merrill
 * 17-Apr-1999
 */
using System;

public class MyC
{
public const int MAXBUF = 512;
public const int MAXSTR = 128;

/* main program */
public static void Main()
  {
  try
    {
    String[] args = Environment.GetCommandLineArgs();
    Io prog = new Io(args);
    Tok tok = new Tok(prog);
    Parse p = new Parse(prog, tok);
    p.program();
    prog.Finish();
    }
  catch (Exception e)
    {
    Console.WriteLine("Compiler aborting: "+e.ToString());
    }
  }
}
}
