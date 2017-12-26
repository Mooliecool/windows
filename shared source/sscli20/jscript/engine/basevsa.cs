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

namespace Microsoft.Vsa{

  using System;
  using System.Collections;
  using System.Diagnostics;
  using System.Globalization;
  using System.Reflection;
  using System.Runtime.InteropServices;
  using System.Security.Permissions;
  using System.Threading;
  using Microsoft.Win32;

  [Obsolete(Microsoft.JScript.Vsa.VsaObsolete.Description)]
  [GuidAttribute("F8932A50-9127-48B6-B115-2BFDC627CEE3")]
  [ComVisible(true)]
  public abstract class BaseVsaEngine : IVsaEngine{

    // === Members ===

    protected string applicationPath;
    protected System.Reflection.Assembly loadedAssembly;
    protected string compiledRootNamespace;
    protected IVsaSite engineSite;
    protected bool genDebugInfo;
    protected bool haveCompiledState; // true if there is compiled state in the engine
    protected bool failedCompilation; // true if IVsaEngine.Compile returned false on the most recent call (false by default)
    protected bool isClosed;
    protected bool isEngineCompiled;  // true if current compiled state reflects current source state
    protected bool isDebugInfoSupported;
    protected bool isEngineDirty;     // true if current source state does not reflect the persisted source state
    protected bool isEngineInitialized;
    protected bool isEngineRunning;
    protected IVsaItems vsaItems;
    protected string scriptLanguage;
    protected int errorLocale;
    static protected Hashtable nameTable = new Hashtable(10);
    protected string engineName;
    protected string engineMoniker;
    protected string rootNamespace;
    protected Type startupClass;
    protected BaseVsaStartup startupInstance;
    protected string assemblyVersion;
    protected System.Security.Policy.Evidence executionEvidence;

    // === Constructor ===

/////////////////////////////////////////////////////////////////////////////
//
// Security Issue
//
/////////////////////////////////////////////////////////////////////////////
//
// [EricLi] 12 November 2001
//
// We do not want third parties to extend BaseVsaEngine, so make the ctor
// internal.  If third parties cannot extend the interface then they cannot
// access the protected members either.
//
// In vnext we should eliminate this class altogether and ship a reference
// implementation of VsaEngine as source, not binary.
//
/////////////////////////////////////////////////////////////////////////////


    internal BaseVsaEngine(string language, string version, bool supportDebug){
      // Set default property values and initial state
      this.applicationPath = "";
      this.compiledRootNamespace = null;
      this.genDebugInfo = false;
      this.haveCompiledState = false;
      this.failedCompilation = false;
      this.isClosed = false;
      this.isEngineCompiled = false;
      this.isEngineDirty = false;
      this.isEngineInitialized = false;
      this.isEngineRunning = false;
      this.vsaItems = null;
      this.engineSite = null;
      this.errorLocale = CultureInfo.CurrentUICulture.LCID;
      this.engineName = "";
      this.rootNamespace = "";
      this.engineMoniker = "";

      // Set implementation-dependent values
      this.scriptLanguage = language;
      this.assemblyVersion = version;
      this.isDebugInfoSupported = supportDebug;
      this.executionEvidence = null;
    }

    // === Helper Methods ===

    protected VsaException Error(VsaError vsaErrorNumber){
      return new VsaException(vsaErrorNumber);
    }

    internal void TryObtainLock() {
      if (!Monitor.TryEnter(this))
        throw new VsaException(VsaError.EngineBusy);
    }

    internal void ReleaseLock() {
      Monitor.Exit(this);
    }

    // Pre is a set of preconditions that must hold in order to perform certain
    // operations on or with the engine

    [Flags()]
    protected enum Pre{
      None                  = 0x0000,
      EngineNotClosed       = 0x0001,     // optional; tested by default
      SupportForDebug       = 0x0002,
      EngineCompiled        = 0x0004,
      EngineRunning         = 0x0008,
      EngineNotRunning      = 0x0010,
      RootMonikerSet        = 0x0020,
      RootMonikerNotSet     = 0x0040,
      RootNamespaceSet      = 0x0080,
      SiteSet               = 0x0100,
      SiteNotSet            = 0x0200,
      EngineInitialised     = 0x0400,
      EngineNotInitialised  = 0x0800,
    }

