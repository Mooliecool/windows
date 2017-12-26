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
    
    public sealed class Namespace{
	    private String name;
	    internal VsaEngine engine;
	    
	    private Namespace(String name, VsaEngine engine){
	      this.name = name;
	      this.engine = engine;
	    }

	    public static Namespace GetNamespace(String name, VsaEngine engine){
	      return new Namespace(name, engine);
	    }
	    
	    internal Type GetType(String typeName){
	      return this.engine.GetType(typeName);
	    }
	
	    internal String Name{
	      get {return this.name;}
	    }
    }
}


