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
    using System.Reflection;
    using System.Globalization;
    using System.Collections;
    using System.Runtime.InteropServices;
        
    [Serializable] internal sealed class JSBinder : Binder{
      internal static readonly JSBinder ob = new JSBinder();
        
      // both args and namedParameters cannot be null and args length must be bigger or equal to namedParameters.Length
      internal static Object[] ArrangeNamedArguments(MethodBase method, Object[] args, string[] namedParameters){
        ParameterInfo[] pars = method.GetParameters();
        int formalLength = pars.Length;
        if (formalLength == 0) 
          throw new JScriptException(JSError.MissingNameParameter);
        Object[] newArgs = new Object[formalLength];
        int argsLength = args.Length;
        int namesLength = namedParameters.Length;
        int numPositional = argsLength - namesLength;
        ArrayObject.Copy(args, namesLength, newArgs, 0, numPositional);
        for (int i = 0; i < namesLength; i++){
          string name = namedParameters[i];
          if (name == null || name.Equals(""))
            throw new JScriptException(JSError.MustProvideNameForNamedParameter);
          int j;
          for (j = numPositional; j < formalLength; j++){
            if (name.Equals(pars[j].Name))
              if (newArgs[j] is Empty)
                throw new JScriptException(JSError.DuplicateNamedParameter);
              else{
                newArgs[j] = args[i];
                break;
              }
          }
          if (j == formalLength)
            throw new JScriptException(JSError.MissingNameParameter);
        }
        // go through the array and replace null/missing entries with the default value of the corresponding param (if any)
        if (method is JSMethod) return newArgs; //JSMethods get special treatment elsewhere
        for (int i = 0; i < formalLength; i++)
          if (newArgs[i] == null || newArgs[i] == Missing.Value){
            Object dv = TypeReferences.GetDefaultParameterValue(pars[i]);
            if (dv == System.Convert.DBNull) //No default value was specified
              throw new ArgumentException(pars[i].Name);
            newArgs[i] = dv;
          }
        return newArgs;
      }

      public override FieldInfo BindToField(BindingFlags bindAttr, FieldInfo[] match, Object value, CultureInfo locale){
        if (value == null)
          value = DBNull.Value;
        int minval = Int32.MaxValue;
        int dupCount = 0;
        FieldInfo bestField = null;
        Type vtype = value.GetType();
        for (int i = 0, j = match.Length; i < j; i++){
          FieldInfo field = match[i];
          int distance = TypeDistance(Runtime.TypeRefs, field.FieldType, vtype);
          if (distance < minval){
            minval = distance;
            bestField = field;
            dupCount = 0;
          }else if (distance == minval)
            dupCount += 1;
        }
        if (dupCount > 0)
          throw new AmbiguousMatchException();
        return bestField;
      }
      
      public override MethodBase BindToMethod(BindingFlags bindingAttr, MethodBase[] match, ref Object[] args, ParameterModifier[] modifiers, 
                                              CultureInfo locale, String[] namedParameters, out Object state){
        state = null;
        return JSBinder.SelectMethodBase(Runtime.TypeRefs, match, ref args, modifiers, namedParameters);
      }
      
      public override Object ChangeType(Object value, Type target_type, CultureInfo locale){
        return Convert.CoerceT(value, target_type);
      }
      
      internal static MemberInfo[] GetDefaultMembers(IReflect ir){
        return JSBinder.GetDefaultMembers(Globals.TypeRefs, ir);
      }
      
      internal static MemberInfo[] GetDefaultMembers(TypeReferences typeRefs, IReflect ir){
        while (ir is ClassScope){
          ClassScope csc = (ClassScope)ir;
          csc.owner.IsExpando(); //Force the class to expose its expando property if it has not already done so
          if (csc.itemProp != null) return new MemberInfo[]{csc.itemProp};
          ir = csc.GetParent();
          if (ir is WithObject) ir = (IReflect)((WithObject)ir).contained_object;
        }
        if (ir is Type) return JSBinder.GetDefaultMembers((Type)ir);
        if (ir is JSObject) return typeRefs.ScriptObject.GetDefaultMembers();
        return null;
      }
      
      internal static MemberInfo[] GetDefaultMembers(Type t){
        while (t != typeof(Object) && t != null){
          MemberInfo[] defaultMembers = t.GetDefaultMembers();
          if (defaultMembers != null && defaultMembers.Length > 0)
            return defaultMembers;
          t = t.BaseType;
        }
        return null;
      }
      
      internal static MethodInfo GetDefaultPropertyForArrayIndex(Type t, int index, Type elementType, bool getSetter){
        try{
          MemberInfo[] defaultMembers = JSBinder.GetDefaultMembers(Runtime.TypeRefs, t);
          int n = 0;
          if (defaultMembers == null || (n = defaultMembers.Length) == 0)
            return null;
          for (int i = 0; i < n; i++){
            MemberInfo mem = defaultMembers[i];
            MemberTypes mt = mem.MemberType;
            MethodInfo meth = null;
            switch (mt){
              case MemberTypes.Property:
                meth = ((PropertyInfo)mem).GetGetMethod();
                break;
              case MemberTypes.Method:
                meth = (MethodInfo)mem;
                break;
              default:
                continue;
            }
            if (meth != null){
              ParameterInfo[] pars = meth.GetParameters();
              if (pars == null || pars.Length == 0){
                //See if meth is a default method/property-getter that returns an Array or List
                Type rt = meth.ReturnType;
                if (typeof(Array).IsAssignableFrom(rt) || typeof(IList).IsAssignableFrom(rt))
                  return meth;
              }else if (pars.Length == 1 && mt == MemberTypes.Property){
                //See if meth is the getter of a suitable default indexed property
                PropertyInfo prop = (PropertyInfo)mem;
                if (elementType == null || prop.PropertyType.IsAssignableFrom(elementType))
                  try{
                    Convert.CoerceT(index, pars[0].ParameterType);
                    if (getSetter)
                      return prop.GetSetMethod();
                    else
                      return meth;
                  }catch(JScriptException){}
              }
            }
          }
        }catch(InvalidOperationException){}
        return null;
      }

      internal static MemberInfo[] GetInterfaceMembers(String name, Type t){
        BindingFlags flags = BindingFlags.Public|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly;
        MemberInfo[] members = t.GetMember(name, flags);
        Type[] baseInts = t.GetInterfaces();
        if (baseInts == null || baseInts.Length == 0) return members;
        ArrayList baseInterfaces = new ArrayList(baseInts);
        MemberInfoList result = new MemberInfoList();
        result.AddRange(members);
        for (int i = 0; i < baseInterfaces.Count; i++){
          Type bi = (Type)baseInterfaces[i];
          members = bi.GetMember(name, flags);
          if (members != null) result.AddRange(members);
          foreach (Type bbi in bi.GetInterfaces()){
            if (baseInterfaces.IndexOf(bbi) == -1)
              baseInterfaces.Add(bbi);
          }
        }
        return result.ToArray();
      }

      private static bool FormalParamTypeIsObject(ParameterInfo par){
        ParameterDeclaration pd = par as ParameterDeclaration;
        if (pd != null) return pd.ParameterIReflect == Typeob.Object;
        return par.ParameterType == Typeob.Object;
      }
      
      public override void ReorderArgumentArray(ref Object[] args, Object state){
      }

      internal static MemberInfo Select(TypeReferences typeRefs, MemberInfo[] match, int matches, IReflect[] argIRs, MemberTypes memberType){
        Debug.Assert(matches > 1);
        int candidates = 0;
        ParameterInfo[][] fparams = new ParameterInfo[matches][];
        bool lookForPropertyGetters = memberType == MemberTypes.Method;
        for (int i = 0; i < matches; i++){
          MemberInfo mem = match[i];
          if (mem is PropertyInfo && lookForPropertyGetters) mem = ((PropertyInfo)mem).GetGetMethod(true);
          if (mem == null) continue;
          if (mem.MemberType == memberType){
            if (mem is PropertyInfo)
              fparams[i] = ((PropertyInfo)mem).GetIndexParameters();
            else
              fparams[i] = ((MethodBase)mem).GetParameters();
            candidates++;
          }
        }
        int j = JSBinder.SelectBest(typeRefs, match, matches, argIRs, fparams, null, candidates, argIRs.Length);
        if (j < 0) return null;
        return match[j];
      }
      
      internal static MemberInfo Select(TypeReferences typeRefs, MemberInfo[] match, int matches, ref Object[] args, String[] namedParameters, MemberTypes memberType){
        Debug.Assert(matches > 1);
        bool hasNamedParams = false;
        if (namedParameters != null && namedParameters.Length > 0)
          if (args.Length >= namedParameters.Length)
            hasNamedParams = true;
          else
            throw new JScriptException(JSError.MoreNamedParametersThanArguments);
        int candidates = 0;
        ParameterInfo[][] fparams = new ParameterInfo[matches][];
        Object[][] aparams = new Object[matches][];
        bool lookForPropertyGetters = memberType == MemberTypes.Method;
        for (int i = 0; i < matches; i++){
          MemberInfo mem = match[i];
          if (lookForPropertyGetters && mem.MemberType == MemberTypes.Property)
            mem = ((PropertyInfo)mem).GetGetMethod(true);
          if (mem.MemberType == memberType){
            if (memberType == MemberTypes.Property)
              fparams[i] = ((PropertyInfo)mem).GetIndexParameters();
            else
              fparams[i] = ((MethodBase)mem).GetParameters();
            if (hasNamedParams)
              aparams[i] = JSBinder.ArrangeNamedArguments((MethodBase)mem, args, namedParameters);
            else
              aparams[i] = args;
            candidates++;
          }
        }
        int j = JSBinder.SelectBest(typeRefs, match, matches, null, fparams, aparams, candidates, args.Length);
        if (j < 0) return null;
        args = aparams[j];
        MemberInfo result = match[j];
        if (lookForPropertyGetters && result.MemberType == MemberTypes.Property)
          result = ((PropertyInfo)result).GetGetMethod(true);
        return result;
      }
      
      private static int SelectBest(TypeReferences typeRefs, MemberInfo[] match, int matches, IReflect[] argIRs, ParameterInfo[][] fparams, Object[][] aparams, int candidates, int parameters){ 
        Debug.Assert(matches > 1);
        if (candidates == 0) return -1;
        if (candidates == 1)
          for (int i = 0; i < matches; i++)
            if (fparams[i] != null) return i;
        bool[] eliminated = new bool[matches];
        //Set up penalties to discourage selection of methods that have more formal parameters than there are actual parameters
        int[] penalty = new int[matches];
        for (int i = 0; i < matches; i++){
          ParameterInfo[] fpars = fparams[i];
          if (fpars != null){
            int m = fpars.Length;
            int actuals =  (argIRs == null ? aparams[i].Length : argIRs.Length);
            if (actuals > m && (m == 0 || !CustomAttribute.IsDefined(fpars[m-1], typeof(ParamArrayAttribute), false))){
              fparams[i] = null;
              candidates--;
              Debug.Assert(candidates >= 0);
              continue;
            }
            for (int j = parameters; j < m; j++){
              ParameterInfo fpar = fpars[j];
              if (j == m-1 && CustomAttribute.IsDefined(fpar, typeof(ParamArrayAttribute), false))
                break;
              Object dv = TypeReferences.GetDefaultParameterValue(fpar);
              if (dv is System.DBNull){
                //No default value, set up a penalty
                penalty[i] = 50;
              }
            }
          }
        }
        for (int p = 0; candidates > 1; p++){
          int candidatesWithFormalParametersStillToBeConsidered = 0;
          //Eliminate any candidate that is worse match than any other candidate for this (possibly missing) actual parameter
          int minval = Int32.MaxValue;
          bool objectCanWin = false;
          for (int i = 0; i < matches; i++){
            int penaltyForUsingDefaultValue = 0;
            ParameterInfo[] fpars = fparams[i];
            if (fpars != null){ //match[i] is a candidate
              IReflect aIR = typeRefs.Missing;
              if (argIRs == null){
                if (aparams[i].Length > p){
                  Object apar = aparams[i][p];
                  if (apar == null) apar = DBNull.Value;
                  aIR = typeRefs.ToReferenceContext(apar.GetType());
                }
              }else if (p < parameters)
                aIR = argIRs[p];
              int m = fpars.Length;
              if (m-1 > p) 
                candidatesWithFormalParametersStillToBeConsidered++;
              IReflect fIR = typeRefs.Missing;
              if (m > 0 && p >= m-1 && CustomAttribute.IsDefined(fpars[m-1], typeof(ParamArrayAttribute), false) && 
              !(aIR is TypedArray || aIR == typeRefs.ArrayObject || (aIR is Type && ((Type)aIR).IsArray))){
                ParameterInfo fpar = fpars[m-1];
                if (fpar is ParameterDeclaration){
                  fIR = ((ParameterDeclaration)fpar).ParameterIReflect;
                  fIR = ((TypedArray)fIR).elementType;
                }else
                  fIR = fpar.ParameterType.GetElementType();
                if (p == m-1) penalty[i]++;
              }else if (p < m){
                ParameterInfo fpar = fpars[p];
                fIR = fpar is ParameterDeclaration ? ((ParameterDeclaration)fpar).ParameterIReflect : fpar.ParameterType;
                if (aIR == typeRefs.Missing){
                  //No actual parameter was supplied, if the formal parameter has default value, make the match perfect
                  Object dv = TypeReferences.GetDefaultParameterValue(fpar);
                  if (!(dv is System.DBNull)) {
                    aIR = fIR;
                    penaltyForUsingDefaultValue = 1;
                  }
                }
              }
              int distance = TypeDistance(typeRefs, fIR, aIR) + penalty[i] + penaltyForUsingDefaultValue;
              if (distance == minval){
                if (p == m-1 && eliminated[i]){
                  candidates--;
                  fparams[i] = null;
                }
                objectCanWin = objectCanWin && eliminated[i];
                continue;
              }
              if (distance > minval){
                if (objectCanWin && p < m && JSBinder.FormalParamTypeIsObject(fparams[i][p])){
                   minval = distance; //Make sure that a future, better match than Object can win.
                   continue;
                }
                if (p > m-1 && aIR == typeRefs.Missing && CustomAttribute.IsDefined(fpars[m-1], typeof(ParamArrayAttribute), false)) continue;
                eliminated[i] = true;
                continue;
              }
              //If we get here, we have a match that is strictly better than all previous matches.
              //If there are no other remaining candidates, we're done.
              if (candidates == 1 && !eliminated[i])
                return i;
              //Eliminate those other candidates from consideration.
              objectCanWin = eliminated[i];
              for (int j = 0; j < i; j++)
                if (fparams[j] != null && !eliminated[j]){
                  bool noFormalForCurrParam = fparams[j].Length <= p;
                  if (noFormalForCurrParam && parameters <= p)
                    //Do not eliminate an overload if it does not have a formal for the current position
                    //and the call does not have an actual parameter for the current position either.
                    continue;
                  if (noFormalForCurrParam || !objectCanWin || !JSBinder.FormalParamTypeIsObject(fparams[j][p]))
                    eliminated[j] = true;
                }
              minval = distance;
            }
          }
          if (p >= parameters-1 && candidatesWithFormalParametersStillToBeConsidered < 1)
            //Looked at all actual parameters as well as all formal parameters, no further progress can be made
            break;
        }
        int best = -1;
        for (int j = 0; j < matches && candidates > 0; j++){
          ParameterInfo[] fpars = fparams[j];
          if (fpars != null){
            if (eliminated[j]){
              candidates--; fparams[j] = null; continue;
            }
            int pc = fpars.Length;
            if (best == -1){ //Choose the first remaining candidate as "best"
              best = j; continue;
            }
            if (Class.ParametersMatch(fpars, fparams[best])){
              MemberInfo bstm = match[best];
              JSWrappedMethod jswm = match[best] as JSWrappedMethod;
              if (jswm != null) bstm = jswm.method;
              if (bstm is JSFieldMethod || bstm is JSConstructor || bstm is JSProperty){
                //The first match is always the most derived, go with it
                candidates--; fparams[j] = null; continue;
              }
              Type bestMemT = match[best].DeclaringType;
              Type memT = match[j].DeclaringType;
              if (bestMemT != memT){
                if (memT.IsAssignableFrom(bestMemT)){
                  candidates--; fparams[j] = null; continue;
                }else if (bestMemT.IsAssignableFrom(memT)){
                  fparams[best] = null; best = j; candidates--; continue;
                }
              }
            }  
          }
        }
        if (candidates != 1)
          throw new AmbiguousMatchException();
        return best;
      }
      
      
      internal static ConstructorInfo SelectConstructor(MemberInfo[] match, ref Object[] args, String[] namedParameters){
        return JSBinder.SelectConstructor(Globals.TypeRefs, match, ref args, namedParameters);
      }
      
      internal static ConstructorInfo SelectConstructor(TypeReferences typeRefs, MemberInfo[] match, ref Object[] args, String[] namedParameters){
        if (match == null) return null;
        int n = match.Length;
        if (n == 0) return null;
        if (n == 1){
          Type t = match[0] as Type;
          if (t != null) {
            match = t.GetConstructors();
            n = match.Length;
          }
        }
        if (n == 1) return match[0] as ConstructorInfo;
        return (ConstructorInfo)JSBinder.Select(typeRefs, match, n, ref args, namedParameters, MemberTypes.Constructor);
      }  

      internal static ConstructorInfo SelectConstructor(MemberInfo[] match, IReflect[] argIRs){
        return JSBinder.SelectConstructor(Globals.TypeRefs, match, argIRs);
      }
      
      internal static ConstructorInfo SelectConstructor(TypeReferences typeRefs, MemberInfo[] match, IReflect[] argIRs){
        if (match == null) return null;
        int n = match.Length;
        if (n == 1){
          Object val = match[0];
          if (val is JSGlobalField) val = ((JSGlobalField)val).GetValue(null);
          Type t = val as Type;
          if (t != null){
            match = t.GetConstructors();
          }
          n = match.Length;
        }
        if (n == 0) return null;
        if (n == 1) return match[0] as ConstructorInfo;
        return (ConstructorInfo)JSBinder.Select(typeRefs, match, n, argIRs, MemberTypes.Constructor);
      }  
      
      internal static MemberInfo SelectCallableMember(MemberInfo[] match, IReflect[] argIRs)
      {
        if (match == null) return null;
        int n = match.Length;
        if (n == 0) return null;
        MemberInfo result = n == 1 ? match[0] : JSBinder.Select(Globals.TypeRefs, match, n, argIRs, MemberTypes.Method);
        return result as MemberInfo;
      }

      internal static MethodInfo SelectMethod(MemberInfo[] match, ref Object[] args, String[] namedParameters){
        return JSBinder.SelectMethod(Globals.TypeRefs, match, ref args, namedParameters);
      }
      
      internal static MethodInfo SelectMethod(TypeReferences typeRefs, MemberInfo[] match, ref Object[] args, String[] namedParameters){
        if (match == null) return null;
        int n = match.Length;
        if (n == 0) return null;
        MemberInfo result = n == 1 ? match[0] : JSBinder.Select(typeRefs, match, n, ref args, namedParameters, MemberTypes.Method);
        if (result != null && result.MemberType == MemberTypes.Property)
          result = ((PropertyInfo)result).GetGetMethod(true);
        return result as MethodInfo;
      }  
      
      internal static MethodInfo SelectMethod(MemberInfo[] match, IReflect[] argIRs){
        return JSBinder.SelectMethod(Globals.TypeRefs, match, argIRs);
      }
      
      internal static MethodInfo SelectMethod(TypeReferences typeRefs, MemberInfo[] match, IReflect[] argIRs){
        if (match == null) return null;
        int n = match.Length;
        if (n == 0) return null;
        MemberInfo result = n == 1 ? match[0] : JSBinder.Select(typeRefs, match, n, argIRs, MemberTypes.Method);
        if (result != null && result.MemberType == MemberTypes.Property)
          return ((PropertyInfo)result).GetGetMethod(true);
        return result as MethodInfo;
      }
      
      public override MethodBase SelectMethod(BindingFlags bindingAttr, MethodBase[] match, Type[] types, ParameterModifier[] modifiers){
        if (match == null) return null;
        int n = match.Length;
        if (n == 0) return null;
        if (n == 1) return match[0];
        if (match[0].MemberType == MemberTypes.Constructor)
          return (ConstructorInfo)JSBinder.Select(Runtime.TypeRefs, match, n, types, MemberTypes.Constructor);
        else
          return (MethodInfo)JSBinder.Select(Runtime.TypeRefs, match, n, types, MemberTypes.Method);
      }
      
      private static MethodBase SelectMethodBase(TypeReferences typeRefs, MethodBase[] match, ref Object[] args, ParameterModifier[] modifiers, String[] namedParameters){
        if (match == null) return null;
        int n = match.Length;
        if (n == 0) return null;
        if (n == 1) return match[0];
        MethodBase result = (MethodBase)JSBinder.Select(typeRefs, match, n, ref args, namedParameters, MemberTypes.Method);
        if (result == null)
          result = (MethodBase)JSBinder.Select(typeRefs, match, n, ref args, namedParameters, MemberTypes.Constructor);
        return result;
      }  
      
      internal static MethodInfo SelectOperator(MethodInfo op1, MethodInfo op2, Type t1, Type t2){
        ParameterInfo[] params1 = null;
        if (op1 == null || (op1.Attributes & MethodAttributes.SpecialName) == 0 || (params1 = op1.GetParameters()).Length != 2)
          op1 = null;
        ParameterInfo[] params2 = null;
        if (op2 == null || (op2.Attributes & MethodAttributes.SpecialName) == 0 || (params2 = op2.GetParameters()).Length != 2)
          op2 = null;
        if (op1 == null) return op2;
        if (op2 == null) return op1;
        int op1cost = TypeDistance(Globals.TypeRefs, params1[0].ParameterType, t1) + TypeDistance(Globals.TypeRefs, params1[1].ParameterType, t2);
        int op2cost = TypeDistance(Globals.TypeRefs, params2[0].ParameterType, t1) + TypeDistance(Globals.TypeRefs, params2[1].ParameterType, t2);
        if (op1cost <= op2cost) return op1;
        return op2;
      }
      
      internal static PropertyInfo SelectProperty(MemberInfo[] match, Object[] args){
        return JSBinder.SelectProperty(Globals.TypeRefs, match, args);
      }
      
      internal static PropertyInfo SelectProperty(TypeReferences typeRefs, MemberInfo[] match, Object[] args){
        if (match == null) return null;
        int matches = match.Length;
        if (matches == 0) return null;
        if (matches == 1) return match[0] as PropertyInfo;
        int candidates = 0;
        PropertyInfo result = null;
        ParameterInfo[][] fparams = new ParameterInfo[matches][];
        Object[][] aparams = new Object[matches][];
        for (int i = 0; i < matches; i++){
          MemberInfo mem = match[i];
          if (mem.MemberType == MemberTypes.Property){
            MethodInfo getter = (result = (PropertyInfo)mem).GetGetMethod(true);
            if (getter == null)
              fparams[i] = result.GetIndexParameters();
            else
              fparams[i] = getter.GetParameters();
            aparams[i] = args;
            candidates++;
          }
        }
        if (candidates <= 1) return result;
        int j = JSBinder.SelectBest(typeRefs, match, matches, null, fparams, aparams, candidates, args.Length);
        if (j < 0) return null;
        return (PropertyInfo)match[j];
      }
      
      public override PropertyInfo SelectProperty(BindingFlags bindingAttr, PropertyInfo[] match, Type rtype, Type[] types, ParameterModifier[] modifiers){
        if (match == null) return null;
        int matches = match.Length;
        if (matches == 0) return null;
        if (matches == 1) return match[0];
        int candidates = 0;
        PropertyInfo result = null;
        int minval = Int32.MaxValue;
        ParameterInfo[][] fparams = new ParameterInfo[matches][];
        for (int i = 0; i < matches; i++){
          result = match[i];
          if (rtype != null){
            int distance = TypeDistance(Globals.TypeRefs, result.PropertyType, rtype);
            if (distance > minval)
              continue;
            if (distance < minval) //This property is strictly a better than all previous one w.r.t. rtype. Remove all of those from consideration
              for (int j = 0; j < i; j++)
                if (fparams[j] != null){
                  fparams[j] = null;
                  candidates--;
                }
          }
          fparams[i] = result.GetIndexParameters();
          candidates++;
        }
        if (candidates <= 1) return result;
        int k = JSBinder.SelectBest(Globals.TypeRefs, match, matches, types, fparams, null, candidates, types.Length);
        if (k < 0) return null;
        return match[k];
      }
      
      internal static PropertyInfo SelectProperty(MemberInfo[] match, IReflect[] argIRs){
        return JSBinder.SelectProperty(Globals.TypeRefs, match, argIRs);
      }
      
      internal static PropertyInfo SelectProperty(TypeReferences typeRefs, MemberInfo[] match, IReflect[] argIRs){
        if (match == null) return null;
        int n = match.Length;
        if (n == 0) return null;
        if (n == 1) return match[0] as PropertyInfo;
        return (PropertyInfo)JSBinder.Select(typeRefs, match, n, argIRs, MemberTypes.Property);
      }
      
      //Returns a value that quantifies the cost/badness of converting a value of type actual to type formal.
      //Used to select the overload that is the closest match to the actual parameter.

      private static int TypeDistance(TypeReferences typeRefs, IReflect formal, IReflect actual){
        if (formal is TypedArray){
          if (actual is TypedArray){
            TypedArray f = (TypedArray)formal;
            TypedArray a = (TypedArray)actual;
            if (f.rank == a.rank) 
              return TypeDistance(typeRefs, f.elementType, a.elementType) == 0 ? 0 : 100;
          }else if (actual is Type){
            TypedArray f = (TypedArray)formal;
            Type a = (Type)actual;
            if (a.IsArray && f.rank == a.GetArrayRank()) 
              return TypeDistance(typeRefs, f.elementType, a.GetElementType()) == 0 ? 0 : 100;
            else if (a == typeRefs.Array || a == typeRefs.ArrayObject)
              return 30;
          }
          return 100;
        }
        if (actual is TypedArray){
          if (formal is Type){
            Type f = (Type)formal;
            TypedArray a = (TypedArray)actual;
            if (f.IsArray && f.GetArrayRank() == a.rank) 
              return TypeDistance(typeRefs, f.GetElementType(), a.elementType) == 0 ? 0 : 100;
            else if (f == typeRefs.Array)
              return 30;
            else if (f == typeRefs.Object)
              return 50;
          }
          return 100;
        }
        if (formal is ClassScope){
          if (actual is ClassScope)
            return ((ClassScope)actual).IsSameOrDerivedFrom((ClassScope)formal) ? 0 : 100;
          else
            return 100;
        }
        if (actual is ClassScope){
          if (formal is Type)
            return ((ClassScope)actual).IsPromotableTo((Type)formal) ? 0 : 100;
          else
            return 100;
        }
        return TypeDistance(typeRefs, Convert.ToType(typeRefs, formal), Convert.ToType(typeRefs, actual)); 
      }
      
      private static int TypeDistance(TypeReferences typeRefs, Type formal, Type actual){
        TypeCode atc = Type.GetTypeCode(actual);
        TypeCode ftc = Type.GetTypeCode(formal);
        if (actual.IsEnum) atc = TypeCode.Object;
        if (formal.IsEnum) ftc = TypeCode.Object;
        switch(atc){
          case TypeCode.DBNull:
            switch(ftc){
              default: return formal == typeRefs.Object ? 0 : 1;
            }
            
          case TypeCode.Boolean:
            switch(ftc){
              case TypeCode.Boolean: return 0;
              case TypeCode.Byte: return 1;
              case TypeCode.UInt16: return 2;
              case TypeCode.UInt32: return 3;
              case TypeCode.UInt64: return 4;
              case TypeCode.SByte: return 5;
              case TypeCode.Int16: return 6;
              case TypeCode.Int32: return 7;
              case TypeCode.Int64: return 8;
              case TypeCode.Single: return 9;
              case TypeCode.Double: return 10;
              case TypeCode.Decimal: return 11; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 12);
              case TypeCode.String: return 13;
              default: return 100;
            }
            
          case TypeCode.Char:
            switch(ftc){
              case TypeCode.Char: return 0;
              case TypeCode.UInt16: return 1;
              case TypeCode.UInt32: return 2;
              case TypeCode.Int32:  return 3;
              case TypeCode.UInt64: return 4;
              case TypeCode.Int64:  return 5;
              case TypeCode.Single: return 6;
              case TypeCode.Double: return 7;
              case TypeCode.Decimal: return 8;
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 9);
              case TypeCode.String: return 10;
              case TypeCode.Int16: return 11;
              case TypeCode.Byte: return 12;
              case TypeCode.SByte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.SByte:
            switch(ftc){
              case TypeCode.SByte: return 0;
              case TypeCode.Int16: return 1;
              case TypeCode.Int32:  return 2;
              case TypeCode.Int64:  return 3;
              case TypeCode.Single: return 4;
              case TypeCode.Double: return 5;
              case TypeCode.Decimal: return 6;
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 7);
              case TypeCode.String: return 8;
              case TypeCode.Byte: return 9;
              case TypeCode.UInt16: return 10;
              case TypeCode.UInt32: return 12;
              case TypeCode.UInt64: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.Byte: 
            switch(ftc){
              case TypeCode.Byte: return 0;
              case TypeCode.UInt16: return 1;
              case TypeCode.Int16: return 3;
              case TypeCode.UInt32: return 4;
              case TypeCode.Int32: return 5;
              case TypeCode.UInt64: return 6;
              case TypeCode.Int64: return 7;
              case TypeCode.Single: return 8;
              case TypeCode.Double: return 9;
              case TypeCode.Decimal: return 10; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 11);
              case TypeCode.String: return 12;
              case TypeCode.SByte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.Int16:
            switch(ftc){
              case TypeCode.Int16: return 0;
              case TypeCode.Int32: return 1;
              case TypeCode.Int64: return 2;
              case TypeCode.Single: return 3;
              case TypeCode.Double: return 4;
              case TypeCode.Decimal: return 5; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 6);
              case TypeCode.String: return 7;
              case TypeCode.UInt16: return 8;
              case TypeCode.UInt32: return 10;
              case TypeCode.UInt64: return 11;
              case TypeCode.SByte: return 12;
              case TypeCode.Byte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.UInt16: 
            switch(ftc){
              case TypeCode.UInt16: return 0;
              case TypeCode.UInt32: return 1;
              case TypeCode.UInt64: return 2;
              case TypeCode.Int32: return 4;
              case TypeCode.Int64: return 5;
              case TypeCode.Single: return 6;
              case TypeCode.Double: return 7;
              case TypeCode.Decimal: return 8; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 9);
              case TypeCode.String: return 10;
              case TypeCode.Int16: return 11;
              case TypeCode.Byte: return 12;
              case TypeCode.SByte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.Int32:
            switch(ftc){
              case TypeCode.Int32: return 0;
              case TypeCode.Int64: return 1;
              case TypeCode.Double: return 2;
              case TypeCode.Decimal: return 3; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 4);
              case TypeCode.String: return 5;
              case TypeCode.UInt64: return 6;
              case TypeCode.UInt32: return 7;
              case TypeCode.Single: return 8;
              case TypeCode.Int16: return 9;
              case TypeCode.UInt16: return 10;
              case TypeCode.SByte: return 12;
              case TypeCode.Byte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
           
          case TypeCode.UInt32: 
            switch(ftc){
              case TypeCode.UInt32: return 0;
              case TypeCode.UInt64: return 1;
              case TypeCode.Int64: return 2;
              case TypeCode.Double: return 3;
              case TypeCode.Decimal: return 4; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 5);
              case TypeCode.String: return 6;
              case TypeCode.Int32: return 7;
              case TypeCode.Single: return 8;
              case TypeCode.UInt16: return 9;
              case TypeCode.Int16: return 11;
              case TypeCode.Byte: return 12;
              case TypeCode.SByte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.Int64: 
            switch(ftc){
              case TypeCode.Int64: return 0;
              case TypeCode.Decimal: return 1; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 2);
              case TypeCode.String: return 3;
              case TypeCode.Double: return 4;
              case TypeCode.Single: return 5;
              case TypeCode.Int32: return 6;
              case TypeCode.Int16: return 7;
              case TypeCode.SByte: return 8;
              case TypeCode.UInt64: return 9;
              case TypeCode.UInt32: return 10;
              case TypeCode.UInt16: return 11;
              case TypeCode.Byte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.UInt64:
            switch(ftc){
              case TypeCode.UInt64: return 0;
              case TypeCode.Decimal: return 1; 
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 2);
              case TypeCode.String: return 3;
              case TypeCode.Int64: return 4;
              case TypeCode.Double: return 5;
              case TypeCode.Single: return 6;
              case TypeCode.UInt32: return 7;
              case TypeCode.UInt16: return 8;
              case TypeCode.Byte: return 10;
              case TypeCode.Int32: return 11;
              case TypeCode.Int16: return 12;
              case TypeCode.SByte: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.Single:
            switch(ftc){
              case TypeCode.Single: return 0;
              case TypeCode.Double: return 1;
              case TypeCode.Decimal: return 2; 
              case TypeCode.Int64: return 3;
              case TypeCode.UInt64: return 4;
              case TypeCode.Int32: return 5;
              case TypeCode.UInt32: return 6;
              case TypeCode.Int16: return 7;
              case TypeCode.UInt16: return 8;
              case TypeCode.SByte: return 10;
              case TypeCode.Byte: return 11;
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 12);
              case TypeCode.String: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.Double:
            switch(ftc){
              case TypeCode.Double: return 0;
              case TypeCode.Decimal: return 1; 
              case TypeCode.Single: return 2;
              case TypeCode.Int64: return 3;
              case TypeCode.UInt64: return 4;
              case TypeCode.Int32: return 5;
              case TypeCode.UInt32: return 6;
              case TypeCode.Int16: return 7;
              case TypeCode.UInt16: return 8;
              case TypeCode.SByte: return 10;
              case TypeCode.Byte: return 11;
              case TypeCode.Object: return TypeDistance(typeRefs, formal, actual, 12);
              case TypeCode.String: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
          
          case TypeCode.Decimal:
            switch(ftc){
              case TypeCode.Decimal: return 0; 
              case TypeCode.Double: return 1;
              case TypeCode.Single: return 2;
              case TypeCode.Int64: return 3;
              case TypeCode.UInt64: return 4;
              case TypeCode.Int32: return 5;
              case TypeCode.UInt32: return 6;
              case TypeCode.Int16: return 7;
              case TypeCode.UInt16: return 8;
              case TypeCode.SByte: return 10;
              case TypeCode.Byte: return 11;
              case TypeCode.Object: return formal == typeRefs.Object ? 12 : 100;
              case TypeCode.String: return 13;
              case TypeCode.Boolean: return 14;
              default: return 100;
            }
            
          case TypeCode.DateTime:
            switch(ftc){
              case TypeCode.DateTime: return 0;
              case TypeCode.Object: return formal == typeRefs.Object ? 1 : 100;
              case TypeCode.String: return 3;
              case TypeCode.Double: return 4;
              case TypeCode.Decimal: return 5; 
              case TypeCode.UInt64: return 6;
              case TypeCode.Int64: return 7;
              case TypeCode.UInt32: return 8;
              case TypeCode.Int32: return 9;
              default: return 100;
            }
            
          case TypeCode.String:
            switch(ftc){
              case TypeCode.String: return 0;
              case TypeCode.Object: return formal == typeRefs.Object ? 1 : 100;
              case TypeCode.Char: return 2;
              default: return 100;
            }
            
          case TypeCode.Object:
            if (formal == actual) return 0;
            if (formal == typeRefs.Missing) return 200;
            if (!(formal.IsAssignableFrom(actual))){
              if (typeRefs.Array.IsAssignableFrom(formal) && (actual == typeRefs.Array || typeRefs.ArrayObject.IsAssignableFrom(actual)))
                return 10;
              else if (ftc == TypeCode.String)
                return 20; 
              else if (actual == typeRefs.ScriptFunction && typeRefs.Delegate.IsAssignableFrom(formal))
                return 19;
              else
                return 100;
            }
            Type[] interfaces = actual.GetInterfaces();
            int i;
            int n = interfaces.Length;
            for (i = 0; i < n; i++)
              if (formal == interfaces[i]) return i+1;
            for (i = 0; actual != typeRefs.Object && actual != null; i++){
              if (formal == actual) return i+n+1;
              actual = actual.BaseType;
            }
            return i+n+1;
        }
        return 0; //should never get here
      }
      
      private static int TypeDistance(TypeReferences typeRefs, Type formal, Type actual, int distFromObject){
        if (formal == typeRefs.Object) return distFromObject;
        if (formal.IsEnum) return TypeDistance(typeRefs, Enum.GetUnderlyingType(formal), actual)+10;
        return 100;
      }
    }
}
