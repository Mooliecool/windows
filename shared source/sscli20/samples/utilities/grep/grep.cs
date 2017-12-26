//------------------------------------------------------------------------------
// <copyright file="grep.cs" company="Microsoft">
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
using System.IO;
using System.Text.RegularExpressions;
using System.Collections;
using System.Diagnostics;
using System.Text;


public class grep 
{
  static bool showHelp = false;
  static bool ignoreCase = false;
  static bool subDirs = false;
  static bool quiet = false;
  static bool filenamesOnly = false;
  static bool linesOnly = false;
  static bool showExpr = false;
  static bool testReplace = false;
  static bool showBothExprs = false;
  static ArrayList filespecs = new ArrayList();
  static string pattern = null;
  static string replacePattern = null;
  static Regex regex;
  static int totalMatches, filesMatched;


  public static void Main(string[] args) 
  {
    try 
    {
      IEnumerator argEnumerator = args.GetEnumerator();
      while (argEnumerator.MoveNext()) 
      {
        string arg = (string) argEnumerator.Current;
#if PLATFORM_UNIX
                // Since fully qualified paths might start with '/' on Unix
                // Don't treat '/' as an option delimiter
                if (arg[0] == '-')
#else
        if (arg[0] == '-' || arg[0] == '/') 
#endif
        {
          arg = arg.Substring(1);
          if (arg.IndexOf('?') != -1)
            showHelp = true;
          if (arg.IndexOf('i') != -1)
            ignoreCase = true;
          if (arg.IndexOf('s') != -1)
            subDirs = true;
          if (arg.IndexOf('q') != -1)
            quiet = true;
          if (arg.IndexOf('m') != -1)
            filenamesOnly = true;
          if (arg.IndexOf('l') != -1)
            linesOnly = true;
          if (arg.IndexOf('e') != -1)
            showExpr = true;
          if (arg.IndexOf('E') != -1)
            showBothExprs = true;
          if (arg.IndexOf('t') != -1)
            testReplace = true;
          if (arg.IndexOf('r') != -1) 
          {
            bool ok = argEnumerator.MoveNext();
            if (!ok) 
            {
              showHelp = true;
              continue;
            }
            replacePattern = (string) argEnumerator.Current;
            continue;
          }
        }
        else if (pattern == null)
          pattern = arg;
        else
          filespecs.Add(arg);
      }

      if (showHelp || pattern == null || (linesOnly && filenamesOnly) || (testReplace && replacePattern == null)) 
      {
        ShowHelp();
        return;
      }

      RegexOptions options = RegexOptions.Compiled;
      if (ignoreCase)
        options |= RegexOptions.IgnoreCase;
      regex = new Regex(pattern, options);

      totalMatches = 0;
      filesMatched = 0;
      ScanFiles(Environment.CurrentDirectory);
      if (!filenamesOnly && !linesOnly) Console.WriteLine("Matched " + totalMatches + " lines in " + filesMatched + " files.");
    }
    catch (Exception e) 
    {
      Console.WriteLine(e.ToString());
      Environment.ExitCode = 1;
    }
  }

  private static int ProcessFile(string filename, TextReader reader, TextWriter writer) 
  {
    int lineNum = 1;
    string line = reader.ReadLine();
    int matches = 0;
    bool filePrinted = false;
    string newLine = null;
    while (line != null) 
    {
      newLine = line;
      Match match = regex.Match(line);
      if (match.Success) 
      {
        matches++;
        totalMatches++;
        if (replacePattern != null)
          newLine = regex.Replace(line, replacePattern);
        if (filenamesOnly) 
        {
          if (!quiet && !filePrinted) Console.WriteLine(filename);
          filePrinted = true;
          if (replacePattern == null)
            break;
        }
        else if (!quiet) 
        {
          if (!linesOnly) Console.Write(filename + "(" + lineNum + "): ");
          if (showExpr) 
          {
            int i = 0;
            while (match != null && match.Success) 
            {
              if (replacePattern == null || showBothExprs)
                Console.Write(match.ToString());
              if (showBothExprs)
                Console.Write(" -> ");
              if (replacePattern != null)
                Console.Write(match.Result(replacePattern));
              Console.WriteLine();
              match = match.NextMatch();
              i++;
            }
          }
          else 
          {
            Console.Write(line);
            if (replacePattern != null)
              Console.Write(" -> " + newLine);
            Console.WriteLine();
          }
        }
      }
      if (replacePattern != null && !testReplace)
        writer.WriteLine(newLine);
      line = reader.ReadLine();
      lineNum++;
    }
    return matches;
  }

