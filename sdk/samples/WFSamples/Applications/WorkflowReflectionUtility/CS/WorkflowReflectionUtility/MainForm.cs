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
using System.Reflection;
using System.Windows.Forms;
using Microsoft.Samples.Workflow.WorkflowReflectionUtility.Components;
using Microsoft.Samples.Workflow.WorkflowReflectionUtility.Reflection;
using System.Globalization;

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility
{
    internal partial class MainForm : Form
    {
        private Dictionary<Type, List<Type>> componentMapping;
        private Dictionary<Type, Dictionary<string, int>> imageIndexMapping;
        private OpenFileDialogAssemblyResolver assemblyResolverDialog;

        /// <summary>
        /// Create the main form and flow the programs inbound
        /// command line arguments.
        /// </summary>
        /// <param name="args">Command line arguments passed
        /// by the hosting program.</param>
        public MainForm(string[] args)
        {
            InitializeComponent();

            assemblyResolverDialog = new OpenFileDialogAssemblyResolver();

            imageList1.Images.Add(OutOfBoxComponentResources.EmptyNode);

            componentMapping = new Dictionary<Type, List<Type>>();
            imageIndexMapping = new Dictionary<Type, Dictionary<string, int>>();

            WorkflowReflectionUtilitySection section = System.Configuration.ConfigurationManager.GetSection("WorkflowReflectorSettings") as WorkflowReflectionUtilitySection;

            // If no configuration section was found then
            // use the default OOB components.
            if (section == null)
            {
                section = new WorkflowReflectionUtilitySection();
                section.ShowAssemblyResolveExplanation = true;
                section.Components.Add(new ReflectionComponentElement(typeof(AssemblyComponent)));
                section.Components.Add(new ReflectionComponentElement(typeof(WorkflowComponent)));
                section.Components.Add(new ReflectionComponentElement(typeof(RuleComponent)));
                section.Components.Add(new ReflectionComponentElement(typeof(RequiredServiceInterfacesComponent)));
                section.Components.Add(new ReflectionComponentElement(typeof(XamlContextMenuComponent)));
                section.Components.Add(new ReflectionComponentElement(typeof(WorkflowDesignerViewComponent)));
                section.Components.Add(new ReflectionComponentElement(typeof(RuleEvaluatorComponent)));
                section.Components.Add(new ReflectionComponentElement(typeof(WorkflowActivitiesComponent)));
            }

            assemblyResolverDialog.ShowAssemblyResolveExplanation = section.ShowAssemblyResolveExplanation;

            InitializationContext context = new InitializationContext(componentMapping, imageList1, imageIndexMapping);

            // Initialize each component.
            foreach (ReflectionComponentElement element in section.Components)
            {
                Type nodeType = Type.GetType(element.Type);

                if (nodeType == null)
                {
                    MessageBox.Show(string.Format("Could not find type '{0}'.  Application will run without that component.", element.Type));
                    continue;
                }

                WorkflowReflectionComponent node = nodeType.InvokeMember(string.Empty, BindingFlags.CreateInstance, null, null, null, CultureInfo.InvariantCulture) as WorkflowReflectionComponent;
                node.Initialize(context);
            }

            if (args != null && args.Length > 0)
            {
                LoadAssembly(args[0]);
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                LoadAssembly(openFileDialog1.FileName);
            }
        }

        private void LoadAssembly(string fullName)
        {
            WorkflowAssembly workflowAssembly = null;

            try
            {
                workflowAssembly = WorkflowAssembly.LoadFrom(fullName);
            }
            catch (Exception exception)
            {
                MessageBox.Show("Could not load the requested assembly: " + exception.ToString());
                return;
            }

            assemblyResolverDialog.DialogEnabled = false;
            Assembly assembly = null;

            // Check that the Assembly, or an Assembly with
            // the same name, has not already been loaded by
            // the program.  If either of the above is true then
            // the newly loaded assembly would just be ignored. 
            // Instead of showing duplicate assembly nodes, we
            // will just display an error and then make this a
            // no-op.
            try
            {
                assembly = Assembly.Load(workflowAssembly.Assembly.FullName);
            }
            catch (Exception)
            {
            }

            if (assembly != null)
            {
                MessageBox.Show("Either the assembly is already loaded or an assembly with the same Assembly Qualified Name is already loaded.  Please choose a different assembly.");
                return;
            }

            assemblyResolverDialog.DialogEnabled = true;
            assemblyResolverDialog.ToResolve.Add(workflowAssembly.Assembly);

            ReflectionContext context = new ReflectionContext(workflowAssembly, imageIndexMapping, null, componentMapping, treeView1);

            // Start the reflection chain.
            RootComponent component = new RootComponent();
            component.PerformReflection(context);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        protected override void OnClosed(EventArgs e)
        {
            assemblyResolverDialog.Close();
            base.OnClosed(e);
        }
    }
}