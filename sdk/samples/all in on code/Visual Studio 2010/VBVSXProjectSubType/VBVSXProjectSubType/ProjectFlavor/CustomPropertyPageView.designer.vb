Imports System.Windows.Forms
Imports System.Drawing

Namespace ProjectFlavor
    Partial Friend Class CustomPropertyPageView
        ''' <summary>
        ''' Required designer variable.
        ''' </summary>
        Private components As System.ComponentModel.IContainer = Nothing

        ''' <summary>
        ''' Clean up any resources being used.
        ''' </summary>
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing AndAlso (components IsNot Nothing) Then
                components.Dispose()
            End If
            MyBase.Dispose(disposing)
        End Sub

#Region "Windows Form Designer generated code"

        ''' <summary>
        ''' Required method for Designer support - do not modify
        ''' the contents of this method with the code editor.
        ''' </summary>
        Private Sub InitializeComponent()
            Me.lbStringProperty = New Label()
            Me.tbStringProperty = New TextBox()
            Me.chkBooleanProperty = New CheckBox()
            Me.SuspendLayout()
            ' 
            ' lbStringProperty
            ' 
            Me.lbStringProperty.AutoSize = True
            Me.lbStringProperty.Location = New Point(21, 25)
            Me.lbStringProperty.Name = "lbStringProperty"
            Me.lbStringProperty.Size = New Size(76, 13)
            Me.lbStringProperty.TabIndex = 0
            Me.lbStringProperty.Text = "String Property"
            ' 
            ' tbStringProperty
            ' 
            Me.tbStringProperty.Anchor = (CType(((AnchorStyles.Top Or AnchorStyles.Left) Or AnchorStyles.Right), AnchorStyles))
            Me.tbStringProperty.Location = New Point(115, 23)
            Me.tbStringProperty.Name = "tbStringProperty"
            Me.tbStringProperty.Size = New Size(326, 20)
            Me.tbStringProperty.TabIndex = 1
            ' 
            ' chkBooleanProperty
            ' 
            Me.chkBooleanProperty.AutoSize = True
            Me.chkBooleanProperty.Checked = True
            Me.chkBooleanProperty.CheckState = CheckState.Checked
            Me.chkBooleanProperty.Location = New Point(21, 63)
            Me.chkBooleanProperty.Name = "chkBooleanProperty"
            Me.chkBooleanProperty.Size = New Size(107, 17)
            Me.chkBooleanProperty.TabIndex = 2
            Me.chkBooleanProperty.Text = "Boolean Property"
            ' 
            ' CustomPropertyPageView
            ' 
            Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
            Me.AutoScaleMode = AutoScaleMode.Font
            Me.Controls.Add(Me.chkBooleanProperty)
            Me.Controls.Add(Me.tbStringProperty)
            Me.Controls.Add(Me.lbStringProperty)
            Me.Name = "CustomPropertyPageView"
            Me.Size = New Size(445, 232)
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub

#End Region

        Private lbStringProperty As Label
        Private tbStringProperty As TextBox
        Private chkBooleanProperty As CheckBox
    End Class
End Namespace