/*********************************** Module Header ************************************\
* Module Name:	MainForm.cs
* Project:		CSCodeDOM
* Copyright (c) Microsoft Corporation.
* 
* The CSCodeDOM project demonstrates how to use the .NET CodeDOM mechanism to enable
* dynamic souce code generation and compilation at runtime.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 6/14/2009 1:00 PM Jie Wang Created
\**************************************************************************************/

#region Using directives
using System;
using System.IO;
using System.Text;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Reflection;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
#endregion


namespace CSCodeDOM
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            cboLang.SelectedIndex = 1;    // Set default langauge to VB.NET
        }

        private void txtSource_TextChanged(object sender, EventArgs e)
        {
            // Enable Run button if there is something in the TextBox.
            btnRun.Enabled = txtSource.Text.Trim().Length > 0;
        }

        private void txtNamespace_TextChanged(object sender, EventArgs e)
        {
            // Enable/disable the add namespace button
            btnAddNamespace.Enabled = txtNamespace.Text.Trim().Length > 0;
        }

        private void txtAssemblyRef_TextChanged(object sender, EventArgs e)
        {
            // Enable/disable the add reference button
            btnAddAsmRef.Enabled = txtAssemblyRef.Text.Trim().Length > 0;
        }

        private void lstNamespaces_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Enable/disable the remove namespace button
            btnRemoveNamespace.Enabled = lstNamespaces.SelectedItem != null;
        }

        private void lstAssemblyRef_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Enable/disable the remove reference button
            btnRemoveAsmRef.Enabled = lstAssemblyRef.SelectedItem != null;
        }

        private void btnAddNamespace_Click(object sender, EventArgs e)
        {
            // Add namespace import
            string ns = txtNamespace.Text.Trim();
            if (lstNamespaces.Items.IndexOf(ns) < 0)
            {
                lstNamespaces.Items.Add(ns);
            }
            txtNamespace.Text = string.Empty;
        }

        private void btnRemoveNamespace_Click(object sender, EventArgs e)
        {
            // Remove namespace import
            lstNamespaces.Items.Remove(lstNamespaces.SelectedItem);
        }

        private void btnAddAsmRef_Click(object sender, EventArgs e)
        {
            // Add reference
            string asm = txtAssemblyRef.Text.Trim();
            if (lstAssemblyRef.Items.IndexOf(asm) < 0)
            {
                lstAssemblyRef.Items.Add(asm);
            }
            txtAssemblyRef.Text = string.Empty;
        }

        private void btnRemoveAsmRef_Click(object sender, EventArgs e)
        {
            // Remove reference
            lstAssemblyRef.Items.Remove(lstAssemblyRef.SelectedItem);
        }

        private void btnRun_Click(object sender, EventArgs e)
        {
            ScriptControl.Language lang;

            // Set the langauge enum according to user selection.
            switch (cboLang.SelectedItem.ToString().ToLowerInvariant())
            {
                case "visual basic":
                    lang = ScriptControl.Language.VisualBasic; break;
                case "jscript":
                    lang = ScriptControl.Language.JScript; break;
                default:
                    lang = ScriptControl.Language.CSharp; break;
            }

            try
            {
                // Create new instance of ScriptControl, passing in script and language.
                ScriptControl sc = new ScriptControl(txtSource.Text, lang);
                sc.RunInSeparateDomain = chkSpDomain.Checked;

                // Set namespace imports
                for (int i = 0; i < lstNamespaces.Items.Count; i++)
                {
                    sc.CodeNamespaceImports.Add((string)lstNamespaces.Items[i]);
                }

                // Set assembly references
                for (int i = 0; i < lstAssemblyRef.Items.Count; i++)
                {
                    sc.AssemblyReferences.Add((string)lstAssemblyRef.Items[i]);
                }

                object result = sc.Run();   // Run the script and get the return value.

                MessageBox.Show(string.Format("Result from the script: {0}", result),
                    this.Text, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                if ((ex as TargetInvocationException) != null)
                {
                    ex = ex.InnerException;
                }
                MessageBox.Show(string.Format("Error in script: \r\n\r\n{0}", ex.Message),
                    this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}