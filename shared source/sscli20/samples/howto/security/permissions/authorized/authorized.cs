//------------------------------------------------------------------------------
// <copyright file="authorized.cs" company="Microsoft">
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
using System.Collections.Specialized;
using System.Reflection;

[assembly:AssemblyVersionAttribute("1.0.1.0")]
//Note that this uses the special authorized key file.  

[assembly:AssemblyKeyFileAttribute("authorized.snk")]

public class MyApp
{
  public static NameStore m_namestore;

/// <summary>
/// Main application entry point.
/// </summary>
  public static void Main() 
  {
    try
    {
      m_namestore = new NameStore();
    } //try
    catch (Exception e)
    {
      Console.WriteLine("Exception creating NameStore object: {0}", e.ToString());
      return;
    } //catch

    while (true)
    {
      Console.Write("\nDo you want to read (r) or add (a) to the names.txt file or exit (q)? ");
      string Reply = Console.ReadLine();

      switch (Reply.ToLower())
      {
        case "a":
          AddToList(m_namestore);
          break;

        case "r":
          ReadListNames(m_namestore);
          break;

        case "q":
          return;

        case "":
          return;

        default:
          Console.WriteLine("Invalid input, please try again.");
          continue;
      } //switch
    } //while (true)


  } //Main()

  public static void AddToList(NameStore n)
  {
    try 
    {
      // add a name
      Console.Write("\nName to add: ");
      string name = Console.ReadLine();
      if (name != null) 
      {
        n.AddName(name);
        Console.WriteLine("\nName = {0} added\n", name);
      } //if
    } //try
    catch (Exception e) 
    {
      Console.WriteLine("Attempt to add name failed due to:");
      Console.WriteLine(e.ToString());
    } //catch
  } //AddToList()

  public static void ReadListNames(NameStore n)
  {
    try 
    {
      // get the names form the name store
      Console.WriteLine("\nGetting list of stored names");
      StringCollection s = n.GetNames();

      foreach (string ns in s) 
      {
        Console.WriteLine(ns);
      } //foreach
    } //try
    catch (Exception e) 
    {
      Console.WriteLine("Attempt to get names failed due to:");
      Console.WriteLine(e);
    } //catch
  } //GetListNames()

} //class MyApp





