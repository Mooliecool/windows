namespace CSVstoVBAInterop
{
    partial class InteropRibbon
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tabCfx = new Microsoft.Office.Tools.Ribbon.RibbonTab();
            this.grpVBAInteropSample = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.btnShowFormCS = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.tabCfx.SuspendLayout();
            this.grpVBAInteropSample.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabCfx
            // 
            this.tabCfx.Groups.Add(this.grpVBAInteropSample);
            this.tabCfx.Label = "VSTO Samples";
            this.tabCfx.Name = "tabCfx";
            // 
            // grpVBAInteropSample
            // 
            this.grpVBAInteropSample.Items.Add(this.btnShowFormCS);
            this.grpVBAInteropSample.Label = "VBA Interop Sample";
            this.grpVBAInteropSample.Name = "grpVBAInteropSample";
            // 
            // btnShowFormCS
            // 
            this.btnShowFormCS.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge;
            this.btnShowFormCS.Label = "Interop Form (C#)";
            this.btnShowFormCS.Name = "btnShowFormCS";
            this.btnShowFormCS.OfficeImageId = "VisualBasic";
            this.btnShowFormCS.ShowImage = true;
            this.btnShowFormCS.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.btnShowFormCS_Click);
            // 
            // InteropRibbon
            // 
            this.Name = "InteropRibbon";
            this.RibbonType = "Microsoft.Excel.Workbook";
            this.Tabs.Add(this.tabCfx);
            this.Load += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonUIEventArgs>(this.InteropRibbon_Load);
            this.tabCfx.ResumeLayout(false);
            this.tabCfx.PerformLayout();
            this.grpVBAInteropSample.ResumeLayout(false);
            this.grpVBAInteropSample.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonTab tabCfx;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpVBAInteropSample;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnShowFormCS;
    }

    partial class ThisRibbonCollection : Microsoft.Office.Tools.Ribbon.RibbonReadOnlyCollection
    {
        internal InteropRibbon InteropRibbon
        {
            get { return this.GetRibbon<InteropRibbon>(); }
        }
    }
}
