'*************************** Module Header ******************************'
' Module Name:  WordDocumentImageManipulator.vb
' Project:      VBManipulateImagesInWordDocument
' Copyright (c) Microsoft Corporation.
' 
' The class WordDocumentImageManipulator is used to export, delete or replace
' the images in a word document. 
' 
' The image data in a document are stored as a ImagePart, and the Blip element
' in a Drawing element will refers to the ImagePart. Different Blip elements may 
' refer to the same ImagePart.
' 
' To delete/replace the images in a document, we need to edit the Blip/Drawing 
' element.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.IO
Imports System.Linq
Imports DocumentFormat.OpenXml
Imports DocumentFormat.OpenXml.Drawing
Imports DocumentFormat.OpenXml.Packaging
Imports DocumentFormat.OpenXml.Wordprocessing

Public Class WordDocumentImageManipulator
    Implements IDisposable
    Private disposed As Boolean = False

    ' Occured when an image is deleted or replaced.
    Public Event ImagesChanged As EventHandler

    ' The WordprocessingDocument instance.
    Private privateDocument As WordprocessingDocument
    Public Property Document() As WordprocessingDocument
        Get
            Return privateDocument
        End Get
        Private Set(ByVal value As WordprocessingDocument)
            privateDocument = value
        End Set
    End Property

    ''' <summary>
    ''' Initialize the WordDocumentImageManipulator instance.
    ''' </summary>
    ''' <param name="path">
    ''' The document file path.
    ''' </param>
    Public Sub New(ByVal path As FileInfo)

        ' Open the document as editable.
        Document = WordprocessingDocument.Open(path.FullName, True)

    End Sub

    ''' <summary>
    ''' Get all images in the documents.
    ''' The image in a document is stored as a Blip element.
    ''' </summary>
    ''' <returns></returns>
    Public Function GetAllImages() As IEnumerable(Of Blip)

        ' Get the drawing elements in the document.
        Dim drawingElements = From run In Document.MainDocumentPart.Document.
                                  Descendants(Of DocumentFormat.OpenXml.Wordprocessing.Run)()
                              Where run.Descendants(Of Drawing)().Count() <> 0
                              Select run.Descendants(Of Drawing)().First()

        ' Get the blip elements in the drawing elements.
        Dim blipElements = From drawing In drawingElements
                           Where drawing.Descendants(Of Blip)().Count() > 0
                           Select drawing.Descendants(Of Blip)().First()

        Return blipElements
    End Function

    ''' <summary>
    ''' Get the image from the Blip element.
    ''' </summary>
    Public Function GetImageInBlip(ByVal blipElement As Blip) As Image

        ' Get the ImagePart referred by the Blip element.
        Dim imagePart = TryCast(
            Document.MainDocumentPart.GetPartById(blipElement.Embed.Value), 
            ImagePart)

        If imagePart IsNot Nothing Then
            Using imageStream As Stream = imagePart.GetStream()
                Dim img As New Bitmap(imageStream)
                Return img
            End Using
        Else
            Throw New ApplicationException("Can not find image part : " _
                                           & blipElement.Embed.Value)
        End If
    End Function

    ''' <summary>
    ''' Delete the Drawing element that contains the Blip element.
    ''' </summary>
    ''' <param name="blipElement"></param>
    Public Sub DeleteImage(ByVal blipElement As Blip)
        Dim parent As OpenXmlElement = blipElement.Parent
        Do While parent IsNot Nothing _
            AndAlso Not (TypeOf parent Is DocumentFormat.OpenXml.Wordprocessing.Drawing)
            parent = parent.Parent
        Loop

        If parent IsNot Nothing Then
            Dim drawing_Renamed As Drawing = TryCast(parent, Drawing)
            drawing_Renamed.Parent.RemoveChild(Of Drawing)(drawing_Renamed)

            ' Raise the ImagesChanged event.
            Me.OnImagesChanged()

        End If
    End Sub

    ''' <summary>
    ''' To replace an image in a document
    ''' 1. Add an ImagePart to the document.
    ''' 2. Edit the Blip element to refer to the new ImagePart.
    ''' </summary>
    ''' <param name="blipElement"></param>
    ''' <param name="newImg"></param>
    Public Sub ReplaceImage(ByVal blipElement As Blip, ByVal newImg As FileInfo)
        Dim rid As String = AddImagePart(newImg)
        blipElement.Embed.Value = rid
        Me.OnImagesChanged()
    End Sub

    ''' <summary>
    ''' Add ImagePart to the document.
    ''' </summary>
    Private Function AddImagePart(ByVal newImg As FileInfo) As String
        Dim type As ImagePartType = ImagePartType.Bmp
        Select Case newImg.Extension.ToLower()
            Case ".jpeg", ".jpg"
                type = ImagePartType.Jpeg
            Case ".png"
                type = ImagePartType.Png
            Case Else
                type = ImagePartType.Bmp
        End Select

        Dim newImgPart As ImagePart = Document.MainDocumentPart.AddImagePart(type)
        Using stream As FileStream = newImg.OpenRead()
            newImgPart.FeedData(stream)
        End Using

        Dim rId As String = Document.MainDocumentPart.GetIdOfPart(newImgPart)
        Return rId
    End Function


    ''' <summary>
    '''  Raise the ImagesChanged event.
    ''' </summary>
    Protected Overridable Sub OnImagesChanged()
        RaiseEvent ImagesChanged(Me, EventArgs.Empty)
    End Sub

    Public Sub Dispose() Implements IDisposable.Dispose
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub

    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        ' Protect from being called multiple times.
        If disposed Then
            Return
        End If

        If disposing Then
            If Document IsNot Nothing Then
                Document.Dispose()
            End If
            disposed = True
        End If
    End Sub
End Class
