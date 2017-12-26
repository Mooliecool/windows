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
    using System.Globalization;
    using System.Reflection;
    
    public sealed class JSLocalField : JSVariableField{
      internal int slotNumber;
      internal IReflect inferred_type; //Records a guess of what the type of the field is. null = unknown.
      private ArrayList dependents; //A list of other fields whose types have been inferred (guessed) based on the inferred type of this field.
                                    //The dependend fields need to have their guesses changed.
      internal bool debugOn; // whether this local field was in a section of code with debug set or not. Control emitting debug info for the local
      internal JSLocalField outerField; //If this field is a copy of an outer scope field, the outer scope field is kept in here.
      internal bool isDefined; //Indicates whether an assignment to the field has been encountered.
      internal bool isUsedBeforeDefinition;
      
      public JSLocalField(String name, RuntimeTypeHandle handle, int slotNumber)
        : this(name, null, slotNumber, Missing.Value){
        this.type = new TypeExpression(new ConstantWrapper(Type.GetTypeFromHandle(handle), null));
        this.isDefined = true;
      }
      
      internal JSLocalField(String name, FunctionScope scope, int slotNumber, Object value)
        : base(name, scope, FieldAttributes.Static|FieldAttributes.Public) {
        this.slotNumber = slotNumber;
        this.inferred_type = null;
        this.dependents = null;
        this.value = value;
        this.debugOn = false;
        this.outerField = null;
        this.isDefined = false;
        this.isUsedBeforeDefinition = false;
      }
      
      public override Type FieldType{
        get{
          if (this.type != null)
            return base.FieldType;
          else
            return Convert.ToType(this.GetInferredType(null));
        }
      }
      
      internal override IReflect GetInferredType(JSField inference_target){
        if (this.outerField != null)
          return this.outerField.GetInferredType(inference_target);
        if (this.type != null)
          return base.GetInferredType(inference_target);
        if (this.inferred_type == null || this.inferred_type == Typeob.Object)
          return Typeob.Object;
        if (inference_target != null && inference_target != this){
          if (this.dependents == null)
            this.dependents = new ArrayList();
          this.dependents.Add(inference_target);
        }
        return this.inferred_type;
      }
    
      public override Object GetValue(Object obj){
        if ((this.attributeFlags&FieldAttributes.Literal) != (FieldAttributes)0 && !(this.value is FunctionObject))
          return this.value;
        else{
          while (obj is BlockScope) obj = ((BlockScope)obj).GetParent();
          StackFrame sf = (StackFrame)obj;
          JSLocalField f = this.outerField;
          int slot = this.slotNumber;
          while (f != null){
            slot = f.slotNumber;
            sf = (StackFrame)sf.GetParent();
            f = f.outerField;
          }
          return sf.localVars[slot];
        }
      }
      
      internal void SetInferredType(IReflect ir, AST expr){
        this.isDefined = true;
        if (this.type != null)
          return; //The local variable has a type annotation.
        if (this.outerField != null){
          this.outerField.SetInferredType(ir, expr);
          return;
        }
        if (Convert.IsPrimitiveNumericTypeFitForDouble(ir)) 
          ir = Typeob.Double;
        else if (ir == Typeob.Void)
          ir = Typeob.Object;
        if (this.inferred_type == null)
          this.inferred_type = ir;
        else{
          //Check to see if ir is compatible with this.inferred_type. If not, generalize this.inferred_type to Object and invalidate dependants.
          if (ir == this.inferred_type) return;
          if (!Convert.IsPrimitiveNumericType(this.inferred_type) || !Convert.IsPrimitiveNumericType(ir) || 
              !Convert.IsPromotableTo(ir, this.inferred_type)){
            this.inferred_type = Typeob.Object;
            if (this.dependents != null)
              for (int i = 0, n = this.dependents.Count; i < n; i++)
                ((JSLocalField)this.dependents[i]).SetInferredType(Typeob.Object, null);
          }
        }
      }
      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale){
        if (this.type != null)
          value = Convert.Coerce(value, this.type);
        while (obj is BlockScope) obj = ((BlockScope)obj).GetParent();
        StackFrame sf = (StackFrame)obj;
        JSLocalField f = this.outerField;
        int slot = this.slotNumber;
        while (f != null){
          slot = f.slotNumber;
          sf = (StackFrame)sf.GetParent();
          f = f.outerField;
        }
        if (sf.localVars == null) return; //happens when assigning to a constant from latebound code
        sf.localVars[slot] = value;
      }
   }
}
