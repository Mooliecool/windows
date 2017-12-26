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

namespace Microsoft.JScript.Vsa{

    using Microsoft.JScript;
    using Microsoft.Vsa;
    using System;
    using System.Collections;
    using System.IO;
    using System.Globalization;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Resources;
    using System.Text;
    using System.Threading;
    using System.Xml;
    using System.Runtime.Remoting.Messaging;
    using System.Security;
    using System.Security.Cryptography;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;


    internal enum LoaderAPI {
      LoadFrom,
      LoadFile,
      ReflectionOnlyLoadFrom
    }
    

    [GuidAttribute("B71E484D-93ED-4b56-BFB9-CEED5134822B")]
    [ComVisible(true)]
    [Obsolete(VsaObsolete.Description)]
    public sealed class VsaEngine : BaseVsaEngine, IEngine2, IRedirectOutput{
      internal bool alwaysGenerateIL;
      private bool autoRef;
      private Hashtable Defines;
      internal bool doCRS;
      internal bool doFast;
      internal bool doPrint;
      internal bool doSaveAfterCompile;
      private bool doWarnAsError;
      private int nWarningLevel;
      internal bool genStartupClass;
      internal bool isCLSCompliant;
      internal bool versionSafe;
      private String PEFileName;
      internal PEFileKinds PEFileKind;
      internal PortableExecutableKinds PEKindFlags;
      internal ImageFileMachine PEMachineArchitecture;
      internal LoaderAPI ReferenceLoaderAPI;
      private Version versionInfo;

      private CultureInfo errorCultureInfo;

    
      //In appdomain used to executing JScript debugger expression evaluator
      static internal bool executeForJSEE = false;
      
      private String libpath;
      private String[] libpathList;

      private bool isCompilerSet;

      internal VsaScriptScope globalScope;
      private ArrayList packages;
      private ArrayList scopes;
      private ArrayList implicitAssemblies;
      private SimpleHashtable implicitAssemblyCache;
      private ICollection managedResources;
      private string debugDirectory;
      private string tempDirectory;
      private RNGCryptoServiceProvider randomNumberGenerator;
      private byte[] rawPE;
      private byte[] rawPDB;
      internal int classCounter;

      private SimpleHashtable cachedTypeLookups;

      internal Thread runningThread;
      private CompilerGlobals compilerGlobals;
      private Globals globals;

      // Used only during VsaEngine.Compile. It is reset at the beginning of the 
      // function. It is incremented as errors are reported via OnCompilerError.
      private int numberOfErrors;

      private String runtimeDirectory;
      private static readonly Version CurrentProjectVersion = new Version("1.0");

      private Hashtable typenameTable; // for checking CLS compliance

      private static string engineVersion = GetVersionString();
      private static string GetVersionString(){
        return BuildVersionInfo.MajorVersion
                     + "." + BuildVersionInfo.MinorVersion.ToString(CultureInfo.InvariantCulture).PadLeft(2, '0')
                     + "." + BuildVersionInfo.Revision.ToString(CultureInfo.InvariantCulture)
                     + "." + BuildVersionInfo.Build.ToString(CultureInfo.InvariantCulture).PadLeft(4, '0');        

      }
      
      public VsaEngine()
        :this(true){
      }
      
      public VsaEngine(bool fast) : base("JScript", VsaEngine.engineVersion, true){
        this.alwaysGenerateIL = false;
        this.autoRef = false;
        this.doCRS = false;
        this.doFast = fast;
        this.genDebugInfo = false;
        this.genStartupClass = true;
        this.doPrint = false;
        this.doWarnAsError = false;
        this.nWarningLevel = 4;
        this.isCLSCompliant = false;
        this.versionSafe = false;
        this.PEFileName = null;
        this.PEFileKind = PEFileKinds.Dll;
        this.PEKindFlags = PortableExecutableKinds.ILOnly;
        this.PEMachineArchitecture = ImageFileMachine.I386;
        this.ReferenceLoaderAPI = LoaderAPI.LoadFrom;
        this.errorCultureInfo = null;
        this.libpath = null;
        this.libpathList = null;

        this.globalScope = null;
        this.vsaItems = new VsaItems(this);
        this.packages = null;
        this.scopes = null;
        this.classCounter = 0;
        this.implicitAssemblies = null;
        this.implicitAssemblyCache = null;
        this.cachedTypeLookups = null;

        this.isEngineRunning = false;
        this.isEngineCompiled = false;
        this.isCompilerSet = false;
        this.isClosed = false;

        this.runningThread = null;
        this.compilerGlobals = null;
        this.globals = null;
        this.runtimeDirectory = null;
        Globals.contextEngine = this;
        this.runtimeAssembly = null;
        this.typenameTable = null;
      }

      private Assembly runtimeAssembly;
      private static Hashtable assemblyReferencesTable = null;
      // This constructor is called at run time to instantiate an engine for a given assembly
      private VsaEngine(Assembly runtimeAssembly) : this(true){
        this.runtimeAssembly = runtimeAssembly;
      }

      private static Module reflectionOnlyVsaModule = null; 
      private static Module reflectionOnlyJScriptModule = null;
      
      internal void EnsureReflectionOnlyModulesLoaded() {
        if (VsaEngine.reflectionOnlyVsaModule == null) {
          VsaEngine.reflectionOnlyVsaModule = Assembly.ReflectionOnlyLoadFrom(typeof(IVsaEngine).Assembly.Location).GetModule("Microsoft.Vsa.dll");
          VsaEngine.reflectionOnlyJScriptModule = Assembly.ReflectionOnlyLoadFrom(typeof(VsaEngine).Assembly.Location).GetModule("Microsoft.JScript.dll");
        }
      }
      
      internal Module VsaModule { 
        get {
          if (this.ReferenceLoaderAPI != LoaderAPI.ReflectionOnlyLoadFrom)
            return typeof(IVsaEngine).Module;
          EnsureReflectionOnlyModulesLoaded();
          return VsaEngine.reflectionOnlyVsaModule;
        }
      }
      
      internal Module JScriptModule { 
        get {
          if (this.ReferenceLoaderAPI != LoaderAPI.ReflectionOnlyLoadFrom)
            return typeof(VsaEngine).Module;
          EnsureReflectionOnlyModulesLoaded();
          return VsaEngine.reflectionOnlyJScriptModule;
        }
      }
      
      private void AddChildAndValue(XmlDocument doc, XmlElement parent, string name, string value){
        XmlElement option = doc.CreateElement(name);
        this.CreateAttribute(doc, option, "Value", value);
        parent.AppendChild(option);
      }
      
      internal void AddPackage(PackageScope pscope){
        if (this.packages == null)
          this.packages = new ArrayList(8);
        IEnumerator e = this.packages.GetEnumerator();
        while (e.MoveNext()){
          PackageScope cps = (PackageScope)e.Current;
          if (cps.name.Equals(pscope.name)){
            cps.owner.MergeWith(pscope.owner);
            return;
          }
        }
        this.packages.Add(pscope);
      }
      
