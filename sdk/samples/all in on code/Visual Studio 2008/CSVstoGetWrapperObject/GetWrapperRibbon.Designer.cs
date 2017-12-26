namespace CSVstoGetWrapperObject
{
    partial class GetWrapperRibbon
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
            this.grpGetWrapperObject = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.btnShowGetWrapperCS = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.tabCfx.SuspendLayout();
            this.grpGetWrapperObject.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabCfx
            // 
            this.tabCfx.Groups.Add(this.grpGetWrapperObject);
            this.tabCfx.Label = "VSTO Samples";
            this.tabCfx.Name = "tabCfx";
            // 
            // grpGetWrapperObject
            // 
            this.grpGetWrapperObject.Items.Add(this.btnShowGetWrapperCS);
            this.grpGetWrapperObject.Label = "GetVstoObject Sample";
            this.grpGetWrapperObject.Name = "grpGetWrapperObject";
            // 
            // btnShowGetWrapperCS
            // 
            this.btnShowGetWrapperCS.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge;
            this.btnShowGetWrapperCS.Label = "Get VSTO Wrapper (C#)";
            this.btnShowGetWrapperCS.Name = "btnShowGetWrapperCS";
            this.btnShowGetWrapperCS.OfficeImageId = "UpgradeWorkbook";
            this.btnShowGetWrapperCS.ShowImage = true;
            this.btnShowGetWrapperCS.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.btnShowGetWrapperCS_Click);
            // 
            // GetWrapperRibbon
            // 
            this.Name = "GetWrapperRibbon";
            this.RibbonType = "Microsoft.Excel.Workbook";
            this.Tabs.Add(this.tabCfx);
            this.Load += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonUIEventArgs>(this.GetWrapperRibbon_Load);
            this.tabCfx.ResumeLayout(false);
            this.tabCfx.PerformLayout();
            this.grpGetWrapperObject.ResumeLayout(false);
            this.grpGetWrapperObject.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonTab tabCfx;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpGetWrapperObject;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnShowGetWrapperCS;
    }

    partial class ThisRibbonCollection : Microsoft.Office.Tools.Ribbon.RibbonReadOnlyCollection
    {
        internal GetWrapperRibbon GetWrapperRibbon
        {
            get { return this.GetRibbon<GetWrapperRibbon>(); }
        }
    }
}
