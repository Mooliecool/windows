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
  
  using System;
  using System.Collections;
  using Microsoft.Vsa;
  using Microsoft.JScript.Vsa;
  using System.Security.Permissions;

  public sealed class VsaItems : IVsaItems{
    private ArrayList items;
    private bool isClosed;
    private VsaEngine engine;
    internal int staticCodeBlockCount;

    // === Constructor ===

    public VsaItems(VsaEngine engine){
      this.engine = engine;
      this.staticCodeBlockCount = 0;
      this.items = new ArrayList(10);
    }

    // === Properties ===

    public IVsaItem this[int index]{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.isClosed)
          throw new VsaException(VsaError.EngineClosed);
        if (index < 0 || index >= items.Count)
          throw new VsaException(VsaError.ItemNotFound);
        return (IVsaItem)this.items[index];
      }
    }

    public IVsaItem this[string itemName]{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.isClosed)
          throw new VsaException(VsaError.EngineClosed);
        if (itemName != null){
          for (int i = 0, n = this.items.Count; i < n; i++){
            IVsaItem item = (IVsaItem)this.items[i];
            if (item.Name.Equals(itemName))
              return item;
          }
        }
        throw new VsaException(VsaError.ItemNotFound);
      }
    }

    public int Count{
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      get{
        if (this.isClosed)
          throw new VsaException(VsaError.EngineClosed);
        if (null != this.items)
          return this.items.Count;
        return 0;
      }
    }

    // === Methods ===

    public void Close(){
      if (this.isClosed)
        throw new VsaException(VsaError.EngineClosed);
      this.TryObtainLock();
      try{
        // close this item container
        this.isClosed = true;
        // close individual items
        foreach (Object item in this.items){
          ((VsaItem)item).Close();
        }
        this.items = null;
      }finally{
        this.ReleaseLock();
        this.engine = null;
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public IVsaItem CreateItem(string name, VsaItemType itemType, VsaItemFlag itemFlag){
      if (this.isClosed)
        throw new VsaException(VsaError.EngineClosed);
      if (this.engine.IsRunning)
        throw new VsaException(VsaError.EngineRunning);
      this.TryObtainLock();
      try{
        // The name must be valid for all items except reference items (in which case we don't care)
        if (itemType != VsaItemType.Reference && !this.engine.IsValidIdentifier(name))
          throw new VsaException(VsaError.ItemNameInvalid);
        // Make sure the name isn't already in use
        foreach (Object vsaItem in this.items){
          if (((VsaItem)vsaItem).Name.Equals(name))
            throw new VsaException(VsaError.ItemNameInUse);
        }
        IVsaItem item = null;
        switch (itemType){
          // IVsaReference
          case VsaItemType.Reference:
            if (itemFlag != VsaItemFlag.None)
              throw new VsaException(VsaError.ItemFlagNotSupported);
            // create a wrapper around an assembly
            item = new VsaReference((VsaEngine)this.engine, name);
            break;

          //IVsaGlobalItem
          case VsaItemType.AppGlobal:
            if (itemFlag != VsaItemFlag.None)
              throw new VsaException(VsaError.ItemFlagNotSupported);
            item = new VsaHostObject((VsaEngine)this.engine, name, VsaItemType.AppGlobal);
            ((VsaHostObject)item).isVisible = true;
            break;

          // IVsaCodeItem
          case VsaItemType.Code:
            if (itemFlag == VsaItemFlag.Class)
              throw new VsaException(VsaError.ItemFlagNotSupported);
            item = new VsaStaticCode((VsaEngine)this.engine, name, itemFlag);
            this.staticCodeBlockCount++;
            break;
        }
        if (item != null){
          this.items.Add(item);
        }else{
          throw new VsaException(VsaError.ItemTypeNotSupported);
        }
        ((VsaEngine)this.engine).IsDirty = true;
        return item;
      }finally{
        this.ReleaseLock();
      }
    }

    public IEnumerator GetEnumerator(){
      if (this.isClosed)
        throw new VsaException(VsaError.EngineClosed);
      return this.items.GetEnumerator();
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public void Remove(string itemName){
      if (this.isClosed)
        throw new VsaException(VsaError.EngineClosed);
      this.TryObtainLock();
      try{
        if (itemName == null)
          throw new System.ArgumentNullException("itemName");
        for (int i = 0, n = this.items.Count; i < n; i++){
          IVsaItem item = (IVsaItem)this.items[i];
          if (item.Name.Equals(itemName)){
            ((VsaItem)item).Remove();
            this.items.RemoveAt(i);
            ((VsaEngine)this.engine).IsDirty = true;
            if (item is VsaStaticCode)
              this.staticCodeBlockCount--;
            return;
          }
        }
        throw new VsaException(VsaError.ItemNotFound);
      }finally{
        this.ReleaseLock();
      }
    }

    [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
    public void Remove(int itemIndex){
      if (this.isClosed)
        throw new VsaException(VsaError.EngineClosed);
      this.TryObtainLock();
      try{
        if (0 <= itemIndex && itemIndex < items.Count){
          VsaItem item = (VsaItem)this.items[itemIndex];
          item.Remove();
          this.items.RemoveAt(itemIndex);
            if (item is VsaStaticCode)
              this.staticCodeBlockCount--;
          return;
        }
        throw new VsaException(VsaError.ItemNotFound);
      }finally{
        this.ReleaseLock();
      }
    }

    // Helper methods

    private void TryObtainLock(){
      ((VsaEngine)this.engine).TryObtainLock();
    }

    private void ReleaseLock(){
      ((VsaEngine)this.engine).ReleaseLock();
    }
  }
}
