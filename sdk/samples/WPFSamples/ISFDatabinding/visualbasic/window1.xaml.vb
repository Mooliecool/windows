Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Ink
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Shapes
Imports System.Xml
Imports System.IO
Imports System.Globalization



Namespace SignatureCollector

    ' Interaction logic for Window1.xaml
    Partial Public Class Window1
        Inherits System.Windows.Window

        Public Sub New()
            InitializeComponent()
        End Sub
        ''' <summary>
        ''' The Commit button Click handler.
        '''   Takes the strokes from the InkCanvas and adds them to the 
        '''   InkData.xml file.  Databinding to the XMLDataProvider will
        '''   update the listview with a new signature item.
        ''' </summary>
        Sub OnCommitClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If (myInkCanvas.Strokes.Count > 0) Then
                Dim clonedStrokes As StrokeCollection = myInkCanvas.Strokes.Clone()
                Dim strokesRect As Rect = clonedStrokes.GetBounds()
                'Apply a transform that shifts the strokes to the top-left.  The
                'ViewBox in InkDataTemplate will center the strokes
                clonedStrokes.Transform( _
                    New Matrix(1, 0, 0, 1, (0 - strokesRect.X), (0 - strokesRect.Y)), False)

                'Add the ISF from the strokes to the XML document.
                'Databinding in Window1.xaml will update the list view each time you
                'add a signature.
                Dim xdp As XmlDataProvider = Me.FindResource("InkData")
                Dim xdoc As XmlDocument = xdp.Document
                Dim newInkFile As XmlElement = xdoc.CreateElement("InkFile")
                Dim newSig As XmlElement = xdoc.CreateElement("Signature")
                Dim base64Ink As MemoryStream = New MemoryStream()
                clonedStrokes.Save(base64Ink)
                newSig.InnerText = Convert.ToBase64String(base64Ink.ToArray())
                newInkFile.AppendChild(newSig)
                xdoc.DocumentElement.AppendChild(newInkFile)
                'Uncomment this line if you would like to save strokes to the 
                'resource InkData.xml, which the ListViewItems are databound to.
                'xdoc.Save("..\\..\\InkData.xml")

                base64Ink.Close()
                myInkCanvas.Strokes.Clear()
                'Bring the last item added into view
                signatureView.ScrollIntoView(signatureView.Items(signatureView.Items.Count - 1))

            End If

        End Sub

    End Class
End Namespace
''' <summary>
''' A Converter class which is used to determine if an InkCanvas contains Strokes
''' </summary>
''' <remarks></remarks>
Public Class StrokeCountToBoolConverter
    Implements IValueConverter

    ''' <summary>
    ''' Returns a bool indicating whether or not the InkCanvas contains a Stroke.
    ''' </summary>
    ''' <param name="value"></param>
    ''' <param name="targetType"></param>
    ''' <param name="parameter"></param>
    ''' <param name="culture"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        ' If there is no stroke on InkCanvas, return False. Otherwise, return True.
        If value = 0 Then
            Return False
        Else
            Return True
        End If
    End Function

    ''' <summary>
    ''' Do not convert back.
    ''' </summary>
    ''' <param name="value"></param>
    ''' <param name="targetType"></param>
    ''' <param name="parameter"></param>
    ''' <param name="culture"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ConvertBack(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        Return Nothing
    End Function
End Class

