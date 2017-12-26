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

//*************************************************************************************
// Vsa2.cs
//
//  Contain the definitons of all the extra interfaces needed to make IActiveScript work
//*************************************************************************************

namespace Microsoft.JScript{

  using Microsoft.JScript.Vsa;
  using System;
  using Microsoft.Vsa;
  using System.Reflection;
  using System.Reflection.Emit;
  using System.Runtime.InteropServices;
  using System.Security;
  using System.Security.Permissions;

  //*************************************************************************************
  // IEngine2
  //
  //  Implemented by the VsaEngine, provide the extra functionality needed by an
  //  IActiveScript host
  //*************************************************************************************
  [System.Runtime.InteropServices.GuidAttribute("BFF6C97F-0705-4394-88B8-A03A4B8B4CD7")]
  [System.Runtime.InteropServices.ComVisible(true)]
  public interface IEngine2{
    System.Reflection.Assembly GetAssembly();
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void Run(System.AppDomain domain);
    bool CompileEmpty();
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void RunEmpty();
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void DisconnectEvents();
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void ConnectEvents();
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void RegisterEventSource(String name);
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void Interrupt();
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void InitVsaEngine(String rootMoniker, IVsaSite site);
    IVsaScriptScope GetGlobalScope();
    Module GetModule();
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    IVsaEngine Clone(System.AppDomain domain);
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    void Restart();
  }

  //*************************************************************************************
  // ISite2
  //
  //  Implemented by the IActiveScript site, provide the extra functionality needed by
  //  the VsaEngine
  //*************************************************************************************
  [System.Runtime.InteropServices.GuidAttribute("BFF6C980-0705-4394-88B8-A03A4B8B4CD7")]
  [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
  [System.Runtime.InteropServices.ComVisible(true)]
  public interface ISite2{
    Object[] GetParentChain(Object obj);
  }


  //*************************************************************************************
  // VSAITEMTYPE2
  //
  //  Extendion to the types of Item Type available
  //*************************************************************************************
  [System.Runtime.InteropServices.GuidAttribute("581AD3D9-2BAA-3770-B92B-38607E1B463A")]
  [System.Runtime.InteropServices.ComVisible(true)]
  public enum VSAITEMTYPE2{
    None = 0x00,
    HOSTOBJECT = 0x10,
    HOSTSCOPE,
    HOSTSCOPEANDOBJECT,
    SCRIPTSCOPE,
  }


  //*************************************************************************************
  // IVsaScriptScope
  //
  //  What was known to be a module in the IActiveScript interface. This functionality
  //  is what behavior in IE will use.
  //*************************************************************************************
  [System.Runtime.InteropServices.GuidAttribute("ED4BAE22-2F3C-419a-B487-CF869E716B95")]
  [System.Runtime.InteropServices.ComVisible(true)]
  public interface IVsaScriptScope : IVsaItem{
    IVsaScriptScope Parent{ get; }

    IVsaItem AddItem(string itemName, VsaItemType type);
    IVsaItem GetItem(string itemName);
    void RemoveItem(string itemName);
    void RemoveItem(IVsaItem item);

    int GetItemCount();
    IVsaItem GetItemAtIndex(int index);
    void RemoveItemAtIndex(int index);
    Object GetObject();

    IVsaItem CreateDynamicItem(string itemName, VsaItemType type);
  }


  //*************************************************************************************
  // IVsaFullErrorInfo
  //
  //  Used by the authoring environment to get extra information about errors
  //*************************************************************************************
  [System.Runtime.InteropServices.GuidAttribute("DC3691BC-F188-4b67-8338-326671E0F3F6")]
  [System.Runtime.InteropServices.ComVisible(true)]
  public interface IVsaFullErrorInfo : Microsoft.Vsa.IVsaError{
    int EndLine{ get; }
  }

}
