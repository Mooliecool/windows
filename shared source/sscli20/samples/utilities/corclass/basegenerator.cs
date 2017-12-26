//------------------------------------------------------------------------------
// <copyright file="basegenerator.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------



namespace DefaultNamespace 
{
  using System.Text;
  using System.Runtime.Remoting;
  using System.ComponentModel;

  using System.Diagnostics;
  using System;
  using System.Reflection;
  using System.Collections;

  internal abstract class BaseGenerator 
  {

    public static string lineSeparator = "\n";

    protected class MemberInfoComparer : IComparer 
    {

      public static readonly MemberInfoComparer comp = new MemberInfoComparer();

      public virtual int Compare(Object x, Object y) 
      {
        if (x == null || y == null) 
        {
          return 0;
        }

        string leftMethod = ((MemberInfo)x).Name;
        string rightMethod = ((MemberInfo)y).Name;
        int res = String.Compare(((MemberInfo)x).Name, ((MemberInfo)y).Name);
        if (res == 0 && x is MethodBase && y is MethodBase) 
        {
          ParameterInfo[] left = ((MethodBase) x).GetParameters();
          ParameterInfo[] right = ((MethodBase) y).GetParameters();

          int i = 0;
          while (res == 0 && i < left.Length && i < right.Length) 
          {
            string leftName = left[i].Name;
            string rightName = right[i].Name;
            if (leftName == null || rightName == null)
              res = 0;
            else
              res = String.Compare(leftName, rightName);
            i++;
          }
          if (res == 0) // length mismatch--longer is considered smaller
            res = left.Length - right.Length;
        }
        return res;
      }
    }

    //
    public static bool  showInherited = true;
    // supress banners, class not found errors, etc.
    public static bool  beQuiet = false;
    // show full package qualifiers and type names
    public static bool  showFullNames = false;
    // different status fields.
    public static bool  showPackage = true;
    public static bool  showPublic = true;
    public static bool  showPrivate = true;
    public static bool  showProtected = true;
    public static bool  showModule = true;

    // Whether to display some visual indication about the presence or absence 
    // of an "Advanced" attribute.
    public static bool showAdvancedness = false;

    //
    public static bool  allowAbbreviations = true;
    public static TypeNames  defaultAbbreviation = TypeNames.Short;

    protected static bool IsVirtual(MethodBase method) 
    {
      return method.IsVirtual;
    }

    protected static bool IsPropertyGet(MethodBase method) 
    {
      return method.Name.StartsWith("get_");
    }

    protected static bool IsPropertySet(MethodBase method) 
    {
      return method.Name.StartsWith("set_");
    }

    protected static bool IsPropertyAccessor(MethodBase method) 
    {
      return IsPropertyGet(method) || IsPropertySet(method);
    }

    protected static bool IsPropertyAccessor(MemberInfo member) 
    {
      return member is MethodBase && IsPropertyAccessor((MethodBase) member);
    }

    protected virtual void AppendMethodVisibility(StringBuilder sb, MethodBase mtd) 
    {
      // Since these are mutually exclusive, a cascading if() works fine.
      if (mtd.IsPublic)
        sb.Append("public ");
      else if (mtd.IsPrivate)
        sb.Append("private ");
      else if (mtd.IsFamily)
        sb.Append("protected ");
      else
        sb.Append("internal ");
    }

    /// <summary>
    /// Retrieve the return type of the method.  note that we can only do so
    /// on a normal method, not on a ctor.  to detect the difference, we check</summary>
    /// that the parameter is an instanceof MethodInfo.<param name="sb"></param>
    /// <param name="mi"></param>
    /// <param name="options"></param>
    protected virtual void AppendMethodReturnType( StringBuilder sb, MethodInfo mi, TypeNames options ) 
    {
      AppendTypeName(sb, mi.ReturnType, options);
      sb.Append(" ");
    }
    
