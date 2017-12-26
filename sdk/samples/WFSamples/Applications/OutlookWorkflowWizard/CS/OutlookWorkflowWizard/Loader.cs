//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.IO;
using System.Xml;
using System.ComponentModel.Design;
using System.ComponentModel.Design.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.ComponentModel.Serialization;

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    internal sealed class Loader : WorkflowDesignerLoader
    {
        private string xamlFile = string.Empty;

        public override string FileName
        {
            get
            {
                return this.xamlFile;
            }
        }

        internal string XamlFile
        {
            get
            {
                return this.xamlFile;
            }
            set
            {
                this.xamlFile = value;
            }
        }

        public override TextReader GetFileReader(string file)
        {
            return null;
        }

        public override TextWriter GetFileWriter(string file)
        {
            return null;
        }

        protected override void PerformLoad(IDesignerSerializationManager serializationManager)
        {
        }

        protected override void PerformFlush(IDesignerSerializationManager manager)
        {
        }

        public void PerformFlush(IDesignerHost host)
        {
            if (host != null && host.RootComponent != null)
            {
                Activity service = host.RootComponent as Activity;

                if (service != null)
                {
                    using (XmlWriter writer = XmlWriter.Create(this.xamlFile))
                    {
                        WorkflowMarkupSerializer xamlSerializer = new WorkflowMarkupSerializer();
                        xamlSerializer.Serialize(writer, service);
                    }
                }
            }
        }
    }
}
