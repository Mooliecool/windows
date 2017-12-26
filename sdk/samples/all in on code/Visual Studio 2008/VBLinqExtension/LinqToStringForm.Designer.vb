<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class LinqToStringForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
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

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(LinqToStringForm))
        Me.groupBox4 = New System.Windows.Forms.GroupBox
        Me.label2 = New System.Windows.Forms.Label
        Me.txtBoxSearchItems = New System.Windows.Forms.TextBox
        Me.btnSearchSentence = New System.Windows.Forms.Button
        Me.txtBoxDigit = New System.Windows.Forms.TextBox
        Me.label1 = New System.Windows.Forms.Label
        Me.groupBox3 = New System.Windows.Forms.GroupBox
        Me.txtBoxOccurence = New System.Windows.Forms.TextBox
        Me.btnSearchOccurence = New System.Windows.Forms.Button
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.txtSearchSource = New System.Windows.Forms.TextBox
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.btnDigits = New System.Windows.Forms.Button
        Me.groupBox4.SuspendLayout()
        Me.groupBox3.SuspendLayout()
        Me.groupBox2.SuspendLayout()
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox4
        '
        Me.groupBox4.Controls.Add(Me.label2)
        Me.groupBox4.Controls.Add(Me.txtBoxSearchItems)
        Me.groupBox4.Controls.Add(Me.btnSearchSentence)
        Me.groupBox4.Location = New System.Drawing.Point(20, 266)
        Me.groupBox4.Name = "groupBox4"
        Me.groupBox4.Size = New System.Drawing.Size(534, 59)
        Me.groupBox4.TabIndex = 8
        Me.groupBox4.TabStop = False
        Me.groupBox4.Text = "Query for Sentences that Contain a Specified Set of Words"
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(21, 25)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(83, 12)
        Me.label2.TabIndex = 6
        Me.label2.Text = "Search Items:"
        '
        'txtBoxSearchItems
        '
        Me.txtBoxSearchItems.Location = New System.Drawing.Point(116, 20)
        Me.txtBoxSearchItems.Name = "txtBoxSearchItems"
        Me.txtBoxSearchItems.Size = New System.Drawing.Size(202, 21)
        Me.txtBoxSearchItems.TabIndex = 5
        Me.txtBoxSearchItems.Text = "framework,skeleton,Microsoft"
        '
        'btnSearchSentence
        '
        Me.btnSearchSentence.Location = New System.Drawing.Point(447, 19)
        Me.btnSearchSentence.Name = "btnSearchSentence"
        Me.btnSearchSentence.Size = New System.Drawing.Size(75, 23)
        Me.btnSearchSentence.TabIndex = 4
        Me.btnSearchSentence.Text = "Search"
        Me.btnSearchSentence.UseVisualStyleBackColor = True
        '
        'txtBoxDigit
        '
        Me.txtBoxDigit.Location = New System.Drawing.Point(20, 22)
        Me.txtBoxDigit.Name = "txtBoxDigit"
        Me.txtBoxDigit.Size = New System.Drawing.Size(214, 21)
        Me.txtBoxDigit.TabIndex = 0
        Me.txtBoxDigit.Text = "A1B2C3D4E5F6G7H8I9J0"
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(20, 26)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(77, 12)
        Me.label1.TabIndex = 6
        Me.label1.Text = "Search Item:"
        '
        'groupBox3
        '
        Me.groupBox3.Controls.Add(Me.label1)
        Me.groupBox3.Controls.Add(Me.txtBoxOccurence)
        Me.groupBox3.Controls.Add(Me.btnSearchOccurence)
        Me.groupBox3.Location = New System.Drawing.Point(20, 194)
        Me.groupBox3.Name = "groupBox3"
        Me.groupBox3.Size = New System.Drawing.Size(534, 56)
        Me.groupBox3.TabIndex = 7
        Me.groupBox3.TabStop = False
        Me.groupBox3.Text = "Count Occurences of a Word in the String "
        '
        'txtBoxOccurence
        '
        Me.txtBoxOccurence.Location = New System.Drawing.Point(116, 23)
        Me.txtBoxOccurence.Name = "txtBoxOccurence"
        Me.txtBoxOccurence.Size = New System.Drawing.Size(202, 21)
        Me.txtBoxOccurence.TabIndex = 5
        Me.txtBoxOccurence.Text = "you"
        '
        'btnSearchOccurence
        '
        Me.btnSearchOccurence.Location = New System.Drawing.Point(447, 21)
        Me.btnSearchOccurence.Name = "btnSearchOccurence"
        Me.btnSearchOccurence.Size = New System.Drawing.Size(75, 23)
        Me.btnSearchOccurence.TabIndex = 3
        Me.btnSearchOccurence.Text = "Search"
        Me.btnSearchOccurence.UseVisualStyleBackColor = True
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.groupBox4)
        Me.groupBox2.Controls.Add(Me.groupBox3)
        Me.groupBox2.Controls.Add(Me.txtSearchSource)
        Me.groupBox2.Location = New System.Drawing.Point(22, 108)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(580, 361)
        Me.groupBox2.TabIndex = 8
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Search Word"
        '
        'txtSearchSource
        '
        Me.txtSearchSource.Location = New System.Drawing.Point(20, 23)
        Me.txtSearchSource.Multiline = True
        Me.txtSearchSource.Name = "txtSearchSource"
        Me.txtSearchSource.Size = New System.Drawing.Size(534, 149)
        Me.txtSearchSource.TabIndex = 2
        Me.txtSearchSource.Text = resources.GetString("txtSearchSource.Text")
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.txtBoxDigit)
        Me.groupBox1.Controls.Add(Me.btnDigits)
        Me.groupBox1.Location = New System.Drawing.Point(22, 22)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(580, 64)
        Me.groupBox1.TabIndex = 7
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Show Digits in the String"
        '
        'btnDigits
        '
        Me.btnDigits.Location = New System.Drawing.Point(467, 22)
        Me.btnDigits.Name = "btnDigits"
        Me.btnDigits.Size = New System.Drawing.Size(87, 23)
        Me.btnDigits.TabIndex = 1
        Me.btnDigits.Text = "Show Digits"
        Me.btnDigits.UseVisualStyleBackColor = True
        '
        'LinqToStringForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(630, 490)
        Me.Controls.Add(Me.groupBox2)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "LinqToStringForm"
        Me.Text = "LINQ To String"
        Me.groupBox4.ResumeLayout(False)
        Me.groupBox4.PerformLayout()
        Me.groupBox3.ResumeLayout(False)
        Me.groupBox3.PerformLayout()
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox2.PerformLayout()
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents groupBox4 As System.Windows.Forms.GroupBox
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents txtBoxSearchItems As System.Windows.Forms.TextBox
    Private WithEvents btnSearchSentence As System.Windows.Forms.Button
    Private WithEvents txtBoxDigit As System.Windows.Forms.TextBox
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents groupBox3 As System.Windows.Forms.GroupBox
    Private WithEvents txtBoxOccurence As System.Windows.Forms.TextBox
    Private WithEvents btnSearchOccurence As System.Windows.Forms.Button
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Private WithEvents txtSearchSource As System.Windows.Forms.TextBox
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents btnDigits As System.Windows.Forms.Button
End Class
