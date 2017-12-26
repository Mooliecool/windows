//------------------------------------------------------------------------------
// <copyright file="validationstate.cs" company="Microsoft">
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
// <owner current="true" primary="true">priyal</owner>                                                               
//------------------------------------------------------------------------------

namespace System.Xml.Schema {
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Explicit)]
    internal struct StateUnion {
        [FieldOffset(0)] public int State;  //DFA 
        [FieldOffset(0)] public int AllElementsRequired; //AllContentValidator
        [FieldOffset(0)] public int CurPosIndex; //NFAContentValidator
        [FieldOffset(0)] public int NumberOfRunningPos; //RangeContentValidator
    }

    internal sealed class ValidationState {
        
        public bool              IsNill;
        public bool              IsDefault;
        public bool              NeedValidateChildren;  // whether need to validate the children of this element   
        public bool CheckRequiredAttribute; //PSVI
        public bool ValidationSkipped;
        public int               Depth;         // The validation state  
        public XmlSchemaContentProcessing ProcessContents;
        public XmlSchemaValidity          Validity;
        public SchemaElementDecl ElementDecl;            // ElementDecl
        public SchemaElementDecl ElementDeclBeforeXsi; //elementDecl before its changed by that of xsi:type's
        public string LocalName;
        public string Namespace;
        public ConstraintStruct[] Constr;

        public StateUnion   CurrentState; 

        //For content model validation
        public bool HasMatched;       // whether the element has been verified correctly
        
        //For NFAs
        public BitSet[] CurPos = new BitSet[2];
    
        //For all
        public BitSet AllElementsSet;

        //For MinMaxNFA
        public List<RangePositionInfo>  RunningPositions;
        public bool                     TooComplex;
    };



}
  