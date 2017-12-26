Partial Class MyRibbon
    Inherits Microsoft.Office.Tools.Ribbon.OfficeRibbon

    <System.Diagnostics.DebuggerNonUserCode()> _
   Public Sub New(ByVal container As System.ComponentModel.IContainer)
        MyClass.New()

        'Required for Windows.Forms Class Composition Designer support
        If (container IsNot Nothing) Then
            container.Add(Me)
        End If

    End Sub

    <System.Diagnostics.DebuggerNonUserCode()> _
    Public Sub New()
        MyBase.New()

        'This call is required by the Component Designer.
        InitializeComponent()

    End Sub

    'Component overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Component Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Component Designer
    'It can be modified using the Component Designer.
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim RibbonDialogLauncher1 As Microsoft.Office.Tools.Ribbon.RibbonDialogLauncher = New Microsoft.Office.Tools.Ribbon.RibbonDialogLauncher
        Dim RibbonDropDownItem1 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem2 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem3 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem4 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem5 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem6 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem7 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem8 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Dim RibbonDropDownItem9 As Microsoft.Office.Tools.Ribbon.RibbonDropDownItem = New Microsoft.Office.Tools.Ribbon.RibbonDropDownItem
        Me.Tab1 = New Microsoft.Office.Tools.Ribbon.RibbonTab
        Me.grpOne = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.btnWeb = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.separator1 = New Microsoft.Office.Tools.Ribbon.RibbonSeparator
        Me.cboMyList = New Microsoft.Office.Tools.Ribbon.RibbonComboBox
        Me.tbSecondTab = New Microsoft.Office.Tools.Ribbon.RibbonToggleButton
        Me.chkShowGroup = New Microsoft.Office.Tools.Ribbon.RibbonCheckBox
        Me.grpTwo = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.splitButton = New Microsoft.Office.Tools.Ribbon.RibbonSplitButton
        Me.btnAlignLeft = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.btnAlignCenter = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.btnAlignRight = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.txtEdit = New Microsoft.Office.Tools.Ribbon.RibbonEditBox
        Me.lblSample = New Microsoft.Office.Tools.Ribbon.RibbonLabel
        Me.grpThree = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.buttonGroup1 = New Microsoft.Office.Tools.Ribbon.RibbonButtonGroup
        Me.btnOne = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.btnTwo = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.button10 = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.mnuSample = New Microsoft.Office.Tools.Ribbon.RibbonMenu
        Me.mnuSubMenu = New Microsoft.Office.Tools.Ribbon.RibbonMenu
        Me.button3 = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.button4 = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.button5 = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.button1 = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.separator2 = New Microsoft.Office.Tools.Ribbon.RibbonSeparator
        Me.button2 = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.separator3 = New Microsoft.Office.Tools.Ribbon.RibbonSeparator
        Me.glrCd = New Microsoft.Office.Tools.Ribbon.RibbonGallery
        Me.button11 = New Microsoft.Office.Tools.Ribbon.RibbonButton
        Me.colorDlg = New System.Windows.Forms.ColorDialog
        Me.mySecondTab = New Microsoft.Office.Tools.Ribbon.RibbonTab
        Me.grpMail = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.lblMailMode = New Microsoft.Office.Tools.Ribbon.RibbonLabel
        Me.grpAppointmentItem = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.label1 = New Microsoft.Office.Tools.Ribbon.RibbonLabel
        Me.grpTaskItem = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.label2 = New Microsoft.Office.Tools.Ribbon.RibbonLabel
        Me.grpContactItem = New Microsoft.Office.Tools.Ribbon.RibbonGroup
        Me.label3 = New Microsoft.Office.Tools.Ribbon.RibbonLabel
        Me.Tab1.SuspendLayout()
        Me.grpOne.SuspendLayout()
        Me.grpTwo.SuspendLayout()
        Me.grpThree.SuspendLayout()
        Me.buttonGroup1.SuspendLayout()
        Me.mySecondTab.SuspendLayout()
        Me.grpMail.SuspendLayout()
        Me.grpAppointmentItem.SuspendLayout()
        Me.grpTaskItem.SuspendLayout()
        Me.grpContactItem.SuspendLayout()
        Me.SuspendLayout()
        '
        'Tab1
        '
        Me.Tab1.ControlId.ControlIdType = Microsoft.Office.Tools.Ribbon.RibbonControlIdType.Office
        Me.Tab1.Groups.Add(Me.grpOne)
        Me.Tab1.Groups.Add(Me.grpTwo)
        Me.Tab1.Groups.Add(Me.grpThree)
        Me.Tab1.Label = "Sample Tab"
        Me.Tab1.Name = "Tab1"
        '
        'grpOne
        '
        Me.grpOne.DialogLauncher = RibbonDialogLauncher1
        Me.grpOne.Items.Add(Me.btnWeb)
        Me.grpOne.Items.Add(Me.separator1)
        Me.grpOne.Items.Add(Me.cboMyList)
        Me.grpOne.Items.Add(Me.tbSecondTab)
        Me.grpOne.Items.Add(Me.chkShowGroup)
        Me.grpOne.Label = "Group One"
        Me.grpOne.Name = "grpOne"
        '
        'btnWeb
        '
        Me.btnWeb.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge
        Me.btnWeb.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.Globe
        Me.btnWeb.Label = "Project Home"
        Me.btnWeb.Name = "btnWeb"
        Me.btnWeb.ShowImage = True
        '
        'separator1
        '
        Me.separator1.Name = "separator1"
        '
        'cboMyList
        '
        RibbonDropDownItem1.Label = "Item0"
        RibbonDropDownItem2.Label = "Item1"
        RibbonDropDownItem3.Label = "Item2"
        Me.cboMyList.Items.Add(RibbonDropDownItem1)
        Me.cboMyList.Items.Add(RibbonDropDownItem2)
        Me.cboMyList.Items.Add(RibbonDropDownItem3)
        Me.cboMyList.Label = "ComboBox:"
        Me.cboMyList.Name = "cboMyList"
        Me.cboMyList.OfficeImageId = "FormControlComboBox"
        Me.cboMyList.ShowImage = True
        Me.cboMyList.SuperTip = "This is a ComboBox" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Drop down and Edit are both enabled."
        Me.cboMyList.Text = Nothing
        '
        'tbSecondTab
        '
        Me.tbSecondTab.Checked = True
        Me.tbSecondTab.Label = "Second Tab"
        Me.tbSecondTab.Name = "tbSecondTab"
        Me.tbSecondTab.OfficeImageId = "ControlTabControl"
        Me.tbSecondTab.ShowImage = True
        '
        'chkShowGroup
        '
        Me.chkShowGroup.Checked = True
        Me.chkShowGroup.Label = "Group Three"
        Me.chkShowGroup.Name = "chkShowGroup"
        '
        'grpTwo
        '
        Me.grpTwo.Items.Add(Me.splitButton)
        Me.grpTwo.Items.Add(Me.txtEdit)
        Me.grpTwo.Items.Add(Me.lblSample)
        Me.grpTwo.Label = "Group Two"
        Me.grpTwo.Name = "grpTwo"
        '
        'splitButton
        '
        Me.splitButton.Items.Add(Me.btnAlignLeft)
        Me.splitButton.Items.Add(Me.btnAlignCenter)
        Me.splitButton.Items.Add(Me.btnAlignRight)
        Me.splitButton.Label = "SplitButton"
        Me.splitButton.Name = "splitButton"
        Me.splitButton.OfficeImageId = "AlignLeft"
        '
        'btnAlignLeft
        '
        Me.btnAlignLeft.Label = "Left"
        Me.btnAlignLeft.Name = "btnAlignLeft"
        Me.btnAlignLeft.OfficeImageId = "AlignLeft"
        Me.btnAlignLeft.ShowImage = True
        '
        'btnAlignCenter
        '
        Me.btnAlignCenter.Label = "Center"
        Me.btnAlignCenter.Name = "btnAlignCenter"
        Me.btnAlignCenter.OfficeImageId = "AlignCenter"
        Me.btnAlignCenter.ShowImage = True
        '
        'btnAlignRight
        '
        Me.btnAlignRight.Label = "Right"
        Me.btnAlignRight.Name = "btnAlignRight"
        Me.btnAlignRight.OfficeImageId = "AlignRight"
        Me.btnAlignRight.ShowImage = True
        '
        'txtEdit
        '
        Me.txtEdit.Label = "Edit Box:"
        Me.txtEdit.Name = "txtEdit"
        Me.txtEdit.OfficeImageId = "ActiveXTextBox"
        Me.txtEdit.ShowImage = True
        Me.txtEdit.Text = Nothing
        '
        'lblSample
        '
        Me.lblSample.Label = "This is just a label"
        Me.lblSample.Name = "lblSample"
        '
        'grpThree
        '
        Me.grpThree.Items.Add(Me.buttonGroup1)
        Me.grpThree.Items.Add(Me.mnuSample)
        Me.grpThree.Items.Add(Me.separator3)
        Me.grpThree.Items.Add(Me.glrCd)
        Me.grpThree.Label = "Group Three"
        Me.grpThree.Name = "grpThree"
        '
        'buttonGroup1
        '
        Me.buttonGroup1.Items.Add(Me.btnOne)
        Me.buttonGroup1.Items.Add(Me.btnTwo)
        Me.buttonGroup1.Items.Add(Me.button10)
        Me.buttonGroup1.Name = "buttonGroup1"
        '
        'btnOne
        '
        Me.btnOne.Label = "One"
        Me.btnOne.Name = "btnOne"
        '
        'btnTwo
        '
        Me.btnTwo.Label = "Two"
        Me.btnTwo.Name = "btnTwo"
        '
        'button10
        '
        Me.button10.Label = "Three"
        Me.button10.Name = "button10"
        '
        'mnuSample
        '
        Me.mnuSample.Dynamic = True
        Me.mnuSample.Items.Add(Me.mnuSubMenu)
        Me.mnuSample.Items.Add(Me.button1)
        Me.mnuSample.Items.Add(Me.separator2)
        Me.mnuSample.Items.Add(Me.button2)
        Me.mnuSample.Label = "Menu Sample"
        Me.mnuSample.Name = "mnuSample"
        Me.mnuSample.OfficeImageId = "HappyFace"
        Me.mnuSample.ShowImage = True
        '
        'mnuSubMenu
        '
        Me.mnuSubMenu.Dynamic = True
        Me.mnuSubMenu.Items.Add(Me.button3)
        Me.mnuSubMenu.Items.Add(Me.button4)
        Me.mnuSubMenu.Items.Add(Me.button5)
        Me.mnuSubMenu.Label = "Sub Menu"
        Me.mnuSubMenu.Name = "mnuSubMenu"
        Me.mnuSubMenu.ShowImage = True
        '
        'button3
        '
        Me.button3.Label = "Sub Item 1"
        Me.button3.Name = "button3"
        Me.button3.ShowImage = True
        '
        'button4
        '
        Me.button4.Label = "Sub Item 2"
        Me.button4.Name = "button4"
        Me.button4.ShowImage = True
        '
        'button5
        '
        Me.button5.Label = "Sub Item 3"
        Me.button5.Name = "button5"
        Me.button5.ShowImage = True
        '
        'button1
        '
        Me.button1.Label = "Item 1"
        Me.button1.Name = "button1"
        Me.button1.ShowImage = True
        '
        'separator2
        '
        Me.separator2.Name = "separator2"
        '
        'button2
        '
        Me.button2.Label = "Item 2"
        Me.button2.Name = "button2"
        Me.button2.ShowImage = True
        '
        'separator3
        '
        Me.separator3.Name = "separator3"
        '
        'glrCd
        '
        Me.glrCd.Buttons.Add(Me.button11)
        Me.glrCd.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge
        Me.glrCd.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.blank_cd
        RibbonDropDownItem4.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.AudioCD
        RibbonDropDownItem4.Label = "Audio CD"
        RibbonDropDownItem5.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.AudioCDPlus
        RibbonDropDownItem5.Label = "Audio CD Plus"
        RibbonDropDownItem6.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.audiodvd
        RibbonDropDownItem6.Label = "Audio DVD"
        RibbonDropDownItem7.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.BluRayMovieDisk
        RibbonDropDownItem7.Label = "BD Movie Disk"
        RibbonDropDownItem8.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.blank_cd
        RibbonDropDownItem8.Label = "Blank CD"
        RibbonDropDownItem9.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.CD_V
        RibbonDropDownItem9.Label = "VCD"
        Me.glrCd.Items.Add(RibbonDropDownItem4)
        Me.glrCd.Items.Add(RibbonDropDownItem5)
        Me.glrCd.Items.Add(RibbonDropDownItem6)
        Me.glrCd.Items.Add(RibbonDropDownItem7)
        Me.glrCd.Items.Add(RibbonDropDownItem8)
        Me.glrCd.Items.Add(RibbonDropDownItem9)
        Me.glrCd.Label = "Disk Gallery"
        Me.glrCd.Name = "glrCd"
        Me.glrCd.ShowImage = True
        '
        'button11
        '
        Me.button11.Image = Global.VBOutlookRibbonDesigner.My.Resources.Resources.BurnCD
        Me.button11.Label = "Burn Disk"
        Me.button11.Name = "button11"
        Me.button11.ShowImage = True
        '
        'mySecondTab
        '
        Me.mySecondTab.Groups.Add(Me.grpMail)
        Me.mySecondTab.Groups.Add(Me.grpAppointmentItem)
        Me.mySecondTab.Groups.Add(Me.grpTaskItem)
        Me.mySecondTab.Groups.Add(Me.grpContactItem)
        Me.mySecondTab.Label = "Second Sample Tab"
        Me.mySecondTab.Name = "mySecondTab"
        '
        'grpMail
        '
        Me.grpMail.Items.Add(Me.lblMailMode)
        Me.grpMail.Label = "Mail Item"
        Me.grpMail.Name = "grpMail"
        Me.grpMail.Visible = False
        '
        'lblMailMode
        '
        Me.lblMailMode.Label = "label1"
        Me.lblMailMode.Name = "lblMailMode"
        '
        'grpAppointmentItem
        '
        Me.grpAppointmentItem.Items.Add(Me.label1)
        Me.grpAppointmentItem.Label = "Appointment Item"
        Me.grpAppointmentItem.Name = "grpAppointmentItem"
        Me.grpAppointmentItem.Visible = False
        '
        'label1
        '
        Me.label1.Label = "This is an Appointment Item"
        Me.label1.Name = "label1"
        '
        'grpTaskItem
        '
        Me.grpTaskItem.Items.Add(Me.label2)
        Me.grpTaskItem.Label = "Task Item"
        Me.grpTaskItem.Name = "grpTaskItem"
        Me.grpTaskItem.Visible = False
        '
        'label2
        '
        Me.label2.Label = "This is a Task Item"
        Me.label2.Name = "label2"
        '
        'grpContactItem
        '
        Me.grpContactItem.Items.Add(Me.label3)
        Me.grpContactItem.Label = "Contact Item"
        Me.grpContactItem.Name = "grpContactItem"
        Me.grpContactItem.Visible = False
        '
        'label3
        '
        Me.label3.Label = "This is a Contact Item"
        Me.label3.Name = "label3"
        '
        'MyRibbon
        '
        Me.Name = "MyRibbon"
        Me.RibbonType = "Microsoft.Outlook.Appointment, Microsoft.Outlook.Contact, Microsoft.Outlook.Mail." & _
            "Compose, Microsoft.Outlook.Mail.Read, Microsoft.Outlook.Task"
        Me.Tabs.Add(Me.Tab1)
        Me.Tabs.Add(Me.mySecondTab)
        Me.Tab1.ResumeLayout(False)
        Me.Tab1.PerformLayout()
        Me.grpOne.ResumeLayout(False)
        Me.grpOne.PerformLayout()
        Me.grpTwo.ResumeLayout(False)
        Me.grpTwo.PerformLayout()
        Me.grpThree.ResumeLayout(False)
        Me.grpThree.PerformLayout()
        Me.buttonGroup1.ResumeLayout(False)
        Me.buttonGroup1.PerformLayout()
        Me.mySecondTab.ResumeLayout(False)
        Me.mySecondTab.PerformLayout()
        Me.grpMail.ResumeLayout(False)
        Me.grpMail.PerformLayout()
        Me.grpAppointmentItem.ResumeLayout(False)
        Me.grpAppointmentItem.PerformLayout()
        Me.grpTaskItem.ResumeLayout(False)
        Me.grpTaskItem.PerformLayout()
        Me.grpContactItem.ResumeLayout(False)
        Me.grpContactItem.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents Tab1 As Microsoft.Office.Tools.Ribbon.RibbonTab
    Private WithEvents colorDlg As System.Windows.Forms.ColorDialog
    Friend WithEvents grpOne As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents btnWeb As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents separator1 As Microsoft.Office.Tools.Ribbon.RibbonSeparator
    Friend WithEvents cboMyList As Microsoft.Office.Tools.Ribbon.RibbonComboBox
    Friend WithEvents tbSecondTab As Microsoft.Office.Tools.Ribbon.RibbonToggleButton
    Friend WithEvents chkShowGroup As Microsoft.Office.Tools.Ribbon.RibbonCheckBox
    Friend WithEvents grpTwo As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents splitButton As Microsoft.Office.Tools.Ribbon.RibbonSplitButton
    Friend WithEvents btnAlignLeft As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents btnAlignCenter As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents btnAlignRight As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents txtEdit As Microsoft.Office.Tools.Ribbon.RibbonEditBox
    Friend WithEvents lblSample As Microsoft.Office.Tools.Ribbon.RibbonLabel
    Friend WithEvents grpThree As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents buttonGroup1 As Microsoft.Office.Tools.Ribbon.RibbonButtonGroup
    Friend WithEvents btnOne As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents btnTwo As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents button10 As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents mnuSample As Microsoft.Office.Tools.Ribbon.RibbonMenu
    Friend WithEvents mnuSubMenu As Microsoft.Office.Tools.Ribbon.RibbonMenu
    Friend WithEvents button3 As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents button4 As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents button5 As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents button1 As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents separator2 As Microsoft.Office.Tools.Ribbon.RibbonSeparator
    Friend WithEvents button2 As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents separator3 As Microsoft.Office.Tools.Ribbon.RibbonSeparator
    Friend WithEvents glrCd As Microsoft.Office.Tools.Ribbon.RibbonGallery
    Private WithEvents button11 As Microsoft.Office.Tools.Ribbon.RibbonButton
    Friend WithEvents mySecondTab As Microsoft.Office.Tools.Ribbon.RibbonTab
    Friend WithEvents grpMail As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents lblMailMode As Microsoft.Office.Tools.Ribbon.RibbonLabel
    Friend WithEvents grpAppointmentItem As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents label1 As Microsoft.Office.Tools.Ribbon.RibbonLabel
    Friend WithEvents grpTaskItem As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents label2 As Microsoft.Office.Tools.Ribbon.RibbonLabel
    Friend WithEvents grpContactItem As Microsoft.Office.Tools.Ribbon.RibbonGroup
    Friend WithEvents label3 As Microsoft.Office.Tools.Ribbon.RibbonLabel
End Class

Partial Class ThisRibbonCollection
    Inherits Microsoft.Office.Tools.Ribbon.RibbonReadOnlyCollection

    <System.Diagnostics.DebuggerNonUserCode()> _
    Friend ReadOnly Property MyRibbon() As MyRibbon
        Get
            Return Me.GetRibbon(Of MyRibbon)()
        End Get
    End Property
End Class
