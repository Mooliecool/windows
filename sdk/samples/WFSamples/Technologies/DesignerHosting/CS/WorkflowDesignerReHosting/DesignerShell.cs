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
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Windows.Forms;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.ComponentModel.Compiler;

namespace Microsoft.Samples.Workflow.WorkflowDesignerRehosting
{
    #region Class DesignerShell

    public partial class DesignerShell : Form
    {
        #region Members and Constructors

        private WorkflowViewPanel workflowPanel;

        public DesignerShell()
        {
            InitializeComponent();

            //Now initialize the contained components
            this.workflowPanel = new WorkflowViewPanel(this);
            this.workflowPanel.Dock = DockStyle.Fill;
            this.workflowPanel.Name = "WorkflowViewPanel";

            this.Panel2.Controls.Add(this.workflowPanel);

            //load the default workflow to start with
            this.workflowPanel.LoadDefaultWorkflow();
        }

        #endregion

        private void zoomNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (this.workflowPanel != null)
                this.workflowPanel.OnZoomChanged(Convert.ToInt16(this.zoomNumericUpDown.Value));
        }

        private void addButton_Click(object sender, EventArgs e)
        {
            SequentialWorkflowRootDesigner rootDesigner = this.workflowPanel.GetWorkflowView().RootDesigner as SequentialWorkflowRootDesigner;
            int viewId = rootDesigner.ActiveView.ViewId;
            if(viewId == 1)
            {
               this.workflowPanel.OnCodeActivityAdded();
            }
            else
            {
               DialogResult resultBox = MessageBox.Show("This sample supports adding a code activity only in workflow view");
            }            
        }

