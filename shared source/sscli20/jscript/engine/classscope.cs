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
    
    internal sealed class ClassScope : ActivationObject, IComparable{
      internal String name;
      internal Type classwriter;
      internal Class owner;
      internal ConstructorInfo[] constructors;
      internal bool noExpando;
      internal PackageScope package;
      internal JSProperty itemProp; //Holds the expando property if present
      internal FieldInfo outerClassField; //Holds a reference to the outer class instance of a nested class instance
      internal bool inStaticInitializerCode; //Set this to true when partially evaluating the initializer of a static variable, false otherwise
      internal bool staticInitializerUsesEval;  // True if a static member initializer calls eval and thus needs the scope setup
      internal bool instanceInitializerUsesEval;  // True if in instance member initializer calls eval and thus needs the scope setup
      
      internal ClassScope(AST name, GlobalScope scope)
        : base(scope) { //The parent must be set to a proper value before the class can be used.
        this.name = name.ToString();
        this.engine = scope.engine;
        this.fast = scope.fast;
        this.noExpando = true;
        this.isKnownAtCompileTime = true;
        this.owner = null; //set by Class constructor
        this.constructors = new JSConstructor[0];
        ScriptObject enclosingScope = this.engine.ScriptObjectStackTop();
        while (enclosingScope is WithObject)
          enclosingScope = enclosingScope.GetParent();
        if (enclosingScope is ClassScope)
          this.package = ((ClassScope)enclosingScope).GetPackage();
        else if (enclosingScope is PackageScope)
          this.package = (PackageScope)enclosingScope;
        else
          this.package = null;
        this.itemProp = null;
        this.outerClassField = null;
        this.inStaticInitializerCode = false;
        this.staticInitializerUsesEval = false;
        this.instanceInitializerUsesEval = false;
      }
      
      internal void AddClassesFromInheritanceChain(String name, ArrayList result){
        IReflect ir = this;
        bool noMatch = true;
        while (ir is ClassScope){
          if (ir.GetMember(name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.DeclaredOnly).Length > 0){
            result.Add(ir);
            noMatch = false;
          }
          if (ir is ClassScope)
            ir = ((ClassScope)ir).GetSuperType();
          else
            ir = ((Type)ir).BaseType;
        }
        if (noMatch && ir is Type && ir.GetMember(name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance).Length > 0)
          result.Add(ir);
      }
      
      static internal ClassScope ScopeOfClassMemberInitializer(ScriptObject scope) {
        while (scope != null) {
          if (scope is FunctionScope)
            return null;
          ClassScope cscope = scope as ClassScope;
          if (cscope != null)
            return cscope;
          scope = scope.GetParent();
        }
        return null;
      }
      
      //A class is always less than its superclass
      public int CompareTo(Object ob){
        if (ob == this) return 0;
        if (ob is ClassScope && ((ClassScope)ob).IsSameOrDerivedFrom(this)) return 1;
        return -1;
      }
      
      protected override JSVariableField CreateField(String name, FieldAttributes attributeFlags, Object value){
        return new JSMemberField(this, name, value, attributeFlags);
      }

      internal Object FakeCallToTypeMethod(MethodInfo method, Object[] arguments, Exception e){
        ParameterInfo[] pars = method.GetParameters(); int n = pars.Length;
        Type[] parTypes = new Type[n]; for (int i = 0; i < n; i++) parTypes[i] = pars[i].ParameterType;
        MethodInfo fakeMethod = typeof(ClassScope).GetMethod(method.Name, parTypes);
        if (fakeMethod != null) return fakeMethod.Invoke(this, arguments);
        throw e;
      }

      public Object[] GetCustomAttributes(bool inherit){
        CustomAttributeList caList = this.owner.customAttributes;
        if (caList == null) return new Object[0]; else return (Object[])caList.Evaluate();
      }

      public ConstructorInfo[] GetConstructors(){
        return this.constructors;
      }

      public FieldInfo GetField(String name){
        return this.GetField(name, BindingFlags.Public|BindingFlags.Instance);
      }

      public MethodInfo GetMethod(String name){
        return this.GetMethod(name, BindingFlags.Public|BindingFlags.Instance);
      }

      public PropertyInfo GetProperty(String name){
        return this.GetProperty(name, BindingFlags.Public|BindingFlags.Instance);
      }

      internal override Object GetDefaultValue(PreferredType preferred_type){
        return this.GetFullName();
      }
      
      internal override void GetPropertyEnumerator(ArrayList enums, ArrayList objects){
      }

      internal String GetFullName(){
        PackageScope package = this.GetPackage();
        if (package != null)
          return package.GetName() + "." + this.name;
        else if (this.owner.enclosingScope is ClassScope)
          return ((ClassScope)this.owner.enclosingScope).GetFullName() + "." + this.name;
        else 
          return this.name;
      }     

      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        MemberInfoList mems = new MemberInfoList();
        FieldInfo field = (FieldInfo)(this.name_table[name]);
        if (field != null){
          if (field.IsPublic){
            if ((bindingAttr & BindingFlags.Public) == 0) goto parent;
          }else{
            if ((bindingAttr & BindingFlags.NonPublic) == 0) goto parent;
          }
          if (field.IsLiteral){
            Object val = ((JSMemberField)field).value;
            if (val is FunctionObject){
              FunctionObject func = (FunctionObject)val;
              if (func.isConstructor)
                return new MemberInfo[0]; //Suppress constructors, they are awkward to handle. The compiler always obtains them via GetConstructor
              if (func.isExpandoMethod){
                if ((bindingAttr & BindingFlags.Instance) == 0) goto parent;
                mems.Add(field);
              }else
                ((JSMemberField)field).AddOverloadedMembers(mems, this, bindingAttr|BindingFlags.DeclaredOnly);
              goto parent;
            }else if (val is JSProperty){
              JSProperty prop = (JSProperty)val;
              MethodInfo getterOrSetter = prop.getter != null ? prop.getter : prop.setter;
              if (getterOrSetter.IsStatic){
                if ((bindingAttr & BindingFlags.Static) == 0) goto parent;
              }else{
                if ((bindingAttr & BindingFlags.Instance) == 0) goto parent;
              }
              mems.Add(prop);
              goto parent;
            }else if (val is ClassScope){
              if ((bindingAttr & BindingFlags.Instance) != 0){
                if (!((ClassScope)val).owner.isStatic){
                  mems.Add(field);
                  goto parent;
                }
              }
            }
          }
          if (field.IsStatic){
            if ((bindingAttr & BindingFlags.Static) == 0) goto parent;
          }else{
            if ((bindingAttr & BindingFlags.Instance) == 0) goto parent;
          }
          mems.Add(field);
        }
      parent:
        if (this.owner != null && this.owner.isInterface && ((bindingAttr & BindingFlags.DeclaredOnly) == 0))
          return this.owner.GetInterfaceMember(name);
        if (this.parent != null && ((bindingAttr & BindingFlags.DeclaredOnly) == 0)){ //Add any members found on the superclass
          MemberInfo[] supMembers = this.parent.GetMember(name, bindingAttr);
          if (supMembers != null)
            foreach (MemberInfo mem in supMembers){
              if (mem.MemberType == MemberTypes.Field){
                field = (FieldInfo)mem;
                if (!field.IsStatic && !field.IsLiteral && !(field is JSWrappedField))
                  field = new JSWrappedField(field, this.parent);
                mems.Add(field);
              }else
                mems.Add(ScriptObject.WrapMember(mem, this.parent));
            }
        }
        return mems.ToArray();
      }

      internal bool HasInstance(Object ob){
        if (!(ob is JSObject))
          return false;
        ScriptObject parent = ((JSObject)ob).GetParent();
        while (parent != null){
          if (parent == this)
            return true;
          else if (parent is WithObject){
            Object wob = ((WithObject)parent).contained_object;
            if (wob == this) return true;
          }
          parent = parent.GetParent();
        }
        return false;  
      }
      
      internal JSMemberField[] GetMemberFields(){
        int n = this.field_table.Count;
        JSMemberField[] result = new JSMemberField[n];
        for (int i = 0; i < n; i++) result[i] = (JSMemberField)this.field_table[i];
        return result;
      }

      //Called when class is used as a base class. Need to give the same information as the type would, once this is compiled to IL.
      //However, bindingAttr is ignored because this method is never legitimately called by something other than the compiler.
      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){
        MemberInfoList mems = new MemberInfoList();
        IEnumerator enu = this.field_table.GetEnumerator();
        while (enu.MoveNext()){
          FieldInfo field = (FieldInfo)enu.Current;
          if (field.IsLiteral && field is JSMemberField){
            Object val = null;
            if ((val = ((JSMemberField)field).value) is FunctionObject){
              if (((FunctionObject)val).isConstructor)
                continue; //Suppress constructors, they are awkward to handle. The compiler always obtains them via GetConstructor
              ((JSMemberField)field).AddOverloadedMembers(mems, this, 
                 BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly);
            }else if (val is JSProperty){
              mems.Add((MemberInfo)val);
            }else
              mems.Add(field);
          }else
            mems.Add(field);
        }
        if (this.parent != null) //Add any members found on the prototype
          mems.AddRange(this.parent.GetMembers(bindingAttr));
        return mems.ToArray();
      }

      internal override String GetName(){
        return this.name;
      }
      
      internal Type GetBakedSuperType(){
        this.owner.PartiallyEvaluate();
        if (this.owner is EnumDeclaration)
          return ((EnumDeclaration)this.owner).baseType.ToType();
        Object supertype = ((WithObject)this.parent).contained_object;
        if (supertype is ClassScope)
          return ((ClassScope)supertype).GetBakedSuperType();
        else if (supertype is Type)
          return (Type)supertype;
        else
          return Globals.TypeRefs.ToReferenceContext(supertype.GetType());
      }
      
      internal PackageScope GetPackage(){
        return this.package;
      }
      
      internal IReflect GetSuperType(){
        this.owner.PartiallyEvaluate();
        return (IReflect)((WithObject)this.parent).contained_object;
      }
      
      internal Type GetTypeBuilderOrEnumBuilder(){
        if (this.classwriter == null)
          this.classwriter = this.owner.GetTypeBuilderOrEnumBuilder();
        return this.classwriter;
      }
      
      internal TypeBuilder GetTypeBuilder(){
        return (TypeBuilder)this.GetTypeBuilderOrEnumBuilder();
      }

      internal IReflect GetUnderlyingTypeIfEnum(){
        if (this.owner is EnumDeclaration)
          return ((EnumDeclaration)this.owner.PartiallyEvaluate()).baseType.ToIReflect();
        else
          return this;
      }
      
      internal bool ImplementsInterface(IReflect iface){
        this.owner.PartiallyEvaluate();
        Object supertype = ((WithObject)this.parent).contained_object;
        if (supertype is ClassScope)
          return ((ClassScope)supertype).ImplementsInterface(iface) || this.owner.ImplementsInterface(iface);
        else if (supertype is Type && iface is Type)
          return ((Type)iface).IsAssignableFrom((Type)supertype) || this.owner.ImplementsInterface(iface);
        else
          return this.owner.ImplementsInterface(iface);
      }
      
      internal bool IsCLSCompliant(){
        this.owner.PartiallyEvaluate();
        TypeAttributes visibility = this.owner.attributes & TypeAttributes.VisibilityMask;
        if (visibility != TypeAttributes.Public && visibility != TypeAttributes.NestedPublic)
          return false;
        else if (this.owner.clsCompliance == CLSComplianceSpec.NotAttributed)
          return this.owner.Engine.isCLSCompliant;
        else if (this.owner.clsCompliance == CLSComplianceSpec.CLSCompliant)
          return true;
        else 
          return false;
      }
      
      internal bool IsNestedIn(ClassScope other, bool isStatic){
        if (this.parent == null)
          return false;
        this.owner.PartiallyEvaluate();
        if (this.owner.enclosingScope == other)
          return isStatic || !this.owner.isStatic;
        else if (this.owner.enclosingScope is ClassScope)
          return ((ClassScope)this.owner.enclosingScope).IsNestedIn(other, isStatic);
        else
          return false;
      }

      internal bool IsSameOrDerivedFrom(ClassScope other){
        if (this == other)
          return true;
        if (other.owner.isInterface)
          return this.ImplementsInterface(other);
        if (this.parent == null)
          return false;
        this.owner.PartiallyEvaluate();
        Object supertype = ((WithObject)this.parent).contained_object;
        if (supertype is ClassScope)
          return ((ClassScope)supertype).IsSameOrDerivedFrom(other);
        else
          return false;
      }
      
      internal bool IsSameOrDerivedFrom(Type other){
        if (this.owner.GetTypeBuilder() == other)
          return true;
        if (this.parent == null)
          return false;
        this.owner.PartiallyEvaluate();
        Object supertype = ((WithObject)this.parent).contained_object;
        if (supertype is ClassScope)
          return ((ClassScope)supertype).IsSameOrDerivedFrom(other);
        else
          return other.IsAssignableFrom((Type)supertype);
      }
      
      internal bool IsPromotableTo(Type other){
        Type st = this.GetBakedSuperType();
        if (other.IsAssignableFrom(st)) return true;
        if (other.IsInterface && this.ImplementsInterface(other)) return true;
        EnumDeclaration ed = this.owner as EnumDeclaration;
        if (ed != null && Convert.IsPromotableTo(ed.baseType.ToType(), other)) return true;
        return false;
      }
      
      internal bool ParentIsInSamePackage(){
        Object supertype = ((WithObject)this.parent).contained_object;
        if (supertype is ClassScope)
          return ((ClassScope)supertype).package == this.package;
        return false;
      }
    }
}
