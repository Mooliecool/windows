'****************************** Module Header ******************************\
' Module Name:  Default.aspx.vb
' Project:      VBASPNETImageMap
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to use ImageMap to create an introduction of 
' the planets in Solar System via VB.NET language. When the planet in the 
' image is clicked, the brief information of this planet will be displayed
' under the image and the iframe will navigate to the corresponding page in
' WikiPedia.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,  
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/


Partial Public Class _Default
    Inherits System.Web.UI.Page

    Protected arrPlanets() As String = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"}

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub

    Protected Sub imgMapSolarSystem_Click(ByVal sender As Object, ByVal e As System.Web.UI.WebControls.ImageMapEventArgs) Handles imgMapSolarSystem.Click

        ifSelectResult.Attributes.Item("src") = "http://en.wikipedia.org/wiki/" & e.PostBackValue

        Select Case e.PostBackValue
            Case "Sun"
                'when user clicks on the Sun area
                lbDirection.Text = "The Sun is the star at the centre of the Solar System."

            Case "Mercury"
                lbDirection.Text = "Mercury is the innermost and smallest planet in the Solar System."

            Case "Venus"
                lbDirection.Text = "Venus is the second-closest planet to the Sun."

            Case "Earth"
                lbDirection.Text = "Earth is the third planet from the Sun. It is also referred to as the World and the Blue Planet."

            Case "Mars"
                lbDirection.Text = "Mars is the fourth planet from the Sun in the Solar System."

            Case "Jupiter"
                lbDirection.Text = "Jupiter is the fifth planet from the Sun and the largest planet within the Solar System."

            Case "Saturn"
                lbDirection.Text = "Saturn is the sixth planet from the Sun and the second largest planet in the Solar System, after Jupiter."

            Case "Uranus"
                lbDirection.Text = "Uranus is the seventh planet from the Sun, and the third-largest and fourth most massive planet in the Solar System."

            Case "Neptune"
                lbDirection.Text = "Neptune is the eighth planet from the Sun in our Solar System."

            Case Else

        End Select

    End Sub
End Class