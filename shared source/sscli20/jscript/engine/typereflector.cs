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
    
    /* 
    This class provides a fast implementation of GetMember and GetDefaultMembers for use by LateBinding.
    The BCL implementation for System.Type is disasterously slow.
    This implementation trades memory for speed and does things lazily.
    
    It also replaces RuntimeMethodInfo instances with wrappers that cache the results of GetParameters and that provide an
    efficient implementation of Invoke.
    */
    
    using System;
    using System.Reflection;
    using System.Collections;
    
    public sealed class TypeReflector : ScriptObject{
      private MemberInfo[] defaultMembers;
      private SimpleHashtable staticMembers;
      private SimpleHashtable instanceMembers;
      private MemberInfo[][] memberInfos;
      private ArrayList memberLookupTable;
      internal Type type;
      private Object implementsIReflect;
      
      internal uint hashCode;
      internal TypeReflector next;
      
      private static MemberInfo[] EmptyMembers = new MemberInfo[0];
      private static TRHashtable Table = new TRHashtable();
      
      internal TypeReflector(Type type)
        : base(null){
        this.defaultMembers = null;
        ArrayList memberLookupTable = new ArrayList(512);
        int count = 0;
        SimpleHashtable staticMembers = new SimpleHashtable(256);
        foreach (MemberInfo member in type.GetMembers(BindingFlags.Public|BindingFlags.Static|BindingFlags.FlattenHierarchy)){
          String name = member.Name;
          Object ob = staticMembers[name];
          if (ob == null){
            staticMembers[name] = count++; //Avoid allocating an array until needed
            memberLookupTable.Add(member);
          }else{
            int index = (int)ob;
            ob = memberLookupTable[index];
            MemberInfo firstMember = ob as MemberInfo;
            if (firstMember != null){
              MemberInfoList mems = new MemberInfoList(); //Allocate a flexarray of MemberInfo, and turn it into a fixed array when GetMember is called for this name
              mems.Add(firstMember);
              mems.Add(member);
              memberLookupTable[index] = mems;
            }else
              ((MemberInfoList)ob).Add(member);
          }
        }
        this.staticMembers = staticMembers;
        SimpleHashtable instanceMembers = new SimpleHashtable(256);
        foreach (MemberInfo member in type.GetMembers(BindingFlags.Public|BindingFlags.Instance)){
          String name = member.Name;
          Object ob = instanceMembers[name];
          if (ob == null){
            instanceMembers[name] = count++; //Avoid allocating an array until needed
            memberLookupTable.Add(member);
          }else{
            int index = (int)ob;
            ob = memberLookupTable[index];
            MemberInfo firstMember = ob as MemberInfo;
            if (firstMember != null){
              MemberInfoList mems = new MemberInfoList();
              mems.Add(firstMember);
              mems.Add(member);
              memberLookupTable[index] = mems;
            }else
              ((MemberInfoList)ob).Add(member);
          }
        }
        this.instanceMembers = instanceMembers;
        this.memberLookupTable = memberLookupTable;
        this.memberInfos = new MemberInfo[count][];
        this.type = type;
        this.implementsIReflect = null;
        this.hashCode = (uint)type.GetHashCode();
        this.next = null;
      }
      
      internal MemberInfo[] GetDefaultMembers(){
        MemberInfo[] result = this.defaultMembers;
        if (result == null){
          result = JSBinder.GetDefaultMembers(this.type);
          if (result == null) result = new MemberInfo[0];
          TypeReflector.WrapMembers(this.defaultMembers = result);
        }
        return result;
      }
      
      public override MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        bool lookForInstanceMembers = (bindingAttr & BindingFlags.Instance) != 0;
        SimpleHashtable lookupTable = lookForInstanceMembers ? this.instanceMembers : this.staticMembers;
        Object ob = lookupTable[name];
        if (ob == null){
          if ((bindingAttr & BindingFlags.IgnoreCase) != 0)
            ob = lookupTable.IgnoreCaseGet(name);
          if (ob == null){
            if (lookForInstanceMembers && (bindingAttr & BindingFlags.Static) != 0)
              return this.GetMember(name, bindingAttr & ~BindingFlags.Instance);
            else
              return TypeReflector.EmptyMembers;
          }
        }
        int index = (int)ob;
        MemberInfo[] members = this.memberInfos[index];
        if (members == null)
          return this.GetNewMemberArray(name, index);
        return members;
      }
      
      private MemberInfo[] GetNewMemberArray(String name, int index){
        MemberInfo[] result = null;
        Object ob = this.memberLookupTable[index];
        // ob could be null if another thread got in here before us
        if (ob == null)
          return this.memberInfos[index];
        MemberInfo member = ob as MemberInfo;
        if (member != null)
          result = new MemberInfo[]{member};
        else
          result = ((MemberInfoList)ob).ToArray();
        this.memberInfos[index] = result;
        this.memberLookupTable[index] = null;
        TypeReflector.WrapMembers(result);
        return result;
      }
      
      public override MemberInfo[] GetMembers(BindingFlags bindingAttr){
        throw new JScriptException(JSError.InternalError);
      }
      
      internal static TypeReflector GetTypeReflectorFor(Type type){
        TypeReflector result = TypeReflector.Table[type];
        if (result != null) return result;
        result = new TypeReflector(type);
        lock(TypeReflector.Table){
          TypeReflector r = TypeReflector.Table[type];
          if (r != null) return r;
          TypeReflector.Table[type] = result;
        }
        return result;
      }
      
      internal bool ImplementsIReflect(){
        Object result = this.implementsIReflect;
        if (result != null) return (bool)result;
        bool bresult = typeof(IReflect).IsAssignableFrom(this.type);
        this.implementsIReflect = bresult;
        return bresult;
      }
      

      private static void WrapMembers(MemberInfo[] members){
        for (int i = 0, n = members.Length; i < n; i++){
          MemberInfo mem = members[i];
          switch (mem.MemberType){
            case MemberTypes.Field:
              members[i] = new JSFieldInfo((FieldInfo)mem); break;
            case MemberTypes.Method:
              members[i] = new JSMethodInfo((MethodInfo)mem); break;
            case MemberTypes.Property:
              members[i] = new JSPropertyInfo((PropertyInfo)mem); break;
          }
        }
      }
    }
    
    internal sealed class TRHashtable{
      private TypeReflector[] table;
      private int count;
      private int threshold;

      internal TRHashtable(){
    	  table = new TypeReflector[511];
        count = 0;
    	  threshold = 256;
      }

      internal TypeReflector this[Type type]{
        get{
      	  uint hashCode = (uint)type.GetHashCode();
      	  int index = (int)(hashCode % (uint)this.table.Length);
      	  TypeReflector tr = this.table[index];
      	  while (tr != null){
      	    if (tr.type == type) return tr;
      	    tr = tr.next;
      	  }
      	  return null;
      	}
      	set{
      	  Debug.Assert(this[type] == null);
          if (++this.count >= this.threshold) this.Rehash();
      	  int index = (int)(value.hashCode % (uint)this.table.Length);
      	  value.next = this.table[index];
      	  this.table[index] = value;
      	}
      }

      private void Rehash(){
    	  TypeReflector[] oldTable = this.table;
      	int threshold = this.threshold = oldTable.Length+1;
      	int newCapacity = threshold * 2 - 1;
    	  TypeReflector[] newTable = this.table = new TypeReflector[newCapacity];
    	  for (int i = threshold-1; i-- > 0;){
    	    for (TypeReflector old = oldTable[i]; old != null; ){
    	      TypeReflector tr = old; old = old.next;
          	  int index = (int)(tr.hashCode % (uint)newCapacity);
    	      tr.next = newTable[index];
    	      newTable[index] = tr;
    	    }
      	}
      }
    }
}    
