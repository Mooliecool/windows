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
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel.Design;
using System.ComponentModel.Design.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.Activities;
using System.Workflow.ComponentModel.Design;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.ObjectModel;
using System.IO;
using System.Xml;
using System.Workflow.ComponentModel.Serialization;

namespace Microsoft.Samples.Workflow.DesignerControl
{
    public partial class WorkflowDesignerControl : UserControl, IDisposable, IServiceProvider
    {
        private WorkflowView            workflowView;
        private DesignSurface           designSurface;
        private WorkflowLoader          loader;

        public WorkflowDesignerControl()
        {
            InitializeComponent();
            
            WorkflowTheme.CurrentTheme.ReadOnly = false;
            WorkflowTheme.CurrentTheme.AmbientTheme.ShowConfigErrors = false;
            WorkflowTheme.CurrentTheme.ReadOnly = true;            
        }

        private string xamlValue;

        public string Xaml
        {
            get
            {
                string xaml = string.Empty;
                if (this.loader != null)
                {
                    this.loader.Flush();
                    xaml = this.loader.Xaml;
                }
                return xaml;
            }

            set
            {
                this.xamlValue = value;
                if (!String.IsNullOrEmpty(value))
                    LoadWorkflow();
            }
        }

        private Type workflowTypeValue =null;

        public Type WorkflowType
        {
            get { return workflowTypeValue; }
            set { workflowTypeValue= value;
                if (value != null) LoadWorkflow();
            }
        }
    

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                UnloadWorkflow();
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        new public object GetService(Type serviceType)
        {
            return (this.workflowView != null) ? ((IServiceProvider)this.workflowView).GetService(serviceType) : null;
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);            
        }
        
        private void LoadWorkflow()
        {
            SuspendLayout();
            
            DesignSurface designSurface = new DesignSurface();
            WorkflowLoader loader = new WorkflowLoader();
            loader.WorkflowType = workflowTypeValue;
            loader.Xaml = xamlValue;            
            designSurface.BeginLoad(loader);

            IDesignerHost designerHost = designSurface.GetService(typeof(IDesignerHost)) as IDesignerHost;
            if (designerHost != null && designerHost.RootComponent != null)
            {
                IRootDesigner rootDesigner = designerHost.GetDesigner(designerHost.RootComponent) as IRootDesigner;
                if (rootDesigner != null)
                {
                    UnloadWorkflow();

                    this.designSurface = designSurface;
                    this.loader = loader;
                    this.workflowView = rootDesigner.GetView(ViewTechnology.Default) as WorkflowView;
                    this.panel1.Controls.Add(this.workflowView);
                    this.workflowView.Dock = DockStyle.Fill;
                    this.workflowView.TabIndex = 1;
                    this.workflowView.TabStop = true;
                    this.workflowView.HScrollBar.TabStop = false;
                    this.workflowView.VScrollBar.TabStop = false;
                    this.workflowView.Focus();
                    this.workflowView.FitToScreenSize();
                }
            }

            ResumeLayout(true);            
        }


        public event EventHandler SelectionChanged
        {
            add
            {
                ISelectionService selectionService = GetService(typeof(ISelectionService)) as ISelectionService;
                if (selectionService != null)
                    selectionService.SelectionChanged += new EventHandler(value);
            }
            remove
            {
                ISelectionService selectionService = GetService(typeof(ISelectionService)) as ISelectionService;
                if (selectionService != null)
                    selectionService.SelectionChanged -= new EventHandler(value);
            }
        }

        private void UnloadWorkflow()
        {
            IDesignerHost designerHost = GetService(typeof(IDesignerHost)) as IDesignerHost;
            if (designerHost != null && designerHost.Container.Components.Count > 0)
                WorkflowLoader.DestroyObjectGraphFromDesignerHost(designerHost, designerHost.RootComponent as Activity);
           
            if (this.designSurface != null)
            {
                this.designSurface.Dispose();
                this.designSurface = null;
            }

            if (this.workflowView != null)
            {
                Controls.Remove(this.workflowView);
                this.workflowView.Dispose();
                this.workflowView = null;
            }
        }

        public void ShowDefaultWorkflow()
        {
            SequentialWorkflowActivity workflow = new SequentialWorkflowActivity();
            workflow.Name = "Workflow1";

            using (StringWriter stringWriter = new StringWriter())
            {
                using (XmlWriter xmlWriter = XmlWriter.Create(stringWriter))
                {
                    WorkflowMarkupSerializer serializer = new WorkflowMarkupSerializer();
                    serializer.Serialize(xmlWriter, workflow);
                    this.Xaml = stringWriter.ToString();
                }
            }
        }
    }
}
