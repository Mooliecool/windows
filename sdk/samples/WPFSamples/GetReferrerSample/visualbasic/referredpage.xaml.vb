Public Class ReferredPage
    Inherits Page

    Public Sub New()
        Me.InitializeComponent()
        Me.referringPageTextBlock.Text = App.Referrer.OriginalString
    End Sub

End Class