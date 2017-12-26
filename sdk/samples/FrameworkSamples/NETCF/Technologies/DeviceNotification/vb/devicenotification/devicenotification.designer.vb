Partial Public Class NotificationSample
    Inherits System.Windows.Forms.Form

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer
    Private m_Menu As System.Windows.Forms.MainMenu

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> Private Sub InitializeComponent()
        Me.m_Menu = New System.Windows.Forms.MainMenu
        Me.NotifyMenu = New System.Windows.Forms.MenuItem
        Me.NotifyShow = New System.Windows.Forms.MenuItem
        Me.NotifyHide = New System.Windows.Forms.MenuItem
        Me.NewNotificationAdd = New System.Windows.Forms.MenuItem
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.Label4 = New System.Windows.Forms.Label
        Me.Label5 = New System.Windows.Forms.Label
        Me.NotifyCaption = New System.Windows.Forms.TextBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.Label7 = New System.Windows.Forms.Label
        Me.NotifyText = New System.Windows.Forms.TextBox
        Me.NotifyCritical = New System.Windows.Forms.CheckBox
        Me.Label8 = New System.Windows.Forms.Label
        Me.Label9 = New System.Windows.Forms.Label
        Me.NotifyReponce = New System.Windows.Forms.Label
        Me.NotifyVisable = New System.Windows.Forms.Label
        Me.NotifyIcon = New System.Windows.Forms.ComboBox
        Me.NotifyDuration = New System.Windows.Forms.NumericUpDown
        Me.SuspendLayout()
        '
        'm_Menu
        '
        Me.m_Menu.MenuItems.Add(Me.NotifyMenu)
        Me.m_Menu.MenuItems.Add(Me.NewNotificationAdd)
        '
        'NotifyMenu
        '
        Me.NotifyMenu.MenuItems.Add(Me.NotifyShow)
        Me.NotifyMenu.MenuItems.Add(Me.NotifyHide)
        Me.NotifyMenu.Text = "Notification"
        '
        'NotifyShow
        '
        Me.NotifyShow.Text = "Show"
        '
        'NotifyHide
        '
        Me.NotifyHide.Text = "Hide"
        '
        'NewNotificationAdd
        '
        Me.NewNotificationAdd.Text = "AddNew"
        '
        'Label1
        '
        Me.Label1.Location = New System.Drawing.Point(10, 6)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(229, 32)
        Me.Label1.Text = "Select Notification options below then use the visible menu"
        '
        'Label2
        '
        Me.Label2.Font = New System.Drawing.Font("Tahoma", 9.0!, System.Drawing.FontStyle.Bold)
        Me.Label2.Location = New System.Drawing.Point(9, 190)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(228, 23)
        Me.Label2.Text = "Output from Notification"
        '
        'Label3
        '
        Me.Label3.Font = New System.Drawing.Font("Tahoma", 9.0!, System.Drawing.FontStyle.Bold)
        Me.Label3.Location = New System.Drawing.Point(11, 44)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(211, 16)
        Me.Label3.Text = "Options"
        '
        'Label4
        '
        Me.Label4.Location = New System.Drawing.Point(24, 210)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(66, 20)
        Me.Label4.Text = "Response:"
        '
        'Label5
        '
        Me.Label5.Location = New System.Drawing.Point(40, 236)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(50, 20)
        Me.Label5.Text = "Visible:"
        '
        'NotifyCaption
        '
        Me.NotifyCaption.Location = New System.Drawing.Point(96, 61)
        Me.NotifyCaption.Name = "NotifyCaption"
        Me.NotifyCaption.Size = New System.Drawing.Size(126, 21)
        Me.NotifyCaption.TabIndex = 8
        Me.NotifyCaption.Text = "Sample Caption"
        '
        'Label6
        '
        Me.Label6.Location = New System.Drawing.Point(41, 62)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(54, 20)
        Me.Label6.Text = "Caption:"
        '
        'Label7
        '
        Me.Label7.Location = New System.Drawing.Point(56, 89)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(39, 20)
        Me.Label7.Text = "Text:"
        '
        'NotifyText
        '
        Me.NotifyText.Location = New System.Drawing.Point(96, 88)
        Me.NotifyText.Name = "NotifyText"
        Me.NotifyText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.NotifyText.Size = New System.Drawing.Size(126, 21)
        Me.NotifyText.TabIndex = 11
        '
        'NotifyCritical
        '
        Me.NotifyCritical.Location = New System.Drawing.Point(157, 143)
        Me.NotifyCritical.Name = "NotifyCritical"
        Me.NotifyCritical.Size = New System.Drawing.Size(65, 22)
        Me.NotifyCritical.TabIndex = 13
        Me.NotifyCritical.Text = "Critical"
        '
        'Label8
        '
        Me.Label8.Location = New System.Drawing.Point(56, 115)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(39, 22)
        Me.Label8.Text = "Icon:"
        '
        'Label9
        '
        Me.Label9.Location = New System.Drawing.Point(34, 143)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(61, 22)
        Me.Label9.Text = "Duration:"
        '
        'NotifyReponce
        '
        Me.NotifyReponce.Location = New System.Drawing.Point(96, 210)
        Me.NotifyReponce.Name = "NotifyReponce"
        Me.NotifyReponce.Size = New System.Drawing.Size(123, 20)
        '
        'NotifyVisable
        '
        Me.NotifyVisable.Location = New System.Drawing.Point(99, 236)
        Me.NotifyVisable.Name = "NotifyVisable"
        Me.NotifyVisable.Size = New System.Drawing.Size(123, 20)
        '
        'NotifyIcon
        '
        Me.NotifyIcon.Items.Add("Icon1")
        Me.NotifyIcon.Items.Add("Icon2")
        Me.NotifyIcon.Location = New System.Drawing.Point(96, 115)
        Me.NotifyIcon.Name = "NotifyIcon"
        Me.NotifyIcon.Size = New System.Drawing.Size(125, 22)
        Me.NotifyIcon.TabIndex = 26
        '
        'NotifyDuration
        '
        Me.NotifyDuration.Location = New System.Drawing.Point(96, 140)
        Me.NotifyDuration.Maximum = New Decimal(New Integer() {20, 0, 0, 0})
        Me.NotifyDuration.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.NotifyDuration.Name = "NotifyDuration"
        Me.NotifyDuration.Size = New System.Drawing.Size(55, 22)
        Me.NotifyDuration.TabIndex = 38
        Me.NotifyDuration.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'NotificationSample
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 268)
        Me.Controls.Add(Me.NotifyDuration)
        Me.Controls.Add(Me.NotifyIcon)
        Me.Controls.Add(Me.NotifyVisable)
        Me.Controls.Add(Me.NotifyReponce)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.NotifyCritical)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.NotifyText)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.NotifyCaption)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Menu = Me.m_Menu
        Me.Name = "NotificationSample"
        Me.Text = "VB Notification Sample"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents NotifyShow As System.Windows.Forms.MenuItem
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents NotifyHide As System.Windows.Forms.MenuItem
    Friend WithEvents NotifyCaption As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents NotifyText As System.Windows.Forms.TextBox
    Friend WithEvents NotifyCritical As System.Windows.Forms.CheckBox
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents NotifyReponce As System.Windows.Forms.Label
    Friend WithEvents NotifyVisable As System.Windows.Forms.Label
    Friend WithEvents NewNotificationAdd As System.Windows.Forms.MenuItem
    Friend WithEvents NotifyIcon As System.Windows.Forms.ComboBox
    Friend WithEvents NotifyMenu As System.Windows.Forms.MenuItem
    Friend WithEvents NotifyDuration As System.Windows.Forms.NumericUpDown

End Class
