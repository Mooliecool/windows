'****************************** Module Header ******************************'
' Module Name:  Solution2.vb
' Project:      VBAutomateVisio
' Copyright (c) Microsoft Corporation.
' 
' 
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

Imports System.IO
Imports System.Reflection

Imports Visio = Microsoft.Office.Interop.Visio

#End Region


Class Solution2

    Public Shared Sub AutomateVisio()

        AutomateVisioImpl()


        ' Clean up the unmanaged Visio COM resources by forcing a garbage 
        ' collection as soon as the calling function is off the stack (at 
        ' which point these objects are no longer rooted).

        GC.Collect()
        GC.WaitForPendingFinalizers()
        ' GC needs to be called twice in order to get the Finalizers called 
        ' - the first time in, it simply makes a list of what is to be 
        ' finalized, the second time in, it actually the finalizing. Only 
        ' then will the object do its automatic ReleaseComObject.
        GC.Collect()
        GC.WaitForPendingFinalizers()

    End Sub


    Private Shared Sub AutomateVisioImpl()

        Try
            ' Create an instance of Microsoft Excel and make it invisible.

            Dim oVisio As New Visio.Application
            oVisio.Visible = False
            Console.WriteLine("Visio.Application is started")

            ' Create a new Document based on no template.

            Dim oDoc As Visio.Document = oVisio.Documents.Add("")
            Console.WriteLine("A new document is created")

            ' Draw a rectangle and a oval on the first page.

            Console.WriteLine("Draw a rectangle and a oval")
            oDoc.Pages(1).DrawRectangle(0.5, 10.25, 6.25, 7.375)
            oDoc.Pages(1).DrawOval(1.125, 6, 6.875, 2.125)

            ' Save the document as a vsd file and close it.

            Console.WriteLine("Save and close the document")
            Dim fileName As String = Path.GetDirectoryName( _
            Assembly.GetExecutingAssembly.Location) & "\Sample2.vsd"
            oDoc.SaveAs(fileName)
            oDoc.Close()

            ' Quit the Visio application.

            Console.WriteLine("Quit the Visio application")
            oVisio.Quit()

        Catch ex As Exception
            Console.WriteLine("Solution2.AutomateVisio throws the error: {0}", _
                              ex.Message)
        End Try

    End Sub

End Class