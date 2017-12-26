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
    
    using System;
    using System.Collections;
    using System.Reflection;
    using System.Reflection.Emit;
    
    public class BlockScope : ActivationObject{
      private static int counter = 0;
      internal bool catchHanderScope = false;
      
      internal int scopeId;
      // the following field tracks the JSLocalFields that have to be emitted in a debugger local block
      private ArrayList localFieldsForDebugInfo;
      
      internal BlockScope(ScriptObject parent)
        : base(parent){
        this.scopeId = BlockScope.counter++;
        this.isKnownAtCompileTime = true;
        this.fast = parent is ActivationObject ? ((ActivationObject)parent).fast : true;
        this.localFieldsForDebugInfo = new ArrayList();
      }
      
      public BlockScope(ScriptObject parent, String name, int scopeId)
        : base(parent){
        this.scopeId = scopeId;
        JSField field = (JSField)this.parent.GetField(name+":"+this.scopeId, BindingFlags.Public);
        this.name_table[name] = field;
        this.field_table.Add(field);
      }
      
      internal void AddFieldForLocalScopeDebugInfo(JSLocalField field){
        this.localFieldsForDebugInfo.Add(field);
      }
    
      protected override JSVariableField CreateField(String name, FieldAttributes attributeFlags, Object value){
        if (!(this.parent is ActivationObject))
          return base.CreateField(name, attributeFlags, value);
        JSVariableField field = ((ActivationObject)this.parent).AddNewField(name+":"+this.scopeId, value, attributeFlags);
        field.debuggerName = name;
        return field;
      }
      
      internal void EmitLocalInfoForFields(ILGenerator il){
        foreach(JSLocalField field in this.localFieldsForDebugInfo){
          ((LocalBuilder)field.metaData).SetLocalSymInfo(field.debuggerName);
        }
        if (this.parent is GlobalScope){
          LocalBuilder loc = il.DeclareLocal(Typeob.Int32);
          loc.SetLocalSymInfo("scopeId for catch block");
          ConstantWrapper.TranslateToILInt(il, this.scopeId);
          il.Emit(OpCodes.Stloc, loc);
        }
      }
    
    }
}
