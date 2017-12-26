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
using System.Reflection;
using System.Windows.Forms;
using Microsoft.Samples.Workflow.WorkflowReflectionUtility.Dialogs;

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Components
{
    /// <summary>
    /// Assembly resolver which searches for assemblies in a local
    /// cache as well as provides an OpenFileDialog for discovering
    /// assemblies not in the cache.
    /// </summary>
    public class OpenFileDialogAssemblyResolver : Component
    {
        private OpenFileDialog openFileDialog;
        private List<Assembly> toResolveValue;
        private bool dialogEnabledValue;
        private bool showAssemblyResolveExplanationValue;
        private AssemblyResolveExplanationDialog assemblyResolveExplanationDialog;

        /// <summary>
        /// Default constructor.  Sets default values.
        /// </summary>
        internal OpenFileDialogAssemblyResolver()
        {
            this.openFileDialog = new OpenFileDialog();
            this.openFileDialog.FileName = string.Empty;
            this.openFileDialog.Filter = "Assembly|*.exe;*.dll";
            this.openFileDialog.Multiselect = false;

            this.toResolveValue = new List<Assembly>();

            this.dialogEnabledValue = true;
            this.showAssemblyResolveExplanationValue = true;
            this.assemblyResolveExplanationDialog = new AssemblyResolveExplanationDialog();

            AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(CurrentDomain_AssemblyResolve);
        }
        internal void Close()
        {
            AppDomain.CurrentDomain.AssemblyResolve -= CurrentDomain_AssemblyResolve;
        }

        /// <summary>
        /// Gets or sets whether to show the explanation for why a
        /// File Open dialog is being displayed.
        /// </summary>
        public bool ShowAssemblyResolveExplanation
        {
            get
            {
                return this.showAssemblyResolveExplanationValue;
            }
            set
            {
                this.showAssemblyResolveExplanationValue = value;
            }
        }

        internal Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            // Check the local cache
            foreach (Assembly assembly in ToResolve)
            {
                if (assembly == null)
                {
                    continue;
                }

                if (assembly.FullName == args.Name)
                {
                    return assembly;
                }
            }

            // Only display the dialog if it is enabled
            if (dialogEnabledValue)
            {
                if (showAssemblyResolveExplanationValue)
                {
                    this.assemblyResolveExplanationDialog.ShowDialog();

                    this.showAssemblyResolveExplanationValue = !assemblyResolveExplanationDialog.DoNotShowExplanation;
                }

                openFileDialog.Title = string.Format("Find assembly: {0}", args.Name);

                bool keepTrying = true;

                while (keepTrying)
                {
                    if (openFileDialog.ShowDialog() == DialogResult.OK)
                    {
                        Assembly assembly = Assembly.LoadFrom(openFileDialog.FileName);

                        // If the wrong assembly was opened, open a different
                        // one.
                        if (assembly.FullName != args.Name)
                        {
                            MessageBox.Show("The selected assembly is not the correct assembly.");
                            continue;
                        }

                        this.ToResolve.Add(assembly);

                        return assembly;
                    }
                    else
                    {
                        // Provide the opportunity to try again if the
                        // OpenFileDialog is cancelled
                        if (MessageBox.Show("If you cancel now the assembly cannot be properly displayed.  Press Retry to try to locate the dependency one more time.", "Cancel Assembly Resolution?", MessageBoxButtons.RetryCancel) == DialogResult.Cancel)
                        {
                            keepTrying = false;
                        }
                    }
                }
            }

            return null;
        }

        /// <summary>
        /// Gets the list of cached assemblies for resolution.
        /// </summary>
        public List<Assembly> ToResolve
        {
            get
            {
                return this.toResolveValue;
            }
        }

        /// <summary>
        /// Gets or sets whether to display the OpenFileDialog.
        /// </summary>
        /// <remarks>
        /// It is useful to set DialogEnabled to false when trying
        /// to determine whether an assembly already exists in the
        /// cache.  
        /// </remarks>
        public bool DialogEnabled
        {
            get
            {
                return this.dialogEnabledValue;
            }
            set
            {
                this.dialogEnabledValue = value;
            }
        }
    }
}