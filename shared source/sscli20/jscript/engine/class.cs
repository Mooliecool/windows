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
    using System.Globalization;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Text;
    
    internal class Class : AST{
      internal String name;
      private TypeExpression superTypeExpression;
      private TypeExpression[] interfaces;
      internal Block body;
      internal ScriptObject enclosingScope;
      internal TypeAttributes attributes;
      private bool hasAlreadyBeenAskedAboutExpando;
      internal bool isAbstract;
      private bool isAlreadyPartiallyEvaluated;
      private bool isCooked;
      private Type cookedType;
      private bool isExpando;
      internal bool isInterface;
      internal bool isStatic;
      protected bool needsEngine;
      internal AttributeTargets validOn;
      internal bool allowMultiple;
      protected ClassScope classob;
      private FunctionObject implicitDefaultConstructor;
      private JSVariableField ownField;
      protected JSMemberField[] fields;
      private Class superClass;
      private IReflect superIR;
      private Object[] superMembers;
      private SimpleHashtable firstIndex;
      private MethodInfo fieldInitializer;
      internal CustomAttributeList customAttributes;
      internal CLSComplianceSpec clsCompliance;
      // expando related fields
      private bool generateCodeForExpando; // true if this class is the first expando class in a hierarchy chain
      private PropertyBuilder expandoItemProp;
      private MethodBuilder getHashTableMethod;
      private MethodBuilder getItem;
      private MethodBuilder setItem;
      internal MethodBuilder deleteOpMethod;
      private static int badTypeNameCount = 0; // If the type name is invalid, we generate one using "bad type name " + this index.
      
      internal Class(Context context, AST id, TypeExpression superTypeExpression, TypeExpression[] interfaces, Block body, 
        FieldAttributes attributes, bool isAbstract, bool isFinal, bool isStatic, bool isInterface, CustomAttributeList customAttributes)
        : base(context) {
        this.name = id.ToString();
        this.superTypeExpression = superTypeExpression;
        this.interfaces = interfaces;
        this.body = body;
        this.enclosingScope = (ScriptObject)Globals.ScopeStack.Peek(1);
        this.attributes = TypeAttributes.Class|TypeAttributes.Serializable;
        this.SetAccessibility(attributes);
        if (isAbstract)
          this.attributes |= TypeAttributes.Abstract;
        this.isAbstract = isAbstract || isInterface;
        this.isAlreadyPartiallyEvaluated = false;
        if (isFinal)
          this.attributes |= TypeAttributes.Sealed;
        if (isInterface)
          this.attributes |= TypeAttributes.Interface | TypeAttributes.Abstract;
        this.isCooked = false;
        this.cookedType = null;
        this.isExpando = false;
        this.isInterface = isInterface;
        this.isStatic = isStatic;
        this.needsEngine = !isInterface;
        this.validOn = (AttributeTargets)0;
        this.allowMultiple = true;
        this.classob = (ClassScope)Globals.ScopeStack.Peek();
        this.classob.name = this.name;
        this.classob.owner = this;
        this.implicitDefaultConstructor = null;
        if (!isInterface && !(this is EnumDeclaration))
          this.SetupConstructors();
        this.EnterNameIntoEnclosingScopeAndGetOwnField(id, isStatic);
        this.fields = this.classob.GetMemberFields();
        this.superClass = null;
        this.superIR = null;
        this.superMembers = null;
        this.firstIndex = null;
        this.fieldInitializer = null;
        this.customAttributes = customAttributes;
        this.clsCompliance = CLSComplianceSpec.NotAttributed;
        this.generateCodeForExpando = false;
        this.expandoItemProp = null;
        this.getHashTableMethod = null;
        this.getItem = null;
        this.setItem = null;
      }
      
      private void AddImplicitInterfaces(IReflect iface, IReflect[] explicitInterfaces, ArrayList implicitInterfaces){
        Type ifaceT = iface as Type;
        if (ifaceT != null){
          Type[] implInterfaces = ifaceT.GetInterfaces();
          foreach (Type implIface in implInterfaces){
            if (Array.IndexOf(explicitInterfaces, implIface, 0) >= 0) return;
            if (implicitInterfaces.IndexOf(implIface, 0) >= 0) return;
            implicitInterfaces.Add(implIface);
          }
          return;
        }
        foreach (TypeExpression ifaceExpr in ((ClassScope)iface).owner.interfaces){
          IReflect implIface = ifaceExpr.ToIReflect();
          if (Array.IndexOf(explicitInterfaces, implIface, 0) >= 0) return;
          if (implicitInterfaces.IndexOf(implIface, 0) >= 0) return;
          implicitInterfaces.Add(implIface);
        }
        return;
      }

      private void AllocateImplicitDefaultConstructor(){
        this.implicitDefaultConstructor = new FunctionObject(".ctor", new ParameterDeclaration[0], null, new Block(this.context), 
          new FunctionScope(this.classob, true), this.classob, this.context, MethodAttributes.Virtual|MethodAttributes.NewSlot|MethodAttributes.Public, null, true);
        this.implicitDefaultConstructor.isImplicitCtor = true; 
        this.implicitDefaultConstructor.isConstructor = true;  
        this.implicitDefaultConstructor.proto = this.classob;
      }
      
      private bool CanSee(MemberInfo member){
        switch (member.MemberType){
          case MemberTypes.Method:
            {MethodAttributes visibility = ((MethodBase)member).Attributes & MethodAttributes.MemberAccessMask;
            if (visibility == MethodAttributes.Private || visibility == MethodAttributes.PrivateScope || visibility == MethodAttributes.FamANDAssem)
              return false;
            if (visibility == MethodAttributes.Assembly)
              return IsInTheSamePackage(member);
            return true;}
          case MemberTypes.Field:
            {FieldAttributes visibility = ((FieldInfo)member).Attributes & FieldAttributes.FieldAccessMask;
            if (visibility == FieldAttributes.Private || visibility == FieldAttributes.PrivateScope || visibility == FieldAttributes.FamANDAssem)
              return false;
            if (visibility == FieldAttributes.Assembly)
              return IsInTheSamePackage(member);
            return true;}
          case MemberTypes.Property:
            {MethodBase propMethod = JSProperty.GetGetMethod((PropertyInfo)member, true);
            if (propMethod == null)
              propMethod = JSProperty.GetSetMethod((PropertyInfo)member, true);
            if (propMethod == null)
              return false;
            else{
              MethodAttributes visibility = propMethod.Attributes & MethodAttributes.MemberAccessMask;
              if (visibility == MethodAttributes.Private || visibility == MethodAttributes.PrivateScope || visibility == MethodAttributes.FamANDAssem)
                return false;
              if (visibility == MethodAttributes.Assembly)
                return IsInTheSamePackage(member);
            }
            return true;}
          case MemberTypes.Event:
            {MethodBase addMethod = ((EventInfo)member).GetAddMethod();
            if (addMethod == null)
              return false;
            else{
              MethodAttributes visibility = addMethod.Attributes & MethodAttributes.MemberAccessMask;
              if (visibility == MethodAttributes.Private || visibility == MethodAttributes.PrivateScope || visibility == MethodAttributes.FamANDAssem)
                return false;
              if (visibility == MethodAttributes.Assembly)
                return IsInTheSamePackage(member);
            }
            return true;}
          case MemberTypes.TypeInfo: 
          case MemberTypes.NestedType:
            {TypeAttributes visibility = ((Type)member).Attributes & TypeAttributes.VisibilityMask;
            if (visibility == TypeAttributes.NestedPrivate || visibility == TypeAttributes.NestedFamANDAssem)
              return false;
            if (visibility == TypeAttributes.NestedAssembly)
              return IsInTheSamePackage(member);
            return true;}
        }
        return true;
      }
      
      private void CheckFieldDeclarationConsistency(JSMemberField field){
        Object index = this.firstIndex[field.Name];
        if (index == null) return; //There is no super class member with the same name as the field
        for (int i = (int)index, n = this.superMembers.Length; i < n; i++){
          Object supMem = this.superMembers[i];
          if (!(supMem is MemberInfo)) return;
          MemberInfo member = (MemberInfo)supMem;
          if (!member.Name.Equals(field.Name)) return; 
          if (this.CanSee(member)){
            String supMemberName = this.GetFullNameFor(member);
            field.originalContext.HandleError(JSError.HidesParentMember, supMemberName, this.IsInTheSameCompilationUnit(member));
            return;
          }
        }
      }
      
      private void CheckIfOKToGenerateCodeForExpando(bool superClassIsExpando){
        if (superClassIsExpando){
          this.context.HandleError(JSError.BaseClassIsExpandoAlready);
          this.generateCodeForExpando = false;
          return;
        }
        
        // make sure the current class does not define an Item property
        if (this.classob.GetMember("Item", BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly).Length > 0){
          this.context.HandleError(JSError.ItemNotAllowedOnExpandoClass);
          this.generateCodeForExpando = false;
          return;
        }
        if (this.classob.GetMember("get_Item", BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly).Length > 0 ||
            this.classob.GetMember("set_Item", BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly).Length > 0){
          this.context.HandleError(JSError.MethodNotAllowedOnExpandoClass);
          this.generateCodeForExpando = false;
          return;
        }
            
        // make sure the current class does not implements IEnumerable
        if (this.ImplementsInterface(Typeob.IEnumerable)){
          this.context.HandleError(JSError.ExpandoClassShouldNotImpleEnumerable);
          this.generateCodeForExpando = false;
          return;
        }
              
        // make sure up in the hierarchy chain no property named 'Item' is defined
        if (this.superIR.GetMember("Item", BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance).Length > 0 ||
            this.superIR.GetMember("get_Item", BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance).Length > 0 ||
            this.superIR.GetMember("set_Item", BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance).Length > 0){
          this.context.HandleError(JSError.MethodClashOnExpandoSuperClass);
          this.generateCodeForExpando = false;
          return;
        }

        // add an expando property to the current class for Binding to find
        JSProperty itemProp = this.classob.itemProp = new JSProperty("Item");
        itemProp.getter = new JSExpandoIndexerMethod(this.classob, true);
        itemProp.setter = new JSExpandoIndexerMethod(this.classob, false);
        this.classob.AddNewField("Item", itemProp, FieldAttributes.Literal);
      }

      private String GetFullName(){
        String scopeName = ((ActivationObject)this.enclosingScope).GetName();
        if (scopeName == null){
          VsaEngine engine = this.context.document.engine;
          if (engine != null && engine.genStartupClass)
            scopeName = engine.RootNamespace;
        }
        if (scopeName != null)
          return scopeName + "." + this.name;
        else
          return this.name;
      }

      protected void CheckMemberNamesForCLSCompliance(){
        // Check top-level class name for CLS compliance.
        if (!(this.enclosingScope is ClassScope))
          this.Engine.CheckTypeNameForCLSCompliance(this.name, this.GetFullName(), this.context);
        // Check is all member names are CLS compliant
        Hashtable caseInsensitiveNameTable = new Hashtable(StringComparer.OrdinalIgnoreCase);
        for (int i = 0, n = this.fields.Length; i < n; i++){
          JSMemberField field = this.fields[i];
          if (field.IsPrivate) continue;
          if (!VsaEngine.CheckIdentifierForCLSCompliance(field.Name))
            field.originalContext.HandleError(JSError.NonCLSCompliantMember);
          else{
            if ((JSMemberField)caseInsensitiveNameTable[field.Name] == null)
              caseInsensitiveNameTable.Add(field.Name, field);
            else
              field.originalContext.HandleError(JSError.NonCLSCompliantMember);
          }
        }
      }

      private void CheckIfValidExtensionOfSuperType(){   
        this.GetIRForSuperType();
        ClassScope csc = this.superIR as ClassScope;
        if (csc != null){
          if (this.IsStatic){
            if (!csc.owner.IsStatic){
              this.superTypeExpression.context.HandleError(JSError.NestedInstanceTypeCannotBeExtendedByStatic);
              this.superIR = Typeob.Object;
              this.superTypeExpression = null;
            }
          }else{
            if (!csc.owner.IsStatic && this.enclosingScope != csc.owner.enclosingScope){
              this.superTypeExpression.context.HandleError(JSError.NestedInstanceTypeCannotBeExtendedByStatic);
              this.superIR = Typeob.Object;
              this.superTypeExpression = null;
            }
          }
        }
        this.GetSuperTypeMembers();
        this.GetStartIndexForEachName();
        
        bool checkCLSCompliance = this.NeedsToBeCheckedForCLSCompliance();
        if (checkCLSCompliance)
          this.CheckMemberNamesForCLSCompliance();

        //First check only the methods the implement interfaces
        for (int i = 0, n = this.fields.Length; i < n; i++){
          JSMemberField field = this.fields[i];
          if (field.IsLiteral){
            Object value = field.value;
            if (value is FunctionObject){
              JSMemberField startMethod = field;
              while(true){
                FunctionObject func = (FunctionObject)value;
                if (func.implementedIface == null) break;
                this.CheckMethodDeclarationConsistency(func);
                if (func.implementedIfaceMethod == null)
                  func.funcContext.HandleError(JSError.NoMethodInBaseToOverride);
                if (field.IsPublic || field.IsFamily || field.IsFamilyOrAssembly)
                  func.CheckCLSCompliance(checkCLSCompliance);
                field = field.nextOverload;
                if (field == null) break;
                value = field.value;
              }
              continue;
            }else if (value is JSProperty){
              continue;
            }
          }
        }
        for (int i = 0, n = this.fields.Length; i < n; i++){
          JSMemberField field = this.fields[i];
          if (field.IsLiteral){
            Object value = field.value;
            if (value is FunctionObject){
              JSMemberField startMethod = field;
              while(true){
                FunctionObject func = (FunctionObject)value;
                if (func.implementedIface != null) break;
                this.CheckMethodDeclarationConsistency(func);
                if (field.IsPublic || field.IsFamily || field.IsFamilyOrAssembly)
                  func.CheckCLSCompliance(checkCLSCompliance);
                field = field.nextOverload;
                if (field == null) break;
                value = field.value;
              }
              continue;
            }else if (value is JSProperty){
              continue;
            }
          }
          this.CheckFieldDeclarationConsistency(field);
          if (field.IsPublic || field.IsFamily || field.IsFamilyOrAssembly)
            field.CheckCLSCompliance(checkCLSCompliance);
        }
      }
          
      private void CheckMethodDeclarationConsistency(FunctionObject func){
        if (func.isStatic && !func.isExpandoMethod) return; //static methods do not clash with superclass methods
        if (func.isConstructor) return; //Constructors cannot clash with superclass members
        Object index = this.firstIndex[func.name];
        if (index == null){
          //There is no super class member with the same name as the function
          this.CheckThatMethodIsNotMarkedWithOverrideOrHide(func);
          if ((func.attributes & MethodAttributes.Final) != 0)
            func.attributes &= ~(MethodAttributes.Virtual|MethodAttributes.NewSlot|MethodAttributes.Final);
          return;
        }
        MemberInfo differentTypeOfMember = null;

        for (int i = (int)index, n = this.superMembers.Length; i < n; i++){
          MemberInfo member = this.superMembers[i] as MemberInfo;
          if (member == null) 
            //if we do not get a MemberInfo we have already processed this superclass member and it matches another member of the current class
            continue;
          if (!member.Name.Equals(func.name)) break;
          if (!this.CanSee(member)) continue;
          if (member.MemberType != MemberTypes.Method){
            //JScript does not allow overloading among different member types. 
            //Unless there is a superclass method with the same signature as the method, we have to give an error 
            differentTypeOfMember = member;
            continue;
          }
          if (func.isExpandoMethod){
            differentTypeOfMember = member;
            break;
          }
          MethodInfo supmeth = (MethodInfo)member;
          if (func.implementedIface != null){
            //Skip superclass methods that do not come from the appropriate interface
            if (supmeth is JSFieldMethod){
              if (((JSFieldMethod)supmeth).EnclosingScope() != func.implementedIface) continue;
            }else{
              if (supmeth.DeclaringType != func.implementedIface) continue;
            }
          }
          if (Class.ParametersMatch(supmeth.GetParameters(), func.parameter_declarations)){
            if (supmeth is JSWrappedMethod)
              supmeth = ((JSWrappedMethod)supmeth).method;
            if (func.noVersionSafeAttributeSpecified || (func.attributes & MethodAttributes.VtableLayoutMask) != MethodAttributes.NewSlot){
              //Check consistency of implicit or explicit override (a hiding method may be inconsistent)
              this.CheckMatchingMethodForConsistency(supmeth, func, i, n);
            }
            return;
          }
        }
        if (differentTypeOfMember != null){
          //Did not find a superclass method with the same signature, but did find a member with same name that was not a method
          //This is a no no for JScript unless the hide attribute has been specified
          if (func.noVersionSafeAttributeSpecified || 
            (func.attributes & MethodAttributes.VtableLayoutMask) != MethodAttributes.NewSlot && !func.isExpandoMethod){
            String supMemberName = this.GetFullNameFor(differentTypeOfMember);
            func.funcContext.HandleError(JSError.HidesParentMember, supMemberName, this.IsInTheSameCompilationUnit(differentTypeOfMember));
          }
          return;
        }
        //No matching method in superclass. Give an error if hide/override was specified
        this.CheckThatMethodIsNotMarkedWithOverrideOrHide(func);
        //Make final methods into non virtual
        if ((func.attributes & MethodAttributes.Final) != 0)
          func.attributes &= ~(MethodAttributes.Virtual|MethodAttributes.NewSlot|MethodAttributes.Final);
      }
        
      private void CheckMatchingMethodForConsistency(MethodInfo matchingMethod, FunctionObject func, int i, int n){
        // return-type consistency - the return-type has to be the same
        IReflect rir = func.ReturnType(null);
        IReflect mrir = matchingMethod is JSFieldMethod ? ((JSFieldMethod)matchingMethod).func.ReturnType(null) : matchingMethod.ReturnType;
        if (!rir.Equals(mrir)){
          func.funcContext.HandleError(JSError.DifferentReturnTypeFromBase, func.name, true);
          return;
        }
        
        //Special treatment for methods that implement interface methods    
        if (func.implementedIface != null){
          func.implementedIfaceMethod = matchingMethod;
          this.superMembers[i] = func.name; //obliterate it so that it does not show up as unimplemented
          return;
        }

        // visibility consistency - the visibility specification has to be the same
        MethodAttributes visibility = func.attributes & MethodAttributes.MemberAccessMask;
        if ((matchingMethod.Attributes & MethodAttributes.MemberAccessMask) != visibility)
          //Allow Family to match FamORAssem
          if ((matchingMethod.Attributes & MethodAttributes.MemberAccessMask) != MethodAttributes.FamORAssem || visibility != MethodAttributes.Family)
            func.funcContext.HandleError(JSError.CannotChangeVisibility);

        // hiding, overriding and layout consistency
        // if i >= 0 after this, the base method is an overridden abstract method and steps should be taken to prevent a not implemented error
        if (func.noVersionSafeAttributeSpecified){ // current method does not specify any attribute (i.e. hide or override)
          if (this.Engine.versionSafe){
            //Give a message. The compiler option requires a method to say hide or override when there is a match.
            if ((matchingMethod.Attributes & MethodAttributes.Abstract) != 0){ // base is abstract
              func.funcContext.HandleError(JSError.HidesAbstractInBase, this.name + "." + func.name);
              func.attributes &= ~MethodAttributes.NewSlot;
              //Recover from error by overriding, it may be less bad than throwing a class load exception
            }else{ 
              func.funcContext.HandleError(JSError.NewNotSpecifiedInMethodDeclaration, this.IsInTheSameCompilationUnit(matchingMethod));
              i = -1;
            }
          }else{
            //No message, override if possible, otherwise hide
            if ((matchingMethod.Attributes & MethodAttributes.Virtual) == 0 || 
                (matchingMethod.Attributes & MethodAttributes.Final) != 0){ // base is non virtual or final, hide
              i = -1;
            }else{
              func.attributes &= ~MethodAttributes.NewSlot; //override
              if ((matchingMethod.Attributes & MethodAttributes.Abstract) == 0)
                i = -1;
            }
          }
        }else{ //Current method is marked override or hide
          if ((func.attributes & MethodAttributes.VtableLayoutMask) == MethodAttributes.ReuseSlot){ // current method specifies override
            if ((matchingMethod.Attributes & MethodAttributes.Virtual) == 0 || 
              (matchingMethod.Attributes & MethodAttributes.Final) != 0){ // base is non virtual or final, hide
              func.funcContext.HandleError(JSError.MethodInBaseIsNotVirtual);
              i = -1;
            }else{ 
              func.attributes &= ~MethodAttributes.NewSlot; //override
              if ((matchingMethod.Attributes & MethodAttributes.Abstract) == 0)
                i = -1;
            }
          }else{ // current method specifies hide
            Debug.Assert((func.attributes & MethodAttributes.VtableLayoutMask) == MethodAttributes.NewSlot);
            if ((matchingMethod.Attributes & MethodAttributes.Abstract) != 0){ // base is abstract
              func.funcContext.HandleError(JSError.HidesAbstractInBase, this.name + "." + func.name);
              func.attributes &= ~MethodAttributes.NewSlot;
              //Recover from error by overriding, it may be less bad than throwing a class load exception
            }else
              i = -1;
          }
        }

        if (i >= 0){
          //Overriding an abstract method. Take steps to prevent error messages.
          Debug.Assert((matchingMethod.Attributes & MethodAttributes.Abstract) != 0);
          this.superMembers[i] = func.name; //obliterate it so that it does not show up as unimplemented
          //Do likewise for any matching abstract members declared in less derived base classes
          for (int j = i+1; j < n; j++){ //Most derived class is always first
            MemberInfo mem = this.superMembers[j] as MemberInfo;
            if (mem == null) continue;
            if (mem.Name != matchingMethod.Name) break;
            MethodInfo meth2 = mem as MethodInfo;
            if (meth2 == null) continue;
            if (meth2.IsAbstract && Class.ParametersMatch(meth2.GetParameters(), matchingMethod.GetParameters())){
              IReflect rt = matchingMethod is JSFieldMethod ? ((JSFieldMethod)matchingMethod).ReturnIR() : matchingMethod.ReturnType;
              IReflect rt2 = meth2 is JSFieldMethod ? ((JSFieldMethod)meth2).ReturnIR() : meth2.ReturnType;
              if (rt == rt2) this.superMembers[j] = func.name;
            }
          }
        }
      }

      private void CheckThatAllAbstractSuperClassMethodsAreImplemented(){
        //By the time we get here all abstract superclass members that are implemented by this class are already replaced by
        //their names in superMembers. What remains is to weed out those that are implemented by more derived base classes
        //and then to complain about any that are left over.
        for (int i = 0, n = this.superMembers.Length; i < n; i++){
          Object ob = this.superMembers[i];
          MethodInfo meth = ob as MethodInfo;
          if (meth != null){
            if (!meth.IsAbstract) continue;
            //Check to see if implemented by a more derived base class
            for (int j = i-1; j >= 0; j--){
              Object mem = this.superMembers[j];
              if (mem is MethodInfo){
                MethodInfo meth2 = (MethodInfo)mem;
                if (meth2.Name != meth.Name) break;
                if (!meth2.IsAbstract && Class.ParametersMatch(meth2.GetParameters(), meth.GetParameters())){
                  IReflect rt = meth is JSFieldMethod ? ((JSFieldMethod)meth).ReturnIR() : meth.ReturnType;
                  IReflect rt2 = meth2 is JSFieldMethod ? ((JSFieldMethod)meth2).ReturnIR() : meth2.ReturnType;
                  if (rt == rt2){
                    this.superMembers[i] = meth.Name;
                    goto noError;
                  }
                }
              }
            }
            //Nope, give an error (unless class is marked abstract, but not if the method comes from an interface)
            if (!this.isAbstract || (!this.isInterface && Class.DefinedOnInterface(meth))){
              StringBuilder sig = new StringBuilder(meth.DeclaringType.ToString());
              sig.Append('.');
              sig.Append(meth.Name);
              sig.Append('(');
              ParameterInfo[] pars = meth.GetParameters();
              for (int j = 0, m = pars.Length; j < m; j++){
                sig.Append(pars[j].ParameterType.FullName);
                if (j < m-1)
                  sig.Append(", ");
              }
              sig.Append(")");
              if (meth.ReturnType != Typeob.Void){
                sig.Append(" : ");
                sig.Append(meth.ReturnType.FullName);
              }
              this.context.HandleError(JSError.MustImplementMethod, sig.ToString());
              this.attributes |= TypeAttributes.Abstract; //Make the class compilable
            }
            //If the super class method comes from an interface, emit an abstract method that implements it
          }
        noError:;
        }
      }

      private void CheckThatMethodIsNotMarkedWithOverrideOrHide(FunctionObject func){
        if (func.noVersionSafeAttributeSpecified) return;
        //It is marked override or hide, give an appropriate error
        if ((func.attributes & MethodAttributes.VtableLayoutMask) == MethodAttributes.ReuseSlot) // current method specifies override
          func.funcContext.HandleError(JSError.NoMethodInBaseToOverride);
        else // current method specifies hide
          func.funcContext.HandleError(JSError.NoMethodInBaseToNew);
      }

      private static bool DefinedOnInterface(MethodInfo meth){
        JSFieldMethod jsmeth = meth as JSFieldMethod;
        if (jsmeth != null) return ((ClassScope)jsmeth.func.enclosing_scope).owner.isInterface;
        return meth.DeclaringType.IsInterface;
      }
      
      private void EmitILForINeedEngineMethods(){
        if (!this.needsEngine)
          return;
        TypeBuilder classwriter = (TypeBuilder)this.classob.classwriter;
        FieldBuilder backingField = classwriter.DefineField("vsa Engine", Typeob.VsaEngine, FieldAttributes.Private | FieldAttributes.NotSerialized);
        
        MethodBuilder getMeth = classwriter.DefineMethod("GetEngine", MethodAttributes.Private|MethodAttributes.Virtual, Typeob.VsaEngine, null);
        ILGenerator il = getMeth.GetILGenerator();
        il.Emit(OpCodes.Ldarg_0);
        il.Emit(OpCodes.Ldfld, backingField);
        il.Emit(OpCodes.Ldnull);
        Label endif = il.DefineLabel();
        il.Emit(OpCodes.Bne_Un_S, endif);
        il.Emit(OpCodes.Ldarg_0);
        if (this.body.Engine.doCRS)        
          il.Emit(OpCodes.Ldsfld, CompilerGlobals.contextEngineField);
        else {
          if (this.context.document.engine.PEFileKind == PEFileKinds.Dll){
            il.Emit(OpCodes.Ldtoken, classwriter);
            il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngineWithType);
          }else
            il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngine);
        }

        il.Emit(OpCodes.Stfld, backingField);
        il.MarkLabel(endif);
        il.Emit(OpCodes.Ldarg_0);
        il.Emit(OpCodes.Ldfld, backingField);
        il.Emit(OpCodes.Ret);
        classwriter.DefineMethodOverride(getMeth, CompilerGlobals.getEngineMethod);
        
        MethodBuilder setMeth = classwriter.DefineMethod("SetEngine", MethodAttributes.Private|MethodAttributes.Virtual, Typeob.Void, new Type[]{Typeob.VsaEngine});
        il = setMeth.GetILGenerator();
        il.Emit(OpCodes.Ldarg_0);
        il.Emit(OpCodes.Ldarg_1);
        il.Emit(OpCodes.Stfld, backingField);
        il.Emit(OpCodes.Ret);
        classwriter.DefineMethodOverride(setMeth, CompilerGlobals.setEngineMethod);
      }
      
      internal void EmitInitialCalls(ILGenerator il, MethodBase supcons, ParameterInfo[] pars, ASTList argAST, int callerParameterCount){
        bool callFieldInitializer = true;
        if (supcons != null){
          il.Emit(OpCodes.Ldarg_0);
          int n = pars.Length;
          int m = argAST == null ? 0 : argAST.count;
          Object[] tempArray = new Object[n];
          for (int i = 0; i < n; i++){
            AST actual = i < m ? argAST[i] : new ConstantWrapper(null, null);
            if (pars[i].ParameterType.IsByRef)
              tempArray[i] = actual.TranslateToILReference(il, pars[i].ParameterType.GetElementType());
            else{
              actual.TranslateToIL(il, pars[i].ParameterType);
              tempArray[i] = null;
            }
          }
          if (supcons is JSConstructor){
            JSConstructor cons = (JSConstructor)supcons;
            callFieldInitializer = cons.GetClassScope() != this.classob;
            supcons = cons.GetConstructorInfo(this.compilerGlobals);
            //if cons belongs to a nested instance class, load the outer class instance on the stack
            if (cons.GetClassScope().outerClassField != null)
              Convert.EmitLdarg(il, (short)callerParameterCount);
          }
          il.Emit(OpCodes.Call, (ConstructorInfo)supcons);
          for (int i = 0; i < m; i++){
            AST arg = argAST[i];
            if (arg is AddressOf && tempArray[i] != null){
              Type argType = Convert.ToType(arg.InferType(null));
              arg.TranslateToILPreSet(il);
              il.Emit(OpCodes.Ldloc, (LocalBuilder)tempArray[i]);
              Convert.Emit(this, il, pars[i].ParameterType, argType);
              arg.TranslateToILSet(il);
            }
          }
        }
        if (this.classob.outerClassField != null){
          il.Emit(OpCodes.Ldarg_0);
          Convert.EmitLdarg(il, (short)callerParameterCount);
          il.Emit(OpCodes.Stfld, this.classob.outerClassField);
        }
        if (callFieldInitializer){
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Call, this.fieldInitializer);
          ((Block)(this.body)).TranslateToILInitOnlyInitializers(il);
        }
      }
        
      private void EnterNameIntoEnclosingScopeAndGetOwnField(AST id, bool isStatic){
        if (((IActivationObject)this.enclosingScope).GetLocalField(this.name) != null){
          id.context.HandleError(JSError.DuplicateName, true);
          this.name = this.name + " class";
        }
        FieldAttributes fieldAttrs = FieldAttributes.Literal;
        switch (this.attributes & TypeAttributes.VisibilityMask){
          case TypeAttributes.NestedAssembly : fieldAttrs |= FieldAttributes.Assembly; break;
          case TypeAttributes.NestedFamANDAssem : fieldAttrs |= FieldAttributes.FamANDAssem; break;
          case TypeAttributes.NestedFamily : fieldAttrs |= FieldAttributes.Family; break;
          case TypeAttributes.NestedFamORAssem : fieldAttrs |= FieldAttributes.FamORAssem; break;
          case TypeAttributes.NestedPrivate : fieldAttrs |= FieldAttributes.Private; break;
          default: fieldAttrs |= FieldAttributes.Public; break;
        }
        ScriptObject enclScope = this.enclosingScope;
        while (enclScope is BlockScope) enclScope = enclScope.GetParent();
        if (!(enclScope is GlobalScope) && !(enclScope is PackageScope) && !(enclScope is ClassScope)){
          isStatic = false;
          if (this is EnumDeclaration)
            this.context.HandleError(JSError.EnumNotAllowed);
          else
            this.context.HandleError(JSError.ClassNotAllowed);
        }
        if (isStatic) fieldAttrs |= FieldAttributes.Static;
        if (this.enclosingScope is ActivationObject){
          if (this.enclosingScope is ClassScope){
            if (this.name == ((ClassScope)this.enclosingScope).name){
              context.HandleError(JSError.CannotUseNameOfClass);
              this.name = this.name + " nested class";
            }
          }
          this.ownField = ((ActivationObject)this.enclosingScope).AddNewField(this.name, this.classob, fieldAttrs);
          if (this.ownField is JSLocalField)
            ((JSLocalField)this.ownField).isDefined = true;
        }else
          this.ownField = ((StackFrame)this.enclosingScope).AddNewField(this.name, this.classob, fieldAttrs);
        this.ownField.originalContext = id.context;
      }
      
      internal override Object Evaluate(){
        this.Globals.ScopeStack.GuardedPush(this.classob);
        try{
          this.body.EvaluateStaticVariableInitializers();
        }finally{
          this.Globals.ScopeStack.Pop();
        }
        return new Completion();
      }

      // generate the field containing the expando hashtable and the default indexer      
      private void GenerateGetEnumerator(){
        TypeBuilder classwriter = this.classob.GetTypeBuilder();
        MethodBuilder getEnumerator = classwriter.DefineMethod("get enumerator", MethodAttributes.Private|MethodAttributes.Virtual, Typeob.IEnumerator, null);
        ILGenerator il = getEnumerator.GetILGenerator();
        il.Emit(OpCodes.Ldarg_0);
        il.Emit(OpCodes.Call, this.getHashTableMethod);
        il.Emit(OpCodes.Call, CompilerGlobals.hashTableGetEnumerator);
        il.Emit(OpCodes.Ret);
        classwriter.DefineMethodOverride(getEnumerator, CompilerGlobals.getEnumeratorMethod);
      }

      // generate the field containing the expando hashtable and the default indexer      
      private void GetExpandoFieldGetter(TypeBuilder classwriter){
        if (this.expandoItemProp == null){
          this.expandoItemProp = classwriter.DefineProperty("Item", PropertyAttributes.None, Typeob.Object, new Type[]{Typeob.String});
     
          // if it is an expando, generate a SimpleHashtable field called 'expando table'
          FieldInfo expandoTableField = classwriter.DefineField("expando table", Typeob.SimpleHashtable, FieldAttributes.Private);

          // generate code for a hashtable getter. It will initialize the hashtable if null
          this.getHashTableMethod = classwriter.DefineMethod("get expando table", MethodAttributes.Private, Typeob.SimpleHashtable, null);
          ILGenerator il = this.getHashTableMethod.GetILGenerator();
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Ldfld, expandoTableField);
          il.Emit(OpCodes.Ldnull);
          Label endif = il.DefineLabel();
          il.Emit(OpCodes.Bne_Un_S, endif);
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Ldc_I4_8);
          il.Emit(OpCodes.Newobj, CompilerGlobals.hashtableCtor);
          il.Emit(OpCodes.Stfld, expandoTableField);
          il.MarkLabel(endif);
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Ldfld, expandoTableField);
          il.Emit(OpCodes.Ret);
        }
      }
      
      // can be called from outside this class (JSExpandoIndexerMethod)
      internal MethodInfo GetExpandoIndexerGetter(){
        if (this.getItem == null){
          TypeBuilder classwriter = this.classob.GetTypeBuilder();
          this.GetExpandoFieldGetter(classwriter);
          // create the getter
          this.getItem = classwriter.DefineMethod("get_Item", MethodAttributes.Public|MethodAttributes.SpecialName, Typeob.Object, new Type[]{Typeob.String});
          ILGenerator il = this.getItem.GetILGenerator();
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Call, this.getHashTableMethod);
          il.Emit(OpCodes.Ldarg_1);
          il.Emit(OpCodes.Call, CompilerGlobals.hashtableGetItem);
          il.Emit(OpCodes.Dup);
          Label endif = il.DefineLabel();
          il.Emit(OpCodes.Brtrue_S, endif);
          il.Emit(OpCodes.Pop);
          il.Emit(OpCodes.Ldsfld, CompilerGlobals.missingField);
          il.MarkLabel(endif);
          il.Emit(OpCodes.Ret);
          this.expandoItemProp.SetGetMethod(this.getItem);
        }
        return this.getItem;
      }
      
      // can be called from outside this class (JSExpandoIndexerMethod)
      internal MethodInfo GetExpandoIndexerSetter(){
        if (this.setItem == null){
          TypeBuilder classwriter = this.classob.GetTypeBuilder();
          this.GetExpandoFieldGetter(classwriter);
          // create the setter
          this.setItem = classwriter.DefineMethod("set_Item", MethodAttributes.Public|MethodAttributes.SpecialName, Typeob.Void, new Type[2]{Typeob.String, Typeob.Object});
          ILGenerator il = this.setItem.GetILGenerator();
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Call, this.getHashTableMethod);
          il.Emit(OpCodes.Ldarg_2);
          il.Emit(OpCodes.Ldsfld, CompilerGlobals.missingField);
          Label endif = il.DefineLabel();
          il.Emit(OpCodes.Beq_S, endif);
          il.Emit(OpCodes.Ldarg_1);
          il.Emit(OpCodes.Ldarg_2);
          il.Emit(OpCodes.Call, CompilerGlobals.hashtableSetItem);
          il.Emit(OpCodes.Ret);
          il.MarkLabel(endif);
          il.Emit(OpCodes.Ldarg_1);
          il.Emit(OpCodes.Call, CompilerGlobals.hashtableRemove);
          il.Emit(OpCodes.Ret);
          this.expandoItemProp.SetSetMethod(this.setItem);
        }
        return this.setItem;
      }
      
      private void GetExpandoDeleteMethod(){
        TypeBuilder tb = this.classob.GetTypeBuilder();
        MethodBuilder mb = this.deleteOpMethod = tb.DefineMethod("op_Delete", MethodAttributes.Public|MethodAttributes.Static|MethodAttributes.SpecialName, 
          Typeob.Boolean, new Type[2]{tb, Typeob.ArrayOfObject});
        ParameterBuilder pb = mb.DefineParameter(2, (ParameterAttributes)0, null);
        pb.SetCustomAttribute(new CustomAttributeBuilder(Typeob.ParamArrayAttribute.GetConstructor(Type.EmptyTypes), new Object[]{}));
        ILGenerator il = mb.GetILGenerator();
        il.Emit(OpCodes.Ldarg_0);
        il.Emit(OpCodes.Call, this.getHashTableMethod);
        il.Emit(OpCodes.Ldarg_1);
        il.Emit(OpCodes.Dup);
        il.Emit(OpCodes.Ldlen);
        il.Emit(OpCodes.Ldc_I4_1);
        il.Emit(OpCodes.Sub);
        il.Emit(OpCodes.Ldelem_Ref);
        il.Emit(OpCodes.Call, CompilerGlobals.hashtableRemove);
        il.Emit(OpCodes.Ldc_I4_1);
        il.Emit(OpCodes.Ret);
      }
      
      private String GetFullNameFor(MemberInfo supMem){
        String supMemberName;
        if (supMem is JSField)
          supMemberName = ((JSField)supMem).GetClassFullName();
        else if (supMem is JSConstructor)
          supMemberName = ((JSConstructor)supMem).GetClassFullName();
        else if (supMem is JSMethod)
          supMemberName = ((JSMethod)supMem).GetClassFullName();
        else if (supMem is JSProperty)
          supMemberName = ((JSProperty)supMem).GetClassFullName();
        else if (supMem is JSWrappedProperty)
          supMemberName = ((JSWrappedProperty)supMem).GetClassFullName();
        else
          supMemberName = ((MemberInfo)supMem).DeclaringType.FullName;
        return supMemberName + "." + ((MemberInfo)supMem).Name;
      }

      internal MemberInfo[] GetInterfaceMember(String name){
        this.PartiallyEvaluate();
        MemberInfo[] result;
        if (this.isInterface){
          result = this.classob.GetMember(name, BindingFlags.Public|BindingFlags.Instance|BindingFlags.DeclaredOnly);
          if (result != null && result.Length > 0) return result;
        }
        foreach (TypeExpression ifaceExpr in this.interfaces){
          IReflect ir = ifaceExpr.ToIReflect();
          result = ir.GetMember(name, BindingFlags.Public|BindingFlags.Instance|BindingFlags.DeclaredOnly);
          if (result != null && result.Length > 0) return result;
        }
        return new MemberInfo[0];
      }

      private void GetIRForSuperType(){
        IReflect supIR = this.superIR = Typeob.Object;
        if (this.superTypeExpression != null){
          this.superTypeExpression.PartiallyEvaluate();
          supIR = this.superIR = this.superTypeExpression.ToIReflect();
        }
        Type supType = supIR as Type;
        if (supType != null){
          if (supType.IsSealed || supType.IsInterface || supType == Typeob.ValueType || supType == Typeob.ArrayObject){
            if (this.superTypeExpression.Evaluate() is Namespace)
              this.superTypeExpression.context.HandleError(JSError.NeedType);
            else
              this.superTypeExpression.context.HandleError(JSError.TypeCannotBeExtended, supType.FullName);
            this.superTypeExpression = null;
            this.superIR = Typeob.Object;
          }else if (Typeob.INeedEngine.IsAssignableFrom(supType))
            this.needsEngine = false;
        }else if (supIR is ClassScope){
          if (((ClassScope)supIR).owner.IsASubClassOf(this)){
            this.superTypeExpression.context.HandleError(JSError.CircularDefinition);
            this.superTypeExpression = null;
            this.superIR = Typeob.Object;
          }else{
            this.needsEngine = false;
            this.superClass = ((ClassScope)supIR).owner;
            if ((this.superClass.attributes & TypeAttributes.Sealed) != 0){
              this.superTypeExpression.context.HandleError(JSError.TypeCannotBeExtended, this.superClass.name);
              this.superClass.attributes &= ~TypeAttributes.Sealed;
              this.superTypeExpression = null;
            }else if (this.superClass.isInterface){
              this.superTypeExpression.context.HandleError(JSError.TypeCannotBeExtended, this.superClass.name);
              this.superIR = Typeob.Object;
              this.superTypeExpression = null;
            }
          }
        }else{
          this.superTypeExpression.context.HandleError(JSError.TypeCannotBeExtended);
          this.superIR = Typeob.Object;
          this.superTypeExpression = null;
        }
      }
        
      private void GetStartIndexForEachName(){
        SimpleHashtable firstIndex = new SimpleHashtable(32);
        String lastName = null;
        for (int i = 0, n = this.superMembers.Length; i < n; i++){
          String name = ((MemberInfo)this.superMembers[i]).Name;
          if (name != lastName)
            firstIndex[lastName = name] = i;
        }
        this.firstIndex = firstIndex;
      }

      internal ConstructorInfo GetSuperConstructor(IReflect[] argIRs){
        Object val = null;
        if (this.superTypeExpression != null)
          val = this.superTypeExpression.Evaluate();
        else
          val = Typeob.Object;
        if (val is ClassScope)
          return JSBinder.SelectConstructor(((ClassScope)val).constructors, argIRs);
        else
          return JSBinder.SelectConstructor(((Type)val).GetConstructors(BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance), argIRs);
      }

      private void GetSuperTypeMembers(){
        SuperTypeMembersSorter sorter = new SuperTypeMembersSorter();
        IReflect ir = this.superIR;
        //Add members on superclasses
        while (ir != null){
          sorter.Add(ir.GetMembers(BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly));
          if (ir is Type){
            ir = ((Type)ir).BaseType;
          }else{
            ir = ((ClassScope)ir).GetSuperType();
          }
        }
        //Add any implicit interfaces to the end of the explicit list of interfaces
        ArrayList implicitInterfaces = new ArrayList();
        int n = this.interfaces.Length;
        IReflect[] explicitInterfaces = new IReflect[n];
        for (int i = 0; i < n; i++){
          IReflect iface = explicitInterfaces[i] = this.interfaces[i].ToIReflect();
          Type t = iface as Type;
          bool isInterface;
          if (t != null) 
            isInterface = t.IsInterface;
          else{
            ClassScope csc = (ClassScope)iface;
            isInterface = csc.owner.isInterface;
          }
          if (!isInterface) this.interfaces[i].context.HandleError(JSError.NeedInterface);
        }
        foreach (IReflect iface in explicitInterfaces)
          this.AddImplicitInterfaces(iface, explicitInterfaces, implicitInterfaces);
        for (int i = 0; i < implicitInterfaces.Count; i++){
          IReflect iface = (IReflect)implicitInterfaces[i];
          this.AddImplicitInterfaces(iface, explicitInterfaces, implicitInterfaces);
        }
        int m = implicitInterfaces.Count;
        if (m > 0){
          TypeExpression[] newInterfaces = new TypeExpression[n + m];
          for (int i = 0; i < n; i++) newInterfaces[i] = this.interfaces[i];
          for (int i = 0; i < m; i++) newInterfaces[i+n] = new TypeExpression(new ConstantWrapper(implicitInterfaces[i], null));
          this.interfaces = newInterfaces;
        }
        //Add members on interfaces implemented by the class. Also check for circular definition;
        foreach (TypeExpression ifaceExpr in this.interfaces){
          ClassScope ifcsc = ifaceExpr.ToIReflect() as ClassScope;
          if (ifcsc != null && ifcsc.owner.ImplementsInterface(this.classob)){
            this.context.HandleError(JSError.CircularDefinition);
            this.interfaces = new TypeExpression[0];
            break;
          }
          sorter.Add(ifaceExpr.ToIReflect().GetMembers(BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance));
        }
        //Add unimplemented members on interfaces implemented by abstract superclasses
        ir = this.superIR;
        while (ir != null){
          Type type = ir as Type;
          if (type != null){
            if (!type.IsAbstract) break;
            Class.GetUnimplementedInferfaceMembersFor(type, sorter);
            ir = type.BaseType;
          }else{
            ClassScope csc = (ClassScope)ir;
            if (!csc.owner.isAbstract) break;
            csc.owner.GetUnimplementedInferfaceMembers(sorter);
            ir = null;
          }
        }
        this.superMembers = sorter.GetMembers();
      }

      internal TypeBuilder GetTypeBuilder(){
        return (TypeBuilder)this.GetTypeBuilderOrEnumBuilder();
      }
      
      internal virtual Type GetTypeBuilderOrEnumBuilder(){
        if (this.classob.classwriter != null) return this.classob.classwriter;
        if (!this.isAlreadyPartiallyEvaluated) this.PartiallyEvaluate();
        Type superType = null;
        if (this.superTypeExpression != null)
          superType = this.superTypeExpression.ToType();
        else
          superType = this.isInterface ? null : Typeob.Object;
        int offset = (this.needsEngine ? 1 : 0) + (this.generateCodeForExpando ? 1 : 0);
        int n = this.interfaces.Length+offset;
        Type[] interfaces = new Type[n];
        for (int i = offset; i < n; i++)
          interfaces[i] = this.interfaces[i-offset].ToType();
        if (this.needsEngine)
          interfaces[--offset] = Typeob.INeedEngine;
        if (this.generateCodeForExpando)
          interfaces[--offset] = Typeob.IEnumerable;
        
        TypeBuilder result;
        if (this.enclosingScope is ClassScope){
          if ((result = (TypeBuilder)this.classob.classwriter) == null){
            TypeBuilder enclosingClass = ((ClassScope)this.enclosingScope).owner.GetTypeBuilder();
            if (this.classob.classwriter != null) return this.classob.classwriter;
            result = enclosingClass.DefineNestedType(this.name, this.attributes, superType, interfaces);
            this.classob.classwriter = result;
            if (!this.isStatic && !this.isInterface)
              this.classob.outerClassField = result.DefineField("outer class instance", enclosingClass, FieldAttributes.Private);
          }
        }else{
          String scopeName = ((ActivationObject)this.enclosingScope).GetName();
          if (scopeName == null){
            VsaEngine engine = this.context.document.engine;
            if (engine != null && engine.genStartupClass)
              scopeName = engine.RootNamespace;
          }
          if ((result = (TypeBuilder)this.classob.classwriter) == null){
            string typeName = this.name;
            if (scopeName != null)
              typeName = scopeName+"."+typeName;
            if (typeName.Length >= 1024) {
              this.context.HandleError(JSError.TypeNameTooLong, typeName);
              typeName = "bad type name "+badTypeNameCount.ToString(CultureInfo.InvariantCulture);
              badTypeNameCount++;
            }
            result = compilerGlobals.module.DefineType(typeName, this.attributes, superType, interfaces);
            this.classob.classwriter = result;
          }
        }
        // deal with custom attributes
        if (this.customAttributes != null){
          CustomAttributeBuilder[] custAtt = this.customAttributes.GetCustomAttributeBuilders(false);
          for (int j = 0; j < custAtt.Length; j++){
            result.SetCustomAttribute(custAtt[j]);
          }
        }
        if (this.clsCompliance == CLSComplianceSpec.CLSCompliant)
          result.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{true}));
        else if (this.clsCompliance == CLSComplianceSpec.NonCLSCompliant)
          result.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{false}));
        if (this.generateCodeForExpando)
          result.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.defaultMemberAttributeCtor, new Object[]{"Item"}));

        //Define the class fields        
        for (int i = 0, m = this.fields.Length; i < m; i++){
          JSMemberField field = this.fields[i];
          if (field.IsLiteral){
            Object value = field.value;
            if (value is JSProperty){
              JSProperty prop = (JSProperty)value;
              ParameterInfo[] pars = prop.GetIndexParameters();
              int np = pars.Length;
              Type[] ptypes = new Type[np];
              for (int j = 0; j < np; j++)
                ptypes[j] = pars[j].ParameterType;
              PropertyBuilder pb = prop.metaData = result.DefineProperty(field.Name, prop.Attributes, prop.PropertyType, ptypes);
              if (prop.getter != null){
                CustomAttributeList cal = ((JSFieldMethod)prop.getter).func.customAttributes;
                if (cal != null){
                  CustomAttributeBuilder[] custAttrs = cal.GetCustomAttributeBuilders(true);
                  foreach (CustomAttributeBuilder cb in custAttrs) pb.SetCustomAttribute(cb);
                }
                pb.SetGetMethod((MethodBuilder)prop.getter.GetMethodInfo(compilerGlobals));
              }
              if (prop.setter != null){
                CustomAttributeList cal = ((JSFieldMethod)prop.setter).func.customAttributes;
                if (cal != null){
                  CustomAttributeBuilder[] custAttrs = cal.GetCustomAttributeBuilders(true);
                  foreach (CustomAttributeBuilder cb in custAttrs) pb.SetCustomAttribute(cb);
                }
                pb.SetSetMethod((MethodBuilder)prop.setter.GetMethodInfo(compilerGlobals));
              }
              continue;
            }else if (value is ClassScope){
              ((ClassScope)value).GetTypeBuilderOrEnumBuilder();
            }else if (Convert.GetTypeCode(value) != TypeCode.Object){
              FieldBuilder fb = result.DefineField(field.Name, field.FieldType, field.Attributes); 
              fb.SetConstant(field.value);
              field.metaData = fb;
              field.WriteCustomAttribute(this.Engine.doCRS);
              continue;
            }else if (value is FunctionObject){
              FunctionObject func = (FunctionObject)value;
              if (func.isExpandoMethod){
                field.metaData = result.DefineField(field.Name, Typeob.ScriptFunction, field.Attributes&~(FieldAttributes.Literal|FieldAttributes.Static));
                func.isStatic = false;
              }
              if (this.isInterface)
                do{
                  func.GetMethodInfo(compilerGlobals);
                  field = field.nextOverload;
                  if (field == null) break;
                  func = (FunctionObject)field.value;
                }while(true);
            }
            continue;
          }
          field.metaData = result.DefineField(field.Name, field.FieldType, field.Attributes);
          field.WriteCustomAttribute(this.Engine.doCRS);
        }
        
        return result;
      }

      private void GetUnimplementedInferfaceMembers(SuperTypeMembersSorter sorter){
        for (int i = 0, n = this.superMembers.Length; i < n; i++){
          MethodInfo meth = this.superMembers[i] as MethodInfo;
          if (meth != null && meth.DeclaringType.IsInterface)
            sorter.Add(meth);
        }
        return;
      }
    
      private static void GetUnimplementedInferfaceMembersFor(Type type, SuperTypeMembersSorter sorter){
        foreach (Type iface in type.GetInterfaces()){
          InterfaceMapping imap = type.GetInterfaceMap(iface);
          MethodInfo[] interfaceMethods = imap.InterfaceMethods;
          MethodInfo[] targetMethods = imap.TargetMethods;
          for (int i = 0, n = interfaceMethods.Length; i < n; i++)
            if (targetMethods[i] == null || targetMethods[i].IsAbstract) sorter.Add(interfaceMethods[i]);
        }
      }
      
      internal bool ImplementsInterface(IReflect iface){
        foreach (TypeExpression t in this.interfaces){
          IReflect ir = t.ToIReflect();
          if (ir == iface) return true;
          if (ir is ClassScope && ((ClassScope)ir).ImplementsInterface(iface)) return true;
          if (ir is Type && iface is Type && ((Type)iface).IsAssignableFrom((Type)ir)) return true;
        }
        return false;
      }
      
      private bool IsASubClassOf(Class cl){
        if (this.superTypeExpression != null){
          this.superTypeExpression.PartiallyEvaluate();
          IReflect supertype = this.superTypeExpression.ToIReflect();
          if (supertype is ClassScope){
            Class superclass = ((ClassScope)supertype).owner;
            if (superclass == cl)
              return true;
            else
              return superclass.IsASubClassOf(cl);
          }
        }
        return false;
      }
      
      internal bool IsCustomAttribute(){
        this.GetIRForSuperType();
        if (this.superIR != Typeob.Attribute) return false;
        if (this.customAttributes == null) return false;
        this.customAttributes.PartiallyEvaluate();
        if (this.validOn == 0) return false;
        return true;
      }
      
      internal bool IsExpando(){
        if (this.hasAlreadyBeenAskedAboutExpando)
          return this.isExpando;
        if (this.customAttributes != null){
          this.customAttributes.PartiallyEvaluate();
          if (this.customAttributes.GetAttribute(Typeob.Expando) != null)
            this.generateCodeForExpando = this.isExpando = true;
        }
        bool superClassIsExpando = false;
        this.GetIRForSuperType();
        ClassScope csc = this.superIR as ClassScope;
        if (csc != null){
          csc.owner.PartiallyEvaluate();
          if (csc.owner.IsExpando())
            this.isExpando = superClassIsExpando = true;
        }else{
          if (CustomAttribute.IsDefined((Type)this.superIR, typeof(Expando), true))
            this.isExpando = superClassIsExpando = true;
        }
        this.hasAlreadyBeenAskedAboutExpando = true;
        if (this.generateCodeForExpando)
          this.CheckIfOKToGenerateCodeForExpando(superClassIsExpando);
        if (this.isExpando){
          this.classob.noExpando = false;
          return true;
        }
        return false;
      }
      
      private bool IsInTheSameCompilationUnit(MemberInfo member){
        return member is JSField || member is JSMethod;
      }

      private bool IsInTheSamePackage(MemberInfo member){
        if (member is JSMethod || member is JSField){
          PackageScope memberPackage = null;
          if (member is JSMethod)
            memberPackage = ((JSMethod)member).GetPackage();
          else if (member is JSConstructor)
            memberPackage = ((JSConstructor)member).GetPackage();
          else
            memberPackage = ((JSField)member).GetPackage();
          PackageScope currentPackage = this.classob.GetPackage();
          return currentPackage == memberPackage;
        }else
          return false;
      }

      internal bool IsStatic{
        get{
          return (this.isStatic || !(this.enclosingScope is ClassScope));
        }
      }

      protected bool NeedsToBeCheckedForCLSCompliance(){
        bool result = false;
        this.clsCompliance = CLSComplianceSpec.NotAttributed;
        if (this.customAttributes != null){
          CustomAttribute clsAttr = this.customAttributes.GetAttribute(Typeob.CLSCompliantAttribute);
          if (clsAttr != null){
            this.clsCompliance = clsAttr.GetCLSComplianceValue();
            result = this.clsCompliance == CLSComplianceSpec.CLSCompliant;
            this.customAttributes.Remove(clsAttr);
          }
        }
        if (this.clsCompliance == CLSComplianceSpec.CLSCompliant && !this.Engine.isCLSCompliant)
          this.context.HandleError(JSError.TypeAssemblyCLSCompliantMismatch);
        if (this.clsCompliance == CLSComplianceSpec.NotAttributed && (this.attributes & TypeAttributes.Public) != 0)
          result = Engine.isCLSCompliant;
        return result;
      }
        
      internal static bool ParametersMatch(ParameterInfo[] suppars, ParameterInfo[] pars){
        if (suppars.Length != pars.Length) return false;
        for (int i = 0, n = pars.Length; i < n; i++){
          IReflect spir = suppars[i] is ParameterDeclaration ? ((ParameterDeclaration)suppars[i]).ParameterIReflect : suppars[i].ParameterType;
          IReflect pir = pars[i] is ParameterDeclaration ? ((ParameterDeclaration)pars[i]).ParameterIReflect : pars[i].ParameterType;
          if (!pir.Equals(spir)) 
            return false;
        }
        return true;
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.isAlreadyPartiallyEvaluated) return this;
        this.isAlreadyPartiallyEvaluated = true;
        this.IsExpando(); //Evaluate the custom attributes, run up the inheritance chain and add expando property if necessary
        this.classob.SetParent(new WithObject(this.enclosingScope, this.superIR, true));
        Globals.ScopeStack.Push(this.classob);
        try{
          this.body.PartiallyEvaluate();
          if (this.implicitDefaultConstructor != null)
            this.implicitDefaultConstructor.PartiallyEvaluate();
        }finally{
          Globals.ScopeStack.Pop();
        }
        foreach (JSMemberField field in this.fields) field.CheckOverloadsForDuplicates();
        this.CheckIfValidExtensionOfSuperType();
        this.CheckThatAllAbstractSuperClassMethodsAreImplemented();
        return this;
      }
      
      private void SetAccessibility(FieldAttributes attributes){
        FieldAttributes accessibility = attributes & FieldAttributes.FieldAccessMask;
        if (this.enclosingScope is ClassScope){
          if (accessibility == FieldAttributes.Public)
            this.attributes |= TypeAttributes.NestedPublic;
          else if (accessibility == FieldAttributes.Family)
            this.attributes |= TypeAttributes.NestedFamily;
          else if (accessibility == FieldAttributes.Assembly)
            this.attributes |= TypeAttributes.NestedAssembly;
          else if (accessibility == FieldAttributes.Private)
            this.attributes |= TypeAttributes.NestedPrivate;
          else if (accessibility == FieldAttributes.FamORAssem)
            this.attributes |= TypeAttributes.NestedFamORAssem;
          else
            this.attributes |= TypeAttributes.NestedPublic;
        }else{
          if (accessibility == FieldAttributes.Public || 0 == (int)accessibility)
            this.attributes |= TypeAttributes.Public;
        }
      }
      
      private void SetupConstructors(){          
        MemberInfo[] consmem = classob.GetMember(this.name, 
          BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly); //See if the class has a member with the name of the class
        if (consmem == null){
          this.AllocateImplicitDefaultConstructor();
          FieldInfo field = this.classob.AddNewField(this.name, this.implicitDefaultConstructor, FieldAttributes.Literal);
          this.classob.constructors = new ConstructorInfo[]{new JSConstructor(this.implicitDefaultConstructor)};
        }else{
          MemberInfo cmem0 = null;
          foreach (MemberInfo cmem in consmem){
            if (cmem is JSFieldMethod){
              FunctionObject cons = ((JSFieldMethod)cmem).func;
              if (cmem0 == null) cmem0 = cmem;
              if (cons.return_type_expr != null)
                cons.return_type_expr.context.HandleError(JSError.ConstructorMayNotHaveReturnType);
              if ((cons.attributes & MethodAttributes.Abstract) != 0 || (cons.attributes & MethodAttributes.Static) != 0){
                cons.isStatic = false;
                JSVariableField f = (JSVariableField)((JSFieldMethod)cmem).field;
                f.attributeFlags &= ~FieldAttributes.Static;
                f.originalContext.HandleError(JSError.NotValidForConstructor);
              }
              cons.return_type_expr = new TypeExpression(new ConstantWrapper(Typeob.Void, context));
              cons.own_scope.AddReturnValueField();
            }
            //The else case can only happen if there is an error in the source code. Messages are generated elsewhere.
          }
          if (cmem0 != null)
            this.classob.constructors = ((JSMemberField)((JSFieldMethod)cmem0).field).GetAsConstructors(this.classob);
          else{
            this.AllocateImplicitDefaultConstructor();
            this.classob.constructors = new ConstructorInfo[]{new JSConstructor(this.implicitDefaultConstructor)};
          }
        }
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.GetTypeBuilderOrEnumBuilder();
        this.TranslateToCOMPlusClass();
        Debug.Assert(rtype == Typeob.Void);
        Object tok = ((JSVariableField)this.ownField).GetMetaData();
        if (tok != null){
          //This case is here to make classes defined in the global scope accessible to eval
          il.Emit(OpCodes.Ldtoken, this.classob.classwriter);
          il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
          if (tok is LocalBuilder)
            il.Emit(OpCodes.Stloc, (LocalBuilder)tok);
          else
            il.Emit(OpCodes.Stsfld, (FieldInfo)tok);
        }
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
      }

      private void EmitUsingNamespaces(ILGenerator il){
        if (this.body.Engine.GenerateDebugInfo){
          ScriptObject ns = this.enclosingScope;
          while (ns != null){
            if (ns is PackageScope)
              il.UsingNamespace(((PackageScope)ns).name);
            else if (ns is WrappedNamespace && !((WrappedNamespace)ns).name.Equals(""))
              il.UsingNamespace(((WrappedNamespace)ns).name);
            ns = ns.GetParent();
          }
        }
      }

      private void TranslateToCOMPlusClass(){
        if (this.isCooked) return;
        this.isCooked = true;
        if (this is EnumDeclaration){
          if (!(this.enclosingScope is ClassScope))
            this.TranslateToCreateTypeCall();
          return;
        }
        if (this.superClass != null)
          this.superClass.TranslateToCOMPlusClass();
        for (int i = 0, n = this.interfaces.Length; i < n; i++){
          IReflect iface = this.interfaces[i].ToIReflect();
          if (iface is ClassScope)
            ((ClassScope)iface).owner.TranslateToCOMPlusClass();
        }
        
        Globals.ScopeStack.Push(this.classob);
        TypeBuilder savedClasswriter = compilerGlobals.classwriter;
        compilerGlobals.classwriter = (TypeBuilder)this.classob.classwriter;  

        if (!this.isInterface){
          //Emit the class initializer
          ConstructorBuilder ccons = compilerGlobals.classwriter.DefineTypeInitializer();
          ILGenerator il = ccons.GetILGenerator();
          LocalBuilder engineLocal = null;
          if (this.classob.staticInitializerUsesEval) {
            engineLocal = il.DeclareLocal(Typeob.VsaEngine);
            il.Emit(OpCodes.Ldtoken, this.classob.GetTypeBuilder());
            ConstantWrapper.TranslateToILInt(il, 0);
            il.Emit(OpCodes.Newarr, Typeob.JSLocalField);
            if (this.Engine.PEFileKind == PEFileKinds.Dll) {
              il.Emit(OpCodes.Ldtoken, this.classob.GetTypeBuilder());
              il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngineWithType);
            }else
              il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngine);
            il.Emit(OpCodes.Dup);
            il.Emit(OpCodes.Stloc, engineLocal);           
            il.Emit(OpCodes.Call, CompilerGlobals.pushStackFrameForStaticMethod);
            il.BeginExceptionBlock();
          }
          ((Block)(this.body)).TranslateToILStaticInitializers(il);
          if (this.classob.staticInitializerUsesEval) {
            il.BeginFinallyBlock();
            il.Emit(OpCodes.Ldloc, engineLocal);
            il.Emit(OpCodes.Call, CompilerGlobals.popScriptObjectMethod);
            il.Emit(OpCodes.Pop);
            il.EndExceptionBlock();
          }
          il.Emit(OpCodes.Ret);

          // Emit all the namespaces used by the class initializer
          this.EmitUsingNamespaces(il);
        
          //Emit the instance field initializer (this also emits the method bodies)
          MethodBuilder initializer = compilerGlobals.classwriter.DefineMethod(".init", MethodAttributes.Private, Typeob.Void, new Type[0]);
          this.fieldInitializer = initializer;
          il = initializer.GetILGenerator();
          if (this.classob.instanceInitializerUsesEval) {
            il.Emit(OpCodes.Ldarg_0);
            ConstantWrapper.TranslateToILInt(il, 0);
            il.Emit(OpCodes.Newarr, Typeob.JSLocalField);
            il.Emit(OpCodes.Ldarg_0);
            il.Emit(OpCodes.Callvirt, CompilerGlobals.getEngineMethod);
            il.Emit(OpCodes.Call, CompilerGlobals.pushStackFrameForMethod);
            il.BeginExceptionBlock();
          }
          ((Block)(this.body)).TranslateToILInstanceInitializers(il);
          if (this.classob.instanceInitializerUsesEval) {
            il.BeginFinallyBlock();
            il.Emit(OpCodes.Ldarg_0);
            il.Emit(OpCodes.Callvirt, CompilerGlobals.getEngineMethod);
            il.Emit(OpCodes.Call, CompilerGlobals.popScriptObjectMethod);
            il.Emit(OpCodes.Pop);
            il.EndExceptionBlock();
          }
          il.Emit(OpCodes.Ret);

          // Emit all the namespaces used by the instance initializer
          this.EmitUsingNamespaces(il);
            
          //Emit the default constructor
          if (this.implicitDefaultConstructor != null)
            this.implicitDefaultConstructor.TranslateToIL(compilerGlobals);
        
          //Emit the expando code
          if (this.generateCodeForExpando){
            this.GetExpandoIndexerGetter();
            this.GetExpandoIndexerSetter();
            this.GetExpandoDeleteMethod();
            this.GenerateGetEnumerator();
          }
          this.EmitILForINeedEngineMethods();
        }        

        if (!(this.enclosingScope is ClassScope))
          this.TranslateToCreateTypeCall();
        compilerGlobals.classwriter = savedClasswriter;
        Globals.ScopeStack.Pop();
      }
      
      private void TranslateToCreateTypeCall(){
        if (this.cookedType != null) return;
        if (this is EnumDeclaration){
          EnumBuilder eb = this.classob.classwriter as EnumBuilder;
          if (eb != null)
            this.cookedType = eb.CreateType();
          else
            this.cookedType =((TypeBuilder)this.classob.classwriter).CreateType();
          return;
        }
        if (this.superClass != null)
          this.superClass.TranslateToCreateTypeCall();
        AppDomain currentDomain = System.Threading.Thread.GetDomain();
        ResolveEventHandler resolveHandler = new ResolveEventHandler(this.ResolveEnum);
        currentDomain.TypeResolve += resolveHandler;
        this.cookedType = ((TypeBuilder)this.classob.classwriter).CreateType();
        currentDomain.TypeResolve -= resolveHandler;
        foreach (JSMemberField field in this.fields){
          ClassScope csc = field.value as ClassScope;
          if (csc == null) continue;
          csc.owner.TranslateToCreateTypeCall();
        }
      }

      private Assembly ResolveEnum(Object sender, ResolveEventArgs args){
        FieldInfo f = this.classob.GetField(args.Name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Static);
        if (f != null && f.IsLiteral){     
          ClassScope csc = TypeReferences.GetConstantValue(f) as ClassScope;
          if (csc != null) csc.owner.TranslateToCreateTypeCall();
        }
        return this.compilerGlobals.assemblyBuilder;
      }
     
      internal override Context GetFirstExecutableContext(){
        return null;
      }
    }
}