    private bool IsCondition(Pre flag, Pre test){
      return ((flag & test) != Pre.None);
    }

    // The Preconditions method tests a set of preconditions and throws the
    // appropriate VsaException if any of them do not hold.
    // Pre.EngineNotClosed is always tested, regardless of whether or not it
    // appears in the flags bitfield (including when only Pre.None is indicated).

    protected void Preconditions(Pre flags){
      // Every operation on this object requires that the engine not be closed
      if (this.isClosed)
        throw Error(VsaError.EngineClosed);
      if (flags == (Pre.EngineNotClosed | Pre.None))
        return;

      if (IsCondition(flags, Pre.SupportForDebug) && !this.isDebugInfoSupported)
        throw Error(VsaError.DebugInfoNotSupported);
      if (IsCondition(flags, Pre.EngineCompiled) && !this.haveCompiledState)
        throw Error(VsaError.EngineNotCompiled);
      if (IsCondition(flags, Pre.EngineRunning) && !this.isEngineRunning)
        throw Error(VsaError.EngineNotRunning);
      if (IsCondition(flags, Pre.EngineNotRunning) && this.isEngineRunning)
        throw Error(VsaError.EngineRunning);
      if (IsCondition(flags, Pre.RootMonikerSet) && (this.engineMoniker == ""))
        throw Error(VsaError.RootMonikerNotSet);
      if (IsCondition(flags, Pre.RootMonikerNotSet) && (this.engineMoniker != ""))
        throw Error(VsaError.RootMonikerAlreadySet);
      if (IsCondition(flags, Pre.RootNamespaceSet) && (this.rootNamespace == ""))
        throw Error(VsaError.RootNamespaceNotSet);
      if (IsCondition(flags, Pre.SiteSet) && (this.engineSite == null))
        throw Error(VsaError.SiteNotSet);
      if (IsCondition(flags, Pre.SiteNotSet) && (this.engineSite != null))
        throw Error(VsaError.SiteAlreadySet);
      if (IsCondition(flags, Pre.EngineInitialised) && !this.isEngineInitialized)
        throw Error(VsaError.EngineNotInitialized);
      if (IsCondition(flags, Pre.EngineNotInitialised) && this.isEngineInitialized)
        throw Error(VsaError.EngineInitialized);
    }

    // === Properties ===

    // IVsaEngine.AppDomain is not supported by managed engines
    public System._AppDomain AppDomain{
      get{
        Preconditions(Pre.EngineNotClosed);
        throw new NotSupportedException();
      }
      set{
        Preconditions(Pre.EngineNotClosed);
        throw new VsaException(VsaError.AppDomainCannotBeSet);
      }
    }


/////////////////////////////////////////////////////////////////////////////
//
// Security Issue
//
/////////////////////////////////////////////////////////////////////////////
//
// [EricLi] 13 November 2001
//
// The Evidence property must demand permission to control evidence
// on both read and write.  On write because an untrusted caller
// could attempt to set the evidence to null, thereby causing the
// emitted assembly to use Microsoft.JScript.DLL's evidence, granting
// full trust.  On read because an untrusted assembly could 
// mutate it or derive information from it.
//
/////////////////////////////////////////////////////////////////////////////