    /// <summary>
    /// GetModifiers returns a string representing the modifiers on a method.
    /// The method only applies to an actual method, not a constructor.  It</summary>
    /// determines the difference by testing instanceof MethodInfo.<param name="sb"></param>
    /// <param name="mi"></param>
    protected virtual void AppendMethodModifiers( StringBuilder sb, MethodInfo mi ) 
    {
      // Test the various interesting bits of the method, and build up
      // a string describing them.
      if (IsVirtual(mi)) 
      {
        string name = mi.Name;
        bool isOverride = false;
        try 
        {
          if (null != mi.DeclaringType.BaseType.GetMethod(name, BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance | BindingFlags.Static))
            isOverride = true;
        }
        catch (Exception) 
        {
          isOverride = true;
        }

        if (isOverride)
          sb.Append( "override ");
        else
          sb.Append( "virtual ");
      }
      if (mi.IsStatic)
        sb.Append( "static ");
      if (((MethodAttributes)mi.Attributes & MethodAttributes.PinvokeImpl) == MethodAttributes.PinvokeImpl)
        sb.Append( "extern ");
    }

    protected virtual void AppendArrayBrackets( StringBuilder sb, Type cls ) 
    {
      while (cls.IsArray && cls != typeof(Array)) 
      {
        cls = cls.GetElementType();
        sb.Append("[]");
      }
    }

    internal virtual Type GetUnderlyingType(Type cls) 
    {
      while (cls.IsArray && cls != typeof(Array)) 
      {
        cls = cls.GetElementType();
      }
      return cls;
    }

    protected virtual void AppendTypeName( StringBuilder sb, Type cls, TypeNames options ) 
    {
      Type baseVar = GetUnderlyingType(cls);
      String name = null;
      if ((options & TypeNames.PkgNames) == TypeNames.PkgNames)
        name = baseVar.FullName;
      else
        name = baseVar.Name;

      if ((options & TypeNames.AbbrPrimitives) == TypeNames.AbbrPrimitives
        && PrimitiveName(baseVar) != null)
        name = PrimitiveName(baseVar);

      if ((options & TypeNames.AbbrPkgNames) == TypeNames.AbbrPkgNames) 
      {
        int lastPeriod = name.LastIndexOf( '.' );
        if (lastPeriod != -1) 
        {
          String pfx = name.Substring( 0, lastPeriod );
          if (pfx.Equals( "System" ))
            name = name.Substring( lastPeriod + 1 );
        }

        name = AbbreviateName(name, "System.Collections", "S.C");
        name = AbbreviateName(name, "System.Security", "S.S");
        name = AbbreviateName(name, "System", "S");
        name = AbbreviateName(name, "Microsoft", "M");
      }

      sb.Append(name);
      AppendArrayBrackets(sb, cls);
    }

    // null if not a primitive
    internal virtual String PrimitiveName(Type cls) 
    {
      if (cls == typeof(System.Int32))
        return "int";
      else if (cls == typeof(System.Int64))
        return "long";
      else if (cls == typeof(System.Int16))
        return "short";
      else if (cls == typeof(System.Boolean))
        return "bool";
      else if (cls == typeof(System.Byte))
        return "byte";
      else if (cls == typeof(System.Char))
        return "char";
      else if (cls == typeof(System.Single))
        return "float";
      else if (cls == typeof(System.Double))
        return "double";
      else if (cls == typeof(void))
        return "void";
      else if (cls == typeof(System.Object))
        return "object";
      else if (cls == typeof(System.String))
        return "string";
      else
        return null;
    }

    public static String AbbreviateName(String name, String search, String replace) 
    {
      if (allowAbbreviations) 
      {
        if (name.StartsWith(search + ".")) 
        {
          name = replace + "." + name.Substring(search.Length + 1);
        }
      }
      return name;
    }

    /// <summary>
    /// Get the modifiers for the class (public, final, abstract).
    /// </summary>
    /// <param name="sb"></param>
    /// <param name="cls"></param>
    protected virtual void AppendClassModifiers( StringBuilder sb, Type cls ) 
    {
      if (cls.IsPublic)
        sb.Append("public ");

      if (cls.IsSealed)
        sb.Append("sealed ");

      if (cls.IsAbstract)
        sb.Append("abstract ");
    }

