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
using System.Text;
using System.ComponentModel.Design;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Workflow.Activities;
using System.Workflow.ComponentModel.Design;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    class WorkflowViewWrapper : System.Windows.Forms.Panel
    {
        internal IDesignerHost Host;
        internal SequentialWorkflowActivity SequentialWorkflow;

        private WorkflowView workflowView;
        private Loader loader; 
        private DesignSurface surface;

        public WorkflowViewWrapper()
        {
            this.loader = new Loader();

            // Create a Workflow Design Surface
            this.surface = new DesignSurface();
            this.surface.BeginLoad(this.loader);

            // Get the Workflow Designer Host
            this.Host = this.surface.GetService(typeof(IDesignerHost)) as IDesignerHost;

            if (this.Host == null)
                return;

            // Create a Sequential Workflow by using the Workflow Designer Host
            SequentialWorkflow = (SequentialWorkflowActivity)Host.CreateComponent(typeof(SequentialWorkflowActivity));
            SequentialWorkflow.Name = "CustomOutlookWorkflow";

            // Create a Workflow View on the Workflow Design Surface
            this.workflowView = new WorkflowView(this.surface as IServiceProvider);

            // Add a message filter to the workflow view, to support panning
            MessageFilter filter = new MessageFilter(this.surface as IServiceProvider, this.workflowView);
            this.workflowView.AddDesignerMessageFilter(filter);

            // Activate the Workflow View
            this.Host.Activate();

            this.workflowView.Dock = DockStyle.Fill;
            this.Controls.Add(workflowView);
            this.Dock = DockStyle.Fill;
        }

        public void PerformSave()
        {
            if (this.XamlFile.Length != 0)
            {
                this.SaveExistingWorkflow(this.XamlFile);
            }
            else
            {
                SaveFileDialog saveFileDialog = new SaveFileDialog();
                saveFileDialog.Filter = "Workflow XAML files (*.xoml)|*.xoml|All files (*.*)|*.*";
                saveFileDialog.FilterIndex = 1;
                saveFileDialog.RestoreDirectory = true;

                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    this.SaveExistingWorkflow(saveFileDialog.FileName);
                    this.Text = "Designer Hosting Sample -- [" + saveFileDialog.FileName + "]";
                }
            }
        }

        internal void SaveExistingWorkflow(string filePath)
        {
            if (this.surface != null && this.loader != null)
            {
                this.loader.XamlFile = filePath;
                this.loader.PerformFlush(this.Host);
            }
        }

        public string XamlFile
        {
            get
            {
                return this.loader.XamlFile;
            }
            set
            {
                this.loader.XamlFile = value;
            }
        }
    }

    internal sealed class MessageFilter : WorkflowDesignerMessageFilter
    {
        private bool mouseDown;
        private IServiceProvider serviceProvider;
        private WorkflowView workflowView;

        public MessageFilter(IServiceProvider provider, WorkflowView view)
        {
            this.serviceProvider = provider;
            this.workflowView = view;
        }

        protected override bool OnMouseDown(MouseEventArgs eventArgs)
        {
            // Allow other components to process this event by not returning true.
            this.mouseDown = true;
            return false;
        }

        protected override bool OnMouseMove(MouseEventArgs eventArgs)
        {
            // Allow other components to process this event by not returning true.
            if (this.mouseDown)
            {
                this.workflowView.ScrollPosition = new Point(eventArgs.X, eventArgs.Y);
            }
            return false;
        }

        protected override bool OnMouseUp(MouseEventArgs eventArgs)
        {
            // Allow other components to process this event by not returning true.
            this.mouseDown = false;
            return false;
        }

        protected override bool OnMouseDoubleClick(MouseEventArgs eventArgs)
        {
            this.mouseDown = false;
            return true;
        }

        protected override bool OnMouseEnter(MouseEventArgs eventArgs)
        {
            // Allow other components to process this event by not returning true.
            this.mouseDown = false;
            return false;
        }

        protected override bool OnMouseHover(MouseEventArgs eventArgs)
        {
            // Allow other components to process this event by not returning true.
            this.mouseDown = false;
            return false;
        }

        protected override bool OnMouseLeave()
        {
            // Allow other components to process this event by not returning true.
            this.mouseDown = false;
            return false;
        }

        protected override bool OnMouseWheel(MouseEventArgs eventArgs)
        {
            this.mouseDown = false;
            return true;
        }

        protected override bool OnMouseCaptureChanged()
        {
            // Allow other components to process this event by not returning true.
            this.mouseDown = false;
            return false;
        }

        protected override bool OnDragEnter(DragEventArgs eventArgs)
        {
            return true;
        }

        protected override bool OnDragOver(DragEventArgs eventArgs)
        {
            return true;
        }

        protected override bool OnDragLeave()
        {
            return true;
        }

        protected override bool OnDragDrop(DragEventArgs eventArgs)
        {
            return true;
        }

        protected override bool OnGiveFeedback(GiveFeedbackEventArgs gfbevent)
        {
            return true;
        }

        protected override bool OnQueryContinueDrag(QueryContinueDragEventArgs qcdevent)
        {
            return true;
        }

        protected override bool OnKeyUp(KeyEventArgs eventArgs)
        {
            return true;
        }

        protected override bool OnShowContextMenu(Point menuPoint)
        {
            return true;
        }

        protected override bool OnKeyDown(KeyEventArgs eventArgs)
        {
            if (eventArgs.KeyCode == Keys.Delete)
            {
                ISelectionService selectionService = (ISelectionService)this.serviceProvider.GetService(typeof(ISelectionService));
                if (selectionService != null && selectionService.PrimarySelection is CodeActivity)
                {
                    CodeActivity codeActivityComponent = (CodeActivity)selectionService.PrimarySelection;
                    CompositeActivity parentActivity = codeActivityComponent.Parent;
                    if (parentActivity != null)
                    {
                        parentActivity.Activities.Remove(codeActivityComponent);
                        this.ParentView.Update();
                    }
                }
            }
            return true;
        }
    }
}
