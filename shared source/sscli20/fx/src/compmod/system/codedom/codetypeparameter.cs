//------------------------------------------------------------------------------
// <copyright file="CodeTypeParameter.cs" company="Microsoft">
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

namespace System.CodeDom {

    using System.Diagnostics;
    using System;
    using Microsoft.Win32;
    using System.Collections;
    using System.Reflection;
    using System.Runtime.InteropServices;

    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeTypeParameter : CodeObject {
        private string name;
        private CodeAttributeDeclarationCollection customAttributes;
        private CodeTypeReferenceCollection constraints;
        private bool hasConstructorConstraint;

        public CodeTypeParameter() {
        }

        public CodeTypeParameter(string name) {
            this.name = name;
        }

        public string Name {
            get {
                return (name == null) ? string.Empty : name;
            }
            set {
                name = value;
            }
        }

        public CodeTypeReferenceCollection Constraints {  
            get {
                if (constraints == null) {
                    constraints = new CodeTypeReferenceCollection();
                }
                return constraints;
            }
        } 

        public CodeAttributeDeclarationCollection CustomAttributes {
            get {
                if (customAttributes == null) {
                    customAttributes = new CodeAttributeDeclarationCollection();
                }
                return customAttributes;
            }
        }

        public bool HasConstructorConstraint {
            get {
                return hasConstructorConstraint;
            } 
            set {
                hasConstructorConstraint = value;
            }
        }

    }
}


