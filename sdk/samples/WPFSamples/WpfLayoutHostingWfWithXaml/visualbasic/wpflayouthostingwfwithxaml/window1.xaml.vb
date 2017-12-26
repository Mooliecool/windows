' <snippet100>
Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Media
Imports System.Windows.Shapes

Class Window1
    Inherits Window
    
    Public Sub New()
        InitializeComponent()

        Me.InitializeFlowLayoutPanel()

    End Sub

    Private Sub button1Click(ByVal sender As Object, ByVal e As EventArgs)
        Dim b As System.Windows.Forms.Button = sender

        b.Top = 20
        b.Left = 20

    End Sub

    Private Sub button2Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.host1.Visibility = Windows.Visibility.Hidden
    End Sub


    Private Sub button3Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.host1.Visibility = Windows.Visibility.Collapsed
    End Sub

    Private Sub InitializeFlowLayoutPanel()
        Dim flp As System.Windows.Forms.FlowLayoutPanel = Me.flowLayoutHost.Child
        
        flp.WrapContents = True

        Const numButtons As Integer = 6

        Dim i As Integer
        For i = 0 To numButtons
            Dim b As New System.Windows.Forms.Button()
            b.Text = "Button"
            b.BackColor = System.Drawing.Color.AliceBlue
            b.FlatStyle = System.Windows.Forms.FlatStyle.Flat

            flp.Controls.Add(b)
        Next i

    End Sub

End Class

' </snippet100>