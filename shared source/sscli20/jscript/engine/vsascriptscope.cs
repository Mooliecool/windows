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
    using System.Security;
    using System.Security.Permissions;
    using Microsoft.Vsa;

    internal class VsaScriptScope : VsaItem, IVsaScriptScope
    {
      
      private VsaScriptScope parent;
      private GlobalScope scope;
      private ArrayList items;
      private bool isCompiled;
      private bool isClosed;

      internal VsaScriptScope(VsaEngine engine, string itemName, VsaScriptScope parent) 
        : base(engine, itemName, (VsaItemType)(int)VSAITEMTYPE2.SCRIPTSCOPE, VsaItemFlag.None){ 
        this.parent = parent;
        this.scope = null;
        this.items = new ArrayList(8);
        this.isCompiled = false;
        this.isClosed = false;
      }

      public virtual Object GetObject(){
        if (null == this.scope)
          if (null != this.parent)
            this.scope = new GlobalScope((GlobalScope)this.parent.GetObject(), this.engine, false);
          else
            this.scope = new GlobalScope(null, this.engine);
        return this.scope;
      }

      public IVsaScriptScope Parent{ 
        get{
          return this.parent;
        } 
      }

      public virtual IVsaItem AddItem(string itemName, VsaItemType type){
        VsaItem item = null;

        if (this.isClosed)
          throw new VsaException(VsaError.EngineClosed);
        if (null != GetItem(itemName))
          throw new VsaException(VsaError.ItemNameInUse);

        switch ((int)type){

        case (int)VSAITEMTYPE2.HOSTOBJECT:  
          case (int)VSAITEMTYPE2.HOSTSCOPE: 
          case (int)VSAITEMTYPE2.HOSTSCOPEANDOBJECT:  
            item = new VsaHostObject(this.engine, itemName, (VsaItemType)type, this);
            if (type == (VsaItemType)VSAITEMTYPE2.HOSTSCOPE || 
                type == (VsaItemType)VSAITEMTYPE2.HOSTSCOPEANDOBJECT){
              ((VsaHostObject)item).exposeMembers = true;
            }
            if (type == (VsaItemType)VSAITEMTYPE2.HOSTOBJECT || 
                type == (VsaItemType)VSAITEMTYPE2.HOSTSCOPEANDOBJECT)
              ((VsaHostObject)item).isVisible = true;
            if (this.engine.IsRunning){
              ((VsaHostObject)item).Compile();
              ((VsaHostObject)item).Run();
            }
            break;

          case (int)VSAITEMTYPE2.SCRIPTSCOPE:
            item = new VsaScriptScope(this.engine, itemName, this);
            break;
        }

        if (null != item){
          //if (!this.engine.IsRunning) 
            this.items.Add(item);
        }else
          throw new VsaException(VsaError.ItemTypeNotSupported);
        return item;
      }

      public virtual IVsaItem GetItem(string itemName){
        for (int i = 0, n = this.items.Count; i < n; i++){
          VsaItem item = (VsaItem)this.items[i];
          if (null == item.Name && null == itemName || null != item.Name && item.Name.Equals(itemName))
            return (IVsaItem)this.items[i];
        }
        return null;
      }

      public virtual void RemoveItem(string itemName){
        for (int i = 0, n = this.items.Count; i < n; i++){
          VsaItem item = (VsaItem)this.items[i];
          if (null == item.Name && null == itemName || null != item.Name && item.Name.Equals(itemName)){
            item.Remove();
            this.items.Remove(i);
            return;
          }
        }
        throw new VsaException(VsaError.ItemNotFound);
      }

      public virtual void RemoveItem(IVsaItem item){
        for (int i = 0, n = this.items.Count; i < n; i++){
          VsaItem vsaItem = (VsaItem)this.items[i];
          if (vsaItem == item){
            vsaItem.Remove();
            this.items.Remove(i);
            return;
          }
        }
        throw new VsaException(VsaError.ItemNotFound);
      }
      
      public virtual int GetItemCount(){
        return this.items.Count;
      }

      public virtual IVsaItem GetItemAtIndex(int index){
        if (index < this.items.Count)
          return (IVsaItem)this.items[index];
        else
          throw new VsaException(VsaError.ItemNotFound);
      }

      public virtual void RemoveItemAtIndex(int index){
        if (index < this.items.Count){
          ((VsaItem)this.items[index]).Remove();
          this.items.Remove(index);
        }else
          throw new VsaException(VsaError.ItemNotFound);
      }

      public virtual IVsaItem CreateDynamicItem(string itemName, VsaItemType type){
        if (this.engine.IsRunning) 
          return AddItem(itemName, type);
        else
          throw new VsaException(VsaError.EngineNotRunning);
      }

      internal override void CheckForErrors(){
        if (this.items.Count == 0)
          return;
        try{
          this.engine.Globals.ScopeStack.Push((ScriptObject)GetObject());
          // compile all the items in this scope
          foreach (Object item in this.items)
            ((VsaItem)item).CheckForErrors();
        }finally{
          this.engine.Globals.ScopeStack.Pop();
        }
      }

      internal override void Compile(){
        if (this.items.Count == 0)
          return;
        if (!this.isCompiled){
          this.isCompiled = true;
          try{
            this.engine.Globals.ScopeStack.Push((ScriptObject)GetObject());
            try{
              // compile all the items in this scope
              foreach (Object item in this.items)
                ((VsaItem)item).Compile();
            }finally{
              this.engine.Globals.ScopeStack.Pop();
            }
          }catch{
            this.isCompiled = false;
            throw;
          }
        }
      }

      internal override void Reset(){
        //this.scope = null;
        foreach (Object item in this.items)
          ((VsaItem)item).Reset();
      }
      
      internal void ReRun(GlobalScope scope){
        foreach (Object item in this.items)
          if (item is VsaHostObject)
            ((VsaHostObject)item).ReRun(scope);
        if (this.parent != null)
          this.parent.ReRun(scope);
      }

      internal override void Run(){
        if (this.items.Count == 0)
          return;
        try{
          this.engine.Globals.ScopeStack.Push((ScriptObject)GetObject());
          foreach (Object item in this.items)
            ((VsaItem)item).Run();
        }finally{
          this.engine.Globals.ScopeStack.Pop();
        }
      }    

      internal override void Close(){
        foreach (Object item in this.items)
          ((VsaItem)item).Close();
        this.items = null;
        this.parent = null;
        this.scope = null;
        this.isClosed = true;
      }

    }
}
