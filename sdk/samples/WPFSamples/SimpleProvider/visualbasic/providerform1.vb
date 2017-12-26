Public Class ProviderForm1
    Inherits Form

    Private Sub btnOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnOk.Click
        Application.Exit()
    End Sub

    Private Sub ProviderForm1_Load(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles MyBase.Load

        ' Create an instance of the custom control.
        Dim controlRect As Rectangle = New Rectangle(30, 15, 50, 40)
        Dim myCustomButton As CustomButton = New CustomButton()

        With myCustomButton
            ' This becomes the Name property for UI Automation.
            .Text = "CustomControl"

            ' Give the control a location and size so that it will trap mouse clicks
            ' and will be repainted as necessary.
            .Location = New System.Drawing.Point(controlRect.X, controlRect.Y)
            .Size = New System.Drawing.Size(controlRect.Width, controlRect.Bottom)
            .TabIndex = 1
        End With

        ' Add it to the form's controls. Among other things, this makes it possible for
        ' UI Automation to discover it, as it will become a child of the application window.
        Me.Controls.Add(myCustomButton)

    End Sub
End Class