    /// <summary>
    /// Retrieves the modifiers on the field (final, static).
    /// </summary>
    /// <param name="sb"></param>
    /// <param name="f"></param>
    protected virtual void AppendFieldModifiers( StringBuilder sb, FieldInfo f ) 
    {
      if (f.IsStatic)
        sb.Append("static ");
      if (f.IsInitOnly)
        sb.Append("readonly ");
      if (f.IsLiteral)
        sb.Append("const ");
    }

    /// <summary>
    /// Retrieves the type of the field, checking for an array type
    /// </summary>
    /// <param name="sb"></param>
    /// <param name="f"></param>
    /// <param name="options"></param>
    protected virtual void AppendFieldType( StringBuilder sb, FieldInfo f, TypeNames options ) 
    {
      // get the base type of the field from the fieldinfo
      AppendTypeName(sb, f.FieldType, options);
      sb.Append(" ");
    }

    /// <summary>
    /// GetVisibility retrieves the visibility of the field (public, etc.).
    /// </summary>
    /// <param name="sb"></param>
    /// <param name="f"></param>
    protected virtual void AppendFieldVisibility(StringBuilder sb, FieldInfo f ) 
    {
      // Since these are mutually exclusive, a cascading if() works fine.
      if (f.IsPublic)
        sb.Append("public ");
      else if (f.IsPrivate)
        sb.Append("private ");
      else if (f.IsFamily)
        sb.Append("protected ");
    }

    private IList SortMemberInfo(MemberInfo[] members) 
    {
      if (members == null) 
      {
        return new ArrayList();
      }
      Array.Sort(members,MemberInfoComparer.comp);            
      return new ArrayList(members);
    }

    public String GetClassCode(Type cls) 
    {
      Console.Error.WriteLine(cls.FullName);

      StringBuilder sb = new StringBuilder(1024);

      // get all the fields, not just the public ones
      IList fields = SortMemberInfo(cls.GetFields(BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance | BindingFlags.Static));
      IList cons = SortMemberInfo(cls.GetConstructors( BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance | BindingFlags.Static ));
      IList methods = SortMemberInfo(cls.GetMethods( BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance | BindingFlags.Static ));
      IList properties = SortMemberInfo(cls.GetProperties());


      // get the class/interface signature
      AppendClassSignature(sb, cls);

      if (fields.Count > 0) 
      {
        AppendFieldHeader(sb);
        for (int  i=0; i<fields.Count; i++) 
        {
          FieldInfo fi = (FieldInfo)fields[i];
          if (Show(fi))
            AppendFieldSignature(sb, fi);
        }
        AppendFieldFooter(sb);
      }

      if (cons.Count > 0) 
      {
        AppendConstructorHeader(sb);
        for (int  i=0; i<cons.Count; i++) 
        {
          ConstructorInfo ctd = (ConstructorInfo)cons[i];
          if (Show(ctd))
            AppendConstructorSignature(sb, ctd);
        }
        AppendConstructorFooter(sb);
      }

      if (properties.Count > 0) 
      {
        AppendPropertyHeader(sb);
        for (int  i=0; i<properties.Count; i++) 
        {
          PropertyInfo property = (PropertyInfo)properties[i];
          if (Show(property))
            AppendPropertySignature(sb, property);
        }
        AppendPropertyFooter(sb);
      }

      if (methods.Count > 0) 
      {
        AppendMethodHeader(sb);
        for (int  i=0; i<methods.Count; i++) 
        {
          MethodInfo mtd = (MethodInfo)methods[i];
          if (Show(mtd))
            AppendMethodSignature(sb, mtd);
        }
        AppendMethodFooter(sb);
      }

      AppendClassTrailer(sb, cls);
      return sb.ToString();
    }

    public static bool Show(Type type) 
    {
      if (type == null)
        return false;

      bool isPublic = type.IsPublic;

      return(showPublic && isPublic) ||
        (showPackage && !isPublic);
    }

