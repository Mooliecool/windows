'****************************** Module Header ******************************'
' Module Name:  Solution2.vb
' Project:      VBAutomatePowerPoint
' Copyright (c) Microsoft Corporation.
' 
' Solution2.AutomatePowerPoint demonstrates automating Microsoft PowerPoint 
' application by using Microsoft PowerPoint Primary Interop Assembly (PIA) 
' and forcing a garbage collection as soon as the automation function is off 
' the stack (at which point the Runtime Callable Wrapper (RCW) objects are no 
' longer rooted) to clean up RCWs and release COM objects.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Import directives"

Imports System.Reflection
Imports System.IO

Imports Office = Microsoft.Office.Core
Imports PowerPoint = Microsoft.Office.Interop.PowerPoint

#End Region


Class Solution2

    Public Shared Sub AutomatePowerPoint()

        AutomatePowerPointImpl()


        ' Clean up the unmanaged PowerPoint COM resources by forcing a 
        ' garbage collection as soon as the calling function is off the stack 
        ' (at which point these objects are no longer rooted).

        GC.Collect()
        GC.WaitForPendingFinalizers()
        ' GC needs to be called twice in order to get the Finalizers called 
        ' - the first time in, it simply makes a list of what is to be 
        ' finalized, the second time in, it actually the finalizing. Only 
        ' then will the object do its automatic ReleaseComObject.
        GC.Collect()
        GC.WaitForPendingFinalizers()

    End Sub


    Private Shared Sub AutomatePowerPointImpl()

        Try
            ' Create an instance of Microsoft PowerPoint and make it invisible.

            Dim oPowerPoint As New PowerPoint.Application
            ' By default PowerPoint is invisible, till you make it visible.
            'oPowerPoint.Visible = Office.MsoTriState.msoFalse

            ' Create a new Presentation.

            Dim oPre As PowerPoint.Presentation = oPowerPoint.Presentations.Add()
            Console.WriteLine("A new presentation is created")

            ' Insert a new Slide and add some text to it.

            Console.WriteLine("Insert a slide")
            Dim oSlide As PowerPoint.Slide = oPre.Slides.Add( _
            1, PowerPoint.PpSlideLayout.ppLayoutText)

            Console.WriteLine("Add some texts")
            oSlide.Shapes(1).TextFrame.TextRange.Text = "All-In-One Code Framework"

            ' Save the presentation as a pptx file and close it.

            Console.WriteLine("Save and close the presentation")

            Dim fileName As String = Path.GetDirectoryName( _
                Assembly.GetExecutingAssembly().Location) + "\\Sample2.pptx"
            oPre.SaveAs(fileName, _
                PowerPoint.PpSaveAsFileType.ppSaveAsOpenXMLPresentation, _
                Office.MsoTriState.msoTriStateMixed)
            oPre.Close()

            ' Quit the PowerPoint application.

            Console.WriteLine("Quit the PowerPoint application")
            oPowerPoint.Quit()

        Catch ex As Exception
            Console.WriteLine("Solution2.AutomatePowerPoint throws the error: {0}", _
                              ex.Message)
        End Try

    End Sub

End Class