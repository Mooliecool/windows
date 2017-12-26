//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
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
//-----------------------------------------------------------------------
using System;
using System.Globalization;
using System.Security;
using System.Security.Permissions;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Data;
using System.Threading;
using System.Runtime.InteropServices;

[assembly: CLSCompliant(true)]
[assembly: ComVisible(false)]
[assembly: FileIOPermissionAttribute(SecurityAction.RequestMinimum)]

namespace Microsoft.Samples.GCDemo
{
    unsafe class formMain : System.Windows.Forms.Form
    {
        private int _allocation;
        private System.Windows.Forms.Label labelTotCount;
        private int _curCount;
        private System.Windows.Forms.Label labelCurCount;
        private System.Windows.Forms.Label labelTotal;
        private System.Windows.Forms.Button buttonClear;
        private int _totCount;

        private System.Windows.Forms.TreeView treeMain;
        private int _gcBaseLine;

        private System.Windows.Forms.Button buttonExit;
        private System.Windows.Forms.GroupBox groupPressure;
        private System.Windows.Forms.RadioButton radioPressureNo;
        private System.Windows.Forms.Button buttonRun;
        private System.Windows.Forms.RadioButton radioPressureYes;
        private System.Windows.Forms.Label labelAllocation;
        private System.Windows.Forms.GroupBox groupVariables;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox comboAmount;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown numericAllocations;
        private System.Windows.Forms.GroupBox groupCollections;
        private System.Windows.Forms.Label labelCount;
        private System.Windows.Forms.Label labelDesc;
        private System.ComponentModel.Container components = null;