    public static bool Show(MemberInfo member) 
    {
      if (member == null)
        return false;

      if (member is ConstructorInfo) 
      {
        ConstructorInfo constructor = (ConstructorInfo) member;
        //                return true;
      }
      else if (member is MethodBase) 
      {
        MethodBase meth = (MethodBase)member;
        if (meth.IsSpecialName) 
        {
          return false;
        }
      }
      else if (member is PropertyInfo) 
      {
        MethodInfo method = ((PropertyInfo) member).GetGetMethod();
        if (method != null) 
        {
          MethodAttributes attributes = method.Attributes;

          bool showByProtectionLevel = ((showPublic && method.IsPublic) ||
            (showProtected && method.IsFamily) ||
            (showPrivate && method.IsPrivate) ||
            (showPackage && method.IsAssembly));
          // bool isInherited = method.DeclaringType != method.ReflectedType;
          bool isInherited = member.DeclaringType != member.ReflectedType;
          bool showByInheritedStatus = (showInherited || !isInherited);

          return showByProtectionLevel && showByInheritedStatus;
        }
        else
          return true;
      }

      /*
            */
      if (member is MethodBase) 
      {
        MethodBase method = (MethodBase) member;
        MethodAttributes attributes = method.Attributes;

        bool showByProtectionLevel = ((showPublic && method.IsPublic) ||
          (showProtected && method.IsFamily) ||
          (showPrivate && method.IsPrivate) ||
          (showPackage && method.IsAssembly));
        bool isInherited = method.DeclaringType != method.ReflectedType;
        bool showByInheritedStatus = (showInherited || !isInherited);

        return showByProtectionLevel && showByInheritedStatus && !IsPropertyAccessor(method);
      }
      else 
      {
        FieldInfo field = (FieldInfo) member;
        if (field == null)
          return false;

        bool isPublic = field.IsPublic;
        bool isProtected = field.IsFamily;
        bool isPrivate = field.IsPrivate;
        bool isPackage = !(isPublic || isProtected || isPrivate);
        bool isInherited = (field.DeclaringType != field.ReflectedType);

        return((showPublic && isPublic) ||
          (showProtected && isProtected) ||
          (showPrivate && isPrivate) ||
          (showPackage && isPackage)) &&
          (!isInherited || showInherited);
      }

    }

    public static bool Show(FieldInfo field) 
    {
      if (field == null)
        return false;

      bool isPublic = field.IsPublic;
      bool isProtected = field.IsFamily;
      bool isPrivate = field.IsPrivate;
      bool isPackage = !(isPublic || isProtected || isPrivate);
      bool isInherited = (field.DeclaringType != field.ReflectedType);

      return((showPublic && isPublic) ||
        (showProtected && isProtected) ||
        (showPrivate && isPrivate) ||
        (showPackage && isPackage)) &&
        (!isInherited || showInherited);
    }

    public abstract void AppendConstructorHeader(StringBuilder sb);
    public abstract void AppendConstructorFooter(StringBuilder sb);
    public abstract void AppendFieldHeader(StringBuilder sb);
    public abstract void AppendFieldFooter(StringBuilder sb);
    public abstract void AppendMethodHeader(StringBuilder sb);
    public abstract void AppendMethodFooter(StringBuilder sb);
    public abstract void AppendPropertyHeader(StringBuilder sb);
    public abstract void AppendPropertyFooter(StringBuilder sb);

    public abstract void AppendClassSignature(StringBuilder sb, Type c);
    public abstract void AppendClassTrailer(StringBuilder sb, Type c);

    public abstract void AppendConstructorSignature(StringBuilder sb, ConstructorInfo mi);
    public abstract void AppendFieldSignature(StringBuilder sb, FieldInfo fi);
    public abstract void AppendMethodSignature(StringBuilder sb, MethodInfo mi);
    public abstract void AppendPropertySignature(StringBuilder sb, PropertyInfo pi);
  }

}
