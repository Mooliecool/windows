Public Partial Class ContentPage
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub

    Protected Sub Button1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles Button1.Click
        Dim lbMasterPageHello As Label = TryCast(Master.FindControl("lbHello"), Label)

        If lbMasterPageHello IsNot Nothing Then
            lbMasterPageHello.Text = "Hello, " & txtName.Text & "!"
        End If
    End Sub
End Class