    public System.Security.Policy.Evidence Evidence{
      [SecurityPermission(SecurityAction.Demand, ControlEvidence=true)]
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.executionEvidence;
      }
      [SecurityPermission(SecurityAction.Demand, ControlEvidence=true)]
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        Preconditions(Pre.EngineNotClosed |
                      Pre.EngineNotRunning | 
                      Pre.EngineInitialised);
        this.executionEvidence = value;
      }
    }

    // IVsaEngine.ApplicationBase is not supported by managed engines
    public string ApplicationBase{
      get{
        Preconditions(Pre.EngineNotClosed);
        throw new NotSupportedException();
      }
      set{
        Preconditions(Pre.EngineNotClosed);
        throw new VsaException(VsaError.ApplicationBaseCannotBeSet);
      }
    }

    public System.Reflection.Assembly Assembly{
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineRunning);
        return this.loadedAssembly;
      }
    }

    public bool GenerateDebugInfo{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.genDebugInfo;
      }
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed |
                        Pre.EngineNotRunning | 
                        Pre.EngineInitialised |
                        Pre.SupportForDebug);
          if (this.genDebugInfo != value){
            this.genDebugInfo = value;
            this.isEngineDirty = true;
            this.isEngineCompiled = false;
          }
        }finally{
          this.ReleaseLock();
        }
      }
    }

    public bool IsCompiled{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.isEngineCompiled;
      }
    }

    public bool IsDirty{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.isEngineDirty;
      }
      // IsDirty setter is used by VsaItems to notify the engine when new items have been
      // manipulated or added to/removed from the engine.  It is not part of an interface.
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed);
          this.isEngineDirty = value;
          if (this.isEngineDirty)
            this.isEngineCompiled = false;
        }finally{
          this.ReleaseLock();
        }
      }
    }

    public bool IsRunning{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.isEngineRunning;
      }
    }

    public IVsaItems Items{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.vsaItems;
      }
    }

    public string Language{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.scriptLanguage;
      }
    }

    public int LCID{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.errorLocale;
      }
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised | Pre.EngineNotRunning);
          try{
            CultureInfo errorCultureInfo = new CultureInfo(value);
          }catch(ArgumentException){
            throw Error(VsaError.LCIDNotSupported);
          }
          this.errorLocale = value;
          this.isEngineDirty = true;
        }finally{
          this.ReleaseLock();
        }
      }
    }

    public string Name{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.engineName;
      }
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed | Pre.EngineNotRunning | Pre.EngineInitialised);
          // if the new name is the same as the old one, do nothing
          if (this.engineName == value) return;
          // put the name into a static table so that we can detect duplicates
          lock(nameTable){
            if (nameTable[value] != null)
              throw Error(VsaError.EngineNameInUse);
            nameTable[value] = new Object();
            if (this.engineName != null && this.engineName.Length > 0)
              nameTable[this.engineName] = null;
          }
          this.engineName = value;
          this.isEngineDirty = true;
          this.isEngineCompiled = false;
        }finally{
          this.ReleaseLock();
        }
      }
    }

    public string RootMoniker{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed);
        return this.engineMoniker;
      }
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed | Pre.RootMonikerNotSet);
          this.ValidateRootMoniker(value);
          this.engineMoniker = value;
        }finally{
          this.ReleaseLock();
        }
      }
    }

    public string RootNamespace{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.rootNamespace;
      }
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed | Pre.EngineNotRunning | Pre.EngineInitialised);
          if (!IsValidNamespaceName(value))
            throw Error(VsaError.RootNamespaceInvalid);
          this.rootNamespace = value;
          this.isEngineDirty = true;
          this.isEngineCompiled = false;
        }finally{
          this.ReleaseLock();
        }
      }
    }

    public IVsaSite Site{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.RootMonikerSet);
        return this.engineSite;
      }
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      set{
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed | Pre.RootMonikerSet | Pre.SiteNotSet);
          if (value == null)
            throw Error(VsaError.SiteInvalid);
          this.engineSite = value;
        }finally{
          this.ReleaseLock();
        }
      }
    }

    public string Version{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        return this.assemblyVersion;
      }
    }

    // === Methods ===

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void Close(){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed);
        if (this.isEngineRunning)
          this.Reset();
        lock(nameTable){
          if (this.engineName != null && this.engineName.Length > 0)
            nameTable[this.engineName] = null;
        }
        this.DoClose();
        this.isClosed = true;
      }finally{
        this.ReleaseLock();
      }
    }

    // See security comment at Run()
    [PermissionSet(SecurityAction.Demand, Name="FullTrust")]
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual bool Compile(){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed |
                      Pre.EngineNotRunning |
                      Pre.EngineInitialised |
                      Pre.RootNamespaceSet);
        // look through items for at least one code item
        bool haveCode = false;
        for (int i = 0, n = this.vsaItems.Count; !haveCode && i < n; ++i){
          IVsaItem item = vsaItems[i];
          haveCode = this.vsaItems[i].ItemType == VsaItemType.Code;
        }
        if (!haveCode)
          throw Error(VsaError.EngineEmpty);
        try{
          this.ResetCompiledState();
          this.isEngineCompiled = DoCompile();
        }catch(VsaException){
          throw;
        }catch(Exception e){
          throw new VsaException(VsaError.InternalCompilerError, e.ToString(), e);
        }catch{
          throw new VsaException(VsaError.InternalCompilerError);
        }
        if (this.isEngineCompiled){
          this.haveCompiledState = true;
          this.failedCompilation = false;
          this.compiledRootNamespace = this.rootNamespace;
        }
        return this.isEngineCompiled;
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual object GetOption(string name){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed | Pre.EngineInitialised);
        object option = GetCustomOption(name);
        return option;
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void InitNew(){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed |
                      Pre.EngineNotInitialised |
                      Pre.RootMonikerSet |
                      Pre.SiteSet);
        this.isEngineInitialized = true;
      }finally{
        this.ReleaseLock();
      }
    }

    // Load the local compiled state into the AppDomain and return the loaded assembly
    protected virtual Assembly LoadCompiledState(){
      Debug.Assert(this.haveCompiledState);
      byte[] pe;
      byte[] pdb;
      this.DoSaveCompiledState(out pe, out pdb);
      return Assembly.Load(pe, pdb, this.executionEvidence);
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void LoadSourceState(IVsaPersistSite site){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed |
                      Pre.EngineNotInitialised |
                      Pre.RootMonikerSet |
                      Pre.SiteSet);
        this.isEngineInitialized = true;
        try{
          this.DoLoadSourceState(site);
        }catch{
          this.isEngineInitialized = false;
          throw;
        }
        this.isEngineDirty = false;
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void Reset(){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed | Pre.EngineRunning);
        try{
          this.startupInstance.Shutdown();
        }catch(Exception e){
          throw new VsaException(VsaError.EngineCannotReset, e.ToString(), e);
        }catch{
          throw new VsaException(VsaError.EngineCannotReset);
        }
        this.isEngineRunning = false;
        this.loadedAssembly = null;
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void RevokeCache(){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed | Pre.EngineNotRunning | Pre.RootMonikerSet);
        try{
          System.AppDomain.CurrentDomain.SetData(this.engineMoniker, null);
        }catch(Exception e){
          throw new VsaException(VsaError.RevokeFailed, e.ToString(), e);
        }catch{
          throw new VsaException(VsaError.RevokeFailed);
        }
      }finally{
        this.ReleaseLock();
      }
    }

