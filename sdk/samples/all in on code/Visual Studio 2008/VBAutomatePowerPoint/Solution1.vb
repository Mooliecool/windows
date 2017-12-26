'******************************** Module Header ********************************'
' Module Name:  Solution1.vb
' Project:      CSAutomatePowerPoint
' Copyright (c) Microsoft Corporation.
' 
' Solution1.AutomatePowerPoint demonstrates automating Microsoft PowerPoint 
' application by using Microsoft PowerPoint Primary Interop Assembly (PIA) and 
' explicitly assigning each COM accessor object to a new varaible that you would 
' explicitly call Marshal.FinalReleaseComObject to release it at the end. When 
' you use this solution, it is important to avoid calls that tunnel into the 
' object model because they will orphan Runtime Callable Wrapper (RCW) on the 
' heap that you will not be able to access in order to call 
' Marshal.ReleaseComObject. You need to be very careful. For example, 
' 
'   Dim oPre As PowerPoint.Presentation = oPowerPoint.Presentations.Add()
'  
' Calling oPowerPoint.Presentations.Add creates an RCW for the Presentations 
' object. If you invoke these accessors via tunneling as this code does, the RCW 
' for Presentations is created on the GC heap, but the reference is created under 
' the hood on the stack and are then discarded. As such, there is no way to call 
' MarshalFinalReleaseComObject on this RCW. To get such kind of RCWs released, 
' you would either need to force a garbage collection as soon as the calling 
' function is off the stack (see Solution2.AutomatePowerPoint), or you would need 
' to explicitly assign each accessor object to a variable and free it.
' 
'   Dim oPres As PowerPoint.Presentations = oPowerPoint.Presentations
'   Dim oPre As PowerPoint.Presentation = oPres.Add()
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*******************************************************************************'

#Region "Imports directives"

Imports System.Reflection
Imports System.IO

Imports Office = Microsoft.Office.Core
Imports PowerPoint = Microsoft.Office.Interop.PowerPoint
Imports System.Runtime.InteropServices

#End Region


Class Solution1

    Public Shared Sub AutomatePowerPoint()

        Dim oPowerPoint As PowerPoint.Application = Nothing
        Dim oPres As PowerPoint.Presentations = Nothing
        Dim oPre As PowerPoint.Presentation = Nothing
        Dim oSlides As PowerPoint.Slides = Nothing
        Dim oSlide As PowerPoint.Slide = Nothing
        Dim oShapes As PowerPoint.Shapes = Nothing
        Dim oShape As PowerPoint.Shape = Nothing
        Dim oTxtFrame As PowerPoint.TextFrame = Nothing
        Dim oTxtRange As PowerPoint.TextRange = Nothing

        Try
            ' Create an instance of Microsoft PowerPoint and make it invisible.

            oPowerPoint = New PowerPoint.Application
            ' By default PowerPoint is invisible, till you make it visible.
            'oPowerPoint.Visible = Office.MsoTriState.msoFalse

            ' Create a new Presentation.

            oPres = oPowerPoint.Presentations
            oPre = oPres.Add()
            Console.WriteLine("A new presentation is created")

            ' Insert a new Slide and add some text to it.

            Console.WriteLine("Insert a slide")
            oSlides = oPre.Slides
            oSlide = oSlides.Add(1, PowerPoint.PpSlideLayout.ppLayoutText)

            Console.WriteLine("Add some texts")
            oShapes = oSlide.Shapes
            oShape = oShapes(1)
            oTxtFrame = oShape.TextFrame
            oTxtRange = oTxtFrame.TextRange
            oTxtRange.Text = "All-In-One Code Framework"

            ' Save the presentation as a pptx file and close it.

            Console.WriteLine("Save and close the presentation")

            Dim fileName As String = Path.GetDirectoryName( _
            Assembly.GetExecutingAssembly().Location) + "\\Sample1.pptx"
            oPre.SaveAs(fileName, PowerPoint.PpSaveAsFileType.ppSaveAsOpenXMLPresentation, _
                        Office.MsoTriState.msoTriStateMixed)
            oPre.Close()

            ' Quit the PowerPoint application.

            Console.WriteLine("Quit the PowerPoint application")
            oPowerPoint.Quit()

        Catch ex As Exception
            Console.WriteLine("Solution1.AutomatePowerPoint throws the error: {0}", _
                              ex.Message)
        Finally

            ' Clean up the unmanaged PowerPoint COM resources by explicitly call 
            ' Marshal.FinalReleaseComObject on all accessor objects. 
            ' See http://support.microsoft.com/kb/317109.

            If Not oTxtRange Is Nothing Then
                Marshal.FinalReleaseComObject(oTxtRange)
                oTxtRange = Nothing
            End If
            If Not oTxtFrame Is Nothing Then
                Marshal.FinalReleaseComObject(oTxtFrame)
                oTxtFrame = Nothing
            End If
            If Not oShape Is Nothing Then
                Marshal.FinalReleaseComObject(oShape)
                oShape = Nothing
            End If
            If Not oShapes Is Nothing Then
                Marshal.FinalReleaseComObject(oShapes)
                oShapes = Nothing
            End If
            If Not oSlide Is Nothing Then
                Marshal.FinalReleaseComObject(oSlide)
                oSlide = Nothing
            End If
            If Not oSlides Is Nothing Then
                Marshal.FinalReleaseComObject(oSlides)
                oSlides = Nothing
            End If
            If Not oPre Is Nothing Then
                Marshal.FinalReleaseComObject(oPre)
                oPre = Nothing
            End If
            If Not oPres Is Nothing Then
                Marshal.FinalReleaseComObject(oPres)
                oPres = Nothing
            End If
            If Not oPowerPoint Is Nothing Then
                Marshal.FinalReleaseComObject(oPowerPoint)
                oPowerPoint = Nothing
            End If

        End Try

    End Sub

End Class