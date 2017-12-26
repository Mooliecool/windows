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

namespace Microsoft.JScript{
  
    using Microsoft.JScript.Vsa;
    using System;
    using System.Collections;
    using System.Reflection;
    using Microsoft.Vsa;

    internal sealed class VsaHostObject : VsaItem, IVsaGlobalItem{
      private Object hostObject;
      internal bool exposeMembers;
      internal bool isVisible;
      private bool exposed;
      private bool compiled;
      private VsaScriptScope scope;
      private FieldInfo field;
      private string typeString;

      internal VsaHostObject(VsaEngine engine, string itemName, VsaItemType type)
        : this(engine, itemName, type, null){
      }

      internal VsaHostObject(VsaEngine engine, string itemName, VsaItemType type, VsaScriptScope scope) 
        : base(engine, itemName, type, VsaItemFlag.None){ 
        this.hostObject = null;
        this.exposeMembers = false;
        this.isVisible = false;
        this.exposed = false;
        this.compiled = false;
        this.scope = scope; // non-null only when the VsaHostObject is added to a VsaScriptScope (rather than being added directly to the engine)
        this.field = null;
        this.typeString = "System.Object";
      }

      public bool ExposeMembers{
        get{
          if (this.engine == null)
            throw new VsaException(VsaError.EngineClosed);
          return this.exposeMembers;
        }
        set{
          if (this.engine == null)
            throw new VsaException(VsaError.EngineClosed);
          this.exposeMembers = value;
        }
      }

      internal FieldInfo Field{
        get{
          JSVariableField jsField = this.field as JSVariableField;
          if (jsField != null) return (FieldInfo)jsField.GetMetaData();
          return this.field;
        }
      }

      public Object GetObject(){
        if (this.engine == null)
          throw new VsaException(VsaError.EngineClosed);
        if (this.hostObject == null){
          if (this.engine.Site == null)
            throw new VsaException(VsaError.SiteNotSet);
          
          this.hostObject = this.engine.Site.GetGlobalInstance(this.name);
        }
        return this.hostObject;
      }

      private void AddNamedItemNamespace(){
        // create an outer scope comprising all the members in the named item
        GlobalScope current = (GlobalScope)this.Scope.GetObject(); //get the global scope object
        if (current.isComponentScope)
          current = (GlobalScope)current.GetParent();
        ScriptObject parent = current.GetParent(); //this can be non null if the current scope is nested inside another
        VsaNamedItemScope namedItemNamespace = new VsaNamedItemScope(this.GetObject(), parent, this.engine);
        // insert into the prototype chain of the global scope object
        current.SetParent(namedItemNamespace);
        namedItemNamespace.SetParent(parent);
      }

      private void RemoveNamedItemNamespace(){
        ScriptObject current = (ScriptObject)this.Scope.GetObject(), parent = current.GetParent();
        while (parent != null){
          if (parent is VsaNamedItemScope){
            if (((VsaNamedItemScope)parent).namedItem == this.hostObject){
              // take this item out of the parent chain
              current.SetParent(parent.GetParent());
              break;
            }
          }
          current = parent;
          parent = parent.GetParent();
        }
      }

      internal override void Remove(){
        base.Remove();
        if (this.exposed){
          if (this.exposeMembers) 
            RemoveNamedItemNamespace();
          if (this.isVisible)
            ((ScriptObject)this.Scope.GetObject()).DeleteMember(this.name);
          this.hostObject = null;
          this.exposed = false;
        }
      }

      internal override void CheckForErrors(){
        this.Compile();
      }
      
      internal override void Compile(){
        // if it is a named item and it has a type push the type into the field so the compiler knows about it      
        if (!this.compiled){
          if (this.isVisible){
            ActivationObject obj = (ActivationObject)this.Scope.GetObject();
            JSVariableField field = (JSVariableField)obj.AddFieldOrUseExistingField(this.name, null, FieldAttributes.Public|FieldAttributes.Static);
            Type type = this.engine.GetType(this.typeString);
            if (type != null)
              field.type = new TypeExpression(new ConstantWrapper(type, null));
            this.field = field;
          }
        }
      }

      internal override void Run(){
        if (!this.exposed){
          // expose the named item 
          if (this.isVisible){
            ActivationObject obj = (ActivationObject)this.Scope.GetObject();
            this.field = obj.AddFieldOrUseExistingField(this.name, this.GetObject(), FieldAttributes.Public|FieldAttributes.Static);
          }
          if (this.exposeMembers)
            this.AddNamedItemNamespace();
          this.exposed = true;
        }
      }

      internal void ReRun(GlobalScope scope){
        if (this.field is JSGlobalField){
          ((JSGlobalField)this.field).ILField = scope.GetField(this.name, BindingFlags.Public|BindingFlags.Static);
          this.field.SetValue(scope, this.GetObject());
          this.field = null;
        }
      }

      internal override void Reset(){
        base.Reset();
        this.hostObject = null;
        this.exposed = false;
        this.compiled = false;
        this.scope = null;
      }

      private VsaScriptScope Scope{
        get{
          if (this.scope == null)
            this.scope = (VsaScriptScope)this.engine.GetGlobalScope();
          return this.scope;
        }
      }

      internal override void Close(){
        this.Remove();
        base.Close();
        this.hostObject = null;
        this.scope = null;
      }

      public string TypeString{
        get{
          if (this.engine == null)
            throw new VsaException(VsaError.EngineClosed);
          return this.typeString;
        }
        set{
          if (this.engine == null)
            throw new VsaException(VsaError.EngineClosed);
          this.typeString = value;
          this.isDirty = true;
          this.engine.IsDirty = true;
        }
      }
    }
 
}