        public formMain()
        {
            InitializeComponent();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formMain));
            this.buttonExit = new System.Windows.Forms.Button();
            this.groupPressure = new System.Windows.Forms.GroupBox();
            this.radioPressureYes = new System.Windows.Forms.RadioButton();
            this.radioPressureNo = new System.Windows.Forms.RadioButton();
            this.buttonRun = new System.Windows.Forms.Button();
            this.labelAllocation = new System.Windows.Forms.Label();
            this.groupVariables = new System.Windows.Forms.GroupBox();
            this.numericAllocations = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.comboAmount = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupCollections = new System.Windows.Forms.GroupBox();
            this.labelTotal = new System.Windows.Forms.Label();
            this.labelCurCount = new System.Windows.Forms.Label();
            this.labelTotCount = new System.Windows.Forms.Label();
            this.labelCount = new System.Windows.Forms.Label();
            this.labelDesc = new System.Windows.Forms.Label();
            this.buttonClear = new System.Windows.Forms.Button();
            this.treeMain = new System.Windows.Forms.TreeView();
            this.groupPressure.SuspendLayout();
            this.groupVariables.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericAllocations)).BeginInit();
            this.groupCollections.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonExit
            // 
            resources.ApplyResources(this.buttonExit, "buttonExit");
            this.buttonExit.Name = "buttonExit";
            this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
            // 
            // groupPressure
            // 
            this.groupPressure.Controls.Add(this.radioPressureYes);
            this.groupPressure.Controls.Add(this.radioPressureNo);
            resources.ApplyResources(this.groupPressure, "groupPressure");
            this.groupPressure.Name = "groupPressure";
            this.groupPressure.TabStop = false;
            // 
            // radioPressureYes
            // 
            resources.ApplyResources(this.radioPressureYes, "radioPressureYes");
            this.radioPressureYes.Name = "radioPressureYes";
            // 
            // radioPressureNo
            // 
            this.radioPressureNo.Checked = true;
            resources.ApplyResources(this.radioPressureNo, "radioPressureNo");
            this.radioPressureNo.Name = "radioPressureNo";
            // 
            // buttonRun
            // 
            resources.ApplyResources(this.buttonRun, "buttonRun");
            this.buttonRun.Name = "buttonRun";
            this.buttonRun.Click += new System.EventHandler(this.buttonRun_Click);
            // 
            // labelAllocation
            // 
            resources.ApplyResources(this.labelAllocation, "labelAllocation");
            this.labelAllocation.Name = "labelAllocation";
            // 
            // groupVariables
            // 
            this.groupVariables.Controls.Add(this.numericAllocations);
            this.groupVariables.Controls.Add(this.label2);
            this.groupVariables.Controls.Add(this.comboAmount);
            this.groupVariables.Controls.Add(this.label1);
            resources.ApplyResources(this.groupVariables, "groupVariables");
            this.groupVariables.Name = "groupVariables";
            this.groupVariables.TabStop = false;
            // 
            // numericAllocations
            // 
            resources.ApplyResources(this.numericAllocations, "numericAllocations");
            this.numericAllocations.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericAllocations.Name = "numericAllocations";
            this.numericAllocations.Value = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.numericAllocations.ValueChanged += new System.EventHandler(this.numericAllocations_ValueChanged);
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // comboAmount
            // 
            this.comboAmount.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboAmount.FormattingEnabled = true;
            resources.ApplyResources(this.comboAmount, "comboAmount");
            this.comboAmount.Name = "comboAmount";
            this.comboAmount.SelectedIndexChanged += new System.EventHandler(this.comboAmount_SelectedIndexChanged);
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // groupCollections
            // 
            this.groupCollections.Controls.Add(this.labelTotal);
            this.groupCollections.Controls.Add(this.labelCurCount);
            this.groupCollections.Controls.Add(this.labelTotCount);
            this.groupCollections.Controls.Add(this.labelCount);
            resources.ApplyResources(this.groupCollections, "groupCollections");
            this.groupCollections.Name = "groupCollections";
            this.groupCollections.TabStop = false;
            // 
            // labelTotal
            // 
            resources.ApplyResources(this.labelTotal, "labelTotal");
            this.labelTotal.Name = "labelTotal";
            // 
            // labelCurCount
            // 
            resources.ApplyResources(this.labelCurCount, "labelCurCount");
            this.labelCurCount.Name = "labelCurCount";
            // 
            // labelTotCount
            // 
            resources.ApplyResources(this.labelTotCount, "labelTotCount");
            this.labelTotCount.Name = "labelTotCount";
            // 
            // labelCount
            // 
            resources.ApplyResources(this.labelCount, "labelCount");
            this.labelCount.Name = "labelCount";
            // 
            // labelDesc
            // 
            resources.ApplyResources(this.labelDesc, "labelDesc");
            this.labelDesc.Name = "labelDesc";
            // 
            // buttonClear
            // 
            resources.ApplyResources(this.buttonClear, "buttonClear");
            this.buttonClear.Name = "buttonClear";
            this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
            // 
            // treeMain
            // 
            resources.ApplyResources(this.treeMain, "treeMain");
            this.treeMain.Name = "treeMain";
            // 
            // formMain
            // 
            resources.ApplyResources(this, "$this");
            this.Controls.Add(this.treeMain);
            this.Controls.Add(this.buttonClear);
            this.Controls.Add(this.labelDesc);
            this.Controls.Add(this.groupCollections);
            this.Controls.Add(this.groupVariables);
            this.Controls.Add(this.labelAllocation);
            this.Controls.Add(this.buttonRun);
            this.Controls.Add(this.groupPressure);
            this.Controls.Add(this.buttonExit);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "formMain";
            this.Load += new System.EventHandler(this.formMain_Load);
            this.groupPressure.ResumeLayout(false);
            this.groupVariables.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numericAllocations)).EndInit();
            this.groupCollections.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        [STAThread]
        static void Main()
        {
            Application.Run(new formMain());
        }

        private void formMain_Load(System.Object sender, System.EventArgs e)
        {
            _allocation = 1000;
            _curCount = 0;
            _totCount = 0;
            comboAmount.Items.Add("1000");
            comboAmount.Items.Add("5000");
            comboAmount.Items.Add("10000");
            comboAmount.Items.Add("100000");
            comboAmount.Items.Add("1000000");
            comboAmount.Items.Add("10000000");
            comboAmount.Items.Add("100000000");
            comboAmount.SelectedIndex = 0;
            UpdateDescLabel();
        }

        private void UpdateAllocationLabel()
        {
            labelAllocation.Text = String.Format(CultureInfo.InvariantCulture, "Allocate {0} bytes of available memory", comboAmount.Text);
        }

        private void UpdateDescLabel()
        {
            labelDesc.Text = String.Format(CultureInfo.InvariantCulture, "This demo allocates {0} bytes of available unmanaged memory, {1} times", comboAmount.Text, numericAllocations.Value);
        }

        private void buttonRun_Click(System.Object sender, System.EventArgs e)
        {
            Cursor = Cursors.WaitCursor;


            // Garbage collection is forced to "clean" the heap, then
            // we pause while waiting for the separate finalizer thread 
            // to clear the finalization queue. A final GC.Collect()
            // call clears out anything that may have been resurrected
            // by the finalizer.
            GC.Collect();
            GC.WaitForPendingFinalizers();
            GC.Collect();

            MEMORYSTATUSEX ms = new MEMORYSTATUSEX();

            ms.Init();
            NativeMethods.GlobalMemoryStatusEx(ref ms);

            int pressure = _allocation;

            treeMain.Nodes.Clear();

            for (int i = 0; i < numericAllocations.Value; i++)
            {
                int nodeElement = treeMain.Nodes.Add(new TreeNode(String.Format(CultureInfo.InvariantCulture, "Allocating 1/{0} for the {1}{2} time", _allocation, i + 1, GetSuffix(i))));

                MemoryHogger m = new MemoryHogger(pressure, radioPressureYes.Checked);
                TreeNode tn = new TreeNode();

                if (m.Success)
                {
                    tn.Text = "\tAllocation Succeeded!";
                    tn.BackColor = Color.FromArgb(0, 255, 0);
                    treeMain.Nodes[nodeElement].Nodes.Add(tn);
                }
                else
                {
                    tn.Text = "\tAllocation Failed!";
                    tn.BackColor = Color.Red;
                    treeMain.Nodes[nodeElement].Nodes.Add(tn);
                }
                tn.Parent.Expand();
            }
            _curCount = GC.CollectionCount(0) - _gcBaseLine - _totCount;
            _totCount = GC.CollectionCount(0) - _gcBaseLine;
            labelCount.Text = _curCount.ToString(CultureInfo.InvariantCulture);
            labelTotCount.Text = _totCount.ToString(CultureInfo.InvariantCulture);
            buttonClear.Enabled = true;
            Cursor = Cursors.Arrow;
        }

        private string GetSuffix(int i)
        {
            if (i == 0)
                return "st";
            else if (i == 1)
                return "nd";
            else if (i == 2)
                return "rd";
            else
                return "th";
        }

        private void buttonExit_Click(System.Object sender, System.EventArgs e)
        {
            this.Close();
        }

        private void comboAmount_SelectedIndexChanged(System.Object sender, System.EventArgs e)
        {
            _allocation = Convert.ToInt32(comboAmount.Text);
            UpdateDescLabel();
            UpdateAllocationLabel();
        }

        private void numericAllocations_ValueChanged(System.Object sender, System.EventArgs e)
        {
            UpdateDescLabel();
        }
        private void buttonClear_Click(object sender, System.EventArgs e)
        {
            treeMain.Nodes.Clear();
            labelCount.Text = "0";
            labelTotCount.Text = "0";
            _gcBaseLine += _totCount;
            _curCount = 0;
            _totCount = 0;

            buttonClear.Enabled = false;
        }

        class MemoryHogger : IDisposable
        {
            int pressure;
            bool addedPressure;
            bool allocated;
            IntPtr memptr;

            const int MEM_RESERVE = 0x2000;
            const int MEM_RELEASE = 0x8000;
            const int PAGE_READWRITE = 4;

            // Allocated unmanaged memory and, optionally, inform the
            // GC of the added memory
            public MemoryHogger(int size, bool addPressure)
            {
                if (addPressure)
                {
                    addedPressure = true;
                    pressure = size;
                    GC.AddMemoryPressure(pressure);
                }

                memptr = NativeMethods.VirtualAlloc((IntPtr)0, size, MEM_RESERVE, PAGE_READWRITE);
                allocated = (memptr != IntPtr.Zero);

                if (addedPressure && !allocated)
                {
                    GC.RemoveMemoryPressure(pressure);
                    addedPressure = false;
                }
            }

            // Finalization code for the MemoryHogger - make sure to reduce
            // the GC pressure by EXACTLY the amount that was added
            ~MemoryHogger()
            {
                if (allocated)
                {
                    NativeMethods.VirtualFree(memptr, 0, MEM_RELEASE);
                }

                if (addedPressure)
                {
                    GC.RemoveMemoryPressure(pressure);
                }
            }

            void IDisposable.Dispose() { }

            public bool Success
            {
                get { return allocated; }
            }
        }
    }
}
