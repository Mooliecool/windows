//------------------------------------------------------------------------------
// <copyright file="webgenerator.cs" company="Microsoft">
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


namespace DefaultNamespace {
    using System.Text;
    using System.Runtime.Remoting;

    using System.Diagnostics;
    using System;
    using System.Reflection;
    using System.Collections;

    internal class WebGenerator : BaseGenerator {
        internal virtual void AppendHeader(StringBuilder sb, String text) {
            sb.Append("<tr><td colspan=2><br></td></tr>");
        }

        public override void AppendConstructorHeader(StringBuilder sb) {
        AppendHeader(sb, "Constructors");
        }
        public override void AppendConstructorFooter(StringBuilder sb) {
        }
        public override void AppendFieldHeader(StringBuilder sb) {
        AppendHeader(sb, "Fields");
        }
        public override void AppendFieldFooter(StringBuilder sb) {
        }
        public override void AppendMethodHeader(StringBuilder sb) {
        AppendHeader(sb, "Methods");
        }
        public override void AppendMethodFooter(StringBuilder sb) {
        }
        public override void AppendPropertyHeader(StringBuilder sb) {
        AppendHeader(sb, "Properties");
        }
        public override void AppendPropertyFooter(StringBuilder sb) {
        }

        public override void AppendClassSignature(StringBuilder sb, Type cls) {
            sb.Append("<h1>");
            AppendTypeName(sb, cls, TypeNames.Long);
            sb.Append("</h1>");

            sb.Append("<i>");
            // get the class modifiers
            AppendClassModifiers(sb, cls);

            // see if it's an interface or a class
            if (cls.IsInterface)
                sb.Append("interface ");
            else
                sb.Append("class ");

            AppendTypeName(sb, cls, TypeNames.Short);
            sb.Append(" ");

            // see if there's a base class
            Type sc = cls.BaseType;
            // see if there are implemented interfaces
            Type[] ifs = cls.GetInterfaces();

            if (sc != null || ifs.Length != 0)
                sb.Append(": ");

            if (sc != null) {
                AppendTypeName(sb, sc, TypeNames.Short);
                sb.Append(" ");
            }

            for (int i = 0; i < ifs.Length; i++) {
                // first one?  need the implements keyword on a new line
                if (i != 0 || sc != null)
                    sb.Append(", ");

                // add the interface name
                AppendTypeName(sb, ifs[i], TypeNames.Short);
            }
            sb.Append("</i>");

            sb.Append("<table>\r\n");
        }
        public override void AppendClassTrailer(StringBuilder sb, Type c) {
            sb.Append("</table>\r\n");
        }
        public override void AppendConstructorSignature(StringBuilder sb, ConstructorInfo mi) {
            sb.Append("<tr><td>");
                AppendMethodVisibility(sb, mi);
            sb.Append("</td><td>");
            AppendMemberName(sb, mi.Name);
                AppendParameters(sb, mi);
            sb.Append("</td></tr>\n");
        }

        public virtual void AppendMemberName(StringBuilder sb, String name) {
        // Pure black and white looks better when printed
        // sb.Append("<b><font color=blue>");
        sb.Append("<b>");
        sb.Append(name);
        //sb.Append("</font></b>");
        sb.Append("</b>");
        }

        public override void AppendFieldSignature(StringBuilder sb, FieldInfo fi) {
            sb.Append("<tr>");
                if (fi == null) {
                    sb.Append("<td>");
                    sb.Append("??? <field>");
                    sb.Append("</td>");
                }
                else {
                    sb.Append("<td>");
            sb.Append("<table width=100% cellpadding=0 cellspacing=0><tr><td>");
                    AppendFieldVisibility(sb, fi);
                    AppendFieldModifiers(sb, fi);
            sb.Append("</td><td align=right>");
                    AppendFieldType(sb, fi, TypeNames.Short);
            sb.Append("</td></tr></table>");
            sb.Append("</td>");
                    sb.Append("<td style=\"font-weight: bold;\">");
            AppendMemberName(sb, fi.Name);
                    sb.Append("</td>");
                }
            sb.Append("</tr>");
        }

