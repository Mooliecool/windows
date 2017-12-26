Public Class Start
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Response.Redirect("en-us/ShowMe.aspx")
    End Sub

End Class