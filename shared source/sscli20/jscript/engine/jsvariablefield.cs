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

/* Base class for parameters, local variables, global variables and class members. Used at compile time for symbol table entries. 
Only one of these should be allocated per source entity. Heavyweight because of the compile time info. */

namespace Microsoft.JScript {
    
    using System;
    using System.Collections;
    using System.Reflection;
    using System.Reflection.Emit;
    
    public abstract class JSVariableField : JSField{
      internal ScriptObject obj;
      private String name;
      internal String debuggerName;
      internal Object metaData;
      internal TypeExpression type; //Records the programmer provided type of the field. null == omitted == Object.
      internal FieldAttributes attributeFlags; 
      private MethodInfo method;
      private ConstructorInfo cons;
      internal Object value;
      internal CustomAttributeList customAttributes;
      internal Context originalContext; // never change this context. It is shared
      internal CLSComplianceSpec clsCompliance;
      
      internal JSVariableField(String name, ScriptObject obj, FieldAttributes attributeFlags){
        this.obj = obj;
        this.name = name;
        this.debuggerName = name;
        this.metaData = null;
        if ((attributeFlags & FieldAttributes.FieldAccessMask) == (FieldAttributes)0)
          attributeFlags |= FieldAttributes.Public; //Fields not marked otherwise, are public
        this.attributeFlags = attributeFlags;
        this.type = null;
        this.method = null;
        this.cons = null;
        this.value = null;
        this.originalContext = null;
        this.clsCompliance = CLSComplianceSpec.NotAttributed;
      }
      
      public override FieldAttributes Attributes{
        get{
          return this.attributeFlags;
        }
      }
       
      internal void CheckCLSCompliance(bool classIsCLSCompliant){
        if (this.customAttributes != null){
          CustomAttribute clsAttr = this.customAttributes.GetAttribute(Typeob.CLSCompliantAttribute);
          if (clsAttr != null){
            this.clsCompliance = clsAttr.GetCLSComplianceValue();
            this.customAttributes.Remove(clsAttr);
          }
        }
        if (classIsCLSCompliant){
          if (this.clsCompliance != CLSComplianceSpec.NonCLSCompliant)
            if (type != null && !type.IsCLSCompliant()){
              this.clsCompliance = CLSComplianceSpec.NonCLSCompliant;
              if (this.originalContext != null)
                this.originalContext.HandleError(JSError.NonCLSCompliantMember);
            }
        }else if (this.clsCompliance == CLSComplianceSpec.CLSCompliant)
          this.originalContext.HandleError(JSError.MemberTypeCLSCompliantMismatch);
      }

      public override Type DeclaringType{
        get{
          if (this.obj is ClassScope)
            return ((ClassScope)(this.obj)).GetTypeBuilderOrEnumBuilder();
          else
            return null;
        }
      }
    
      public override Type FieldType{
        get{
          Type result = Typeob.Object;
          if (this.type != null){
            result = this.type.ToType();
            if (result == Typeob.Void)
              result = Typeob.Object;
          }
          return result;
        }
      }

            
      internal MethodInfo GetAsMethod(Object obj){
        if (this.method == null)
          this.method = new JSFieldMethod(this, obj);
        return this.method;
      }

      internal override String GetClassFullName(){
        if (this.obj is ClassScope){
          return ((ClassScope)this.obj).GetFullName();
        }
        throw new JScriptException(JSError.InternalError);
      }
            
      public override Object[] GetCustomAttributes(bool inherit){
        if (this.customAttributes != null) return (Object[])this.customAttributes.Evaluate();
        return new Object[0];
      }
    
      internal virtual IReflect GetInferredType(JSField inference_target){
        if (this.type != null)
          return this.type.ToIReflect();
        else
          return Typeob.Object;
      }
    
      internal override Object GetMetaData(){
        return this.metaData;
      }
      
      internal override PackageScope GetPackage(){
        if (this.obj is ClassScope){
          return ((ClassScope)this.obj).GetPackage();
        }
        throw new JScriptException(JSError.InternalError);
      }
            
      public override String Name{
        get{
          return this.name;
        }
      }

      internal void WriteCustomAttribute(bool doCRS){
        if (this.metaData is FieldBuilder){
          FieldBuilder fb = (FieldBuilder)this.metaData;
          if (this.customAttributes != null){
            CustomAttributeBuilder[] fieldCustomAttributes = this.customAttributes.GetCustomAttributeBuilders(false);
            for (int attIndex = 0, attLength = fieldCustomAttributes.Length; attIndex < attLength; attIndex++)
              fb.SetCustomAttribute(fieldCustomAttributes[attIndex]);
          }
          if (this.clsCompliance == CLSComplianceSpec.CLSCompliant)
            fb.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{true}));
          else if (this.clsCompliance == CLSComplianceSpec.NonCLSCompliant)
            fb.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{false}));
          if (doCRS){
            if (this.IsStatic)
              fb.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.contextStaticAttributeCtor, new Object[]{}));
          }
        }   
      }
          
    }
}
