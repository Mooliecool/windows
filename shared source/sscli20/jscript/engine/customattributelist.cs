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
  using System.Reflection.Emit;
  
  internal sealed class CustomAttributeList : AST{
    private ArrayList list;
    private ArrayList customAttributes;
    private bool alreadyPartiallyEvaluated;
  
    internal CustomAttributeList(Context context)
      : base(context) {
      this.list = new ArrayList();
      this.customAttributes = null;
      this.alreadyPartiallyEvaluated = false;
    }
  
    internal void Append(CustomAttribute elem){
      this.list.Add(elem);
      this.context.UpdateWith(elem.context);
    }

    internal bool ContainsExpandoAttribute(){ //Use only before partial evaluation has been done
      for (int i = 0, n = this.list.Count; i < n; i++){
        CustomAttribute ca = (CustomAttribute)this.list[i];
        if (ca == null) continue;
        if (ca.IsExpandoAttribute()) return true;
      }
      return false;
    }
    
    // Assume the custom attribute list has been partially evaluated already
    internal CustomAttribute GetAttribute(Type attributeClass){
      for (int i = 0, n = this.list.Count; i < n; i++){
        CustomAttribute ca = (CustomAttribute)this.list[i];
        if (ca == null) continue;
        Object type = ca.type;
        if (type is Type)
          if (type == attributeClass)
            return (CustomAttribute)this.list[i];
      }
      return null;
    }

    internal override Object Evaluate(){
      return this.Evaluate(false);
    }

    internal Object Evaluate(bool getForProperty){
      int n = this.list.Count;
      ArrayList attrValues = new ArrayList(n);
      for (int i = 0; i < n; i++){
        CustomAttribute ca = (CustomAttribute)this.list[i];
        if (ca == null) continue;
        if (ca.raiseToPropertyLevel){
          if (!getForProperty) continue;
        }else
          if (getForProperty) continue;
        attrValues.Add(ca.Evaluate());
      }
      Object[] result = new Object[attrValues.Count];
      attrValues.CopyTo(result);
      return result;
    }

    internal CustomAttributeBuilder[] GetCustomAttributeBuilders(bool getForProperty){
      this.customAttributes = new ArrayList(this.list.Count);
      for (int i = 0, n = this.list.Count; i < n; i++){
        CustomAttribute ca = (CustomAttribute)this.list[i];
        if (ca == null) continue;
        if (ca.raiseToPropertyLevel){
          if (!getForProperty) continue;
        }else
          if (getForProperty) continue;
        CustomAttributeBuilder attribute = ca.GetCustomAttribute();
        if (attribute != null)
          this.customAttributes.Add(attribute);
      }
      CustomAttributeBuilder[] result = new CustomAttributeBuilder[this.customAttributes.Count];
      this.customAttributes.CopyTo(result);
      return result;
    }
        
    internal override AST PartiallyEvaluate(){
      if (this.alreadyPartiallyEvaluated) return this;
      this.alreadyPartiallyEvaluated = true;
      for (int i = 0, n = this.list.Count; i < n; i++)
        this.list[i] = ((CustomAttribute)this.list[i]).PartiallyEvaluate();
      for (int i = 0, n = this.list.Count; i < n; i++){
        CustomAttribute ca = (CustomAttribute)this.list[i];
        if (ca == null) continue; //Already found to be invalid
        Object caType = ca.GetTypeIfAttributeHasToBeUnique();
        if (caType == null) continue;
        for (int j = i+1; j < n; j++){
          CustomAttribute caj = (CustomAttribute)this.list[j];
          if (caj == null) continue;
          if (caType == caj.type){
            caj.context.HandleError(JSError.CustomAttributeUsedMoreThanOnce);
            this.list[j] = null; //Remove duplicate
          }
        }
      }
      return this;
    }
    
    internal void Remove(CustomAttribute elem){
      this.list.Remove(elem);
    }

    internal void SetTarget(AST target){
      for (int i = 0, n = this.list.Count; i < n; i++)
        ((CustomAttribute)this.list[i]).SetTarget(target);
    }
    
    internal override void TranslateToIL(ILGenerator il, Type rtype){
    }
    
    internal override void TranslateToILInitializer(ILGenerator il){
    }

  }

}
