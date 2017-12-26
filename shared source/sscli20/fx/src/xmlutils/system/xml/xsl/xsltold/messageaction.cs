//------------------------------------------------------------------------------
// <copyright file="MessageAction.cs" company="Microsoft">
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
    using System.IO;
    using System.Globalization;
    using System.Diagnostics;
    using System.Xml;
    using System.Xml.XPath;

    internal class MessageAction : ContainerAction {
        bool _Terminate;

        internal override void Compile(Compiler compiler) {
            CompileAttributes(compiler);

            if (compiler.Recurse()) {
                CompileTemplate(compiler);
                compiler.ToParent();
            }
        }

        internal override bool CompileAttribute(Compiler compiler) {
            string name   = compiler.Input.LocalName;
            string value  = compiler.Input.Value;
            if (Keywords.Equals(name, compiler.Atoms.Terminate)) {
                _Terminate = compiler.GetYesNo(value);
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
                TextOnlyOutput output = new TextOnlyOutput(processor, new StringWriter(CultureInfo.InvariantCulture));
                processor.PushOutput(output);
                processor.PushActionFrame(frame);
                frame.State = ProcessingChildren;
                break;

            case ProcessingChildren:
                TextOnlyOutput recOutput = processor.PopOutput() as TextOnlyOutput;
                Debug.Assert(recOutput != null);
                Console.WriteLine(recOutput.Writer.ToString());

                if (_Terminate) {
                    throw XsltException.Create(Res.Xslt_Terminate, recOutput.Writer.ToString());
                }
                frame.Finished();
                break;
            
            default:
                Debug.Fail("Invalid MessageAction execution state");
                break;
            }
        }
    }
}
