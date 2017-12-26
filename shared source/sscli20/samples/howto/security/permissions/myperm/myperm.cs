//------------------------------------------------------------------------------
// <copyright file="myperm.cs" company="Microsoft">
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
using System.Security;
using System.Security.Permissions;
using System.Reflection;

[assembly:AssemblyVersionAttribute("1.0.1.0")]
[assembly:AssemblyKeyFileAttribute("myperm.snk")]

// Note: the AllowPartiallyTrustedCallers attribute *must* be on any strong named assembly 
// that will be loaded by another strongnamed assembly once FullTrust is changed to Everything.
[assembly:System.Security.AllowPartiallyTrustedCallers()]

/// <summary>
/// Custom permissions class.
/// This assembly must be installed in the GAC in order to register it as a custom permission.
/// </summary>
public class myperm : CodeAccessPermission, IUnrestrictedPermission 
{
  public myperm() 
  {
  } //constructor
  
  public myperm(PermissionState ps) 
  {
  } //constructor 2
  
  public bool IsUnrestricted() 
  {
    return true;
  } //IsUnrestricted()

  public override IPermission Copy() 
  {
    return new myperm();
  } //Copy()
  
  public override IPermission Intersect(IPermission target) 
  {
    if (target == null) return null;
    if (typeof(myperm) != target.GetType()) return null;
    return new myperm();
  } //Intersect()

  public override IPermission Union(IPermission target) 
  {
    if (target == null) return new myperm();
    if (typeof(myperm) != target.GetType()) return null;
    return new myperm();
  } //Union()

  public override SecurityElement ToXml() 
  {
    SecurityElement s = new SecurityElement("IPermission");
    s.AddAttribute("class","myperm, myperm, Version=1.0.1.0, Culture=neutral, PublicKeyToken=0e8dcc8628396732");
    s.AddAttribute("version", "1");
    s.AddAttribute("Unrestricted", "true");
    return s;
  } //ToXml()

  public override void FromXml(SecurityElement elem) 
  {
  } //FromXml()

  public override bool IsSubsetOf(IPermission target) 
  {
    if (target == null) return false;
    if (typeof(myperm) != target.GetType()) return false;
    return true;
  } //IsSubsetOf()

} //class myperm






