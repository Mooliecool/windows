'****************************** Module Header ******************************\
' Module Name:  Page.cs
' Project:      CSSL3DeepZoomProject
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to generate the deep zoom content programmatically in Silverlight using C#. It wraps the functionality in a WCF service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 9/4/2009 17:12 Yilun Luo Created
'***************************************************************************/

Imports Microsoft.DeepZoomTools
Imports System.IO
Imports System.Windows
Imports System.ServiceModel.Activation

' NOTE: If you change the class name "GenerateDeepZoomService" here, you must also update the reference to "GenerateDeepZoomService" in Web.config and in the associated .svc file.
<AspNetCompatibilityRequirements(RequirementsMode:=AspNetCompatibilityRequirementsMode.Allowed)> _
Public Class GenerateDeepZoomService
	Implements IGenerateDeepZoomService

	''' <summary>
	''' Generate the deep zoom content using a CollectionCreator.
	''' </summary>
	Private Sub CreateDeepZoom()
		Dim creator As New CollectionCreator
		Dim images As New List(Of Image)
		Dim doc As XDocument = XDocument.Load(HttpContext.Current.Server.MapPath("~/ClientBin/GeneratedImages/Metadata.xml"))
		Dim imageElements As IEnumerable(Of XElement) = doc.Root.Elements("Image")
		Dim aspectRatio As Double = Double.Parse(doc.Root.Element("AspectRatio").Value)
		Dim imageElement As XElement
		'Populates a list of Microsoft.DeepZoomTools.Image objects using the value provided in Metadata.xml.
		For Each imageElement In imageElements
			Dim zOrder As Integer = Integer.Parse(imageElement.Element("ZOrder").Value)
			Dim width As Double = (1 / Double.Parse(imageElement.Element("Width").Value))
			Dim image = New Image(HttpContext.Current.Server.MapPath(("~/SourceImages/" & imageElement.Element("FileName").Value)))
			image.ViewportWidth = width
			image.ViewportOrigin = New Point((Double.Parse(imageElement.Element("x").Value) * -width), ((Double.Parse(imageElement.Element("y").Value) * -width) / aspectRatio))
			images.Add(image)
		Next
		creator.Create(images, HttpContext.Current.Server.MapPath("~/ClientBin/GeneratedImages/dzc_output"))
	End Sub

	Public Function PrepareDeepZoom(ByVal forceGenerateDeepZoom As Boolean) As Boolean Implements IGenerateDeepZoomService.PrepareDeepZoom
		If (Not Directory.Exists(HttpContext.Current.Server.MapPath("~/ClientBin/GeneratedImages/dzc_output_images")) OrElse forceGenerateDeepZoom) Then
			Try
				Me.CreateDeepZoom()
			Catch obj1 As Exception
				Return False
			End Try
		End If
		Return True
	End Function
End Class
