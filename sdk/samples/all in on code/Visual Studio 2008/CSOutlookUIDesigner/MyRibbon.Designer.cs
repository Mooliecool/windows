namespace CSOutlookUIDesigner
{
    partial class MyRibbon
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
            Microsoft.Office.Tools.Ribbon.RibbonDialogLauncher ribbonDialogLauncher1 = new Microsoft.Office.Tools.Ribbon.RibbonDialogLauncher();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem1 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem2 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem3 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem4 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem5 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem6 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem7 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem8 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            Microsoft.Office.Tools.Ribbon.RibbonDropDownItem ribbonDropDownItem9 = new Microsoft.Office.Tools.Ribbon.RibbonDropDownItem();
            this.myFirstTab = new Microsoft.Office.Tools.Ribbon.RibbonTab();
            this.grpOne = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.btnWeb = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.separator1 = new Microsoft.Office.Tools.Ribbon.RibbonSeparator();
            this.cboMyList = new Microsoft.Office.Tools.Ribbon.RibbonComboBox();
            this.tbSecondTab = new Microsoft.Office.Tools.Ribbon.RibbonToggleButton();
            this.chkShowGroup = new Microsoft.Office.Tools.Ribbon.RibbonCheckBox();
            this.grpTwo = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.splitButton = new Microsoft.Office.Tools.Ribbon.RibbonSplitButton();
            this.btnAlignLeft = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.btnAlignCenter = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.btnAlignRight = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.txtEdit = new Microsoft.Office.Tools.Ribbon.RibbonEditBox();
            this.lblSample = new Microsoft.Office.Tools.Ribbon.RibbonLabel();
            this.grpThree = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.buttonGroup1 = new Microsoft.Office.Tools.Ribbon.RibbonButtonGroup();
            this.btnOne = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.btnTwo = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.button10 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.mnuSample = new Microsoft.Office.Tools.Ribbon.RibbonMenu();
            this.mnuSubMenu = new Microsoft.Office.Tools.Ribbon.RibbonMenu();
            this.button3 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.button4 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.button5 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.button1 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.separator2 = new Microsoft.Office.Tools.Ribbon.RibbonSeparator();
            this.button2 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.separator3 = new Microsoft.Office.Tools.Ribbon.RibbonSeparator();
            this.glrCd = new Microsoft.Office.Tools.Ribbon.RibbonGallery();
            this.button11 = new Microsoft.Office.Tools.Ribbon.RibbonButton();
            this.mySecondTab = new Microsoft.Office.Tools.Ribbon.RibbonTab();
            this.grpMail = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.lblMailMode = new Microsoft.Office.Tools.Ribbon.RibbonLabel();
            this.grpAppointmentItem = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.label1 = new Microsoft.Office.Tools.Ribbon.RibbonLabel();
            this.grpTaskItem = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.label2 = new Microsoft.Office.Tools.Ribbon.RibbonLabel();
            this.grpContactItem = new Microsoft.Office.Tools.Ribbon.RibbonGroup();
            this.label3 = new Microsoft.Office.Tools.Ribbon.RibbonLabel();
            this.colorDlg = new System.Windows.Forms.ColorDialog();
            this.myFirstTab.SuspendLayout();
            this.grpOne.SuspendLayout();
            this.grpTwo.SuspendLayout();
            this.grpThree.SuspendLayout();
            this.buttonGroup1.SuspendLayout();
            this.mySecondTab.SuspendLayout();
            this.grpMail.SuspendLayout();
            this.grpAppointmentItem.SuspendLayout();
            this.grpTaskItem.SuspendLayout();
            this.grpContactItem.SuspendLayout();
            this.SuspendLayout();
            // 
            // myFirstTab
            // 
            this.myFirstTab.ControlId.ControlIdType = Microsoft.Office.Tools.Ribbon.RibbonControlIdType.Office;
            this.myFirstTab.Groups.Add(this.grpOne);
            this.myFirstTab.Groups.Add(this.grpTwo);
            this.myFirstTab.Groups.Add(this.grpThree);
            this.myFirstTab.KeyTip = "FT";
            this.myFirstTab.Label = "Sample Tab";
            this.myFirstTab.Name = "myFirstTab";
            // 
            // grpOne
            // 
            this.grpOne.DialogLauncher = ribbonDialogLauncher1;
            this.grpOne.Items.Add(this.btnWeb);
            this.grpOne.Items.Add(this.separator1);
            this.grpOne.Items.Add(this.cboMyList);
            this.grpOne.Items.Add(this.tbSecondTab);
            this.grpOne.Items.Add(this.chkShowGroup);
            this.grpOne.Label = "Group One";
            this.grpOne.Name = "grpOne";
            this.grpOne.DialogLauncherClick += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.myFirstGroup_DialogLauncherClick);
            // 
            // btnWeb
            // 
            this.btnWeb.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge;
            this.btnWeb.Image = global::CSOutlookUIDesigner.Properties.Resources.Globe;
            this.btnWeb.Label = "Project Home";
            this.btnWeb.Name = "btnWeb";
            this.btnWeb.ShowImage = true;
            this.btnWeb.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.btnWeb_Click);
            // 
            // separator1
            // 
            this.separator1.Name = "separator1";
            // 
            // cboMyList
            // 
            ribbonDropDownItem1.Label = "Item0";
            ribbonDropDownItem2.Label = "Item1";
            ribbonDropDownItem3.Label = "Item2";
            this.cboMyList.Items.Add(ribbonDropDownItem1);
            this.cboMyList.Items.Add(ribbonDropDownItem2);
            this.cboMyList.Items.Add(ribbonDropDownItem3);
            this.cboMyList.Label = "ComboBox:";
            this.cboMyList.Name = "cboMyList";
            this.cboMyList.OfficeImageId = "FormControlComboBox";
            this.cboMyList.ShowImage = true;
            this.cboMyList.SuperTip = "This is a ComboBox\r\nDrop down and Edit are both enabled.";
            this.cboMyList.Text = null;
            this.cboMyList.ItemsLoading += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.cboMyList_ItemsLoading);
            // 
            // tbSecondTab
            // 
            this.tbSecondTab.Checked = true;
            this.tbSecondTab.Label = "Second Tab";
            this.tbSecondTab.Name = "tbSecondTab";
            this.tbSecondTab.OfficeImageId = "ControlTabControl";
            this.tbSecondTab.ShowImage = true;
            this.tbSecondTab.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.tbSecondTab_Click);
            // 
            // chkShowGroup
            // 
            this.chkShowGroup.Checked = true;
            this.chkShowGroup.Label = "Group Three";
            this.chkShowGroup.Name = "chkShowGroup";
            this.chkShowGroup.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.chkShowGroup_Click);
            // 
            // grpTwo
            // 
            this.grpTwo.Items.Add(this.splitButton);
            this.grpTwo.Items.Add(this.txtEdit);
            this.grpTwo.Items.Add(this.lblSample);
            this.grpTwo.Label = "Group Two";
            this.grpTwo.Name = "grpTwo";
            // 
            // splitButton
            // 
            this.splitButton.Items.Add(this.btnAlignLeft);
            this.splitButton.Items.Add(this.btnAlignCenter);
            this.splitButton.Items.Add(this.btnAlignRight);
            this.splitButton.Label = "SplitButton";
            this.splitButton.Name = "splitButton";
            this.splitButton.OfficeImageId = "AlignLeft";
            this.splitButton.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.splitButton_Click);
            // 
            // btnAlignLeft
            // 
            this.btnAlignLeft.Label = "Left";
            this.btnAlignLeft.Name = "btnAlignLeft";
            this.btnAlignLeft.OfficeImageId = "AlignLeft";
            this.btnAlignLeft.ShowImage = true;
            this.btnAlignLeft.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.btnAlign_Click);
            // 
            // btnAlignCenter
            // 
            this.btnAlignCenter.Label = "Center";
            this.btnAlignCenter.Name = "btnAlignCenter";
            this.btnAlignCenter.OfficeImageId = "AlignCenter";
            this.btnAlignCenter.ShowImage = true;
            this.btnAlignCenter.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.btnAlign_Click);
            // 
            // btnAlignRight
            // 
            this.btnAlignRight.Label = "Right";
            this.btnAlignRight.Name = "btnAlignRight";
            this.btnAlignRight.OfficeImageId = "AlignRight";
            this.btnAlignRight.ShowImage = true;
            this.btnAlignRight.Click += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.btnAlign_Click);
            // 
            // txtEdit
            // 
            this.txtEdit.Label = "Edit Box:";
            this.txtEdit.Name = "txtEdit";
            this.txtEdit.OfficeImageId = "ActiveXTextBox";
            this.txtEdit.ShowImage = true;
            this.txtEdit.Text = null;
            this.txtEdit.TextChanged += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonControlEventArgs>(this.txtEdit_TextChanged);
            // 
            // lblSample
            // 
            this.lblSample.Label = "This is just a label";
            this.lblSample.Name = "lblSample";
            // 
            // grpThree
            // 
            this.grpThree.Items.Add(this.buttonGroup1);
            this.grpThree.Items.Add(this.mnuSample);
            this.grpThree.Items.Add(this.separator3);
            this.grpThree.Items.Add(this.glrCd);
            this.grpThree.Label = "Group Three";
            this.grpThree.Name = "grpThree";
            // 
            // buttonGroup1
            // 
            this.buttonGroup1.Items.Add(this.btnOne);
            this.buttonGroup1.Items.Add(this.btnTwo);
            this.buttonGroup1.Items.Add(this.button10);
            this.buttonGroup1.Name = "buttonGroup1";
            // 
            // btnOne
            // 
            this.btnOne.Label = "One";
            this.btnOne.Name = "btnOne";
            // 
            // btnTwo
            // 
            this.btnTwo.Label = "Two";
            this.btnTwo.Name = "btnTwo";
            // 
            // button10
            // 
            this.button10.Label = "Three";
            this.button10.Name = "button10";
            // 
            // mnuSample
            // 
            this.mnuSample.Dynamic = true;
            this.mnuSample.Items.Add(this.mnuSubMenu);
            this.mnuSample.Items.Add(this.button1);
            this.mnuSample.Items.Add(this.separator2);
            this.mnuSample.Items.Add(this.button2);
            this.mnuSample.Label = "Menu Sample";
            this.mnuSample.Name = "mnuSample";
            this.mnuSample.OfficeImageId = "HappyFace";
            this.mnuSample.ShowImage = true;
            // 
            // mnuSubMenu
            // 
            this.mnuSubMenu.Dynamic = true;
            this.mnuSubMenu.Items.Add(this.button3);
            this.mnuSubMenu.Items.Add(this.button4);
            this.mnuSubMenu.Items.Add(this.button5);
            this.mnuSubMenu.Label = "Sub Menu";
            this.mnuSubMenu.Name = "mnuSubMenu";
            this.mnuSubMenu.ShowImage = true;
            // 
            // button3
            // 
            this.button3.Label = "Sub Item 1";
            this.button3.Name = "button3";
            this.button3.ShowImage = true;
            // 
            // button4
            // 
            this.button4.Label = "Sub Item 2";
            this.button4.Name = "button4";
            this.button4.ShowImage = true;
            // 
            // button5
            // 
            this.button5.Label = "Sub Item 3";
            this.button5.Name = "button5";
            this.button5.ShowImage = true;
            // 
            // button1
            // 
            this.button1.Label = "Item 1";
            this.button1.Name = "button1";
            this.button1.ShowImage = true;
            // 
            // separator2
            // 
            this.separator2.Name = "separator2";
            // 
            // button2
            // 
            this.button2.Label = "Item 2";
            this.button2.Name = "button2";
            this.button2.ShowImage = true;
            // 
            // separator3
            // 
            this.separator3.Name = "separator3";
            // 
            // glrCd
            // 
            this.glrCd.Buttons.Add(this.button11);
            this.glrCd.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge;
            this.glrCd.Image = global::CSOutlookUIDesigner.Properties.Resources.blank_cd;
            ribbonDropDownItem4.Image = global::CSOutlookUIDesigner.Properties.Resources.AudioCD;
            ribbonDropDownItem4.Label = "Audio CD";
            ribbonDropDownItem5.Image = global::CSOutlookUIDesigner.Properties.Resources.AudioCDPlus;
            ribbonDropDownItem5.Label = "Audio CD Plus";
            ribbonDropDownItem6.Image = global::CSOutlookUIDesigner.Properties.Resources.audiodvd;
            ribbonDropDownItem6.Label = "Audio DVD";
            ribbonDropDownItem7.Image = global::CSOutlookUIDesigner.Properties.Resources.BluRayMovieDisk;
            ribbonDropDownItem7.Label = "BD Movie Disk";
            ribbonDropDownItem8.Image = global::CSOutlookUIDesigner.Properties.Resources.blank_cd;
            ribbonDropDownItem8.Label = "Blank CD";
            ribbonDropDownItem9.Image = global::CSOutlookUIDesigner.Properties.Resources.CD_V;
            ribbonDropDownItem9.Label = "VCD";
            this.glrCd.Items.Add(ribbonDropDownItem4);
            this.glrCd.Items.Add(ribbonDropDownItem5);
            this.glrCd.Items.Add(ribbonDropDownItem6);
            this.glrCd.Items.Add(ribbonDropDownItem7);
            this.glrCd.Items.Add(ribbonDropDownItem8);
            this.glrCd.Items.Add(ribbonDropDownItem9);
            this.glrCd.Label = "Disk Gallery";
            this.glrCd.Name = "glrCd";
            this.glrCd.ShowImage = true;
            // 
            // button11
            // 
            this.button11.Image = global::CSOutlookUIDesigner.Properties.Resources.BurnCD;
            this.button11.Label = "Burn Disk";
            this.button11.Name = "button11";
            this.button11.ShowImage = true;
            // 
            // mySecondTab
            // 
            this.mySecondTab.Groups.Add(this.grpMail);
            this.mySecondTab.Groups.Add(this.grpAppointmentItem);
            this.mySecondTab.Groups.Add(this.grpTaskItem);
            this.mySecondTab.Groups.Add(this.grpContactItem);
            this.mySecondTab.Label = "Second Sample Tab";
            this.mySecondTab.Name = "mySecondTab";
            // 
            // grpMail
            // 
            this.grpMail.Items.Add(this.lblMailMode);
            this.grpMail.Label = "Mail Item";
            this.grpMail.Name = "grpMail";
            this.grpMail.Visible = false;
            // 
            // lblMailMode
            // 
            this.lblMailMode.Label = "label1";
            this.lblMailMode.Name = "lblMailMode";
            // 
            // grpAppointmentItem
            // 
            this.grpAppointmentItem.Items.Add(this.label1);
            this.grpAppointmentItem.Label = "Appointment Item";
            this.grpAppointmentItem.Name = "grpAppointmentItem";
            this.grpAppointmentItem.Visible = false;
            // 
            // label1
            // 
            this.label1.Label = "This is an Appointment Item";
            this.label1.Name = "label1";
            // 
            // grpTaskItem
            // 
            this.grpTaskItem.Items.Add(this.label2);
            this.grpTaskItem.Label = "Task Item";
            this.grpTaskItem.Name = "grpTaskItem";
            this.grpTaskItem.Visible = false;
            // 
            // label2
            // 
            this.label2.Label = "This is a Task Item";
            this.label2.Name = "label2";
            // 
            // grpContactItem
            // 
            this.grpContactItem.Items.Add(this.label3);
            this.grpContactItem.Label = "Contact Item";
            this.grpContactItem.Name = "grpContactItem";
            this.grpContactItem.Visible = false;
            // 
            // label3
            // 
            this.label3.Label = "This is a Contact Item";
            this.label3.Name = "label3";
            // 
            // MyRibbon
            // 
            this.Name = "MyRibbon";
            this.RibbonType = "Microsoft.Outlook.Appointment, Microsoft.Outlook.Contact, Microsoft.Outlook.Mail." +
                "Compose, Microsoft.Outlook.Mail.Read, Microsoft.Outlook.Task";
            this.Tabs.Add(this.myFirstTab);
            this.Tabs.Add(this.mySecondTab);
            this.Load += new System.EventHandler<Microsoft.Office.Tools.Ribbon.RibbonUIEventArgs>(this.MyRibbon_Load);
            this.myFirstTab.ResumeLayout(false);
            this.myFirstTab.PerformLayout();
            this.grpOne.ResumeLayout(false);
            this.grpOne.PerformLayout();
            this.grpTwo.ResumeLayout(false);
            this.grpTwo.PerformLayout();
            this.grpThree.ResumeLayout(false);
            this.grpThree.PerformLayout();
            this.buttonGroup1.ResumeLayout(false);
            this.buttonGroup1.PerformLayout();
            this.mySecondTab.ResumeLayout(false);
            this.mySecondTab.PerformLayout();
            this.grpMail.ResumeLayout(false);
            this.grpMail.PerformLayout();
            this.grpAppointmentItem.ResumeLayout(false);
            this.grpAppointmentItem.PerformLayout();
            this.grpTaskItem.ResumeLayout(false);
            this.grpTaskItem.PerformLayout();
            this.grpContactItem.ResumeLayout(false);
            this.grpContactItem.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonTab myFirstTab;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpOne;
        private Microsoft.Office.Tools.Ribbon.RibbonTab mySecondTab;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnWeb;
        internal Microsoft.Office.Tools.Ribbon.RibbonSeparator separator1;
        internal Microsoft.Office.Tools.Ribbon.RibbonComboBox cboMyList;
        internal Microsoft.Office.Tools.Ribbon.RibbonToggleButton tbSecondTab;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpTwo;
        internal Microsoft.Office.Tools.Ribbon.RibbonEditBox txtEdit;
        internal Microsoft.Office.Tools.Ribbon.RibbonSplitButton splitButton;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnAlignLeft;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnAlignCenter;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnAlignRight;
        internal Microsoft.Office.Tools.Ribbon.RibbonLabel lblSample;
        internal Microsoft.Office.Tools.Ribbon.RibbonCheckBox chkShowGroup;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpThree;
        internal Microsoft.Office.Tools.Ribbon.RibbonButtonGroup buttonGroup1;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnOne;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton btnTwo;
        internal Microsoft.Office.Tools.Ribbon.RibbonMenu mnuSample;
        internal Microsoft.Office.Tools.Ribbon.RibbonMenu mnuSubMenu;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button3;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button4;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button5;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button1;
        internal Microsoft.Office.Tools.Ribbon.RibbonSeparator separator2;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button2;
        internal Microsoft.Office.Tools.Ribbon.RibbonGallery glrCd;
        internal Microsoft.Office.Tools.Ribbon.RibbonSeparator separator3;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button10;
        private Microsoft.Office.Tools.Ribbon.RibbonButton button11;
        private System.Windows.Forms.ColorDialog colorDlg;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpMail;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpAppointmentItem;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpTaskItem;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup grpContactItem;
        internal Microsoft.Office.Tools.Ribbon.RibbonLabel lblMailMode;
        internal Microsoft.Office.Tools.Ribbon.RibbonLabel label1;
        internal Microsoft.Office.Tools.Ribbon.RibbonLabel label2;
        internal Microsoft.Office.Tools.Ribbon.RibbonLabel label3;
    }

    partial class ThisRibbonCollection : Microsoft.Office.Tools.Ribbon.RibbonReadOnlyCollection
    {
        internal MyRibbon MyRibbon
        {
            get { return this.GetRibbon<MyRibbon>(); }
        }
    }
}