        private void updateButton_Click(object sender, EventArgs e)
        {
            this.workflowPanel.OnCodeActivityUpdated();
        }
    }

    #endregion

    #region Class WorkflowViewPanel

    public sealed class WorkflowViewPanel : Panel, IServiceProvider
    {
        #region Members and Constructor

        private WorkflowDesignSurface designSurface;
        private WorkflowView workflowView;
        private SequentialWorkflowActivity rootActivity;
        private IDesignerHost designerHost;
        private DesignerShell parent;

        public WorkflowViewPanel(DesignerShell parent)
        {
            this.parent = parent;
        }

        #endregion

        #region Properties and Methods

        public WorkflowView GetWorkflowView()
        {
            return this.workflowView;
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
                Clear();
            base.Dispose(disposing);
        }
        
        protected override object GetService(Type serviceType)
        {
            if (this.designSurface != null)
                return this.designSurface.GetService(serviceType);
            else
                return null;
        }

        // Loads precreated workflow. The function is similar to the above function except
        // instead of creating an empty workflow we create workflow with contents
        internal void LoadDefaultWorkflow()
        {
            Clear();

            this.designSurface = new WorkflowDesignSurface(this);
            WorkflowLoader loader = new WorkflowLoader();
            this.designSurface.BeginLoad(loader);
            
            this.designerHost = GetService(typeof(IDesignerHost)) as IDesignerHost;
            if (this.designerHost != null)
            {
                this.rootActivity = (SequentialWorkflowActivity)this.designerHost.CreateComponent(typeof(SequentialWorkflowActivity));
                this.rootActivity.Name = "Service1";

                CodeActivity codeActivity1 = new CodeActivity();
                this.rootActivity.Activities.Add(codeActivity1);
                this.designerHost.RootComponent.Site.Container.Add(codeActivity1);

                this.workflowView = new WorkflowView(this.designSurface as IServiceProvider);
                this.workflowView.AddDesignerMessageFilter(new CustomMessageFilter(this.designSurface as IServiceProvider, this.workflowView, loader));

                Controls.Add(this.workflowView);
                this.designerHost.Activate();

                ISelectionService selectionService = (ISelectionService)GetService(typeof(ISelectionService));
                if (selectionService != null)
                {
                    selectionService.SelectionChanged += new EventHandler(OnSelectionChanged);
                    IComponent[] selection = new IComponent[] { rootActivity };
                    selectionService.SetSelectedComponents(selection);
                }
            }
        }

        private CodeActivity GetSelectedActivity()
        {
            ISelectionService selectionService = (ISelectionService)GetService(typeof(ISelectionService));

            if ((selectionService != null) && (selectionService.SelectionCount != 0))
            {
                object[] selection = new object[selectionService.SelectionCount];
                selectionService.GetSelectedComponents().CopyTo(selection, 0);

                if ((selection[0] != null) && (selection[0] is IComponent))
                {
                    if (selection[0] is CodeActivity)
                    {
                        return (CodeActivity)selection[0];
                    }
                }
            }

            return null;
        }

        private void OnSelectionChanged(object sender, System.EventArgs e)
        {
            this.parent.nameTextBox.Clear();
            this.parent.descriptionTextBox.Clear();
            this.parent.nameTextBox.ReadOnly = this.parent.descriptionTextBox.ReadOnly = true;
            this.parent.updateButton.Enabled = false;

            CodeActivity codeActivity = GetSelectedActivity();
            if (codeActivity != null)
            {
                this.parent.nameTextBox.Text = codeActivity.Name;
                this.parent.descriptionTextBox.Text = codeActivity.Description;
                this.parent.nameTextBox.ReadOnly = this.parent.descriptionTextBox.ReadOnly = false;
                this.parent.updateButton.Enabled = true;
            }
        }

        public void OnCodeActivityUpdated()
        {
            CodeActivity codeActivity = GetSelectedActivity();
            if (codeActivity != null)
            {
                try
                {
                    // Using PropertyDescriptor to let a designer update the component name
                    PropertyDescriptor propertyDescriptor = TypeDescriptor.GetProperties(codeActivity)["Name"];
                    propertyDescriptor.SetValue(codeActivity, this.parent.nameTextBox.Text);
                    codeActivity.Description = this.parent.descriptionTextBox.Text;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(this.parent, ex.Message, this.parent.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        public bool OnCodeActivityAdded()
        {

            CodeActivity codeActivity = new CodeActivity();

            ActivityProperties activityPropertiesForm = new ActivityProperties();
            activityPropertiesForm.ShowDialog();
            if (activityPropertiesForm.Result == DialogResult.OK)
            {
                foreach (Activity activity in this.rootActivity.Activities)
                {
                    if (activity.Name.Equals(activityPropertiesForm.ActivityName, StringComparison.OrdinalIgnoreCase))
                    {
                        MessageBox.Show(this.parent, "Cannot add new CodeActivity. The CodeActivity with name '" + activityPropertiesForm.ActivityName + "' already exists.", this.parent.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return false;
                    }
                }

                codeActivity.Name = activityPropertiesForm.ActivityName;
                codeActivity.Description = activityPropertiesForm.Description;

                this.rootActivity.Activities.Add(codeActivity);
                this.designerHost.RootComponent.Site.Container.Add(codeActivity);
                this.workflowView.Update();
            }

            return true;
        }

        public void OnZoomChanged(int zoomFactor)
        {
            this.workflowView.Zoom = zoomFactor;
            this.workflowView.Update();
        }

        private void Clear()
        {
            if (this.designSurface == null)
                return;
            else
                this.designSurface.Dispose();

            if (this.workflowView != null && Controls.Contains(this.workflowView))
                Controls.Remove(this.workflowView);
            this.workflowView = null;
        }

        protected override void OnLayout(LayoutEventArgs levent)
        {
            base.OnLayout(levent);
            if (this.workflowView != null && !this.workflowView.IsDisposed)
            {
                this.workflowView.Width = this.Width;
                this.workflowView.Height = this.Height;
            }
        }

        public void InvokeStandardCommand(CommandID cmd)
        {
            try
            {
                IMenuCommandService menuService = GetService(typeof(IMenuCommandService)) as IMenuCommandService;
                if (menuService != null)
                    menuService.GlobalInvoke(cmd);
            }
            catch
            {
                //We eat exceptions as some of the operations are not supported in samples
            }
        }

        #endregion

        #region IServiceProvider Members

        object System.IServiceProvider.GetService(Type serviceType)
        {
            return GetService(serviceType);
        }

        #endregion
    }
    #endregion

    #region Class WorkflowDesignSurface

    // Design Surface is used to provide services.
    internal sealed class WorkflowDesignSurface : DesignSurface
    {
        internal WorkflowDesignSurface(IServiceProvider serviceProvider)
        {
            this.ServiceContainer.AddService(typeof(IMenuCommandService), new MenuCommandService(this.ServiceContainer));

            TypeProvider typeProvider = new TypeProvider(serviceProvider);
            typeProvider.AddAssemblyReference(typeof(string).Assembly.Location);
            this.ServiceContainer.AddService(typeof(ITypeProvider), typeProvider, true);
        }
    }

    #endregion

    #region Class CustomMessageFilter

    //All Coordinates passed in physical coordinate system
    //Some of the functions will have coordinates in screen coordinates, that is, ShowContextMenu.
    internal sealed class CustomMessageFilter : WorkflowDesignerMessageFilter
    {
        #region Members and Constructor

        private bool mouseDown;
        private IServiceProvider serviceProvider;
        private WorkflowView workflowView;
        private WorkflowDesignerLoader loader;

        public CustomMessageFilter(IServiceProvider provider, WorkflowView workflowView, WorkflowDesignerLoader loader)
        {
            this.serviceProvider = provider;
            this.workflowView = workflowView;
            this.loader = loader;
        }

        #endregion

        #region MessageFilter Overridables

        protected override bool OnMouseDown(MouseEventArgs eventArgs)
        {
            //Allow other components to process this event by not returning true.
            this.mouseDown = true;
            return false;
        }

        protected override bool OnMouseMove(MouseEventArgs eventArgs)
        {
            //Allow other components to process this event by not returning true.
            if (mouseDown)
            {
                workflowView.ScrollPosition = new Point(eventArgs.X, eventArgs.Y);
            }
            return false;
        }

        protected override bool OnMouseUp(MouseEventArgs eventArgs)
        {
            //Allow other components to process this event by not returning true.
            mouseDown = false;
            return false;
        }

        protected override bool OnMouseDoubleClick(MouseEventArgs eventArgs)
        {
            mouseDown = false;
            return true;
        }

        protected override bool OnMouseEnter(MouseEventArgs eventArgs)
        {
            //Allow other components to process this event by not returning true.
            mouseDown = false;
            return false;
        }

        protected override bool OnMouseHover(MouseEventArgs eventArgs)
        {
            //Allow other components to process this event by not returning true.
            mouseDown = false;
            return false;
        }

        protected override bool OnMouseLeave()
        {
            //Allow other components to process this event by not returning true.
            mouseDown = false;
            return false;
        }

        protected override bool OnMouseWheel(MouseEventArgs eventArgs)
        {
            mouseDown = false;
            return true;
        }

        protected override bool OnMouseCaptureChanged()
        {
            //Allow other components to process this event by not returning true.
            mouseDown = false;
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

        protected override bool OnKeyDown(KeyEventArgs eventArgs)
        {
            if (eventArgs.KeyCode == Keys.Delete)
            {
                ISelectionService selectionService = (ISelectionService)serviceProvider.GetService(typeof(ISelectionService));
                if (selectionService != null && selectionService.PrimarySelection is CodeActivity)
                {
                    CodeActivity codeActivityComponent = (CodeActivity)selectionService.PrimarySelection;
                    CompositeActivity parentActivity = codeActivityComponent.Parent;
                    if (parentActivity != null)
                    {
                        parentActivity.Activities.Remove(codeActivityComponent);
                        this.ParentView.Update();
                    }
                    loader.RemoveActivityFromDesigner(codeActivityComponent);
                    
                }
            }
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

        #endregion
    }

    #endregion
}