/////////////////////////////////////////////////////////////////////////////
//
// Security Issue
//
/////////////////////////////////////////////////////////////////////////////
//
// [EricLi] 10 November 2001
//
// Right now the BaseVsa class requires full trust to use as
// an engine that runs code or generates assemblies.
//
// Preventing untrusted callers from actually compiling and
// running code is clearly undesirable -- we want partially-
// trusted host scenarios to work.  For this release however
// there are too many poorly-understood issues involving
// controling the evidence property.
//
/////////////////////////////////////////////////////////////////////////////

    [PermissionSet(SecurityAction.Demand, Name="FullTrust")]
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void Run(){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed |
                      Pre.EngineNotRunning |
                      Pre.RootMonikerSet |
                      Pre.SiteSet |
                      Pre.RootNamespaceSet);
        // managed engines cannot support the AppDomain property so we always use the current one
        AppDomain appDomain = System.AppDomain.CurrentDomain;
        if (this.haveCompiledState){
          // make sure the RootNamespace hasn't been changed since the compilation
          if (this.rootNamespace != this.compiledRootNamespace)
            throw new VsaException(VsaError.RootNamespaceInvalid);
          this.loadedAssembly = this.LoadCompiledState();
          appDomain.SetData(this.engineMoniker, this.loadedAssembly);
        }else{
          // if Compile returned false last time it was called, quit before cache lookup or callback
          if (this.failedCompilation)
            throw new VsaException(VsaError.EngineNotCompiled);
          this.startupClass = null;
          this.loadedAssembly = appDomain.GetData(this.engineMoniker) as Assembly;
          if (this.loadedAssembly == null){
            // assembly is not in the cache, so callback to the site for compiled state
            // prevent multiple callbacks which could result in duplicate assemblies being loaded into the AppDomain
            string mutexName = this.engineMoniker + "/" + appDomain.GetHashCode().ToString(CultureInfo.InvariantCulture);
            Mutex compiledStateCallbackMutex = new Mutex(false, mutexName);
            if (compiledStateCallbackMutex.WaitOne()){
              try{
                // see if there was another callback before we got the lock
                this.loadedAssembly = appDomain.GetData(this.engineMoniker) as Assembly;
                if (this.loadedAssembly == null){
                  byte[] pe;
                  byte[] pdb;
                  this.engineSite.GetCompiledState(out pe, out pdb);
                  if (pe == null)
                    throw new VsaException(VsaError.GetCompiledStateFailed);
                  this.loadedAssembly = Assembly.Load(pe, pdb, this.executionEvidence);
                  appDomain.SetData(this.engineMoniker, this.loadedAssembly);
                }
              }finally{
                compiledStateCallbackMutex.ReleaseMutex();
                compiledStateCallbackMutex.Close();
              }
            }
          }
        }
        // update the _Startup class Type object (if needed)
        try{
          if (this.startupClass == null)
            this.startupClass = this.loadedAssembly.GetType(this.rootNamespace + "._Startup", true);
        }catch(Exception e){
          // the _Startup class was not in the RootNamespace
          throw new VsaException(VsaError.BadAssembly, e.ToString(), e);
        }catch{
          // the _Startup class was not in the RootNamespace
          throw new VsaException(VsaError.BadAssembly);
        }
        // set the engine to running, callback for global and event instances, then run global code
        try{
          this.startupInstance = (BaseVsaStartup)Activator.CreateInstance(this.startupClass);
          this.isEngineRunning = true;
          this.startupInstance.SetSite(this.engineSite);
          this.startupInstance.Startup();
        }catch(Exception e){
          throw new VsaException(VsaError.UnknownError, e.ToString(), e);
        }catch{
          throw new VsaException(VsaError.UnknownError);
        }
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void SetOption(string name, object value){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed |
                      Pre.EngineNotRunning |
                      Pre.EngineInitialised);
        this.SetCustomOption(name, value);
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void SaveCompiledState(out byte[] pe, out byte[] debugInfo){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed |
                      Pre.EngineNotRunning |
                      Pre.EngineCompiled |
                      Pre.EngineInitialised);
        this.DoSaveCompiledState(out pe, out debugInfo);
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void SaveSourceState(IVsaPersistSite site){
      this.TryObtainLock();
      try{
        Preconditions(Pre.EngineNotClosed | Pre.EngineNotRunning | Pre.EngineInitialised);
        if (site == null)
          throw Error(VsaError.SiteInvalid);
        try{
          DoSaveSourceState(site);
        }catch(Exception e){
          throw new VsaException(VsaError.SaveElementFailed, e.ToString(), e);
        }catch{
          throw new VsaException(VsaError.SaveElementFailed);
        }
      }finally{
        this.ReleaseLock();
      }
    }

    protected virtual void ValidateRootMoniker(string rootMoniker){
      // Refer to RFC 2396 for details: ftp://nis.nsf.net/internet/documents/rfc/rfc2396.txt
      if (rootMoniker == null)
        throw new VsaException(VsaError.RootMonikerInvalid);
      Uri moniker = null;
      try{
        moniker = new Uri(rootMoniker);
      }catch(UriFormatException){
        throw new VsaException(VsaError.RootMonikerInvalid);
      }
      // The overall format is okay, so now we validate the protocol
      string protocol = moniker.Scheme;
      if (protocol.Length == 0)
        throw new VsaException(VsaError.RootMonikerProtocolInvalid);
      string[] knownSchemes = {
        "file",
        "ftp",
        "gopher",
        "http",
        "https",
        "javascript",
        "mailto",
        "microsoft",
        "news",
        "res",
        "smtp",
        "socks",
        "vbscript",
        "xlang",
        "xml",
        "xpath",
        "xsd",
        "xsl",
      };
      foreach (string scheme in knownSchemes){
        // Scheme names are case-insensitive (RFC 2396 section 3.2)
        if (String.Compare(scheme, protocol, StringComparison.OrdinalIgnoreCase) == 0)
          throw new VsaException(VsaError.RootMonikerProtocolInvalid);
      }
    }

    // === Abstract Methods ===

    // Implement this method to clean up data structures
    protected abstract void DoClose();

    // Return flag for compilation success.
    // Errors should be reported to the engine site via OnCompilerError.
    // Internal compiler errors should throw exceptions.
    protected abstract bool DoCompile();

    // Implement this method to load source state from site
    protected abstract void DoLoadSourceState(IVsaPersistSite site);

    protected abstract void DoSaveCompiledState(out byte[] pe, out byte[] debugInfo);

    // Save source items using IVsaPersistSite.SaveElement
    protected abstract void DoSaveSourceState(IVsaPersistSite site);

    // Enable custom options
    protected abstract object GetCustomOption(string name);

    // Verify that the name is a valid namespace name
    protected abstract bool IsValidNamespaceName(string name);

    // Verify that the identifier is valid for the language
    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public abstract bool IsValidIdentifier(string ident);

    // Discard the existing compiled state so we can compile a new source state
    // Postconditions:
    //    this.haveCompiledState == false
    //    this.failedCompilation == true
    //    this.compiledRootNamespace == null
    //    this.startupClass == null
    protected abstract void ResetCompiledState();

    // Enable custom options
    protected abstract void SetCustomOption(string name, object value);
  }

  // BaseVsaSite is a default implementation of IVsaSite.  Note that GetCompiledState
  // defers to two properties so that languages that don't support declaring out params
  // can provide an implementation without having to change languages.

  [Obsolete(Microsoft.JScript.Vsa.VsaObsolete.Description)]
  public class BaseVsaSite: IVsaSite{
    public virtual byte[] Assembly{
      get{
        throw new VsaException(VsaError.GetCompiledStateFailed);
      }
    }

    public virtual byte[] DebugInfo{
      get{
        return null;
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void GetCompiledState(out byte[] pe, out byte[] debugInfo){
      pe = this.Assembly;
      debugInfo = this.DebugInfo;
    }

    public virtual object GetEventSourceInstance( string itemName, string eventSourceName){
      throw new VsaException(VsaError.CallbackUnexpected);
    }

    public virtual object GetGlobalInstance(string name){
      throw new VsaException(VsaError.CallbackUnexpected);
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual void Notify(string notify, object optional){
      throw new VsaException(VsaError.CallbackUnexpected);
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public virtual bool OnCompilerError(IVsaError error){
      return false;
    }
  }

  [Obsolete(Microsoft.JScript.Vsa.VsaObsolete.Description)]
  public abstract class BaseVsaStartup{
    protected IVsaSite site;
    public void SetSite(IVsaSite site){
      this.site = site;
    }
    public abstract void Startup();
    public abstract void Shutdown();
  }
}
