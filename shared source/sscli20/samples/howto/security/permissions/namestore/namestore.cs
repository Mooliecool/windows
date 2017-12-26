//------------------------------------------------------------------------------
// <copyright file="namestore.cs" company="Microsoft">
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
using System.IO;
using System.Reflection;
using System.Security.Permissions;
using System.Security;

[assembly:AssemblyVersionAttribute("1.0.1.0")]
[assembly:AssemblyKeyFileAttribute("namestore.snk")]

// Note: the AllowPartiallyTrustedCallers attribute *must* be on any strong named assembly 
// that will be loaded by another strongnamed assembly once FullTrust is changed to Everything.
[assembly:System.Security.AllowPartiallyTrustedCallers()]

/// <summary>
/// Intermediary class
/// </summary>
public class NameStore 
{
  public NameStore() {
    // check for myperm permission on construction, if caller(s) don't
    // have it then you can't use this object
    new myperm().Demand();
  } //constructor
  
  public void AddName(string name) {
    // Don't assert FileIOPermission, caller must have it!
    FileStream fs = new FileStream("names.txt", FileMode.OpenOrCreate, FileAccess.ReadWrite);
    StreamWriter sw = new StreamWriter(fs);
    fs.Seek(0, SeekOrigin.End);
    sw.WriteLine(name);
    sw.Close();
  } //AddName()

  public StringCollection GetNames() {
    // Assert FileIOPermission, caller may not have it!
    new FileIOPermission(PermissionState.Unrestricted).Assert();

    FileStream fs = new FileStream("names.txt", FileMode.Open,FileAccess.ReadWrite);
    StreamReader sr = new StreamReader(fs);
    StringCollection sc = new StringCollection();
    string s = sr.ReadLine();
    while (s != null) {
      sc.Add(s);
      s = sr.ReadLine();
    } //while
    fs.Close();
    return sc;
  } //GetNames()
} //class NameStore






