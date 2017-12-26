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

using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Xml;
using System.Reflection;
using System.Windows.Forms;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Serialization;
using Microsoft.Samples.Workflow.WorkflowReflectionUtility.Reflection;
using Microsoft.Samples.Workflow.WorkflowReflectionUtility.Dialogs;
using System.Globalization;

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility
{
    /// <summary>
    /// Child of the RootComponent which represents
    /// the Assembly as a node in the TreeView.
    /// </summary>
    public class AssemblyComponent : WorkflowReflectionComponent
    {
        private const string AssemblyNodeImageName = "AssemblyNodeImage";

        public override void Initialize(InitializationContext context)
        {
            context.AddImage(OutOfBoxComponentResources.AssemblyNode, Color.White, typeof(AssemblyComponent), AssemblyNodeImageName);
            context.AddChildComponent(typeof(RootComponent), typeof(AssemblyComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            AssemblyNode assemblyNode = new AssemblyNode();
            assemblyNode.Text = Path.GetFileName(context.WorkflowAssembly.Assembly.Location);
            assemblyNode.Assembly = context.WorkflowAssembly;
            assemblyNode.SelectedImageIndex = assemblyNode.ImageIndex = context.GetImageIndex(typeof(AssemblyComponent), AssemblyNodeImageName);

            context.TreeView.Nodes.Add(assemblyNode);

            ReflectChildComponents(context.CreateClone(assemblyNode));
        }
    }

    /// <summary>
    /// TreeNode subclass which adds the WorkflowAssembly
    /// property.
    /// </summary>
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(false)]
    public class AssemblyNode : TreeNode
    {
        [NonSerialized]
        private WorkflowAssembly assembly;

        /// <summary>
        /// Gets the WorkflowAssembly represented by this
        /// node.
        /// </summary>
        public WorkflowAssembly Assembly
        {
            get
            {
                return assembly;
            }
            set
            {
                assembly = value;
            }
        }
    }

    /// <summary>
    /// Child of the AssemblyComponent which adds a TreeNode
    /// for each Workflow type in the assembly.
    /// </summary>
    public class WorkflowComponent : WorkflowReflectionComponent
    {
        private const string WorkflowNodeImageName = "WorkflowNodeImageName";

        public override void Initialize(InitializationContext context)
        {
            context.AddImage(OutOfBoxComponentResources.WorkflowNode, Color.White, typeof(WorkflowComponent), WorkflowNodeImageName); 
            context.AddChildComponent(typeof(AssemblyComponent), typeof(WorkflowComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            Type[] workflowTypes = context.WorkflowAssembly.GetWorkflowTypes();
            
            if (workflowTypes == null)
                return;

            foreach (Type workflowType in workflowTypes)
            {
                WorkflowNode workflowNode = new WorkflowNode();
                workflowNode.Name = workflowType.Name;
                workflowNode.Text = workflowType.FullName;
                workflowNode.WorkflowType = workflowType;
                workflowNode.SelectedImageIndex = workflowNode.ImageIndex = context.GetImageIndex(typeof(WorkflowComponent), WorkflowNodeImageName);

                context.CurrentTreeNode.Nodes.Add(workflowNode);
                ReflectChildComponents(context.CreateClone(workflowNode));
            }

            if (context.WorkflowAssembly.GetWorkflowTypes().Length == 0)
            {
                TreeNode node = new TreeNode();
                node.Text = "<No Workflows Found>";

                context.CurrentTreeNode.Nodes.Add(node);
            }
        }
    }

    /// <summary>
    /// TreeNode subclass which adds the WorkflowType
    /// property.
    /// </summary>
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(false)]
    public class WorkflowNode : TreeNode
    {
        private Type workflowType;

        /// <summary>
        /// Gets or sets the Workflow Type this node represents.
        /// </summary>
        public Type WorkflowType
        {
            get
            {
                return workflowType;
            }
            set
            {
                workflowType = value;
            }
        }
    }

    /// <summary>
    /// Child of the WorkflowComponent which adds a context menu
    /// for displaying the XAML in Notepad.
    /// </summary>
    public class XamlContextMenuComponent : WorkflowReflectionComponent
    {
        public override void Initialize(InitializationContext context)
        {
            context.AddChildComponent(typeof(WorkflowComponent), typeof(XamlContextMenuComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            if (context.CurrentTreeNode.ContextMenu != null)
            {
                throw new ApplicationException("XamlContextMenuComponent only works with ContextMenuStrip menus.  The WorkflowComponent node had an old ContextMenu assigned.");
            }

            ContextMenuStrip menu = null;

            if (context.CurrentTreeNode.ContextMenuStrip != null)
            {
                menu = context.CurrentTreeNode.ContextMenuStrip;
            }
            else
            {
                menu = new ContextMenuStrip();
                context.CurrentTreeNode.ContextMenuStrip = menu;
            }

            ToolStripMenuItem openNotepadItem = new ToolStripMenuItem("Open XAML in Notepad");
            openNotepadItem.Click += OpenXamlInNotepad;
            openNotepadItem.Tag = context.CurrentTreeNode;
            menu.Items.Add(openNotepadItem);
        }

        private void OpenXamlInNotepad(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;
            WorkflowNode node = (WorkflowNode)item.Tag;

            object workflowInstance = null;

            try
            {
                workflowInstance = node.WorkflowType.InvokeMember(string.Empty, BindingFlags.CreateInstance, null, null, null, CultureInfo.InvariantCulture);
            }
            catch (Exception exc)
            {
                MessageBox.Show("The workflow type could not be created: " + exc.ToString());
                return;
            }
            
            WorkflowMarkupSerializer serializer = new WorkflowMarkupSerializer();

            string tempFileName = Path.GetTempFileName();


            using (XmlWriter xmlWriter = XmlWriter.Create(tempFileName))
            {
                serializer.Serialize( xmlWriter, workflowInstance);
            }

            ProcessStartInfo psi = new ProcessStartInfo();
            psi.Arguments = tempFileName;
            psi.FileName = Path.Combine(Environment.GetEnvironmentVariable("windir"), "notepad.exe");

            Process.Start(psi);
        }
    }

    /// <summary>
    /// Child of the WorkflowComponent which adds a context
    /// menu for displaying the workflow in a rehosted designer
    /// window.
    /// </summary>
    public class WorkflowDesignerViewComponent : WorkflowReflectionComponent
    {
        public override void Initialize(InitializationContext context)
        {
            context.AddChildComponent(typeof(WorkflowComponent), typeof(WorkflowDesignerViewComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            if (context.CurrentTreeNode.ContextMenu != null)
            {
                throw new ApplicationException("XamlContextMenuComponent only works with ContextMenuStrip menus.  The WorkflowComponent node had an old ContextMenu assigned.");
            }

            ContextMenuStrip menu = null;

            if (context.CurrentTreeNode.ContextMenuStrip != null)
            {
                menu = context.CurrentTreeNode.ContextMenuStrip;
            }
            else
            {
                menu = new ContextMenuStrip();
                context.CurrentTreeNode.ContextMenuStrip = menu;
            }

            ToolStripMenuItem viewInDesigner = new ToolStripMenuItem();
            viewInDesigner.Tag = ((WorkflowNode)context.CurrentTreeNode).WorkflowType;
            viewInDesigner.Text = "View on Design Surface";
            viewInDesigner.Click += OnItemClick;

            menu.Items.Add(viewInDesigner);
        }

        private void OnItemClick(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            // This code does not reuse the same dialog so that
            // users can have multiple workflow windows open concurrently.
            Type workflowType = (Type)item.Tag;
            WorkflowDesignerDialog dialog = new WorkflowDesignerDialog(workflowType);
            dialog.WindowState = FormWindowState.Maximized;
            dialog.Show();
        }
    }

    /// <summary>
    /// Child of the WorkflowComponent which creates a node
    /// for each local service required by the Workflow.
    /// </summary>
    public class RequiredServiceInterfacesComponent : WorkflowReflectionComponent
    {
        private const string ServiceInterfaceNodeImageName = "ServiceInterfaceNodeImageName";

        public override void Initialize(InitializationContext context)
        {
            context.AddImage(OutOfBoxComponentResources.ServiceInterfaceNode, Color.White, typeof(RequiredServiceInterfacesComponent), ServiceInterfaceNodeImageName);
            context.AddChildComponent(typeof(WorkflowComponent), typeof(RequiredServiceInterfacesComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            Type workflowType = ((WorkflowNode)context.CurrentTreeNode).WorkflowType;

            object workflowInstance = null;

            try
            {
                workflowInstance = workflowType.InvokeMember(string.Empty, BindingFlags.CreateInstance, null, null, null, CultureInfo.InvariantCulture);
            }
            catch (Exception exc)
            {
                MessageBox.Show("Workflow type could not be created: " + exc.ToString());
                return;
            }

            Activity root = (Activity)workflowInstance;

            Queue<Activity> toProcess = new Queue<Activity>();
            List<Type> foundInterfaces = new List<Type>();
            toProcess.Enqueue(root);

            while (toProcess.Count > 0)
            {
                Activity activity = toProcess.Dequeue();

                if (activity is CompositeActivity)
                {
                    CompositeActivity compositeActivity = (CompositeActivity)activity;

                    foreach (Activity child in compositeActivity.Activities)
                    {
                        toProcess.Enqueue(child);
                    }
                }
                else if (activity is HandleExternalEventActivity)
                {
                    HandleExternalEventActivity eventSink = (HandleExternalEventActivity)activity;

                    if (!foundInterfaces.Contains(eventSink.InterfaceType))
                    {
                        foundInterfaces.Add(eventSink.InterfaceType);
                    }
                }
                else if (activity is CallExternalMethodActivity)
                {
                    CallExternalMethodActivity invoke = (CallExternalMethodActivity)activity;

                    if (!foundInterfaces.Contains(invoke.InterfaceType))
                    {
                        foundInterfaces.Add(invoke.InterfaceType);
                    }
                }
            }

            foreach (Type service in foundInterfaces)
            {
                ServiceInterfaceNode node = new ServiceInterfaceNode();
                node.ServiceInterfaceType = service;
                node.Text = "Required Service: " + service.FullName;
                node.SelectedImageIndex = node.ImageIndex = context.GetImageIndex(typeof(RequiredServiceInterfacesComponent), ServiceInterfaceNodeImageName);

                context.CurrentTreeNode.Nodes.Add(node);
                ReflectChildComponents(context.CreateClone(node));
            }
        }
    }

    /// <summary>
    /// TreeNode subclass which adds the ServiceInterfaceType
    /// property.
    /// </summary>
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(false)]
    public class ServiceInterfaceNode : TreeNode
    {
        private Type serviceInterfaceTypeValue;

        /// <summary>
        /// Gets or sets the Type of the service interface
        /// this node represents.
        /// </summary>
        public Type ServiceInterfaceType
        {
            get
            {
                return serviceInterfaceTypeValue;
            }
            set
            {
                serviceInterfaceTypeValue = value;
            }
        }
    }

    /// <summary>
    /// Child of the WorkflowComponent which adds a node
    /// for each rule condition found on the Workflow.
    /// </summary>
    public class RuleComponent : WorkflowReflectionComponent
    {
        private const string RuleNodeImageName = "RuleNodeImageName";

        public override void Initialize(InitializationContext context)
        {
            context.AddImage(OutOfBoxComponentResources.RuleNode, Color.White, typeof(RuleComponent), RuleNodeImageName);
            context.AddChildComponent(typeof(WorkflowComponent), typeof(RuleComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            Type workflowType = ((WorkflowNode)context.CurrentTreeNode).WorkflowType;

            DependencyObject workflow = ((WorkflowNode)context.CurrentTreeNode).WorkflowType.InvokeMember(string.Empty, BindingFlags.CreateInstance, null, null, null, CultureInfo.InvariantCulture) as DependencyObject;

            if (workflow == null)
            {
                throw new ApplicationException("Could not create the workflow and cast to DependencyObject");
            }

            RuleDefinitions definitions = workflow.GetValue(RuleDefinitions.RuleDefinitionsProperty) as RuleDefinitions;

            if (definitions != null)
            {
                foreach (RuleCondition cond in definitions.Conditions)
                {
                    RuleNode node = new RuleNode();
                    node.Rule = cond;
                    node.SelectedImageIndex = node.ImageIndex = context.GetImageIndex(typeof(RuleComponent), RuleNodeImageName);

                    context.CurrentTreeNode.Nodes.Add(node);
                    ReflectChildComponents(context.CreateClone(node));
                }
            }
        }
    }

    /// <summary>
    /// TreeNode subclass which adds the Rule property.
    /// </summary>
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(false)]
    public class RuleNode : TreeNode
    {
        private RuleCondition ruleValue;

        /// <summary>
        /// Gets or sets the rule which this node
        /// represents.
        /// </summary>
        /// <remarks>
        /// Setting the rule will change the text
        /// of the node to display the rule's name
        /// and the serialization of its CodeExpression.
        /// </remarks>
        public RuleCondition Rule
        {
            get
            {
                return ruleValue;
            }
            set
            {
                ruleValue = value;

                if (ruleValue != null)
                {
                    if (ruleValue is RuleExpressionCondition)
                    {
                        RuleExpressionCondition expressionRule = (RuleExpressionCondition)ruleValue;
                        this.Text = expressionRule.Name + ":  " + Serialize(expressionRule.Expression);
                    }
                    else
                    {
                        throw new ApplicationException("Don't yet support rules of type: " + ruleValue.GetType());
                    }
                }
                else
                {
                    throw new ArgumentNullException("value");
                }
            }
        }

        private string Serialize(CodeExpression codeExpression)
        {
            StringWriter stringWriter = new StringWriter();
            CodeDomProvider provider = CodeDomProvider.CreateProvider("C#");
            provider.GenerateCodeFromExpression(codeExpression, stringWriter, new CodeGeneratorOptions());
            String expression = stringWriter.ToString().Trim().Replace(stringWriter.NewLine, " ");

            int startRemove = -1;
            int removeCount = 0;

            for (int i = 0; i < expression.Length; i++)
            {
                if (expression[i] == ' ')
                {
                    if (startRemove == -1)
                    {
                        startRemove = i;
                    }
                    else
                    {
                        removeCount++;
                    }
                }
                else
                {
                    if (removeCount != 0)
                    {
                        expression = expression.Remove(startRemove, removeCount);
                        removeCount = 0;
                    }

                    startRemove = -1;
                }
            }

            return expression;
        }
    }

    /// <summary>
    /// Child of the RuleComponent which creates a context menu
    /// for displaying a RuleEvaluator dialog used for evaluating
    /// the rule through substitution of non-primitive expressions.
    /// </summary>
    public class RuleEvaluatorComponent : WorkflowReflectionComponent
    {
        public override void Initialize(InitializationContext context)
        {
            context.AddChildComponent(typeof(RuleComponent), typeof(RuleEvaluatorComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            if (context.CurrentTreeNode.ContextMenu != null)
            {
                throw new ApplicationException("XamlContextMenuComponent only works with ContextMenuStrip menus.  The WorkflowComponent node had an old ContextMenu assigned.");
            }

            ContextMenuStrip menu = null;

            if (context.CurrentTreeNode.ContextMenuStrip != null)
            {
                menu = context.CurrentTreeNode.ContextMenuStrip;
            }
            else
            {
                menu = new ContextMenuStrip();
                context.CurrentTreeNode.ContextMenuStrip = menu;
            }

            ToolStripMenuItem evaluateItem = new ToolStripMenuItem();
            evaluateItem.Tag = context.CurrentTreeNode;
            evaluateItem.Text = "Evaluate Rule";
            evaluateItem.Click += OnItemClick;

            menu.Items.Add(evaluateItem);
        }

        private void OnItemClick(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            RuleNode node = (RuleNode)item.Tag;
            RuleExpressionCondition expressionCondition = (RuleExpressionCondition)node.Rule;

            WorkflowNode workflowNode = (WorkflowNode)node.Parent;

            EvaluateRuleDialog dialog = new EvaluateRuleDialog(expressionCondition.Expression, workflowNode.WorkflowType);
            dialog.ShowDialog();
        }
    }

    /// <summary>
    /// Child of the WorkflowComponent which displays all of the workflow's
    /// activities (and subactivities) hierarchically in the tree view.
    /// </summary>
    public class WorkflowActivitiesComponent : WorkflowReflectionComponent
    {
        private const string ActivitiesNodeImageName = "ActivitiesNodeImageName";
        private const string ActivityNodeImageName = "ActivityNodeImageName";

        public override void Initialize(InitializationContext context)
        {
            context.AddImage(OutOfBoxComponentResources.ActivitiesNode, Color.White, typeof(WorkflowActivitiesComponent), ActivitiesNodeImageName);
            context.AddImage(OutOfBoxComponentResources.ActivityNode, Color.White, typeof(WorkflowActivitiesComponent), ActivityNodeImageName);
            context.AddChildComponent(typeof(WorkflowComponent), typeof(WorkflowActivitiesComponent));
        }

        public override void PerformReflection(ReflectionContext context)
        {
            WorkflowNode workflowNode = context.CurrentTreeNode as WorkflowNode;

            if (workflowNode == null)
            {
                throw new ApplicationException("Expected the parent node to be a workflow node.");
            }

            object workflowInstance = null;

            try
            {
                workflowInstance = workflowNode.WorkflowType.InvokeMember(string.Empty, BindingFlags.CreateInstance, null, null, null, CultureInfo.InvariantCulture);
            }
            catch (Exception exc)
            {
                MessageBox.Show("Could not create workflow type: " + exc.ToString());
                return;
            }

            TreeNode activitiesNode = new TreeNode();
            activitiesNode.Text = "Activities";
            activitiesNode.ImageIndex = activitiesNode.SelectedImageIndex = context.GetImageIndex(typeof(WorkflowActivitiesComponent), ActivitiesNodeImageName);

            workflowNode.Nodes.Add(activitiesNode);

            if (!(workflowInstance is CompositeActivity))
            {
                TreeNode none = new TreeNode();
                none.Text = "<Workflow is not a CompositeActivity>";
                activitiesNode.Nodes.Add(none);
            }
            else
            {
                Queue<KeyValuePair<CompositeActivity, TreeNode>> toProcess = new Queue<KeyValuePair<CompositeActivity, TreeNode>>();
                toProcess.Enqueue(new KeyValuePair<CompositeActivity, TreeNode>((CompositeActivity)workflowInstance, activitiesNode));

                while (toProcess.Count > 0)
                {
                    KeyValuePair<CompositeActivity, TreeNode> pair = toProcess.Dequeue();
                    CompositeActivity compositeActivity = pair.Key;
                    TreeNode parent = pair.Value;
                    
                    foreach (Activity activity in compositeActivity.Activities)
                    {
                        ActivityNode activityNode = new ActivityNode ();
                        activityNode.Activity = activity;
                        activityNode.ImageIndex = activityNode.SelectedImageIndex = context.GetImageIndex(typeof(WorkflowActivitiesComponent), ActivityNodeImageName);
                        parent.Nodes.Add(activityNode);

                        ReflectChildComponents(context.CreateClone(activityNode));

                        if (activity is CompositeActivity)
                        {
                            toProcess.Enqueue(new KeyValuePair<CompositeActivity, TreeNode>((CompositeActivity)activity, activityNode));
                        }
                    }
                }
            }
        }
    }

    /// <summary>
    /// TreeNode for displaying an activity.
    /// </summary>
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(false)]
    public class ActivityNode : TreeNode
    {
        private Activity activityValue;

        /// <summary>
        /// Gets or sets this node's activity.
        /// </summary>
        public Activity Activity
        {
            get
            {
                return activityValue;
            }
            set
            {
                activityValue = value;

                if (activityValue == null)
                {
                    this.Text = "<No Activity>";
                }
                else
                {
                    this.Text = string.Format("{0}: Name='{1}' Description='{2}'", activityValue.GetType().Name, activityValue.QualifiedName, activityValue.Description);
                }
            }
        }
    }
}