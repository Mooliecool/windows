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

namespace Microsoft.JScript {
    
    using Microsoft.JScript.Vsa;
    using System;
    using System.Collections;
    using System.Configuration.Assemblies;
    using System.Globalization;
    using System.Reflection;
    using System.Runtime.Remoting;
    
    public sealed class Globals{
      [ThreadStatic] private static TypeReferences _typeRefs;
      internal static TypeReferences TypeRefs { 
        get {
          TypeReferences typeRefs = Globals._typeRefs;
          if (null == typeRefs)
            typeRefs = Globals._typeRefs = Runtime.TypeRefs;
          return typeRefs;
        }
        
        set {
          Globals._typeRefs = value;
        }
      }
                  
      private Stack callContextStack;
      private Stack scopeStack;
      internal Object caller;
      private SimpleHashtable regExpTable;
      internal GlobalObject globalObject;
      internal VsaEngine engine;

      //assembly name attributes
      internal bool assemblyDelaySign = false;
      internal CultureInfo assemblyCulture = null;
      internal AssemblyFlags assemblyFlags = AssemblyFlags.EnableJITcompileTracking | AssemblyFlags.DisableJITcompileOptimizer;
      internal AssemblyHashAlgorithm assemblyHashAlgorithm = AssemblyHashAlgorithm.SHA1;
      internal String assemblyKeyFileName = null;
      internal Context assemblyKeyFileNameContext = null;
      internal String assemblyKeyName = null;
      internal Context assemblyKeyNameContext = null;
      internal Version assemblyVersion = null;
      internal AssemblyVersionCompatibility assemblyVersionCompatibility = (AssemblyVersionCompatibility)0;

      private static SimpleHashtable BuiltinFunctionTable = null;
      [ContextStatic] public static VsaEngine contextEngine = null; //This keeps track of the (single) engine running in the current context

      internal Globals(bool fast, VsaEngine engine){
        this.engine = engine;
        this.callContextStack = null;
        this.scopeStack = null;
        this.caller = DBNull.Value;
        this.regExpTable = null;
        if (fast)
          this.globalObject = GlobalObject.commonInstance;
        else
          this.globalObject = new LenientGlobalObject(engine);
      }
      
      internal static BuiltinFunction BuiltinFunctionFor(Object obj, MethodInfo meth){
        if (Globals.BuiltinFunctionTable == null)
          Globals.BuiltinFunctionTable = new SimpleHashtable(64);
        BuiltinFunction result = (BuiltinFunction)Globals.BuiltinFunctionTable[meth];
        if (result != null) return result;
        result = new BuiltinFunction(obj, meth);
        lock (Globals.BuiltinFunctionTable){
          Globals.BuiltinFunctionTable[meth] = result;
        }
        return result;
      }

      internal Stack CallContextStack{
        get{
          if (this.callContextStack == null)
            this.callContextStack = new Stack();
          return this.callContextStack;
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public static ArrayObject ConstructArray(params Object[] args){
        return (ArrayObject)ArrayConstructor.ob.Construct(args);
      }
      
      public static ArrayObject ConstructArrayLiteral(Object[] args){
        return (ArrayObject)ArrayConstructor.ob.ConstructArray(args);
      }

      internal SimpleHashtable RegExpTable{
        get{
          if (this.regExpTable == null)
            this.regExpTable = new SimpleHashtable(8);
          return this.regExpTable;
        }
      }
      
      internal Stack ScopeStack{
        get{
          if (this.scopeStack == null){
            this.scopeStack = new Stack();
            this.scopeStack.Push(this.engine.GetGlobalScope().GetObject()); //This is only needed for ASP+ (since no VsaEngine methods get called in that case).
          }
          return this.scopeStack;
        }
      }
      
    }
    
    internal enum AssemblyFlags{
      PublicKey                 =   0x0001,     // The assembly ref holds the full (unhashed) public key.
      CompatibilityMask         =   0x0070,
      SideBySideCompatible      =   0x0000,     // The assembly is side by side compatible.
      NonSideBySideAppDomain    =   0x0010,     // The assembly cannot execute with other versions if they are executing in the same application domain.
      NonSideBySideProcess      =   0x0020,     // The assembly cannot execute with other versions if they are executing in the same process.
      NonSideBySideMachine      =   0x0030,     // The assembly cannot execute with other versions if they are executing on the same machine.
      EnableJITcompileTracking  =   0x8000,     // From "DebuggableAttribute".
      DisableJITcompileOptimizer=   0x4000,     // From "DebuggableAttribute".
    } 
}