  private static bool IsFullyQualified(string path) 
  {
    return Path.GetDirectoryName(path) != null;
  }

  private static void ScanFiles(string dir) 
  {
    bool processStdIn = true;
    DirectoryInfo curDir = new DirectoryInfo(dir);
    foreach (string filespec in filespecs) 
    {
      processStdIn = false;
      if (IsFullyQualified(filespec)) 
      {
        ScanFile(filespec);
      }
      else 
      {
        FileInfo[] files = curDir.GetFiles(filespec);
        foreach (FileInfo file in files) 
        {
          ScanFile(file.FullName);
        }
      }
    }
    if (processStdIn) 
    {
      int matches = ProcessFile("", Console.In, Console.Out);
      if (matches > 0) 
      {
        if (!quiet && !filenamesOnly && !linesOnly) Console.WriteLine("stdin: " + matches + " lines matched.");
        filesMatched++;
      }
    }
    else if (subDirs) 
    {
      DirectoryInfo[] dirs = curDir.GetDirectories("*");
      foreach (DirectoryInfo newDir in dirs)
        ScanFiles(newDir.FullName);
    }
  }

  private static void ScanFile(string filename) 
  {
    StreamReader reader = new StreamReader(filename);
    StreamWriter writer = null;
    string tempFileName = null;
    if (replacePattern != null && !testReplace) 
    {
      tempFileName = Path.GetTempFileName();
      writer = new StreamWriter(tempFileName);
    }
    int matches = 0;
    try 
    {
      matches = ProcessFile(filename, reader, writer);
    }
    finally 
    {
      reader.Close();
      if (writer != null)
        writer.Close();
    }
    if (matches > 0) 
    {
      if (writer != null) 
      {
        try 
        {
          File.Delete(filename);
          File.Move(tempFileName, filename);
        }
        catch (Exception e) 
        {
          Console.WriteLine("grep: error: Unable to write " + filename +": " + e.GetType().FullName + " (" + e.Message + "). The file has not been changed.");
          File.Delete(tempFileName);
        }
      }
      if (!quiet && !filenamesOnly && !linesOnly) Console.WriteLine(filename + ": " + matches + " lines matched.");
      filesMatched++;
    }
    else if (writer != null)
      File.Delete(tempFileName);
  }

  private static void ShowHelp() 
  {
    Console.WriteLine(
      @"ngrep: Regular expression searcher and replacer based on 
the System.Text.RegularExpressions.Regex class.

Usage: 
  ngrep [-ismqleEt?] [-r replacePattern] pattern [files [files ...]]

Options:
    -?  show this help
    -i  ignore case
    -s  recurse into subdirectories
    -m  only show filenames, not matching lines
    -q  quiet: only show summary data
    -l  lines only: don't show filename/line number. Can't be used with -m
    -e  show only the part of the line matching the expression, not the whole line.
    -E  show original and replacement expressions (with -r). Expressions are separated with "" -> ""
    -r  replace all matches with the given pattern
    -t  test: pretend to replace, but don't modify the file. Use with -r.

Options may occur anywhere on the command line.
If no files are specified, stdin is used.

The combination of -etr is useful for printing only part of what was found. For example,
   ngrep Debug\.Assert\((\w+)\) -letr $1

on a file containing

   Debug.Assert(excep);

will print only
   excep");
  }

}
