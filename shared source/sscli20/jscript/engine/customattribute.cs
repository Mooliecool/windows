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
  using System.Collections.Generic;
  using System.Configuration.Assemblies;
  using System.Globalization;
  using System.Reflection;
  using System.Reflection.Emit;
  using System.Security.Permissions;
  
  internal sealed class CustomAttribute : AST{
    private AST ctor;
    private ASTList args;
    private AST target; //indicates what kind of member the custom attribute is being applied to
    internal Object type; // will contain either the Type or ClassScope for the custom attribute type
    private ArrayList positionalArgValues;
    private ArrayList namedArgFields;
    private ArrayList namedArgFieldValues;
    private ArrayList namedArgProperties;
    private ArrayList namedArgPropertyValues;
    internal bool raiseToPropertyLevel;
          
    internal CustomAttribute(Context context, AST func, ASTList args)
      : base(context) {
      this.ctor = func;
      this.args = args;
      this.target = null;
      this.type = null;
      this.positionalArgValues = new ArrayList();
      this.namedArgFields = new ArrayList();
      this.namedArgFieldValues = new ArrayList();
      this.namedArgProperties = new ArrayList();
      this.namedArgPropertyValues = new ArrayList();
      this.raiseToPropertyLevel = false;
    }

    //Check that custom attribute is applicable to its target
    private bool CheckIfTargetOK(Object caType){
      if (caType == null) return false;
      AttributeTargets validOn = (AttributeTargets)0;
      Type caTypeT = caType as Type;
      if (caTypeT != null){
        Object[] usage = CustomAttribute.GetCustomAttributes(caTypeT, typeof(AttributeUsageAttribute), true);
        validOn = ((AttributeUsageAttribute)usage[0]).ValidOn;
      }else
        validOn = ((ClassScope)caType).owner.validOn;
      Object target = this.target;
      Class c = target as Class;
      if (c != null){
        if (c.isInterface){
          if ((validOn & AttributeTargets.Interface) != 0) return true;
        }else if (c is EnumDeclaration){
          if ((validOn & AttributeTargets.Enum) != 0) return true;
        }else if ((validOn & AttributeTargets.Class) != 0){
          if (caTypeT == typeof(AttributeUsageAttribute)){
            //This is an attribute that describes a new attribute class
            //Set the validOn field of Class so that it becomes easier to check the usage of the new attribute
            if (this.positionalArgValues.Count > 0){
              Object par0 = this.positionalArgValues[0];
              if (par0 is AttributeTargets) c.validOn = (AttributeTargets)par0;
            }
            for (int i = 0, n = this.namedArgProperties.Count; i < n; i++){
              PropertyInfo prop = this.namedArgProperties[i] as PropertyInfo;
              if (prop.Name == "AllowMultiple") c.allowMultiple = (bool)this.namedArgPropertyValues[i];
            }
          }
          return true;
        }else if (caTypeT.FullName == "System.NonSerializedAttribute"){
          c.attributes &= ~TypeAttributes.Serializable;
          return false;
        }
        this.context.HandleError(JSError.InvalidCustomAttributeTarget, CustomAttribute.GetTypeName(caType));
        return false;
      }
      FunctionDeclaration fDecl = target as FunctionDeclaration;
      if (fDecl != null){
        if ((validOn & AttributeTargets.Property) != 0 && fDecl.enclosingProperty != null){
          if (fDecl.enclosingProperty.getter == null || ((JSFieldMethod)fDecl.enclosingProperty.getter).func == fDecl.func){
            this.raiseToPropertyLevel = true; return true;
          }else{
            this.context.HandleError(JSError.PropertyLevelAttributesMustBeOnGetter);
            return false;
          }
        }
        if ((validOn & AttributeTargets.Method) != 0 && fDecl.isMethod) return true;
        if ((validOn & AttributeTargets.Constructor) != 0 && fDecl.func.isConstructor) return true;
        this.context.HandleError(JSError.InvalidCustomAttributeTarget, CustomAttribute.GetTypeName(caType));
        return false;
      }
      if (target is VariableDeclaration || target is Constant){
        if ((validOn & AttributeTargets.Field) != 0) return true;
        this.context.HandleError(JSError.InvalidCustomAttributeTarget, CustomAttribute.GetTypeName(caType));
        return false;
      }
      if (target is AssemblyCustomAttributeList && (validOn & AttributeTargets.Assembly) != 0) return true;
      if (target == null && (validOn & AttributeTargets.Parameter) != 0) return true;
      this.context.HandleError(JSError.InvalidCustomAttributeTarget, CustomAttribute.GetTypeName(caType));
      return false;
    }

    private static ushort DaysSince2000(){
      return (ushort)(DateTime.Now - new DateTime(2000, 1, 1)).Days;
    }

    internal override Object Evaluate(){
      ConstructorInfo c = (ConstructorInfo)((Binding)this.ctor).member;
      ParameterInfo[] pars = c.GetParameters();
      int pn = pars.Length;
      for (int i = positionalArgValues.Count; i < pn; i++) positionalArgValues.Add(Convert.CoerceT(null, pars[i].ParameterType));
      Object[] pArgVals = new Object[pn]; positionalArgValues.CopyTo(0, pArgVals, 0, pn);
      Object ca = c.Invoke(BindingFlags.ExactBinding, null, pArgVals, null);
      for (int i = 0, n = this.namedArgProperties.Count; i < n; i++){
        JSProperty prop = this.namedArgProperties[i] as JSProperty;
        if (prop != null)
          prop.SetValue(ca, Convert.Coerce(this.namedArgPropertyValues[i], prop.PropertyIR()), null);
        else
          ((PropertyInfo)this.namedArgProperties[i]).SetValue(ca, this.namedArgPropertyValues[i], null);
      }
      for (int i = 0, n = this.namedArgFields.Count; i < n; i++){
        JSVariableField field = this.namedArgFields[i] as JSVariableField;
        if (field != null)
          field.SetValue(ca, Convert.Coerce(this.namedArgFieldValues[i], field.GetInferredType(null)));
        else
          ((FieldInfo)this.namedArgFields[i]).SetValue(ca, this.namedArgFieldValues[i]);
      }
      return ca;
    }
    
    internal CLSComplianceSpec GetCLSComplianceValue(){
      Debug.Assert(this.type == Typeob.CLSCompliantAttribute);
      return ((bool)this.positionalArgValues[0]) ? CLSComplianceSpec.CLSCompliant : CLSComplianceSpec.NonCLSCompliant;
    }

    private void ConvertClassScopesAndEnumWrappers(ArrayList vals){
      for (int i = 0, n = vals.Count; i < n; i++){
        ClassScope csc = vals[i] as ClassScope;
        if (csc != null){ vals[i] = csc.GetTypeBuilder(); continue;}
        EnumWrapper wrapper = vals[i] as EnumWrapper;
        if (wrapper != null) vals[i] = wrapper.ToNumericValue();
      }
    }

    private void ConvertFieldAndPropertyInfos(ArrayList vals){
      for (int i = 0, n = vals.Count; i < n; i++){
        JSField jsfld = vals[i] as JSField;
        if (jsfld != null){ vals[i] = jsfld.GetMetaData(); continue;}
        JSProperty jsprop = vals[i] as JSProperty;
        if (jsprop != null){ vals[i] = jsprop.metaData; continue;}
      }
    }

    internal CustomAttributeBuilder GetCustomAttribute(){
      ConstructorInfo c = (ConstructorInfo)((Binding)this.ctor).member;
      ParameterInfo[] pars = c.GetParameters();
      int pn = pars.Length;
      if (c is JSConstructor)
        c = ((JSConstructor)c).GetConstructorInfo(compilerGlobals);
      this.ConvertClassScopesAndEnumWrappers(this.positionalArgValues);
      this.ConvertClassScopesAndEnumWrappers(this.namedArgPropertyValues);
      this.ConvertClassScopesAndEnumWrappers(this.namedArgFieldValues);
      this.ConvertFieldAndPropertyInfos(this.namedArgProperties);
      this.ConvertFieldAndPropertyInfos(this.namedArgFields);
      for (int i = positionalArgValues.Count; i < pn; i++) positionalArgValues.Add(Convert.CoerceT(null, pars[i].ParameterType));
      Object[] pArgVals = new Object[pn]; positionalArgValues.CopyTo(0, pArgVals, 0, pn);
      PropertyInfo[] nArgProps = new PropertyInfo[namedArgProperties.Count]; namedArgProperties.CopyTo(nArgProps);
      Object[] nArgPVals = new Object[namedArgPropertyValues.Count]; namedArgPropertyValues.CopyTo(nArgPVals);
      FieldInfo[] nArgFields = new FieldInfo[namedArgFields.Count]; namedArgFields.CopyTo(nArgFields);
      Object[] nArgFVals = new Object[namedArgFieldValues.Count]; namedArgFieldValues.CopyTo(nArgFVals);
      return new CustomAttributeBuilder(c, pArgVals, nArgProps, nArgPVals, nArgFields, nArgFVals);
    }
    
    internal Object GetTypeIfAttributeHasToBeUnique(){
      Type t = this.type as Type;
      if (t != null){
        Object[] custAttrs = CustomAttribute.GetCustomAttributes(t, typeof(AttributeUsageAttribute), false);
        if (custAttrs.Length > 0 && !((AttributeUsageAttribute)custAttrs[0]).AllowMultiple) return t;
        return null;
      }
      if (!((ClassScope)this.type).owner.allowMultiple) return this.type;
      return null;
    }

    private static String GetTypeName(Object t){
      Type ty = t as Type;
      if (ty != null) return ty.FullName;
      return ((ClassScope)t).GetFullName();
    }

    internal bool IsExpandoAttribute(){ //Use only before partial evaluation has been done
      Lookup id = this.ctor as Lookup;
      return id != null && id.Name == "expando";
    }
    internal override AST PartiallyEvaluate(){
      this.ctor = this.ctor.PartiallyEvaluateAsCallable();

      //first weed out assignment expressions and use them as property initializers
      ASTList positionalArgs = new ASTList(this.args.context);
      ASTList namedArgs = new ASTList(this.args.context);
      for (int i = 0, m = this.args.count; i < m; i++){
        AST arg = this.args[i];
        Assign assign = arg as Assign;
        if (assign != null){
          assign.rhside = assign.rhside.PartiallyEvaluate();
          namedArgs.Append(assign);
        }else
          positionalArgs.Append(arg.PartiallyEvaluate());                    
      }

      int n = positionalArgs.count;
      IReflect[] argIRs = new IReflect[n];
      for (int i = 0; i < n; i++){
        AST arg = positionalArgs[i];
        // only accept ConstantWrappers
        if (arg is ConstantWrapper){
          Object argument = arg.Evaluate();
          if ((argIRs[i] = CustomAttribute.TypeOfArgument(argument)) != null){
            this.positionalArgValues.Add(argument);
            continue;
          }
        }else if (arg is ArrayLiteral && ((ArrayLiteral)arg).IsOkToUseInCustomAttribute()){
          argIRs[i] = Typeob.ArrayObject;
          this.positionalArgValues.Add(arg.Evaluate());
          continue;
        }
        arg.context.HandleError(JSError.InvalidCustomAttributeArgument);
        return null; // the custom attribute is not good and it will be ignored
      }

      //Get the custom attribute and the appropriate constructor (under the covers)
      this.type = this.ctor.ResolveCustomAttribute(positionalArgs, argIRs, this.target);
      if (this.type == null) return null;
      if (Convert.IsPromotableTo((IReflect)this.type, Typeob.CodeAccessSecurityAttribute)){
        this.context.HandleError(JSError.CannotUseStaticSecurityAttribute);
        return null;
      }
      

      //Coerce the positional arguments to the right type and supply default values for optional parameters
      ConstructorInfo c = (ConstructorInfo)((Binding)this.ctor).member;
      ParameterInfo[] parameters = c.GetParameters();
      int j = 0;
      int len = this.positionalArgValues.Count;
      foreach (ParameterInfo p in parameters){
        IReflect ir = p is ParameterDeclaration ? ((ParameterDeclaration)p).ParameterIReflect : p.ParameterType;
        if (j < len ){
          Object value = this.positionalArgValues[j];
          this.positionalArgValues[j] = Convert.Coerce(value, ir, value is ArrayObject);
          j++;
        }else{
          Object value;
          if (TypeReferences.GetDefaultParameterValue(p) == System.Convert.DBNull){
            value = Convert.Coerce(null, ir);
          }else
            value = TypeReferences.GetDefaultParameterValue(p);
          this.positionalArgValues.Add(value);
        }
      }
     
      //Check validity of property/field initializers
      for (int i = 0, m = namedArgs.count; i < m; i++){
        Assign assign = (Assign)namedArgs[i];
        if (assign.lhside is Lookup && 
            (assign.rhside is ConstantWrapper ||
             (assign.rhside is ArrayLiteral && ((ArrayLiteral)assign.rhside).IsOkToUseInCustomAttribute()))){
          Object value = assign.rhside.Evaluate();
          IReflect argType = null;
          if (value is ArrayObject || ((argType = CustomAttribute.TypeOfArgument(value)) != null && argType != Typeob.Object)){
            String name = ((Lookup)assign.lhside).Name;
            MemberInfo [] members = ((IReflect)this.type).GetMember(name, BindingFlags.Public|BindingFlags.Instance);
            if (members == null || members.Length == 0){
              assign.context.HandleError(JSError.NoSuchMember);
              return null;
            } 
            if (members.Length == 1){
              MemberInfo member = members[0];
              if (member is FieldInfo){
                FieldInfo fieldInfo = (FieldInfo)member;
                if (!fieldInfo.IsLiteral && !fieldInfo.IsInitOnly){
                  try{
                    IReflect ir = fieldInfo is JSVariableField ? ((JSVariableField)fieldInfo).GetInferredType(null) : fieldInfo.FieldType;
                    value = Convert.Coerce(value, ir, value is ArrayObject);
                    this.namedArgFields.Add(member);
                    this.namedArgFieldValues.Add(value);
                    continue;
                  }catch(JScriptException){
                    assign.rhside.context.HandleError(JSError.TypeMismatch);
                    return null; // the custom attribute is not good and it will be ignored
                  }
                }
              }else if (member is PropertyInfo){
                PropertyInfo propertyInfo = (PropertyInfo)member;
                MethodInfo setMethodInfo = JSProperty.GetSetMethod(propertyInfo, false);
                if (setMethodInfo != null){
                  ParameterInfo [] paramInfo = setMethodInfo.GetParameters();
                  if (paramInfo != null && paramInfo.Length == 1){
                    try{
                      IReflect ir = paramInfo[0] is ParameterDeclaration ? ((ParameterDeclaration)paramInfo[0]).ParameterIReflect : paramInfo[0].ParameterType;
                      value = Convert.Coerce(value, ir, value is ArrayObject);
                      this.namedArgProperties.Add(member);
                      this.namedArgPropertyValues.Add(value);
                    }catch(JScriptException){
                      assign.rhside.context.HandleError(JSError.TypeMismatch);
                      return null; // the custom attribute is not good and it will be ignored
                    }
                    continue;
                  }
                }
              }
            }
          }
        }
        assign.context.HandleError(JSError.InvalidCustomAttributeArgument);
        return null;
      }

      if (!this.CheckIfTargetOK(this.type))
        return null; //Ignore attribute

      //Consume and discard assembly name attributes
      try{
        Type ty = this.type as Type;
        if (ty != null && this.target is AssemblyCustomAttributeList){
          if (ty.FullName == "System.Reflection.AssemblyAlgorithmIdAttribute"){
            if (this.positionalArgValues.Count > 0)
              this.Engine.Globals.assemblyHashAlgorithm = (AssemblyHashAlgorithm)Convert.CoerceT(this.positionalArgValues[0], typeof(AssemblyHashAlgorithm));
            return null;
          }
          if (ty.FullName == "System.Reflection.AssemblyCultureAttribute"){
            if (this.positionalArgValues.Count > 0){
              String cultureId = Convert.ToString(this.positionalArgValues[0]);
              if (this.Engine.PEFileKind != PEFileKinds.Dll && cultureId.Length > 0){
                this.context.HandleError(JSError.ExecutablesCannotBeLocalized);
                return null;
              }
              this.Engine.Globals.assemblyCulture = new CultureInfo(cultureId);
            }
            return null;
          }
          if (ty.FullName == "System.Reflection.AssemblyDelaySignAttribute"){
            if (this.positionalArgValues.Count > 0)
              this.Engine.Globals.assemblyDelaySign = Convert.ToBoolean(this.positionalArgValues[0], false);
            return null;
          }
          if (ty.FullName == "System.Reflection.AssemblyFlagsAttribute"){
            if (this.positionalArgValues.Count > 0)
              this.Engine.Globals.assemblyFlags = (AssemblyFlags)(uint)Convert.CoerceT(this.positionalArgValues[0], typeof(uint));
            return null;
          }
          if (ty.FullName == "System.Reflection.AssemblyKeyFileAttribute"){
            if (this.positionalArgValues.Count > 0){
              this.Engine.Globals.assemblyKeyFileName = Convert.ToString(this.positionalArgValues[0]);
              this.Engine.Globals.assemblyKeyFileNameContext = this.context;
              if (this.Engine.Globals.assemblyKeyFileName != null && this.Engine.Globals.assemblyKeyFileName.Length == 0)
              {
                this.Engine.Globals.assemblyKeyFileName = null;
                this.Engine.Globals.assemblyKeyFileNameContext = null;
              }
            }
            return null;
          }
          if (ty.FullName == "System.Reflection.AssemblyKeyNameAttribute"){
            if (this.positionalArgValues.Count > 0){
              this.Engine.Globals.assemblyKeyName = Convert.ToString(this.positionalArgValues[0]);
              this.Engine.Globals.assemblyKeyNameContext = this.context;
              if (this.Engine.Globals.assemblyKeyName != null && this.Engine.Globals.assemblyKeyName.Length == 0) {
                this.Engine.Globals.assemblyKeyName = null;
                this.Engine.Globals.assemblyKeyNameContext = null;
              }
            }
            return null;
          }
          if (ty.FullName == "System.Reflection.AssemblyVersionAttribute"){
            if (this.positionalArgValues.Count > 0)
              this.Engine.Globals.assemblyVersion = this.ParseVersion(Convert.ToString(this.positionalArgValues[0]));
            return null;
          }
          if (ty.FullName == "System.CLSCompliantAttribute"){
            this.Engine.isCLSCompliant = this.args == null || this.args.count == 0 || Convert.ToBoolean(this.positionalArgValues[0], false);
            return this;
          }
        }
      }catch(ArgumentException){
        this.context.HandleError(JSError.InvalidCall);
      }

      return this;
    }

    private Version ParseVersion(String vString){
      ushort major = 1;
      ushort minor = 0;
      ushort build = 0;
      ushort revision = 0;
      try{
        int n = vString.Length;
        int i = vString.IndexOf('.', 0);
        if (i < 0) throw new Exception();
        major = UInt16.Parse(vString.Substring(0, i), CultureInfo.InvariantCulture);
        int j = vString.IndexOf('.', i+1);
        if (j < i+1)
          minor = UInt16.Parse(vString.Substring(i+1, n-i-1), CultureInfo.InvariantCulture);
        else{
          minor = UInt16.Parse(vString.Substring(i+1, j-i-1), CultureInfo.InvariantCulture);
          if (vString[j+1] == '*'){
            build = CustomAttribute.DaysSince2000();
            revision = CustomAttribute.SecondsSinceMidnight();
          }else{
            int k = vString.IndexOf('.', j+1);
            if (k < j+1)
              build = UInt16.Parse(vString.Substring(j+1, n-j-1), CultureInfo.InvariantCulture);
            else{
              build = UInt16.Parse(vString.Substring(j+1, k-j-1), CultureInfo.InvariantCulture);
              if (vString[k+1] == '*')
                revision = CustomAttribute.SecondsSinceMidnight();
              else
                revision = UInt16.Parse(vString.Substring(k+1, n-k-1), CultureInfo.InvariantCulture);
            }
          }
        }
      }catch{
        this.args[0].context.HandleError(JSError.NotValidVersionString);
      }
      return new Version(major, minor, build, revision);
    }

    private static ushort SecondsSinceMidnight(){
      TimeSpan sinceMidnight = DateTime.Now - DateTime.Today;
      return (ushort)((sinceMidnight.Hours*60*60+sinceMidnight.Minutes*60+sinceMidnight.Seconds)/2);
    }
    
    internal void SetTarget(AST target){
      this.target = target;
    }
      
    internal override void TranslateToIL(ILGenerator il, Type rtype){
    }
    
    internal override void TranslateToILInitializer(ILGenerator il){
    }

    internal static IReflect TypeOfArgument(Object argument){
      if (argument is Enum)
        return argument.GetType();
      else if (argument is EnumWrapper)
        return ((EnumWrapper)argument).classScopeOrType;
      else{
        switch (Convert.GetTypeCode(argument)){
          case TypeCode.Empty: 
          case TypeCode.DBNull: return Typeob.Object;
          case TypeCode.Boolean: return Typeob.Boolean;
          case TypeCode.Char: return Typeob.Char;
          case TypeCode.Byte: return Typeob.Byte;
          case TypeCode.UInt16: return Typeob.UInt16;
          case TypeCode.UInt32: return Typeob.UInt32;
          case TypeCode.UInt64: return Typeob.UInt64;
          case TypeCode.SByte: return Typeob.SByte;
          case TypeCode.Int16: return Typeob.Int16;
          case TypeCode.Int32: return Typeob.Int32;
          case TypeCode.Int64: return Typeob.Int64;
          case TypeCode.Single: return Typeob.Single;
          case TypeCode.Double: return Typeob.Double;
          case TypeCode.Object:
            if (argument is Type) return Typeob.Type;
            if (argument is ClassScope) return Typeob.Type;
            break;
          case TypeCode.String: return Typeob.String;
        }
        return null;
      }
    }

    // These APIs are to detect and support ReflectionOnlyLoadFrom which is used at compile time. 
    // If it isn't the reflection only case, then the normal API to get attributes is used.
    //
    // At compile time, the only custom attributes which we extract using GetCustomAttributes are:
    //  Microsoft.JScript.JSFunctionAttribute
    //  Microsoft.JScript.NotRecommended
    //  Microsoft.JScript.ReferenceAttribute
    //  System.AttributeUsageAttribute
    //  System.CLSCompliantAttribute
    //  System.ObsoleteAttribute
    //  System.Runtime.InteropServices.CoClassAttribute
    //  System.Security.AllowPartiallyTrustedCallersAttribute
    //
    // The only ones we check for using IsDefined are:
    //  Microsoft.JScript.Expando
    //  Microsoft.JScript.JSFunctionAttribute
    //  System.ParamArrayAttribute
    //  System.Runtime.CompilerServices.RequiredAttributeAttribute
    //    
    // None of these are Inherited attibutes and so the ReflectionOnly code path can ignore the 
    // inherit flag. The System.* attributes are sealed. The Microsoft.JScript ones are not, 
    // though they should be and the compiler will not respect subtypes of these attributes.
    //
    private static Object GetCustomAttributeValue(CustomAttributeTypedArgument arg) {
      Type reflectionOnlyArgType = arg.ArgumentType;
      
      // If it's an enumerated type, the value is the boxed underlying value and must be converted.
      if (reflectionOnlyArgType.IsEnum)
        return Enum.ToObject(Type.GetType(reflectionOnlyArgType.FullName), arg.Value);
      
      return arg.Value;
    }
    
    internal static Object[] GetCustomAttributes(Assembly target, Type caType, bool inherit) {
      if (!target.ReflectionOnly)
        return target.GetCustomAttributes(caType, inherit);
      return CustomAttribute.ExtractCustomAttribute(CustomAttributeData.GetCustomAttributes(target), caType);
    }
    
    internal static Object[] GetCustomAttributes(Module target, Type caType, bool inherit) {
      if (!target.Assembly.ReflectionOnly)
        return target.GetCustomAttributes(caType, inherit);
      return CustomAttribute.ExtractCustomAttribute(CustomAttributeData.GetCustomAttributes(target), caType);
    }
        
    internal static Object[] GetCustomAttributes(MemberInfo target, Type caType, bool inherit) {
      // JScript implements subclasses of MemberInfo which throw an exception when Module is
      // accessed. We know that none of these are from a ReflectionOnly assembly.
      Type t = target.GetType();
      if (t.Assembly == typeof(CustomAttribute).Assembly || !target.Module.Assembly.ReflectionOnly)
        return target.GetCustomAttributes(caType, inherit);
      return CustomAttribute.ExtractCustomAttribute(CustomAttributeData.GetCustomAttributes(target), caType);
    }
     
    internal static Object[] GetCustomAttributes(ParameterInfo target, Type caType, bool inherit) {
      // JScript implements subclasses of ParameterInfo which throw an exception when Module is
      // accessed. We know that none of these are from a ReflectionOnly assembly.
      Type t = target.GetType();
      if (t.Assembly == typeof(CustomAttribute).Assembly || !target.Member.Module.Assembly.ReflectionOnly)
        return target.GetCustomAttributes(caType, inherit);
      return CustomAttribute.ExtractCustomAttribute(CustomAttributeData.GetCustomAttributes(target), caType);
    }

    // This is the common processing code to extract the attributes from the list returned
    // by the custom attribute data reflector.    
    private static Object[] ExtractCustomAttribute(IList<CustomAttributeData> attributes, Type caType) {
      // None of the custom attributes we check for have AllowMultiple == true
      Debug.Assert(
          caType == typeof(Microsoft.JScript.JSFunctionAttribute) ||
          caType == typeof(Microsoft.JScript.NotRecommended) ||
          caType == typeof(Microsoft.JScript.ReferenceAttribute) ||
          caType == typeof(System.AttributeUsageAttribute) ||
          caType == typeof(System.CLSCompliantAttribute) ||
          caType == typeof(System.ObsoleteAttribute) ||
          caType == typeof(System.Security.AllowPartiallyTrustedCallersAttribute) ||
          caType == typeof(System.ParamArrayAttribute));
      
      Type caReflectionOnlyType = Globals.TypeRefs.ToReferenceContext(caType);
      foreach(CustomAttributeData caData in attributes) {
        if (caData.Constructor.DeclaringType == caReflectionOnlyType) {
          // Build up the constructor argument list and create an instance.
          ArrayList args = new ArrayList();
          foreach(CustomAttributeTypedArgument arg in caData.ConstructorArguments) {
            args.Add(CustomAttribute.GetCustomAttributeValue(arg));
          }
          Object instance = Activator.CreateInstance(caType, args.ToArray());
          
          // Populate the public fields.
          foreach(CustomAttributeNamedArgument namedArg in caData.NamedArguments) {
            caType.InvokeMember(namedArg.MemberInfo.Name, 
              BindingFlags.Instance|BindingFlags.Public|BindingFlags.SetField|BindingFlags.SetProperty,
              null,
              instance,
              new Object[1] { CustomAttribute.GetCustomAttributeValue(namedArg.TypedValue) },
              null,
              CultureInfo.InvariantCulture,
              null);
          }
          return new Object[1] { instance };
        }
      }
      return new Object[0];
    }


    internal static bool IsDefined(MemberInfo target, Type caType, bool inherit) {
      // JScript implements subclasses of MemberInfo which throw an exception when Module is
      // accessed. We know that none of these are from a ReflectionOnly assembly.
      Type t = target.GetType();
      if (t.Assembly == typeof(CustomAttribute).Assembly || !target.Module.Assembly.ReflectionOnly)
        return target.IsDefined(caType, inherit);
      return CustomAttribute.CheckForCustomAttribute(CustomAttributeData.GetCustomAttributes(target), caType);
    }
     
    internal static bool IsDefined(ParameterInfo target, Type caType, bool inherit) {
      // JScript implements subclasses of ParameterInfo which throw an exception when Module is
      // accessed. We know that none of these are from a ReflectionOnly assembly.
      Type t = target.GetType();
      if (t.Assembly == typeof(CustomAttribute).Assembly || !target.Member.Module.Assembly.ReflectionOnly)
        return target.IsDefined(caType, inherit);
      return CustomAttribute.CheckForCustomAttribute(CustomAttributeData.GetCustomAttributes(target), caType);
    }

    private static bool CheckForCustomAttribute(IList<CustomAttributeData> attributes, Type caType) {
      Debug.Assert(
          caType == typeof(Microsoft.JScript.Expando) ||
          caType == typeof(Microsoft.JScript.JSFunctionAttribute) ||
          caType == typeof(System.ParamArrayAttribute) ||
          caType == typeof(System.Runtime.CompilerServices.RequiredAttributeAttribute));
     
      Type caReflectionOnlyType = Globals.TypeRefs.ToReferenceContext(caType);
      foreach(CustomAttributeData caData in attributes) {
        if (caData.Constructor.DeclaringType == caReflectionOnlyType)
          return true;
      }
      return false;
    }
    
  }
}
