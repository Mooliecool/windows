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
/*============================================================
**
** Class:  SuppressMessageAttribute 
**
**
**  An attribute to suppress violation messages/warnings   
**  by static code analysis tools. 
**
** 
===========================================================*/

using System;

namespace System.Diagnostics.CodeAnalysis
{

    [AttributeUsage(
     AttributeTargets.All,
     Inherited = false,
     AllowMultiple = true
     )
    ]
    [Conditional("CODE_ANALYSIS")]
    public sealed class SuppressMessageAttribute : Attribute
    {
        private string category;
        private string justification;
        private string checkId;
        private string scope;
        private string target;
        private string messageId;
        
        public SuppressMessageAttribute(string category, string checkId)
        {
            this.category  = category;
            this.checkId = checkId;
        }
        
        public string Category
        {
            get { return category; }
        }
        
        public string CheckId
        {
            get { return checkId; }
        }
        
        public string Scope
        {
            get { return scope; }
            set { scope = value; }
        }
    
        public string Target
        {
            get { return target; }
            set { target = value; }
        }
    
        public string MessageId
        {
            get { return messageId; }
            set { messageId = value; }
        }
        
        public string Justification
        {
            get { return justification; }
            set { justification = value; }
        }
    }
}
