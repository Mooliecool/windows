<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Target
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim ListViewItem1 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("Item1")
        Dim ListViewItem2 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("Item2")
        Dim ListViewItem3 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("Item3")
        Dim ListViewItem4 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("Item4")
        Dim ListViewItem5 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("Item5")
        Dim ListViewItem6 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("Item6")
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.CheckedListBox = New System.Windows.Forms.CheckedListBox
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.ListView = New System.Windows.Forms.ListView
        Me.groupBox3 = New System.Windows.Forms.GroupBox
        Me.ComboBox = New System.Windows.Forms.ComboBox
        Me.groupBox4 = New System.Windows.Forms.GroupBox
        Me.ListBox = New System.Windows.Forms.ListBox
        Me.groupBox2.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.groupBox3.SuspendLayout()
        Me.groupBox4.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.CheckedListBox)
        Me.groupBox2.Location = New System.Drawing.Point(20, 26)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(200, 134)
        Me.groupBox2.TabIndex = 9
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "CheckedListBox"
        '
        'CheckedListBox
        '
        Me.CheckedListBox.CheckOnClick = True
        Me.CheckedListBox.FormattingEnabled = True
        Me.CheckedListBox.Items.AddRange(New Object() {"CheckBoxItem1", "CheckBoxItem2", "CheckBoxItem3", "CheckBoxItem4", "CheckBoxItem5", "CheckBoxItem6"})
        Me.CheckedListBox.Location = New System.Drawing.Point(27, 26)
        Me.CheckedListBox.Name = "CheckedListBox"
        Me.CheckedListBox.Size = New System.Drawing.Size(120, 94)
        Me.CheckedListBox.TabIndex = 3
        Me.CheckedListBox.UseWaitCursor = True
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.ListView)
        Me.groupBox1.Location = New System.Drawing.Point(239, 26)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(200, 134)
        Me.groupBox1.TabIndex = 8
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "ListView"
        '
        'ListView
        '
        Me.ListView.HideSelection = False
        ListViewItem1.Checked = True
        ListViewItem1.StateImageIndex = 1
        ListViewItem1.Tag = ""
        Me.ListView.Items.AddRange(New System.Windows.Forms.ListViewItem() {ListViewItem1, ListViewItem2, ListViewItem3, ListViewItem4, ListViewItem5, ListViewItem6})
        Me.ListView.Location = New System.Drawing.Point(27, 28)
        Me.ListView.Name = "ListView"
        Me.ListView.Size = New System.Drawing.Size(150, 86)
        Me.ListView.TabIndex = 1
        Me.ListView.UseCompatibleStateImageBehavior = False
        '
        'groupBox3
        '
        Me.groupBox3.Controls.Add(Me.ComboBox)
        Me.groupBox3.Location = New System.Drawing.Point(20, 178)
        Me.groupBox3.Name = "groupBox3"
        Me.groupBox3.Size = New System.Drawing.Size(200, 134)
        Me.groupBox3.TabIndex = 10
        Me.groupBox3.TabStop = False
        Me.groupBox3.Text = "ComboBox"
        '
        'ComboBox
        '
        Me.ComboBox.FormattingEnabled = True
        Me.ComboBox.Items.AddRange(New Object() {"Item1", "Item2", "Item3", "Item4", "Item5", "Item6"})
        Me.ComboBox.Location = New System.Drawing.Point(36, 28)
        Me.ComboBox.Name = "ComboBox"
        Me.ComboBox.Size = New System.Drawing.Size(121, 21)
        Me.ComboBox.TabIndex = 2
        '
        'groupBox4
        '
        Me.groupBox4.Controls.Add(Me.ListBox)
        Me.groupBox4.Location = New System.Drawing.Point(239, 178)
        Me.groupBox4.Name = "groupBox4"
        Me.groupBox4.Size = New System.Drawing.Size(200, 134)
        Me.groupBox4.TabIndex = 11
        Me.groupBox4.TabStop = False
        Me.groupBox4.Text = "ListBox"
        '
        'ListBox
        '
        Me.ListBox.FormattingEnabled = True
        Me.ListBox.Items.AddRange(New Object() {"Item1", "Item2", "Item3", "Item4", "Item5", "Item6"})
        Me.ListBox.Location = New System.Drawing.Point(36, 25)
        Me.ListBox.Name = "ListBox"
        Me.ListBox.SelectionMode = System.Windows.Forms.SelectionMode.MultiSimple
        Me.ListBox.Size = New System.Drawing.Size(121, 95)
        Me.ListBox.TabIndex = 0
        '
        'Target
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(459, 338)
        Me.Controls.Add(Me.groupBox2)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.groupBox3)
        Me.Controls.Add(Me.groupBox4)
        Me.Name = "Target"
        Me.Text = "Form1"
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox3.ResumeLayout(False)
        Me.groupBox4.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Private WithEvents CheckedListBox As System.Windows.Forms.CheckedListBox
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents ListView As System.Windows.Forms.ListView
    Private WithEvents groupBox3 As System.Windows.Forms.GroupBox
    Private WithEvents ComboBox As System.Windows.Forms.ComboBox
    Private WithEvents groupBox4 As System.Windows.Forms.GroupBox
    Private WithEvents ListBox As System.Windows.Forms.ListBox

End Class
