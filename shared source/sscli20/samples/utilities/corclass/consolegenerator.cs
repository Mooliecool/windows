//------------------------------------------------------------------------------
// <copyright file="consolegenerator.cs" company="Microsoft">
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

  using System.Diagnostics;
  using System;
  using System.Reflection;
  using System.Collections;

  internal class ConsoleGenerator : BaseGenerator 
  {
    // set the tab indentation for methods
    protected static readonly String indentPrefix = "  ";

    public override void AppendConstructorHeader(StringBuilder sb) 
    {
      sb.Append(lineSeparator);
      sb.Append(indentPrefix);
      sb.Append("// Constructors");
      sb.Append(lineSeparator);
    }
    public override void AppendConstructorFooter(StringBuilder sb) 
    {
    }
    public override void AppendFieldHeader(StringBuilder sb) 
    {
      sb.Append(lineSeparator);
      sb.Append(indentPrefix);
      sb.Append("// Fields");
      sb.Append(lineSeparator);
    }
    public override void AppendFieldFooter(StringBuilder sb) 
    {
    }
    public override void AppendMethodHeader(StringBuilder sb) 
    {
      sb.Append(lineSeparator);
      sb.Append(indentPrefix);
      sb.Append("// Methods");
      sb.Append(lineSeparator);
    }
    public override void AppendMethodFooter(StringBuilder sb) 
    {
    }
    public override void AppendPropertyHeader(StringBuilder sb) 
    {
      sb.Append(lineSeparator);
      sb.Append(indentPrefix);
      sb.Append("// Properties");
      sb.Append(lineSeparator);
    }
    public override void AppendPropertyFooter(StringBuilder sb) 
    {
    }

    public override void AppendClassSignature(StringBuilder sb, Type cls) 
    {
      if (showModule)
        sb.Append("// from module \'" + cls.Module.FullyQualifiedName + "\'" + lineSeparator);

      // get the class modifiers
      AppendClassModifiers(sb, cls);

      // see if it's an interface or a class
      if (cls.IsInterface)
        sb.Append("interface ");
      else if (cls.IsEnum)
        sb.Append("enum ");
      else
        sb.Append("class ");

      // get the name, but don't translate the native type -- just remove the M.R.
      AppendTypeName(sb, cls, defaultAbbreviation);
      sb.Append(" ");

      // see if there's a base class
      Type sc = cls.BaseType;
      // see if there are implemented interfaces
      Type[] ifs = cls.GetInterfaces();

      if (sc != null || ifs.Length != 0)
        sb.Append(":");

      if (sc != null) 
      {
        sb.Append(lineSeparator);
        sb.Append(indentPrefix);
        AppendTypeName(sb, sc, defaultAbbreviation);
      }


      for (int i = 0; i < ifs.Length; i++) 
      {
        // first one?  need the implements keyword on a new line
        if (i != 0 || sc != null) 
        {
          sb.Append(", ");
          sb.Append(lineSeparator);
          sb.Append(indentPrefix);
        }

        // add the interface name
        AppendTypeName(sb, ifs[i], defaultAbbreviation);
      }

      sb.Append(lineSeparator);
      sb.Append("{");
      sb.Append(lineSeparator);
    }
    public override void AppendClassTrailer(StringBuilder sb, Type c) 
    {
      sb.Append("} // end of ");
      sb.Append(c.FullName);
      sb.Append(lineSeparator);
    }
    public override void AppendConstructorSignature(StringBuilder sb, ConstructorInfo mi) 
    {
      sb.Append(indentPrefix);
      AppendMethodVisibility(sb, mi);
      sb.Append(mi.Name);
      AppendParameters(sb, mi);
      sb.Append(";");
      sb.Append(lineSeparator);
    }
    public override void AppendFieldSignature(StringBuilder sb, FieldInfo fi) 
    {
      sb.Append(indentPrefix);
      AppendFieldVisibility(sb, fi);
      AppendFieldModifiers(sb, fi);
      AppendFieldType(sb, fi, defaultAbbreviation);
      sb.Append(fi.Name);
      sb.Append(";");
      sb.Append(lineSeparator);
    }

    private void AppendIndexParameters( StringBuilder sb, MethodBase mtd, TypeNames options ) 
    {
      sb.Append("[");

      // grab the parameter infos
      ParameterInfo[] pi = mtd.GetParameters();

      // if there are any parms...
      if (pi.Length != 0) 
      {
        // for each one...
        for (int i = 0; i < pi.Length; i++) 
        {
          // setup the proper spacing and/or separation
          if (i == 0)
            sb.Append(" ");
          else
            sb.Append(", ");

          // get the type and the name
          AppendTypeName(sb, pi[i].ParameterType, options);
          sb.Append(" ");
          sb.Append(pi[ i ].Name);
        }

        sb.Append(" ");
      }
      sb.Append("]");
    }

    public override void AppendMethodSignature(StringBuilder sb, MethodInfo mi) 
    {
      sb.Append(indentPrefix);
      AppendMethodVisibility(sb, mi);
      AppendMethodModifiers(sb, mi);
      AppendMethodReturnType(sb, mi, defaultAbbreviation);
      sb.Append(mi.Name);
      AppendParameters(sb, mi);
      sb.Append(";");
      sb.Append(lineSeparator);
    }

    private void AppendParameters( StringBuilder sb, MethodBase mtd ) 
    {
      AppendParameters( sb, mtd, defaultAbbreviation );
    }

    private void AppendParameters( StringBuilder sb, MethodBase mtd, TypeNames options ) 
    {
      sb.Append("(");

      // grab the parameter infos
      ParameterInfo[] pi = mtd.GetParameters();

      // if there are any parms...
      if (pi.Length != 0) 
      {
        // for each one...
        for (int i = 0; i < pi.Length; i++) 
        {
          // setup the proper spacing and/or separation
          if (i != 0)
            sb.Append(", ");

          // get the type and the name
          AppendTypeName(sb, pi[i].ParameterType, options);
          sb.Append(" ");
          sb.Append(pi[ i ].Name);
        }
      }
      sb.Append(")");
    }

    public override void AppendPropertySignature(StringBuilder sb, PropertyInfo pi) 
    {
      MethodInfo getter = pi.GetGetMethod();
      if (getter != null) 
      {
        sb.Append( indentPrefix );

        AppendMethodVisibility( sb, getter );

        AppendMethodReturnType( sb, getter, defaultAbbreviation );

        if ((pi.Name.Equals("Item") || pi.Name.Equals("Items"))
          && getter.GetParameters().Length != 0) 
        {
          sb.Append("this");
          AppendIndexParameters( sb, getter, defaultAbbreviation );
        }
        else 
        {
          sb.Append( pi.Name );
        }

        sb.Append( " {" );

        MethodInfo[] mi = pi.GetAccessors();
        String accessors = "";
        for (int i = 0; i < mi.Length; i++) 
        {
          String modifiers = (IsVirtual(mi[i])) ? " virtual" : "";
          if (IsPropertySet(mi[i]))
            accessors = accessors + modifiers + " set;";
          else
            accessors = modifiers + " get;" + accessors;
        }
        sb.Append( accessors + " }" );
        sb.Append(lineSeparator);
      }
    }
  }
}
