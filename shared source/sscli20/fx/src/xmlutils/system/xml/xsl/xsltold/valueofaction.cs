//------------------------------------------------------------------------------
// <copyright file="ValueOfAction.cs" company="Microsoft">
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
    using System.Xml;
    using System.Xml.XPath;

    internal class ValueOfAction : CompiledAction {
        private const int ResultStored = 2;

        private int    selectKey = Compiler.InvalidQueryKey;
        private bool   disableOutputEscaping;

        private static Action s_BuiltInRule = new BuiltInRuleTextAction();

        internal static Action BuiltInRule() {
            Debug.Assert(s_BuiltInRule != null);
            return s_BuiltInRule;
        }

        internal override void Compile(Compiler compiler) {
            CompileAttributes(compiler);
            CheckRequiredAttribute(compiler, selectKey != Compiler.InvalidQueryKey, Keywords.s_Select);
            CheckEmpty(compiler);
        }

        internal override bool CompileAttribute(Compiler compiler) {
            string name   = compiler.Input.LocalName;
            string value  = compiler.Input.Value;

            if (Keywords.Equals(name, compiler.Atoms.Select)) {
                this.selectKey = compiler.AddQuery(value);
            }
            else if (Keywords.Equals(name, compiler.Atoms.DisableOutputEscaping)) {
                this.disableOutputEscaping = compiler.GetYesNo(value);
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
                Debug.Assert(frame != null);
                Debug.Assert(frame.NodeSet != null);

                string value = processor.ValueOf(frame, this.selectKey);

                if (processor.TextEvent(value, disableOutputEscaping)) {
                    frame.Finished();
                }
                else {
                    frame.StoredOutput = value;
                    frame.State        = ResultStored;
                }
                break;

            case ResultStored:
                Debug.Assert(frame.StoredOutput != null);
                processor.TextEvent(frame.StoredOutput);
                frame.Finished();
                break;

            default:
                Debug.Fail("Invalid ValueOfAction execution state");
                break;
            }
        }
    }

    internal class BuiltInRuleTextAction : Action {
        private const int ResultStored = 2;
        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);

            switch (frame.State) {
            case Initialized:
                Debug.Assert(frame != null);
                Debug.Assert(frame.NodeSet != null);

                string value = processor.ValueOf(frame.NodeSet.Current);

                if (processor.TextEvent(value, /*disableOutputEscaping:*/false)) {
                    frame.Finished();
                }
                else {
                    frame.StoredOutput = value;
                    frame.State        = ResultStored;
                }
                break;

            case ResultStored:
                Debug.Assert(frame.StoredOutput != null);
                processor.TextEvent(frame.StoredOutput);
                frame.Finished();
                break;

            default:
                Debug.Fail("Invalid BuiltInRuleTextAction execution state");
                break;
            }
        }
    }    
}
