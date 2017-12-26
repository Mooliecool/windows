//------------------------------------------------------------------------------
// <copyright file="io.cs" company="Microsoft">
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
using System;
using System.Text;
using System.IO;
#if EXEGEN
using System.Reflection;
#endif

class Io
{
private FileStream ifile;
private StreamReader rfile;

private FileStream lst_ofile;
private StreamWriter lst_wfile;

private static String ifilename;
private static String ofilename;
private static String lst_ofilename;
private static String classname;

public static bool gendebug = false;
public static bool genexe = true;	// default to generating exe's
public static bool gendll = false;
public static bool genlist = false;
public static string genpath = ".";	// default output directory to current dir

private String[] args;
private char[] ibuf;

private int ibufidx;
private int ibufread;
private bool _eof;

/* variable declarations */
private char look;	/* lookahead character */
private StringBuilder buf;		/* buffer for comment tracking */
int bufline = 0;		// current line matching buffer
int curline = 0;		// current comment line number

public static string GetClassname()
  {
  return classname;
  }

public String GetInputFilename()
  {
  return ifilename;
  }
/* read new character from input stream */
public void ReadChar()
  {
  if (_eof)			// if already eof, nothing to do here
    return;
  if (ibuf == null || ibufidx >= MyC.MAXBUF)
    {
    ibuf = new char[MyC.MAXBUF];
    _eof = false;
    ibufread = rfile.Read(ibuf, 0, MyC.MAXBUF);
    ibufidx = 0;
    if (buf == null)
      buf = new StringBuilder(MyC.MAXSTR);
    }
  look = ibuf[ibufidx++];
  if (ibufread < MyC.MAXBUF && ibufidx > ibufread)
    _eof = true;

  /*
   * track the read characters
   */
  buf.Append(look);
  if (look == '\n')
    bufline++;
  }

public char getNextChar()
  {
  return look;
  }

public void Abort(string s)
  {
  StringBuilder sb = new StringBuilder();
  sb.Append(ifilename);
  sb.Append("(");
  sb.Append(curline+1);
  sb.Append(") : error M0000: ");
  sb.Append(s);
  Console.WriteLine(sb.ToString());
  throw new ApplicationException("Aborting compilation");
  }

public static void ICE(string s) // internal compiler error
  {
  StringBuilder sb = new StringBuilder();
  sb.Append(ifilename);
  sb.Append("(0) : error M9999: Internal Compiler Error: ");
  sb.Append(s);
  Console.WriteLine(sb.ToString());
  throw new ApplicationException("Aborting compilation");
  }

void ParseArgs()
  {
  int i = 1;

  if (args.Length < 2)
    {
    Abort("myc [/debug] [/nodebug] [/list] [/dll] [/exe] [/outdir:path] filename.myc\n");
    }

  while (true)
    {
    if (args[i][0] != '/')
      break;
    if (args[i].Equals("/?"))
      {
      Console.WriteLine("Compiler options:\n  myc [/debug] [/nodebug] [/list] [/dll] [/exe] [/outdir:path] filename.myc\n");
      Environment.Exit(1);
      }
    if (args[i].Equals("/debug"))
      {
      gendebug = true;
      i++;
      continue;
      }
    if (args[i].Equals("/nodebug"))
      {
      gendebug = false;
      i++;
      continue;
      }
    if (args[i].Equals("/exe"))
      {
      genexe = true;
      gendll = false;
      i++;
      continue;
      }
    if (args[i].Equals("/dll"))
      {
      gendll = true;
      genexe = false;
      i++;
      continue;
      }
    if (args[i].Equals("/list"))
      {
      genlist = true;
      i++;
      continue;
      }
    if (args[i].Length > 8 && args[i].Substring(0,8).Equals("/outdir:"))
      {
      genpath = args[i].Substring(8);
      i++;
      continue;
      }
    /*
     * exit if no switch matched
     */
    Abort("Unmatched switch = '"+args[i]+"'\nArguments are:\nmyc [/debug] [/nodebug] [/list] [/dll] [/exe] [/outdir:path] filename.myc\n");
    }

  if (args.Length-i != 1)
    {
    Abort("myc [/debug] [/nodebug] [/list] [/dll] [/exe] [/outdir:path] filename.myc\n");
    }
  ifilename = args[args.Length-1]; // filename is last
  }

public Io(String[] a)
  {
  int i;

  args = a;
  ParseArgs();

  ifile = new FileStream(ifilename, FileMode.Open,
			 FileAccess.Read, FileShare.Read, 8192);
  if (ifile == null)
    {
    Abort("Could not open file '"+ifilename+"'\n");
    }
  rfile = new StreamReader(ifile); // open up a stream for reading

  /*
   * for now we are going to create a default class using the filename
   */
  i = ifilename.LastIndexOf('.');
  if (i < 0)
    Abort("Bad filename '"+ifilename+"'");
  int j = ifilename.LastIndexOf('\\');
  if (j < 0)
    j = 0;
  else
    j++;

  classname = ifilename.Substring(j,i-j);
  if (genexe)
    ofilename = classname+".exe";
  if (gendll)
    ofilename = classname+".dll";
  if (genlist)
    {
    lst_ofilename = classname+".lst";
    lst_ofile = new FileStream(lst_ofilename, FileMode.Create,
			 FileAccess.Write, FileShare.Write, 8192);
    if (lst_ofile == null)
      Abort("Could not open file '"+ofilename+"'\n");
    lst_wfile = new StreamWriter(lst_ofile);
    }
  }

public void Out(String s)
  {
  lst_wfile.Write(s);		// write the buffer
  lst_wfile.Flush();		// slow, but useful
  }

public void Finish()
  {
  rfile.Close();
  ifile.Close();
  if (genlist)
    {
    lst_wfile.Close();
    lst_ofile.Close();
    }
  }

public bool EOF()
  {
  return _eof;
  }


public String commentEndPreTok(String s)
  {
#if DEBUG
  Console.Write("commentEndPreTok1 S=["+s+"], buf=");
  for (int _debug_i=0; _debug_i<buf.Length;_debug_i++)
    {
    int _debug_d = buf[_debug_i];
    char _debug_c = (char) (_debug_d + 96);
    if (_debug_d < 32)
      Console.Write("^"+Char.ToString(_debug_c));
    else
      Console.Write(buf[_debug_i]);
    Console.Write("[");
    Console.Write(_debug_d);
    Console.Write("],");
    }
  Console.WriteLine(";");
#endif
  /*
   * many times we will already have parsed source code past the point
   * that we want to emit.  We will use the token given to backup.
   */
  String b;
  if (s == null)		// make sure we have something
    return null;
  b = buf.ToString();		// have to convert first
  int i = b.LastIndexOf(s);	// find this token in buffer
  String c = b.Substring(0,i).Trim(); // copy as comment
  buf = new StringBuilder(b.Substring(i), MyC.MAXSTR); // remake buffer
  /*
   * need to update curline to be in synch with last emitted comment
   */
  curline = bufline;
  for (int ci = 0; ci < buf.Length; ci++)
    if (buf[ci] == '\n')
      curline--;
#if DEBUG
  Console.Write("commentEndPreTok2 buf=");
  for (int _debug_i=0; _debug_i<buf.Length;_debug_i++)
    {
    int _debug_d = buf[_debug_i];
    char _debug_c = (char) (_debug_d + 96);
    if (_debug_d < 32)
      Console.Write("^"+_debug_c);
    else
      Console.Write(buf[_debug_i]);
    Console.Write("[");
    Console.Write(_debug_d);
    Console.Write("],");
    }
  Console.WriteLine(";");
#endif
  return c;
  }

public String commentEndTok(String s)
  {
#if DEBUG
  Console.Write("commentEndTok1 S=["+s+"], buf=");
  for (int _debug_i=0; _debug_i<buf.Length;_debug_i++)
    {
    int _debug_d = buf[_debug_i];
    char _debug_c = (char) (_debug_d + 96);
    if (_debug_d < 32)
      Console.Write("^"+_debug_c);
    else
      Console.Write(buf[_debug_i]);
    Console.Write("[");
    Console.Write(_debug_d);
    Console.Write("],");
    }
  Console.WriteLine(";");
#endif

  /*
   * variant to include this token at end of comment
   */
  String b;
  if (s == null)		// make sure we have something
    return null;
  b = buf.ToString();		// have to convert first
  int i = b.LastIndexOf(s);	// find this token in buffer
  String c = b.Substring(0,i+s.Length).Trim(); // copy as comment
  buf = new StringBuilder(b.Substring(i+s.Length), MyC.MAXSTR); // remake buffer
  /*
   * need to update curline to be in synch with last emitted comment
   */
  curline = bufline;
  for (int ci = 0; ci < buf.Length; ci++)
    if (buf[ci] == '\n')
      curline--;
#if DEBUG
  Console.Write("commentEndTok2 buf=");
  for (int _debug_i=0; _debug_i<buf.Length;_debug_i++)
    {
    int _debug_d = buf[_debug_i];
    char _debug_c = (char) (_debug_d + 96);
    if (_debug_d < 32)
      Console.Write("^"+_debug_c);
    else
      Console.Write(buf[_debug_i]);
    Console.Write("[");
    Console.Write(_debug_d);
    Console.Write("],");
    }
  Console.WriteLine(";");
#endif
  return c;
  }

public void commentBegin(String s)
  {
  String b;
  if (s == null)		// make sure we have something
    return;
  b = buf.ToString();		// have to convert first
  int i = b.IndexOf(s);		// find this token in buffer
  if (i < 0)
    i = b.Length;		// if not found, use whole string
  buf = new StringBuilder(b.Substring(i), MyC.MAXSTR); // remake buffer from substr
  /*
   * need to update curline to be in synch with last emitted comment
   */
  curline = bufline;
  for (int ci = 0; ci < buf.Length; ci++)
    if (buf[ci] == '\n')
      curline--;
#if DEBUG
  Console.Write("commentBegin S=["+s+"], buf=");
  for (int _debug_i=0; _debug_i<buf.Length;_debug_i++)
    {
    int _debug_d = buf[_debug_i];
    char _debug_c = (char) (_debug_d + 96);
    if (_debug_d < 32)
      Console.Write("^"+_debug_c);
    else
      Console.Write(buf[_debug_i]);
    Console.Write("[");
    Console.Write(_debug_d);
    Console.Write("],");
    }
  Console.WriteLine(";");
#endif
  }

public int commentGetCurrentLine()
  {
  return curline+1;
  }

public static string GetOutputFilename()
  {
  return ofilename;
  }

}
}
