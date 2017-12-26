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
using System.ComponentModel.Design;
using System.ComponentModel.Design.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    //This type is used to load the workflow definition
    internal sealed class Loader : WorkflowDesignerLoader
    {
        private Activity workflowDefinitionValue = null;

        internal Loader()
        {
        }

        public override TextReader GetFileReader(string filePath)
        {
            return new StreamReader(filePath);
        }

        public override TextWriter GetFileWriter(string filePath)
        {
            return new StreamWriter(filePath);
        }

        public override string FileName
        {
            get
            {
                return string.Empty;
            }
        }

        internal Activity WorkflowDefinition
        {
            set
            {
                this.workflowDefinitionValue = value;
            }
        }

        //Load the workflow definition from WorkflowMarkup
        protected override void PerformLoad(IDesignerSerializationManager serializationManager)
        {
            IDesignerHost designerHost = (IDesignerHost)GetService(typeof(IDesignerHost));
            Activity activity = this.workflowDefinitionValue;

            //Add the rootactivity the designer
            if (activity != null && designerHost != null)
            {
                Helpers.AddObjectGraphToDesignerHost(designerHost, activity);
                SetBaseComponentClassName(activity.Name);
            }
        }

        protected override void PerformFlush(IDesignerSerializationManager manager)
        {
        }
    }
}