        public override void AppendMethodSignature(StringBuilder sb, MethodInfo mi) {
            sb.Append("<tr>");
                sb.Append("<td valign=top>");

                sb.Append("<table width=100% cellpadding=0 cellspacing=0><tr><td>");
                AppendMethodVisibility(sb, mi);
                AppendMethodModifiers(sb, mi);
                sb.Append("&nbsp;</td><td align=right>");
                AppendMethodReturnType(sb, mi, TypeNames.Short);
                sb.Append("</td></tr></table>");

                sb.Append("</td>");
                sb.Append("<td>");
            AppendMemberName(sb, mi.Name);
                AppendParameters(sb, mi);
                sb.Append("</td>");
            sb.Append("</tr>\n");
        }

        private void AppendParameters( StringBuilder sb, MethodBase mtd ) {
            AppendParameters( sb, mtd, TypeNames.Short );
        }

        private void AppendParameters( StringBuilder sb, MethodBase mtd, TypeNames options ) {
        sb.Append("(");

            // grab the parameter infos
            ParameterInfo[] pi = mtd.GetParameters();

            // if there are any parms...
            if (pi.Length != 0) {
                // for each one...
                for (int i = 0; i < pi.Length; i++) {
                    // setup the proper spacing and/or separation
                    if (i == 0)
                        sb.Append(" ");
                    else
                        sb.Append(", ");

                    // get the type and the name
            AppendTypeName(sb, pi[ i ].ParameterType, options );
            sb.Append("&nbsp;<i>");
            sb.Append(pi[ i ].Name);
            sb.Append("</i>");
                }

                sb.Append(" ");
            }
        sb.Append(")");
        }

        protected override void AppendTypeName( StringBuilder sb, Type cls, TypeNames options ) {
        sb.Append("<span title=\"");
        base.AppendTypeName(sb, cls, TypeNames.Long);
        sb.Append("\">");
            base.AppendTypeName(sb, cls, options);
        sb.Append("</span>");
        }

        private void AppendIndexParameters( StringBuilder sb, MethodBase mtd, TypeNames options ) {
            sb.Append("[");

            // grab the parameter infos
            ParameterInfo[] pi = mtd.GetParameters();

            // if there are any parms...
            if (pi.Length != 0) {
                // for each one...
                for (int i = 0; i < pi.Length; i++) {
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

        public override void AppendPropertySignature(StringBuilder sb, PropertyInfo pi) {
            MethodInfo getter = pi.GetGetMethod();
            if (getter != null) {
                sb.Append( "<tr>" );
                sb.Append( "<td valign=top>" );
                sb.Append( "<table width=100% cellpadding=0 cellspacing=0><tr><td>" );

                AppendMethodVisibility( sb, getter );
                sb.Append( "&nbsp;</td><td align=right>" );
                AppendMethodReturnType( sb, getter, TypeNames.Short );
                sb.Append( "</td></tr></table>" );
                sb.Append( "</td>" );
                sb.Append( "<td>" );
                
                                if ((pi.Name.Equals("Item") || pi.Name.Equals("Items"))
                    && getter.GetParameters().Length != 0) {
                AppendMemberName( sb, "this" );
                    AppendIndexParameters( sb, getter, defaultAbbreviation );
                }
                else {
                AppendMemberName( sb, pi.Name );
                }

                
                sb.Append( " {" );

                MethodInfo[] mi = pi.GetAccessors();
                String accessors = "";
                for (int i = 0; i < mi.Length; i++) {
                    String modifiers = (IsVirtual(mi[i])) ? " virtual" : "";
                    if (IsPropertySet(mi[i]))
                        accessors = accessors + modifiers + " set;";
                    else
                        accessors = modifiers + " get;" + accessors;
                }
                sb.Append( accessors + " }\n" );
               sb.Append( "</td>" );
                sb.Append("</tr>\n");
            }
        }
    
    }
}
