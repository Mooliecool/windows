//------------------------------------------------------------------------------
// <copyright file="ForEachAction.cs" company="Microsoft">
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

namespace System.Xml.Xsl.XsltOld {
    using Res = System.Xml.Utils.Res;
    using System;
    using System.Diagnostics;
    using System.Collections;
    using System.Xml;
    using System.Xml.XPath;

    internal class ForEachAction : ContainerAction {
        private const int    ProcessedSort     = 2;
        private const int    ProcessNextNode   = 3;
        private const int    PositionAdvanced  = 4;
        private const int    ContentsProcessed = 5;
        
        private int       selectKey = Compiler.InvalidQueryKey;
        private ContainerAction sortContainer;

        internal override void Compile(Compiler compiler) {
            CompileAttributes(compiler);
            CheckRequiredAttribute(compiler, selectKey != Compiler.InvalidQueryKey, Keywords.s_Select);

            compiler.CanHaveApplyImports = false;
            if (compiler.Recurse()) {
                CompileSortElements(compiler);
                CompileTemplate(compiler);
                compiler.ToParent();
            }
        }

        internal override bool CompileAttribute(Compiler compiler) {
            string name   = compiler.Input.LocalName;
            string value  = compiler.Input.Value;
            if (Keywords.Equals(name, compiler.Atoms.Select)) {
                this.selectKey = compiler.AddQuery(value);
            }
            else {
                return false;
            }

            return true;
        }

        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);

            switch (frame.State) {
            case Initialized:
                if (sortContainer != null) {
                    processor.InitSortArray();
                    processor.PushActionFrame(sortContainer, frame.NodeSet);
                    frame.State = ProcessedSort;
                    break;
                }
                goto case ProcessedSort; 
            case ProcessedSort:
                frame.InitNewNodeSet(processor.StartQuery(frame.NodeSet, this.selectKey));
                if (sortContainer != null) {
                    Debug.Assert(processor.SortArray.Count != 0);
                    frame.SortNewNodeSet(processor, processor.SortArray);
                }
                frame.State = ProcessNextNode;
                goto case ProcessNextNode;

            case ProcessNextNode:
                Debug.Assert(frame.State == ProcessNextNode);
                Debug.Assert(frame.NewNodeSet != null);

                if (frame.NewNextNode(processor)) {
                    frame.State = PositionAdvanced;
                    goto case PositionAdvanced;
                }
                else {
                    frame.Finished();
                    break;
                }

            case PositionAdvanced:
                processor.PushActionFrame(frame, frame.NewNodeSet);
                frame.State = ContentsProcessed;
                break;

            case ContentsProcessed:
                frame.State = ProcessNextNode;
                goto case ProcessNextNode;
            }
        }

        protected void CompileSortElements(Compiler compiler) {
            NavigatorInput input = compiler.Input;            
            do {
                switch(input.NodeType) {
                case XPathNodeType.Element:
                    if (Keywords.Equals(input.NamespaceURI, input.Atoms.XsltNamespace) &&
                        Keywords.Equals(input.LocalName, input.Atoms.Sort)) {
                        if (sortContainer == null) {
                            sortContainer = new ContainerAction();
                        }
                        sortContainer.AddAction(compiler.CreateSortAction());
                        continue;
                    }
                    return;
                case XPathNodeType.Text:
                    return;
                case XPathNodeType.SignificantWhitespace:
                    this.AddEvent(compiler.CreateTextEvent());
                    continue;
                default :
                    continue;
                }
            }
            while (input.Advance());
        }
    }
}
