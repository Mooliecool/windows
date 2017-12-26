/************************************* Module Header **************************************\
* Module Name:  InteropForm.cs
* Project:      CSVstoVBAInterop
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoVBAInterop project demonstrates how to interop with VBA project object model in 
* VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
* Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
* from VBA code. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.Vbe.Interop;
using System.Runtime.InteropServices;
#endregion


namespace CSVstoVBAInterop
{
    /// <summary>
    /// This form contains the UI for the demo.
    /// </summary>
    public partial class InteropForm : Form
    {
        private const string ModuleName = "CSVstoVBAInterop";
        private const string VbaFunctionName = "SayHello";

        public InteropForm()
        {
            InitializeComponent();
        }

        private void InteropForm_Load(object sender, EventArgs e)
        {
            // Set control Enable status based on VBA project OM access option.
            this.SetAccVbaOMStatus();

            if (VBEHelper.AccessVBOM)
            {
                this.GetPrjList(); // Get the VBA Project list
            }
        }

        private void chkEnableVbaAccess_CheckedChanged(object sender, EventArgs e)
        {
            if (!chkEnableVbaAccess.Checked)
            {
                if (MessageBox.Show("Disable this option will prevent access to VBA project OM.",
                    this.Text, MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                {
                    VBEHelper.AccessVBOM = false;
                }
                else
                {
                    chkEnableVbaAccess.Checked = true;
                }
            }
            else
            {
                VBEHelper.AccessVBOM = true;
            }

            SetAccVbaOMStatus();
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            this.GetPrjList();
        }

        private void btnShowVBE_Click(object sender, EventArgs e)
        {
            Globals.ThisAddIn.Application.VBE.MainWindow.Visible = true; // Show the VBA IDE
        }

        private void btnInsertRun_Click(object sender, EventArgs e)
        {
            try
            {
                object item = cboProjects.SelectedItem;

                if (item != null)
                {
                    MessageBox.Show(string.Format("Return value: {0}", InsertAndRun((VBProject)item)),
                        this.Text, MessageBoxButtons.OK, MessageBoxIcon.Information);

                    MessageBox.Show("Now you can run Macro ShowAsmInfo which calls VSTO code from Excel VBA.",
                        this.Text, MessageBoxButtons.OK, MessageBoxIcon.Information);

                    btnShowVBE.PerformClick();
                }
                else
                {
                    MessageBox.Show("Please select a VBA project.", this.Text, MessageBoxButtons.OK,
                        MessageBoxIcon.Error);
                }
            }
            catch (COMException comEx)
            {
                MessageBox.Show(comEx.ToString(), this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (ApplicationException appEx)
            {
                MessageBox.Show(appEx.Message, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Set controls' Enable value according to the VBA project OM access option.
        /// </summary>
        private void SetAccVbaOMStatus()
        {
            bool enabled = VBEHelper.AccessVBOM;
            chkEnableVbaAccess.Checked = enabled;
            cboProjects.Enabled = enabled;
            btnRefresh.Enabled = enabled;
            btnInsertRun.Enabled = enabled;
            btnShowVBE.Enabled = enabled;
        }

        /// <summary>
        /// Gets a list of available VBA projects and update the combobox.
        /// </summary>
        private void GetPrjList()
        {
            cboProjects.Items.Clear();
            
            foreach (VBProject prj in Globals.ThisAddIn.Application.VBE.VBProjects)
            {
                cboProjects.Items.Add(prj);
            }
        }

        /// <summary>
        /// Insert the sample macro code into the specified VBProject and call the code from VSTO.
        /// </summary>
        /// <param name="prj">Target VBProject.</param>
        /// <returns>The return value from the VBA function.</returns>
        private object InsertAndRun(VBProject prj)
        {
            // Check for existing module.
            foreach (VBComponent component in prj.VBComponents)
            {
                if (component.Name == ModuleName)
                {
                    throw new ApplicationException("There is already a " + ModuleName + " in this VBA project.");
                }
            }

            // Add a standard module
            VBComponent mod = prj.VBComponents.Add(vbext_ComponentType.vbext_ct_StdModule);
            // Name the module
            mod.Name = ModuleName;
            // Add code into the module directly from a string.
            mod.CodeModule.AddFromString(txtVbaSub.Text);

            // Call the newly added VBA function.
            // The first parameter to the function is a string "VSTO".
            return Globals.ThisAddIn.Application.Run(VbaFunctionName, "VSTO", Type.Missing, Type.Missing,
                    Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
                    Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
                    Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
                    Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
                    Type.Missing, Type.Missing, Type.Missing);
        }
    }
}