      internal void CheckForErrors(){
        if (!this.isClosed && !this.isEngineCompiled){
          SetUpCompilerEnvironment();
          Globals.ScopeStack.Push(this.GetGlobalScope().GetObject());
          try{
            foreach (Object item in this.vsaItems){
              if (item is VsaReference)
                ((VsaReference)item).Compile(); //Load the assembly into memory.
            }
            if (this.vsaItems.Count > 0) 
              this.SetEnclosingContext(new WrappedNamespace("", this)); //Provide a way to find types that are not inside of a name space
            foreach (Object item in this.vsaItems){
              if (!(item is VsaReference))
                ((VsaItem)item).CheckForErrors();
            }
            if (null != this.globalScope) 
              this.globalScope.CheckForErrors(); //In case the host added items to the global scope. 
          }finally{
            Globals.ScopeStack.Pop();
          }
        }
        this.globalScope = null;
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public IVsaEngine Clone(AppDomain domain){
        throw new NotImplementedException();
      }
      
      // See security comment at BaseVsa.Run()
      [PermissionSet(SecurityAction.Demand, Name="FullTrust")]
      public bool CompileEmpty(){
        this.TryObtainLock();
        try{
          return this.DoCompile();
        }finally{
          this.ReleaseLock();
        }
      }

      private void CreateAttribute(XmlDocument doc, XmlElement elem, string name, string value){
        XmlAttribute attribute = doc.CreateAttribute(name);
        elem.SetAttributeNode(attribute);
        elem.SetAttribute(name, value);
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void ConnectEvents(){
      }

      internal CompilerGlobals CompilerGlobals{
        get{
          if (this.compilerGlobals == null)
            this.compilerGlobals = new CompilerGlobals(this, this.Name, this.PEFileName, this.PEFileKind, 
              this.doSaveAfterCompile || this.genStartupClass, !this.doSaveAfterCompile || this.genStartupClass, this.genDebugInfo, this.isCLSCompliant, 
              this.versionInfo, this.globals);
          return this.compilerGlobals;
        }
      }

      
      private static TypeReferences _reflectionOnlyTypeRefs;
      private TypeReferences TypeRefs {
        get {
          TypeReferences typeRefs;
          if (LoaderAPI.ReflectionOnlyLoadFrom == this.ReferenceLoaderAPI) {
            typeRefs = VsaEngine._reflectionOnlyTypeRefs;
            if (null == typeRefs) 
              typeRefs = VsaEngine._reflectionOnlyTypeRefs = new TypeReferences(this.JScriptModule);
          } else
            typeRefs = Runtime.TypeRefs;          
          return typeRefs;
        }
      }
      
      public static GlobalScope CreateEngineAndGetGlobalScope(bool fast, String[] assemblyNames){
        VsaEngine engine = new VsaEngine(fast);
        engine.InitVsaEngine("JScript.Vsa.VsaEngine://Microsoft.JScript.VsaEngine.Vsa", new DefaultVsaSite());
        engine.doPrint = true;
        engine.SetEnclosingContext(new WrappedNamespace("", engine));
        foreach (String assemblyName in assemblyNames){
          VsaReference r = (VsaReference)engine.vsaItems.CreateItem(assemblyName, VsaItemType.Reference, VsaItemFlag.None);
          r.AssemblyName = assemblyName;
        }
        VsaEngine.exeEngine = engine;
        GlobalScope scope = (GlobalScope)engine.GetGlobalScope().GetObject();
        scope.globalObject = engine.Globals.globalObject;
        return scope;
      }

      public static GlobalScope CreateEngineAndGetGlobalScopeWithType(bool fast, String[] assemblyNames, RuntimeTypeHandle callingTypeHandle){
        return VsaEngine.CreateEngineAndGetGlobalScopeWithTypeAndRootNamespace(fast, assemblyNames, callingTypeHandle, null);
      }      
      
      public static GlobalScope CreateEngineAndGetGlobalScopeWithTypeAndRootNamespace(bool fast, String[] assemblyNames, RuntimeTypeHandle callingTypeHandle, string rootNamespace){
        VsaEngine engine = new VsaEngine(fast);
        engine.InitVsaEngine("JScript.Vsa.VsaEngine://Microsoft.JScript.VsaEngine.Vsa", new DefaultVsaSite());
        engine.doPrint = true;
        engine.SetEnclosingContext(new WrappedNamespace("", engine));
        if (rootNamespace != null) 
          engine.SetEnclosingContext(new WrappedNamespace(rootNamespace, engine));
        
        foreach (String assemblyName in assemblyNames){
          VsaReference r = (VsaReference)engine.vsaItems.CreateItem(assemblyName, VsaItemType.Reference, VsaItemFlag.None);
          r.AssemblyName = assemblyName;
        }
        // Put the engine in the CallContext so that calls to CreateEngineWithType will return this engine
        Type callingType = Type.GetTypeFromHandle(callingTypeHandle);
        Assembly callingAssembly = callingType.Assembly;
        System.Runtime.Remoting.Messaging.CallContext.SetData("JScript:" + callingAssembly.FullName, engine);
        // Get and return the global scope
        GlobalScope scope = (GlobalScope)engine.GetGlobalScope().GetObject();
        scope.globalObject = engine.Globals.globalObject;
        return scope;
      }

      private static volatile VsaEngine exeEngine;    // Instance of VsaEngine used by JScript EXEs

      // This factory method is called by EXE code only
      public static VsaEngine CreateEngine(){
        if (VsaEngine.exeEngine == null){
          VsaEngine e = new VsaEngine(true);
          e.InitVsaEngine("JScript.Vsa.VsaEngine://Microsoft.JScript.VsaEngine.Vsa", new DefaultVsaSite());
          VsaEngine.exeEngine = e;
        }
        return VsaEngine.exeEngine;
      }

      internal static VsaEngine CreateEngineForDebugger(){
        VsaEngine engine = new VsaEngine(true);
        engine.InitVsaEngine("JScript.Vsa.VsaEngine://Microsoft.JScript.VsaEngine.Vsa", new DefaultVsaSite());
        GlobalScope scope = (GlobalScope)engine.GetGlobalScope().GetObject();
        scope.globalObject = engine.Globals.globalObject;
        return engine;
      }

      // This factory method is called by DLL code only
      public static VsaEngine CreateEngineWithType(RuntimeTypeHandle callingTypeHandle){
        Type callingType = Type.GetTypeFromHandle(callingTypeHandle);
        Assembly callingAssembly = callingType.Assembly;
        Object o = System.Runtime.Remoting.Messaging.CallContext.GetData("JScript:" + callingAssembly.FullName);
        if (o != null){
          VsaEngine e = o as VsaEngine;
          if (e != null)
            return e;
        }

        VsaEngine engine = new VsaEngine(callingAssembly);
        engine.InitVsaEngine("JScript.Vsa.VsaEngine://Microsoft.JScript.VsaEngine.Vsa", new DefaultVsaSite());

        GlobalScope scope = (GlobalScope)engine.GetGlobalScope().GetObject();
        scope.globalObject = engine.Globals.globalObject;

        // for every global class generated in this assembly make an instance and call the global code method
        int i = 0;
        Type globalClassType = null;
        do{
          String globalClassName = "JScript " + i.ToString(CultureInfo.InvariantCulture);
          globalClassType = callingAssembly.GetType(globalClassName, false);
          if (globalClassType != null){
            engine.SetEnclosingContext(new WrappedNamespace("", engine));
            ConstructorInfo globalScopeConstructor = globalClassType.GetConstructor(new Type[]{typeof(GlobalScope)});
            MethodInfo globalCode = globalClassType.GetMethod("Global Code");
            try{
              Object globalClassInstance = globalScopeConstructor.Invoke(new Object[]{scope});
              globalCode.Invoke(globalClassInstance, new Object[0]);
            }catch(SecurityException){
              // [stesty] Due to bug 337909, if a JScript assembly is strongly-named but partially-trusted, it will
              //          not succeed here unless it also has the AllowPartiallyTrustedCallersAttribute.  We do not
              //          want to run this constructor with elevated permissions, so we work around by abandoning
              //          the attempt, thus disabling eval in this scenario.
              break;
            }
          }
          i++;
        }while (globalClassType != null);

        if (o == null)
          System.Runtime.Remoting.Messaging.CallContext.SetData("JScript:" + callingAssembly.FullName, engine);
        return engine;
      }

      private void AddReferences(){
        if (VsaEngine.assemblyReferencesTable == null) {
          // Create the cache
          Hashtable h = new Hashtable();
          VsaEngine.assemblyReferencesTable = Hashtable.Synchronized(h);
        }

        String[] references = VsaEngine.assemblyReferencesTable[this.runtimeAssembly.FullName] as String[];
        if (references != null){
          for (int i = 0; i < references.Length; i++){
            VsaReference r = (VsaReference)this.vsaItems.CreateItem(references[i], VsaItemType.Reference, VsaItemFlag.None);
            r.AssemblyName = references[i];
          }
        }else{
          // Read the references from the custom attribute on the assembly and create VsaReferences for each
          // of them.
          Object[] attrs = CustomAttribute.GetCustomAttributes(this.runtimeAssembly, typeof(ReferenceAttribute), false);
          String[] references1 = new String[attrs.Length];
          for (int i = 0; i < attrs.Length; i++){
            String assemblyName = ((ReferenceAttribute)attrs[i]).reference;
            VsaReference r = (VsaReference)this.vsaItems.CreateItem(assemblyName, VsaItemType.Reference, VsaItemFlag.None);
            r.AssemblyName = assemblyName;
            references1[i] = assemblyName;
          }
          VsaEngine.assemblyReferencesTable[this.runtimeAssembly.FullName] = references1;
        }
      }

      private void EmitReferences() 
      {
        SimpleHashtable emitted = new SimpleHashtable((uint)this.vsaItems.Count + (this.implicitAssemblies == null ? 0 : (uint)this.implicitAssemblies.Count));
        foreach (Object item in this.vsaItems){
          if (item is VsaReference){
            String referenceName = ((VsaReference)item).Assembly.GetName().FullName;
            // do not write duplicate assemblies
            if (emitted[referenceName] == null){
              CustomAttributeBuilder cab = new CustomAttributeBuilder(CompilerGlobals.referenceAttributeConstructor, new Object[1] {referenceName});
              this.CompilerGlobals.assemblyBuilder.SetCustomAttribute(cab);
              emitted[referenceName] = item;
            }
          }
        }
        if (this.implicitAssemblies != null){
          foreach (Object item in this.implicitAssemblies){
            Assembly a = item as Assembly;
            if (a != null){
              String referenceName = a.GetName().FullName;
              // do not write duplicate assemblies
              if (emitted[referenceName] == null){
                CustomAttributeBuilder cab = new CustomAttributeBuilder(CompilerGlobals.referenceAttributeConstructor, new Object[1] {referenceName});
                this.CompilerGlobals.assemblyBuilder.SetCustomAttribute(cab);
                emitted[referenceName] = item;
              }
            }
          }
        }
      }

      private void CreateMain(){
        // define a class that will hold the main method
        TypeBuilder mainClass = this.CompilerGlobals.module.DefineType("JScript Main", TypeAttributes.Public);
      
        // create a function with the following signature void Main(String[] args)
        MethodBuilder main = mainClass.DefineMethod("Main", MethodAttributes.Public | MethodAttributes.Static, Typeob.Void, new Type[]{Typeob.ArrayOfString});
        ILGenerator il = main.GetILGenerator();

        // emit code for main method
        this.CreateEntryPointIL(il, null /* site */);

        // cook method and class
        mainClass.CreateType();
        // define the Main() method as the entry point for the exe
        this.CompilerGlobals.assemblyBuilder.SetEntryPoint(main, this.PEFileKind);
      }

      private void CreateStartupClass(){
        // define _Startup class for VSA (in the RootNamespace)
        Debug.Assert(this.rootNamespace != null && this.rootNamespace.Length > 0);
        TypeBuilder startupClass = this.CompilerGlobals.module.DefineType(this.rootNamespace + "._Startup", TypeAttributes.Public, Typeob.BaseVsaStartup);
        FieldInfo site = Typeob.BaseVsaStartup.GetField("site", BindingFlags.NonPublic | BindingFlags.Instance);
        // create a function with the following signature: public virtual void Startup()
        MethodBuilder startup = startupClass.DefineMethod("Startup", MethodAttributes.Public | MethodAttributes.Virtual, Typeob.Void, Type.EmptyTypes);
        this.CreateEntryPointIL(startup.GetILGenerator(), site, startupClass);
        // create a function with the following signature: public virtual void Shutdown()
        MethodBuilder shutdown = startupClass.DefineMethod("Shutdown", MethodAttributes.Public | MethodAttributes.Virtual, Typeob.Void, Type.EmptyTypes);
        this.CreateShutdownIL(shutdown.GetILGenerator());

        // cook method and class
        startupClass.CreateType();
      }

      void CreateEntryPointIL(ILGenerator il, FieldInfo site){
        this.CreateEntryPointIL(il, site, null);
      }

      void CreateEntryPointIL(ILGenerator il, FieldInfo site, TypeBuilder startupClass){
        LocalBuilder globalScope = il.DeclareLocal(Typeob.GlobalScope);

        //Emit code to create an engine. We do this explicitly so that we can control fast mode.
        if (this.doFast)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        //Run through the list of references and emit code to create an array of strings representing them
        //but do not emit duplicates.
        SimpleHashtable uniqueReferences = new SimpleHashtable((uint)this.vsaItems.Count);
        ArrayList references = new ArrayList();
        foreach (Object item in this.vsaItems){
          if (item is VsaReference){
            string assemblyName = ((VsaReference)item).Assembly.GetName().FullName;
            if (uniqueReferences[assemblyName] == null){
              references.Add(assemblyName);
              uniqueReferences[assemblyName] = item;
            }
          }
        }
        if (this.implicitAssemblies != null){
          foreach (Object item in this.implicitAssemblies){
            Assembly a = item as Assembly;
            if (a != null){
              String assemblyName = a.GetName().FullName;
              if (uniqueReferences[assemblyName] == null){
                references.Add(assemblyName);
                uniqueReferences[assemblyName] = item;
              }
            }
          }
        }

        ConstantWrapper.TranslateToILInt(il, references.Count);
        il.Emit(OpCodes.Newarr, Typeob.String);
        int num = 0;
        foreach (string referenceName in references){
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, num++);
          il.Emit(OpCodes.Ldstr, referenceName);
          il.Emit(OpCodes.Stelem_Ref);
        }
        if (startupClass != null){
          il.Emit(OpCodes.Ldtoken, startupClass);
          if (this.rootNamespace != null) 
            il.Emit(OpCodes.Ldstr, this.rootNamespace);
          else 
            il.Emit(OpCodes.Ldnull);
          MethodInfo createEngineAndGetGlobalScopeWithTypeAndRootNamespace = Typeob.VsaEngine.GetMethod("CreateEngineAndGetGlobalScopeWithTypeAndRootNamespace");
          il.Emit(OpCodes.Call, createEngineAndGetGlobalScopeWithTypeAndRootNamespace);
        }else{
          MethodInfo createEngineAndGetGlobalScope = Typeob.VsaEngine.GetMethod("CreateEngineAndGetGlobalScope");
          il.Emit(OpCodes.Call, createEngineAndGetGlobalScope);
        }
        il.Emit(OpCodes.Stloc, globalScope);

        // get global object instances and event source instances (CreateStartupClass scenario only)
        if (site != null) this.CreateHostCallbackIL(il, site);

        bool setUserEntryPoint = this.genDebugInfo;
       
        // for every generated class make an instance and call the main routine method
        
        // When there are multiple VsaStaticCode items, all members of relevance are lifted to the
        // first one. VsaStaticCode does not munge with the runtime scope chain, and instead
        // relies on the code here to set things up before the global code is called.
        bool codeToSetupGlobalScopeEmitted = false;                           // have we hit the first VsaStaticCode item
        foreach (Object item in this.vsaItems){
          Type compiledType = ((VsaItem)item).GetCompiledType();
          if (null != compiledType){
            ConstructorInfo globalScopeConstructor = compiledType.GetConstructor(new Type[]{Typeob.GlobalScope});
            MethodInfo globalCode = compiledType.GetMethod("Global Code");
            if (setUserEntryPoint){
              //Set the Global Code method of the first code item to be the place where the debugger breaks for the first step into
              this.CompilerGlobals.module.SetUserEntryPoint(globalCode);
              setUserEntryPoint = false; //Do it once only
            }
            
            il.Emit(OpCodes.Ldloc, globalScope);
            il.Emit(OpCodes.Newobj, globalScopeConstructor);
            
            if (!codeToSetupGlobalScopeEmitted && item is VsaStaticCode) {
              // This is the first VsaStaticCode item which holds all the relevant members.
              // Push it onto the runtime scope stack.
              LocalBuilder firstStaticScope = il.DeclareLocal(compiledType); // all members lifted to this object
              il.Emit(OpCodes.Stloc, firstStaticScope); 

              // Call globalScope.engine.PushScriptObject(firstStaticScope)
              il.Emit(OpCodes.Ldloc, globalScope);
              il.Emit(OpCodes.Ldfld, CompilerGlobals.engineField);
              il.Emit(OpCodes.Ldloc, firstStaticScope);
              il.Emit(OpCodes.Call, CompilerGlobals.pushScriptObjectMethod);
          
              // Restore stack for the next Call instruction.
              il.Emit(OpCodes.Ldloc, firstStaticScope);
              codeToSetupGlobalScopeEmitted = true;
            }

            il.Emit(OpCodes.Call, globalCode);
            il.Emit(OpCodes.Pop);
          }
        }

        if (codeToSetupGlobalScopeEmitted) {
          // A VsaStaticCode item was encountered and code to setup the runtime
          // stack was emitted. Restore the stack.
          il.Emit(OpCodes.Ldloc, globalScope);
          il.Emit(OpCodes.Ldfld, CompilerGlobals.engineField);
          il.Emit(OpCodes.Call, CompilerGlobals.popScriptObjectMethod);
          il.Emit(OpCodes.Pop);
        }

        // a method needs a return opcode
        il.Emit(OpCodes.Ret);
      }

      private void CreateHostCallbackIL(ILGenerator il, FieldInfo site){
        // Do callbacks to the host for global object instances
        MethodInfo getGlobalInstance = site.FieldType.GetMethod("GetGlobalInstance");
        MethodInfo getEventSourceInstance = site.FieldType.GetMethod("GetEventSourceInstance");
        foreach (Object item in this.vsaItems){
          if (item is VsaHostObject){
            VsaHostObject hostObject = (VsaHostObject)item;
            // get global item instance from site
            il.Emit(OpCodes.Ldarg_0);
            il.Emit(OpCodes.Ldfld, site);
            il.Emit(OpCodes.Ldstr, hostObject.Name);
            il.Emit(OpCodes.Callvirt, getGlobalInstance);
            // cast instance to the correct type and store into the global field
            Type target_type = hostObject.Field.FieldType;
            il.Emit(OpCodes.Ldtoken, target_type);
            il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
            ConstantWrapper.TranslateToILInt(il, 0);
            il.Emit(OpCodes.Call, CompilerGlobals.coerceTMethod);
            if (target_type.IsValueType)
              Microsoft.JScript.Convert.EmitUnbox(il, target_type, Type.GetTypeCode(target_type));
            else
              il.Emit(OpCodes.Castclass, target_type);
            il.Emit(OpCodes.Stsfld, hostObject.Field);
          }
        }
      }

      private void CreateShutdownIL(ILGenerator il){
        // release references to global instances
        foreach (Object item in this.vsaItems){
          if (item is VsaHostObject){
            il.Emit(OpCodes.Ldnull); 
            il.Emit(OpCodes.Stsfld, ((VsaHostObject)item).Field);
          }
        }
        il.Emit(OpCodes.Ret);
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void DisconnectEvents(){
      }

      protected override void DoClose(){
        ((VsaItems)this.vsaItems).Close();
        if (null != this.globalScope)
          this.globalScope.Close();
        this.vsaItems = null;
        this.engineSite = null;
        this.globalScope = null;
        this.runningThread = null;
        this.compilerGlobals = null;
        this.globals = null;
        ScriptStream.Out = Console.Out;
        ScriptStream.Error = Console.Error;
        this.rawPE = null;
        this.rawPDB = null;
        this.isClosed = true;
        if (this.tempDirectory != null && Directory.Exists(this.tempDirectory))
          Directory.Delete(this.tempDirectory);
      }

      protected override bool DoCompile(){
        if (!this.isClosed && !this.isEngineCompiled){
          this.SetUpCompilerEnvironment();
          if (this.PEFileName == null){
            // we use random default names to avoid overwriting cached assembly files when debugging VSA
            this.PEFileName = this.GenerateRandomPEFileName();
          }
          this.SaveSourceForDebugging();  // Save sources needed for debugging (does nothing if no debug info)
          this.numberOfErrors = 0;        // Records number of errors during compilation.
          this.isEngineCompiled = true;         // OnCompilerError sets to false if it encounters an unrecoverable error.
          Globals.ScopeStack.Push(this.GetGlobalScope().GetObject());
          try{
            try{
              foreach (Object item in this.vsaItems){
                Debug.Assert(item is VsaReference || item is VsaStaticCode || item is VsaHostObject);
                if (item is VsaReference)
                  ((VsaReference)item).Compile(); //Load the assembly into memory.
              }
              if (this.vsaItems.Count > 0)
                this.SetEnclosingContext(new WrappedNamespace("", this)); //Provide a way to find types that are not inside of a name space
              // Add VSA global items to the global scope
              foreach (Object item in this.vsaItems){
                if (item is VsaHostObject)
                  ((VsaHostObject)item).Compile();
              }
              foreach (Object item in this.vsaItems){
                if (item is VsaStaticCode)
                  ((VsaStaticCode)item).Parse();
              }
              foreach (Object item in this.vsaItems){
                if (item is VsaStaticCode)
                  ((VsaStaticCode)item).ProcessAssemblyAttributeLists();
              }
              foreach (Object item in this.vsaItems){
                if (item is VsaStaticCode)
                  ((VsaStaticCode)item).PartiallyEvaluate();
              }
              foreach (Object item in this.vsaItems){
                if (item is VsaStaticCode)
                  ((VsaStaticCode)item).TranslateToIL();
              }
              foreach (Object item in this.vsaItems){
                if (item is VsaStaticCode)
                  ((VsaStaticCode)item).GetCompiledType();
              }
              if (null != this.globalScope)
                this.globalScope.Compile(); //In case the host added items to the global scope
            }catch(JScriptException se){
              // It's a bit strange because we may be capturing an exception 
              // thrown by VsaEngine.OnCompilerError (in the case where 
              // this.engineSite is null. This is fine though. All we end up doing
              // is capturing and then rethrowing the same error.
              this.OnCompilerError(se);
            }catch(System.IO.FileLoadException e){
              JScriptException se = new JScriptException(JSError.ImplicitlyReferencedAssemblyNotFound);
              se.value = e.FileName;
              this.OnCompilerError(se);
              this.isEngineCompiled = false;
            }catch(EndOfFile){
              // an error was reported during PartiallyEvaluate and the host decided to abort
              // swallow the exception and keep going
            }catch{
              // internal compiler error -- make sure we don't claim to have compiled, then rethrow
              this.isEngineCompiled = false;
              throw;
            }
          }finally{
            Globals.ScopeStack.Pop();
          }
          if (this.isEngineCompiled){
            // there were no unrecoverable errors, but we want to return true only if there is IL
            this.isEngineCompiled = (this.numberOfErrors == 0 || this.alwaysGenerateIL);
          }
        }


        if (this.managedResources != null){
          foreach (ResInfo managedResource in this.managedResources){
            if (managedResource.isLinked){
              this.CompilerGlobals.assemblyBuilder.AddResourceFile(managedResource.name,
                                                              Path.GetFileName(managedResource.filename),
                                                              managedResource.isPublic?
                                                              ResourceAttributes.Public:
                                                              ResourceAttributes.Private);
            }else{
              try{
                using (ResourceReader reader = new ResourceReader(managedResource.filename))
                {
                  IResourceWriter writer = this.CompilerGlobals.module.DefineResource(managedResource.name,
                                                                                      managedResource.filename,
                                                                                      managedResource.isPublic?
                                                                                      ResourceAttributes.Public:
                                                                                      ResourceAttributes.Private);
                  foreach (DictionaryEntry resource in reader)
                    writer.AddResource((string)resource.Key, resource.Value);
                }
              }catch(System.ArgumentException){
                JScriptException se = new JScriptException(JSError.InvalidResource);
                se.value = managedResource.filename;
                this.OnCompilerError(se);
                this.isEngineCompiled = false;
                return false;
              }
            }
          }
        }

        if (this.isEngineCompiled)
          this.EmitReferences();

        // Save things out to a local PE file when doSaveAfterCompile is set; this is set when an
        // output name is given (allows JSC to avoid IVsaEngine.SaveCompiledState).  The proper
        // values for VSA are doSaveAfterCompile == false and genStartupClass == true.  We allow
        // genStartupClass to be false for scenarios like JSTest and the Debugger
        if (this.isEngineCompiled){
          if (this.doSaveAfterCompile){
            if (this.PEFileKind != PEFileKinds.Dll)
              this.CreateMain();
            // After executing this code path, it is an error to call SaveCompiledState (until the engine is recompiled)
            try{
              compilerGlobals.assemblyBuilder.Save(Path.GetFileName(this.PEFileName), 
                  this.PEKindFlags, this.PEMachineArchitecture);
            }catch(Exception e){
              throw new VsaException(VsaError.SaveCompiledStateFailed, e.Message, e);
            }catch{
              throw new VsaException(VsaError.SaveCompiledStateFailed);
            }
          }else if (this.genStartupClass){
            // this is generated for VSA hosting
            this.CreateStartupClass();
          }
        }
        return this.isEngineCompiled;
      }

      internal CultureInfo ErrorCultureInfo{
        get{
          if (this.errorCultureInfo == null || this.errorCultureInfo.LCID != this.errorLocale)
            this.errorCultureInfo = new CultureInfo(this.errorLocale);
          return this.errorCultureInfo;
        }
      }

      private string GenerateRandomPEFileName(){
        if (this.randomNumberGenerator == null)
          this.randomNumberGenerator = new RNGCryptoServiceProvider();
        // Generate random bytes
        byte[] data = new byte[6];
        this.randomNumberGenerator.GetBytes(data);
        // Turn them into a string containing only characters valid in file names
        string randomString = System.Convert.ToBase64String(data);
        randomString = randomString.Replace('/', '-');
        randomString = randomString.Replace('+', '_');
        // Use the first random filename as the engine's temp directory name
        if (this.tempDirectory == null)
          this.tempDirectory = System.IO.Path.GetTempPath() + randomString;
        string filename = randomString + (this.PEFileKind == PEFileKinds.Dll? ".dll": ".exe");
        return this.tempDirectory + Path.DirectorySeparatorChar + filename;
      }

/////////////////////////////////////////////////////////////////////////////
//
// Security Issue
//
/////////////////////////////////////////////////////////////////////////////
//
// [EricLi] 10 November 2001
//
// Right now the VsaEngine class requires full trust to use as
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
      public Assembly GetAssembly(){
        this.TryObtainLock();
        try{
          if (null != this.PEFileName)
            return Assembly.LoadFrom(this.PEFileName);
          else
            return compilerGlobals.assemblyBuilder;
        }finally{
          this.ReleaseLock();
        }
      }

      internal ClassScope GetClass(String className){
        if (this.packages != null)
          for (int i = 0, n = this.packages.Count; i < n; i++){
            PackageScope pscope = (PackageScope)this.packages[i];
            Object pval = pscope.GetMemberValue(className, 1);
            if (!(pval is Microsoft.JScript.Missing)){
              ClassScope csc = (ClassScope)pval;
              return csc;
            }
          }
        return null;
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public IVsaItem GetItem(String itemName){
        return this.vsaItems[itemName];
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public IVsaItem GetItemAtIndex(int index){
        return this.vsaItems[index];
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public int GetItemCount(){
        return this.vsaItems.Count;
      }

      public IVsaScriptScope GetGlobalScope(){
        if (null == this.globalScope){
          this.globalScope = new VsaScriptScope(this, "Global", null);
          GlobalScope scope = (GlobalScope)this.globalScope.GetObject();
          scope.globalObject = this.Globals.globalObject;
          scope.fast = this.doFast;
          scope.isKnownAtCompileTime = this.doFast;
        }
        return this.globalScope;
      }

      // Called by the debugger to get hold of the global scope from within a class method
      public GlobalScope GetMainScope(){
        ScriptObject o = ScriptObjectStackTop();
        while (o != null && !(o is GlobalScope))
          o = o.GetParent();
        return (GlobalScope)o;
      }

      public Module GetModule(){
        if (null != this.PEFileName){
          Assembly a = GetAssembly();
          Module[] modules = a.GetModules();
          return modules[0];
        }else
          return this.CompilerGlobals.module;
      }

      public ArrayConstructor GetOriginalArrayConstructor(){
        return this.Globals.globalObject.originalArray;
      }

      public ObjectConstructor GetOriginalObjectConstructor(){
        return this.Globals.globalObject.originalObject;
      }
      
      public RegExpConstructor GetOriginalRegExpConstructor(){
        return this.Globals.globalObject.originalRegExp;
      }
      
      protected override Object GetCustomOption(String name){
        if (String.Compare(name, "CLSCompliant", StringComparison.OrdinalIgnoreCase) == 0)
          return this.isCLSCompliant;
        else if (String.Compare(name, "fast", StringComparison.OrdinalIgnoreCase) == 0)
          return this.doFast;
        else if (String.Compare(name, "output", StringComparison.OrdinalIgnoreCase) == 0)
          return this.PEFileName;
        else if (String.Compare(name, "PEFileKind", StringComparison.OrdinalIgnoreCase) == 0)
          return this.PEFileKind;
        else if (String.Compare(name, "PortableExecutableKind", StringComparison.OrdinalIgnoreCase) == 0)
          return this.PEKindFlags;
        else if (String.Compare(name, "ImageFileMachine", StringComparison.OrdinalIgnoreCase) == 0)
          return this.PEMachineArchitecture;
        else if (String.Compare(name, "ReferenceLoaderAPI", StringComparison.OrdinalIgnoreCase) == 0) {
          switch (this.ReferenceLoaderAPI) {
          case LoaderAPI.LoadFrom: return "LoadFrom";
          case LoaderAPI.LoadFile: return "LoadFile";
          case LoaderAPI.ReflectionOnlyLoadFrom: return "ReflectionOnlyLoadFrom";
          default: throw new VsaException(VsaError.OptionNotSupported);
          }
        }        
        else if (String.Compare(name, "print", StringComparison.OrdinalIgnoreCase) == 0)
          return this.doPrint;
        else if (String.Compare(name, "UseContextRelativeStatics", StringComparison.OrdinalIgnoreCase) == 0)
          return this.doCRS;
        // the next two are needed because of the ICompiler interface. They should not fail even though they may not do anything
        else if (String.Compare(name, "optimize", StringComparison.OrdinalIgnoreCase) == 0)
          return null;
        else if (String.Compare(name, "define", StringComparison.OrdinalIgnoreCase) == 0)
          return null;
        else if (String.Compare(name, "defines", StringComparison.OrdinalIgnoreCase) == 0)
          return this.Defines;
        else if (String.Compare(name, "ee", StringComparison.OrdinalIgnoreCase) == 0)
          return VsaEngine.executeForJSEE;
        else if (String.Compare(name, "version", StringComparison.OrdinalIgnoreCase) == 0)
          return this.versionInfo;
        else if (String.Compare(name, "VersionSafe", StringComparison.OrdinalIgnoreCase) == 0)
          return this.versionSafe;
        else if (String.Compare(name, "warnaserror", StringComparison.OrdinalIgnoreCase) == 0)
          return this.doWarnAsError;
        else if (String.Compare(name, "WarningLevel", StringComparison.OrdinalIgnoreCase) == 0)
          return this.nWarningLevel;
        else if (String.Compare(name, "managedResources", StringComparison.OrdinalIgnoreCase) == 0)
          return this.managedResources;
        else if (String.Compare(name, "alwaysGenerateIL", StringComparison.OrdinalIgnoreCase) == 0)
          return this.alwaysGenerateIL;
        else if (String.Compare(name, "DebugDirectory", StringComparison.OrdinalIgnoreCase) == 0)
          return this.debugDirectory;
        else if (String.Compare(name, "AutoRef", StringComparison.OrdinalIgnoreCase) == 0)
          return this.autoRef;
        else
          throw new VsaException(VsaError.OptionNotSupported);
      }

      internal int GetStaticCodeBlockCount(){
        return ((VsaItems)this.vsaItems).staticCodeBlockCount;
      }

      internal Type GetType(String typeName){
        if (this.cachedTypeLookups == null)
          this.cachedTypeLookups = new SimpleHashtable(1000);
        object cacheResult = this.cachedTypeLookups[typeName];
        if (cacheResult == null){
          // proceed with lookup
          for (int i = 0, n = this.Scopes.Count; i < n; i++){
            GlobalScope scope = (GlobalScope)this.scopes[i];
            Type result = Globals.TypeRefs.ToReferenceContext(scope.GetType()).Assembly.GetType(typeName, false);
            if (result != null){
              this.cachedTypeLookups[typeName] = result;
              return result;
            }
          }

          if (this.runtimeAssembly != null) {
            this.AddReferences();
            this.runtimeAssembly = null;
          }

          for (int i = 0, n = this.vsaItems.Count; i < n; i++){
            object item = this.vsaItems[i];
            if (item is VsaReference){
              Type result = ((VsaReference)item).GetType(typeName);
              if (result != null){
                this.cachedTypeLookups[typeName] = result;
                return result;
              }
            }
          }
          if (this.implicitAssemblies == null){
            this.cachedTypeLookups[typeName] = false;
            return null;
          }
          for (int i = 0, n = this.implicitAssemblies.Count; i < n; i++){
            Assembly assembly = (Assembly)this.implicitAssemblies[i];
            Type result = assembly.GetType(typeName, false);
            if (result != null){
              if (!result.IsPublic || CustomAttribute.IsDefined(result, typeof(System.Runtime.CompilerServices.RequiredAttributeAttribute), true))
                result = null; //Suppress the type if it is not public or if it is a funky C++ type.
              else{
                this.cachedTypeLookups[typeName] = result;
                return result;
              }
            }
          }
          
          this.cachedTypeLookups[typeName] = false;
          return null;
        }
        return (cacheResult as Type);
      }

      internal Globals Globals{
        get{
          if (this.globals == null)
            this.globals = new Globals(this.doFast, this);
          return this.globals;
        }
      }

      internal bool HasErrors{
        get{
          return this.numberOfErrors != 0;
        }
      }

      // GetScannerInstance is used by IsValidNamespaceName and IsValidIdentifier to validate names.
      // We return an instance of the scanner only if there is no whitespace in the name text, since
      // we do not want to allow, say, "not. valid" to be a valid namespace name even though that
      // would produce a valid sequence of tokens.
      private JSScanner GetScannerInstance(string name){
        // make sure there's no whitespace in the name (values copied from documentation on String.Trim())
        char[] anyWhiteSpace = {
          (char)0x0009, (char)0x000A, (char)0x000B, (char)0x000C, (char)0x000D, (char)0x0020, (char)0x00A0,
          (char)0x2000, (char)0x2001, (char)0x2002, (char)0x2003, (char)0x2004, (char)0x2005, (char)0x2006,
          (char)0x2007, (char)0x2008, (char)0x2009, (char)0x200A, (char)0x200B, (char)0x3000, (char)0xFEFF
        };
        if (name == null || name.IndexOfAny(anyWhiteSpace) > -1)
          return null;
        // Create a code item whose source is the given text
        VsaItem item = new VsaStaticCode(this, "itemName", VsaItemFlag.None);
        Context context = new Context(new DocumentContext(item), name);
        context.errorReported = -1;
        JSScanner scanner = new JSScanner(); //Use this constructor to avoid allocating a Globals instance
        scanner.SetSource(context);
        return scanner;
      }

      // Use this method to initialize the engine for non-VSA use.  This includes JSC, JSTest, and the JSEE.
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void InitVsaEngine(string rootMoniker, IVsaSite site){
        this.genStartupClass = false;
        this.engineMoniker = rootMoniker;
        this.engineSite = site;
        this.isEngineInitialized = true;
        this.rootNamespace = "JScript.DefaultNamespace";
        this.isEngineDirty = true;
        this.isEngineCompiled = false;
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void Interrupt(){
        if (this.runningThread != null){
          this.runningThread.Abort();    
          this.runningThread = null;
        }
      }

      protected override bool IsValidNamespaceName(string name){
        JSScanner scanner = this.GetScannerInstance(name);
        if (scanner == null)
          return false;
        while(true){
          if (scanner.PeekToken() != JSToken.Identifier)
            return false;
          scanner.GetNextToken();
          if (scanner.PeekToken() == JSToken.EndOfFile)
            break;
          if (scanner.PeekToken() != JSToken.AccessField)
            return false;
          scanner.GetNextToken();
        }
        return true;
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public override bool IsValidIdentifier(string ident){
        JSScanner scanner = this.GetScannerInstance(ident);
        if (scanner == null)
          return false;
        if (scanner.PeekToken() != JSToken.Identifier)
          return false;
        scanner.GetNextToken();
        if (scanner.PeekToken() != JSToken.EndOfFile)
            return false;
        return true;
      }

      public LenientGlobalObject LenientGlobalObject{
        get{
          return (LenientGlobalObject)this.Globals.globalObject;
        }
      }

      protected override Assembly LoadCompiledState(){
        Debug.Assert(this.haveCompiledState);
        byte[] pe;
        byte[] pdb;
        if (!this.genDebugInfo) {
          System.Security.Policy.Evidence compilationEvidence = this.CompilerGlobals.compilationEvidence;
          System.Security.Policy.Evidence executionEvidence = this.executionEvidence;          
          if (compilationEvidence == null && executionEvidence == null || 
              compilationEvidence != null && compilationEvidence.Equals(executionEvidence))
            return this.compilerGlobals.assemblyBuilder;
        }
        // we need to save/reload to properly associate debug symbols with the assembly
        this.DoSaveCompiledState(out pe, out pdb);
        return Assembly.Load(pe, pdb, this.executionEvidence);
      }

      protected override void DoLoadSourceState(IVsaPersistSite site){
        // DoSaveSourceState puts everything in the project item (use null for the name)
        // We assume the site is valid and contains a valid project file so any errors are
        // wrapped in a VsaException and thrown
        string projectElement = site.LoadElement(null);
        try{
          XmlDocument project = new XmlDocument();
          project.LoadXml(projectElement);
          XmlElement root = project.DocumentElement;

          // verify that we support this version of the project file
          if (this.LoadProjectVersion(root) == CurrentProjectVersion){
            this.LoadVsaEngineState(root);
            this.isEngineDirty = false;
          }
        }catch(Exception e){
          throw new VsaException(VsaError.UnknownError, e.ToString(), e);
        }catch{
          throw new VsaException(VsaError.UnknownError);
        }
      }

      private Version LoadProjectVersion(XmlElement root){
        return new Version(root["ProjectVersion"].GetAttribute("Version"));
      }

      private void LoadVsaEngineState(XmlElement parent){
        XmlElement engine = parent["VsaEngine"];
        this.applicationPath = engine.GetAttribute("ApplicationBase");
        this.genDebugInfo = Boolean.Parse(engine.GetAttribute("GenerateDebugInfo"));
        this.scriptLanguage = engine.GetAttribute("Language");
        this.LCID = Int32.Parse(engine.GetAttribute("LCID"), CultureInfo.InvariantCulture);
        this.Name = engine.GetAttribute("Name");
        this.rootNamespace = engine.GetAttribute("RootNamespace");
        this.assemblyVersion = engine.GetAttribute("Version");
        this.LoadCustomOptions(engine);
        this.LoadVsaItems(engine);
      }

      private void LoadCustomOptions(XmlElement parent){
        XmlElement options = parent["Options"];
        Debug.Assert(String.Compare(options.Name, "Options", StringComparison.OrdinalIgnoreCase) == 0);
        this.doFast = Boolean.Parse(options.GetAttribute("fast"));
        this.doPrint = Boolean.Parse(options.GetAttribute("print"));
        this.doCRS = Boolean.Parse(options.GetAttribute("UseContextRelativeStatics"));
        this.versionSafe = Boolean.Parse(options.GetAttribute("VersionSafe"));
        this.libpath = options.GetAttribute("libpath");
        this.doWarnAsError = Boolean.Parse(options.GetAttribute("warnaserror"));
        this.nWarningLevel = Int32.Parse(options.GetAttribute("WarningLevel"), CultureInfo.InvariantCulture);
        this.LoadUserDefines(options);
        this.LoadManagedResources(options);
      }

      private void LoadUserDefines(XmlElement parent){
        XmlElement userDefines = parent["Defines"];
        XmlNodeList defines = userDefines.ChildNodes;
        foreach (XmlElement definition in defines)
          this.Defines[definition.Name] = definition.GetAttribute("Value");
      }

      private void LoadManagedResources(XmlElement parent){
        XmlElement resources = parent["ManagedResources"];
        XmlNodeList managedResources = resources.ChildNodes;
        if (managedResources.Count > 0){
          this.managedResources = new ArrayList(managedResources.Count);
          foreach (XmlElement resource in managedResources){
            string name = resource.GetAttribute("Name");
            string filename = resource.GetAttribute("FileName");
            bool isPublic = Boolean.Parse(resource.GetAttribute("Public"));
            bool isLinked = Boolean.Parse(resource.GetAttribute("Linked"));
            ((ArrayList)this.managedResources).Add(new ResInfo(filename, name, isPublic, isLinked));
          }
        }
      }

      private void LoadVsaItems(XmlElement parent){
        XmlNodeList vsaItems = parent["VsaItems"].ChildNodes;
        string itemType;
        IVsaItem item;
        string reference = VsaItemType.Reference.ToString();
        string appGlobal = VsaItemType.AppGlobal.ToString();
        string code = VsaItemType.Code.ToString();
        foreach (XmlElement vsaItem in vsaItems){
          string name = vsaItem.GetAttribute("Name");
          itemType = vsaItem.GetAttribute("ItemType");
          if (String.Compare(itemType, reference, StringComparison.OrdinalIgnoreCase) == 0){
            item = this.vsaItems.CreateItem(name, VsaItemType.Reference, VsaItemFlag.None);
            ((IVsaReferenceItem)item).AssemblyName = vsaItem.GetAttribute("AssemblyName");
          }else if (String.Compare(itemType, appGlobal, StringComparison.OrdinalIgnoreCase) == 0){
            item = this.vsaItems.CreateItem(name, VsaItemType.AppGlobal, VsaItemFlag.None);
            ((IVsaGlobalItem)item).ExposeMembers = Boolean.Parse(vsaItem.GetAttribute("ExposeMembers"));
            ((IVsaGlobalItem)item).TypeString = vsaItem.GetAttribute("TypeString");
          }else if (String.Compare(itemType, code, StringComparison.OrdinalIgnoreCase) == 0){
            item = this.vsaItems.CreateItem(name, VsaItemType.Code, VsaItemFlag.None);
            XmlCDataSection sourceText = (XmlCDataSection)vsaItem.FirstChild;
            string unescapedText = sourceText.Value.Replace(" >", ">");
            ((IVsaCodeItem)item).SourceText = unescapedText;
          }else
            throw new VsaException(VsaError.LoadElementFailed);
          XmlNodeList vsaItemOptions = vsaItem["Options"].ChildNodes;
          foreach (XmlElement option in vsaItemOptions){
            item.SetOption(option.Name, option.GetAttribute("Value"));
          }
          ((VsaItem)item).IsDirty = false;
        }
      }

      internal bool OnCompilerError(JScriptException se){
        if (se.Severity == 0 || (this.doWarnAsError && se.Severity <= this.nWarningLevel))
          this.numberOfErrors++;
        bool canRecover = this.engineSite.OnCompilerError(se); //true means carry on with compilation.
        if (!canRecover)
          this.isEngineCompiled = false;
        return canRecover;
      }

      public ScriptObject PopScriptObject(){
        return (ScriptObject)this.Globals.ScopeStack.Pop();
      }
      
      public void PushScriptObject(ScriptObject obj){
        this.Globals.ScopeStack.Push(obj);
      }
      
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void RegisterEventSource(String name){
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public override void Reset(){
        if (this.genStartupClass)
          base.Reset();
        else
          this.ResetCompiledState();
      }

      protected override void ResetCompiledState(){
        if (this.globalScope != null){
          this.globalScope.Reset();
          this.globalScope = null;
        }
        this.classCounter = 0;
        this.haveCompiledState = false;
        this.failedCompilation = true;
        this.compiledRootNamespace = null;
        this.startupClass = null;
        this.compilerGlobals = null;
        this.globals = null;
        foreach (Object item in this.vsaItems)
          ((VsaItem)item).Reset();
        this.implicitAssemblies = null;
        this.implicitAssemblyCache = null;
        this.cachedTypeLookups = null;
        this.isEngineCompiled = false;
        this.isEngineRunning = false;
        this.isCompilerSet = false;
        this.packages = null;
        if (!this.doSaveAfterCompile)
          this.PEFileName = null;
        this.rawPE = null;
        this.rawPDB = null;
      }

      // the debugger restart the engine to run different expression evaluation
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void Restart(){
        this.TryObtainLock();
        try{
          ((VsaItems)this.vsaItems).Close();
          if (null != this.globalScope)
            this.globalScope.Close();
          this.globalScope = null;
          this.vsaItems = new VsaItems(this);
          this.isEngineRunning = false;
          this.isEngineCompiled = false;
          this.isCompilerSet = false;
          this.isClosed = false;
          this.runningThread = null;
          this.globals = null;
        }finally{
          this.ReleaseLock();
        }
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void RunEmpty(){
        this.TryObtainLock();
        try{
          Preconditions(Pre.EngineNotClosed | Pre.RootMonikerSet | Pre.SiteSet);
          this.isEngineRunning = true;
          // save the current thread so it can be interrupted
          this.runningThread = Thread.CurrentThread;
          if (null != this.globalScope)
            this.globalScope.Run();
          foreach (Object item in this.vsaItems)
            ((VsaItem)item).Run();
        }finally{
          this.runningThread = null;
          this.ReleaseLock();
        }
      }

      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      public void Run(AppDomain domain){
        // managed engines are not supporting Run in user-provided AppDomains
        throw new System.NotImplementedException();
      }

      protected override void DoSaveCompiledState(out byte[] pe, out byte[] pdb){
        pe = null;
        pdb = null;
        if (this.rawPE == null){
          try{
            // Save things out to a local PE file, then read back into memory
            // PEFileName was set in the Compile method and we must have compiled in order to be calling this
            if (!Directory.Exists(this.tempDirectory))
              Directory.CreateDirectory(this.tempDirectory);
            this.compilerGlobals.assemblyBuilder.Save(Path.GetFileName(this.PEFileName),
                  this.PEKindFlags, this.PEMachineArchitecture);
            string tempPDBName = Path.ChangeExtension(this.PEFileName, ".ildb");
            try{
              FileStream stream = new FileStream(this.PEFileName, FileMode.Open, FileAccess.Read, FileShare.Read);
              try{
                this.rawPE = new byte[(int)stream.Length];
                stream.Read(this.rawPE, 0, this.rawPE.Length);
              }finally{
                stream.Close();
              }
              // genDebugInfo could have been changed since we compiled, so check to make sure the symbols are there
              if (File.Exists(tempPDBName)){
                stream = new FileStream(tempPDBName, FileMode.Open, FileAccess.Read, FileShare.Read);
                try{
                  this.rawPDB = new byte[(int)stream.Length];
                  stream.Read(this.rawPDB, 0, this.rawPDB.Length);
                }finally{
                  stream.Close();
                }
              }
            }finally{
              File.Delete(this.PEFileName);
              if (File.Exists(tempPDBName))
                File.Delete(tempPDBName);
            }
          }catch(Exception e){
            throw new VsaException(VsaError.SaveCompiledStateFailed, e.ToString(), e);
          }catch{
            throw new VsaException(VsaError.SaveCompiledStateFailed);
          }
        }
        pe = this.rawPE;
        pdb = this.rawPDB;
      }

      protected override void DoSaveSourceState(IVsaPersistSite site){
        XmlDocument project = new XmlDocument();
        project.LoadXml("<project></project>");
        XmlElement root = project.DocumentElement;
        this.SaveProjectVersion(project, root);
        this.SaveVsaEngineState(project, root);
        site.SaveElement(null, project.OuterXml);
        this.SaveSourceForDebugging();
        this.isEngineDirty = false;
      }

      private void SaveSourceForDebugging(){
        if (!this.GenerateDebugInfo || this.debugDirectory == null || !this.isEngineDirty)
          return;
        foreach (VsaItem item in this.vsaItems){
          if (item is VsaStaticCode){
            string fileName = this.debugDirectory + item.Name + ".js";
            try{
              using (FileStream file = new FileStream(fileName, FileMode.Create, FileAccess.Write)) {
                using (StreamWriter sw = new StreamWriter(file))
                  sw.Write(((VsaStaticCode)item).SourceText);
                item.SetOption("codebase", fileName);
              }
            }catch{
              // swallow any file creation exceptions
            }
          }
        }
      }

      private void SaveProjectVersion(XmlDocument project, XmlElement root){
        XmlElement version = project.CreateElement("ProjectVersion");
        this.CreateAttribute(project, version, "Version", CurrentProjectVersion.ToString());
        root.AppendChild(version);
      }

      private void SaveVsaEngineState(XmlDocument project, XmlElement parent){
        XmlElement engine = project.CreateElement("VsaEngine");
        // add IVsaEngine properties as attributes
        this.CreateAttribute(project, engine, "ApplicationBase", this.applicationPath);
        this.CreateAttribute(project, engine, "GenerateDebugInfo", this.genDebugInfo.ToString());
        this.CreateAttribute(project, engine, "Language", this.scriptLanguage);
        this.CreateAttribute(project, engine, "LCID", this.errorLocale.ToString(CultureInfo.InvariantCulture));
        this.CreateAttribute(project, engine, "Name", this.engineName);
        this.CreateAttribute(project, engine, "RootNamespace", this.rootNamespace);
        this.CreateAttribute(project, engine, "Version", this.assemblyVersion);
        this.SaveCustomOptions(project, engine);
        this.SaveVsaItems(project, engine);
        parent.AppendChild(engine);
      }

      private void SaveCustomOptions(XmlDocument project, XmlElement parent){
        XmlElement options = project.CreateElement("Options");
        this.CreateAttribute(project, options, "fast", this.doFast.ToString());
        this.CreateAttribute(project, options, "print", this.doPrint.ToString());
        this.CreateAttribute(project, options, "UseContextRelativeStatics", this.doCRS.ToString());
        this.CreateAttribute(project, options, "VersionSafe", this.versionSafe.ToString());
        this.CreateAttribute(project, options, "libpath", this.libpath);
        this.CreateAttribute(project, options, "warnaserror", this.doWarnAsError.ToString());
        this.CreateAttribute(project, options, "WarningLevel", this.nWarningLevel.ToString(CultureInfo.InvariantCulture));
        this.SaveUserDefines(project, options);
        this.SaveManagedResources(project, options);
        parent.AppendChild(options);
      }

      private void SaveUserDefines(XmlDocument project, XmlElement parent){
        XmlElement userDefines = project.CreateElement("Defines");
        if (this.Defines != null){
          foreach (string key in this.Defines.Keys){
            this.AddChildAndValue(project, userDefines, key, (string)this.Defines[key]);
          }
        }
        parent.AppendChild(userDefines);
      }

      private void SaveManagedResources(XmlDocument project, XmlElement parent){
        // Save managed resources
        XmlElement managedResources = project.CreateElement("ManagedResources");
        if (this.managedResources != null){
          foreach (ResInfo resinfo in this.managedResources){
            XmlElement resource = project.CreateElement(resinfo.name);
            this.CreateAttribute(project, resource, "File", resinfo.filename);
            this.CreateAttribute(project, resource, "Public", resinfo.isPublic.ToString());
            this.CreateAttribute(project, resource, "Linked", resinfo.isLinked.ToString());
            managedResources.AppendChild(resource);
          }
        }
        parent.AppendChild(managedResources);
      }

      private void SaveVsaItems(XmlDocument project, XmlElement parent){
        XmlElement vsaItems = project.CreateElement("VsaItems");
        foreach (IVsaItem item in this.vsaItems){
          XmlElement vsaItem = project.CreateElement("IVsaItem");
          this.CreateAttribute(project, vsaItem, "Name", item.Name);
          this.CreateAttribute(project, vsaItem, "ItemType", item.ItemType.ToString(CultureInfo.InvariantCulture));
          XmlElement vsaItemOptions = project.CreateElement("Options");
          if (item is VsaHostObject){
            // VsaItemType.AppGlobal
            this.CreateAttribute(project, vsaItem, "TypeString", ((VsaHostObject)item).TypeString);
            this.CreateAttribute(project, vsaItem, "ExposeMembers", ((VsaHostObject)item).ExposeMembers.ToString(CultureInfo.InvariantCulture));
          }else if (item is VsaReference){
            // VsaItemType.Reference
            CreateAttribute(project, vsaItem, "AssemblyName", ((VsaReference)item).AssemblyName);
          }else if (item is VsaStaticCode){
            // VsaItemType.Code
            string escapedText = ((VsaStaticCode)item).SourceText.Replace(">", " >");
            XmlCDataSection source = project.CreateCDataSection(escapedText);
            vsaItem.AppendChild(source);
            string codebase = (string)item.GetOption("codebase");
            if (codebase != null)
              this.AddChildAndValue(project, vsaItemOptions, "codebase", codebase);
          }else
            throw new VsaException(VsaError.SaveElementFailed);
          ((VsaItem)item).IsDirty = false;
          vsaItem.AppendChild(vsaItemOptions);
          vsaItems.AppendChild(vsaItem);
        }
        parent.AppendChild(vsaItems);
      }

      internal ArrayList Scopes{
        get{
          if (this.scopes == null)
            this.scopes = new ArrayList(8);
          return this.scopes;
        }
      }

      public ScriptObject ScriptObjectStackTop(){
        return (ScriptObject)this.Globals.ScopeStack.Peek();
      }

      internal void SetEnclosingContext(ScriptObject ob){
        ScriptObject s = this.Globals.ScopeStack.Peek();
        while (s.GetParent() != null)
          s = s.GetParent();
        s.SetParent(ob);
      }

      public void SetOutputStream(IMessageReceiver output){
        COMCharStream stream = new COMCharStream(output);
        System.IO.StreamWriter writer = new System.IO.StreamWriter(stream, Encoding.Default);
        writer.AutoFlush = true;
        ScriptStream.Out = writer;
        ScriptStream.Error = writer;
      }

      protected override void SetCustomOption(String name, Object value){
        try{
          if (String.Compare(name, "CLSCompliant", StringComparison.OrdinalIgnoreCase) == 0)
            this.isCLSCompliant = (bool)value;
          else if (String.Compare(name, "fast", StringComparison.OrdinalIgnoreCase) == 0)
            this.doFast = (bool)value;
          else if (String.Compare(name, "output", StringComparison.OrdinalIgnoreCase) == 0){
            if (value is String){
              this.PEFileName = (String)value;
              this.doSaveAfterCompile = true;
            }
          }else if (String.Compare(name, "PEFileKind", StringComparison.OrdinalIgnoreCase) == 0)
            this.PEFileKind = (PEFileKinds)value;
          else if (String.Compare(name, "PortableExecutableKind", StringComparison.OrdinalIgnoreCase) == 0)
            this.PEKindFlags = (PortableExecutableKinds)value;
          else if (String.Compare(name, "ImageFileMachine", StringComparison.OrdinalIgnoreCase) == 0)
            this.PEMachineArchitecture = (ImageFileMachine)value;
          else if (String.Compare(name, "ReferenceLoaderAPI", StringComparison.OrdinalIgnoreCase) == 0) {
            String loaderAPI = (string)value;
            if (String.Compare(loaderAPI, "LoadFrom", StringComparison.OrdinalIgnoreCase) == 0)
              this.ReferenceLoaderAPI = LoaderAPI.LoadFrom;
            else if (String.Compare(loaderAPI, "LoadFile", StringComparison.OrdinalIgnoreCase) == 0)
              this.ReferenceLoaderAPI = LoaderAPI.LoadFile;
            else if (String.Compare(loaderAPI, "ReflectionOnlyLoadFrom", StringComparison.OrdinalIgnoreCase) == 0)
              this.ReferenceLoaderAPI = LoaderAPI.ReflectionOnlyLoadFrom;
            else
              throw new VsaException(VsaError.OptionInvalid);
          }        
          else if (String.Compare(name, "print", StringComparison.OrdinalIgnoreCase) == 0)
            this.doPrint = (bool)value;
          else if (String.Compare(name, "UseContextRelativeStatics", StringComparison.OrdinalIgnoreCase) == 0)
            this.doCRS = (bool)value;
            // the next two are needed because of the ICompiler interface. They should not fail even though they may not do anything
          else if (String.Compare(name, "optimize", StringComparison.OrdinalIgnoreCase) == 0)
            return;
          else if (String.Compare(name, "define", StringComparison.OrdinalIgnoreCase) == 0)
            return;
          else if (String.Compare(name, "defines", StringComparison.OrdinalIgnoreCase) == 0)
            this.Defines = (Hashtable)value;
          else if (String.Compare(name, "ee", StringComparison.OrdinalIgnoreCase) == 0)
            VsaEngine.executeForJSEE = (Boolean)value;
          else if (String.Compare(name, "version", StringComparison.OrdinalIgnoreCase) == 0)
            this.versionInfo = (Version)value;
          else if (String.Compare(name, "VersionSafe", StringComparison.OrdinalIgnoreCase) == 0)
            this.versionSafe = (Boolean)value;
          else if (String.Compare(name, "libpath", StringComparison.OrdinalIgnoreCase) == 0)
            this.libpath = (String)value;
          else if (String.Compare(name, "warnaserror", StringComparison.OrdinalIgnoreCase) == 0)
            this.doWarnAsError = (bool)value;
          else if (String.Compare(name, "WarningLevel", StringComparison.OrdinalIgnoreCase) == 0)
            this.nWarningLevel = (int)value;
          else if (String.Compare(name, "managedResources", StringComparison.OrdinalIgnoreCase) == 0)
            this.managedResources = (ICollection)value;
          else if (String.Compare(name, "alwaysGenerateIL", StringComparison.OrdinalIgnoreCase) == 0)
            this.alwaysGenerateIL = (bool)value;
          else if (String.Compare(name, "DebugDirectory", StringComparison.OrdinalIgnoreCase) == 0){
            // use null to turn off SaveSourceState source generation
            if (value == null){
              this.debugDirectory = null;
              return;
            }
            string dir = value as string;
            if (dir == null)
              throw new VsaException(VsaError.OptionInvalid);
            try{
              dir = Path.GetFullPath(dir + Path.DirectorySeparatorChar);
              if (!Directory.Exists(dir))
                Directory.CreateDirectory(dir);
            }catch(Exception e){
              // we couldn't create the specified directory
              throw new VsaException(VsaError.OptionInvalid, "", e);
            }catch{
              // we couldn't create the specified directory
              throw new VsaException(VsaError.OptionInvalid);
            }
            this.debugDirectory = dir;
          }else if (String.Compare(name, "AutoRef", StringComparison.OrdinalIgnoreCase) == 0)
            this.autoRef = (bool)value;
          else
            throw new VsaException(VsaError.OptionNotSupported);
        }catch(VsaException){
          throw;
        }catch{
          throw new VsaException(VsaError.OptionInvalid);
        }
      }

      internal void SetUpCompilerEnvironment(){
        if (!this.isCompilerSet){
          Microsoft.JScript.Globals.TypeRefs = this.TypeRefs;
          this.globals = this.Globals;
          this.isCompilerSet = true;
        }
      }

      internal void TryToAddImplicitAssemblyReference(String name){
        if (!this.autoRef) return;
        
        String key;
        SimpleHashtable implictAssemblyCache = this.implicitAssemblyCache;
        if (implicitAssemblyCache == null) {
          //Populate cache with things that should not be autoref'd. Canonical form is lower case without extension.
          implicitAssemblyCache = new SimpleHashtable(50);
          
          //PEFileName always includes an extension and is never NULL.
          implicitAssemblyCache[Path.GetFileNameWithoutExtension(this.PEFileName).ToLowerInvariant()] = true;
          
          foreach (Object item in this.vsaItems){
            VsaReference assemblyReference = item as VsaReference;
            if (assemblyReference == null || assemblyReference.AssemblyName == null) continue;
            key = Path.GetFileName(assemblyReference.AssemblyName).ToLowerInvariant();
            if (key.EndsWith(".dll", StringComparison.Ordinal))
              key = key.Substring(0, key.Length-4);
            implicitAssemblyCache[key] = true;
          }          
          this.implicitAssemblyCache = implicitAssemblyCache;          
        }
        
        key = name.ToLowerInvariant();
        if (implicitAssemblyCache[key] != null) return;
        implicitAssemblyCache[key] = true;
        
        try{
          VsaReference assemblyReference = new VsaReference(this, name + ".dll");
          if (assemblyReference.Compile(false)){
            ArrayList implicitAssemblies = this.implicitAssemblies;
            if (implicitAssemblies == null) {
               implicitAssemblies = new ArrayList();
               this.implicitAssemblies = implicitAssemblies;
            }
            implicitAssemblies.Add(assemblyReference.Assembly);
          }
        }catch(VsaException){
        }
      }

      internal String RuntimeDirectory{
        get{
          if (this.runtimeDirectory == null){
            //Get the path to mscorlib.dll
            String s = typeof(Object).Module.FullyQualifiedName;
            //Remove the file part to get the directory
            this.runtimeDirectory = Path.GetDirectoryName(s);
          }
          return this.runtimeDirectory;
        }
      }

      internal String[] LibpathList{
        get{
          if (this.libpathList == null){
            if (this.libpath == null)
              this.libpathList = new String[]{typeof(Object).Module.Assembly.Location};
            else
              this.libpathList = this.libpath.Split(new Char[] {Path.PathSeparator});
          }
          return this.libpathList;
        }
      }

      internal String FindAssembly(String name){
        String path = name;
        if (Path.GetFileName(name) == name){ // just the filename, no path
          // Look in current directory
          if (File.Exists(name))
            path = Directory.GetCurrentDirectory() + Path.DirectorySeparatorChar + name;
          else {
            // Look in COM+ runtime directory
            String path1 = this.RuntimeDirectory + Path.DirectorySeparatorChar + name;
            if (File.Exists(path1))
              path = path1;
            else{
              // Look on the LIBPATH
              String[] libpathList = this.LibpathList;
              foreach( String l in libpathList ){
                if (l.Length > 0){
                  path1 = l + Path.DirectorySeparatorChar + name;
                  if (File.Exists(path1)){
                    path = path1;
                    break;
                  }
                }
              }
            }
          }
        }
        if (!File.Exists(path))
          return null;
        return path;
      }

      protected override void ValidateRootMoniker(string rootMoniker){
        // We override this method to avoid reading the registry in a non-VSA scenario
        if (this.genStartupClass)
          base.ValidateRootMoniker(rootMoniker);
        else if (rootMoniker == null || rootMoniker.Length == 0)
          throw new VsaException(VsaError.RootMonikerInvalid);
      }

      internal static bool CheckIdentifierForCLSCompliance(String name){
        if (name[0] == '_')
          return false;
        for (int i = 0; i < name.Length; i++){
          if (name[i] == '$')
            return false;
        }
        return true;
      }

      internal void CheckTypeNameForCLSCompliance(String name, String fullname, Context context){
        if (!this.isCLSCompliant)
          return;
        if (name[0] == '_'){
          context.HandleError(JSError.NonCLSCompliantType);
          return;
        }
        if (!VsaEngine.CheckIdentifierForCLSCompliance(fullname)){
          context.HandleError(JSError.NonCLSCompliantType);
          return;
        }
        if (this.typenameTable == null)
          this.typenameTable = new Hashtable(StringComparer.OrdinalIgnoreCase);
        if (this.typenameTable[fullname] == null)
          this.typenameTable[fullname] = fullname;
        else
          context.HandleError(JSError.NonCLSCompliantType);
      }
      
    }

    // The VSA spec requires that every IVsaEngine has a host property that is non-null.
    // Since every assembly that we generate creates an engine (see CreateEngineAndGetGlobalScope)
    // we must provide a default site for it.

    class DefaultVsaSite: BaseVsaSite{
      public override bool OnCompilerError(IVsaError error){
        // We expect only JScriptExceptions here, and we throw them to be caught by the host
        throw (JScriptException)error;
      }
    }
}
