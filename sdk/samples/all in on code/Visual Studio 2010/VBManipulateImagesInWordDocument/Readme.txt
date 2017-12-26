================================================================================
				Windows APPLICATION: VBManipulateImagesInWordDocument                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:
The sample demonstrates how to export, delete or replace the images in a word document
using Open XML SDK. 

/////////////////////////////////////////////////////////////////////////////
Prerequisite

Open XML SDK 2.0

You can download it in the following link:
http://www.microsoft.com/downloads/en/details.aspx?FamilyId=C6E744E5-36E9-45F5-8D8C-331DF206E0D0&displaylang=en


////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open this project in  Visual Studio 2010. 
        
Step2. Build the solution. 

Step3. Run VBManipulateImagesInWordDocument.exe.

Step4. Click "Open the word doc" button, and in the OpenFileDialog , select a Word 2007/2010
       document(*.docx file). The listbox will show all images reference ID.

Step5. Click an item in the listbox, you will see the image in the right panel.

Step6. Click the "Export" button, it will show a SaveFileDialog and you can save it 
       to a local file.

Step7. Click the "Delete" button, it will show an alert. If you confirm it, this application
       will delete the image. Close this application, and open the document in Word, you 
       will find that the image has been deleted.

Step7. Run Step3, Step4 and Step5 again. Click the "Replace" button, it will show an
       OpenFileDialog. Choose a local image and confirm the alert, this application
       will replace the image. Close this application, and open the document in Word, you 
       will find that the image has been replaced.

/////////////////////////////////////////////////////////////////////////////
Code Logic:

The image data in a document are stored as a ImagePart, and the Blip element
in a Drawing element will refers to the ImagePart, like following structure

<w:drawing>
  <wp:inline>  
    <a:graphic>
      <a:graphicData>
        <pic:pic>
          <pic:blipFill>
            <a:blip r:embed="rId7">
              <a:extLst>
                <a:ext uri="{28A0092B-C50C-407E-A947-70E740481C1C}">
                  <a14:useLocalDpi val="0" />
                </a:ext>
              </a:extLst>
            </a:blip>
          </pic:blipFill>
        </pic:pic>
      </a:graphicData>
    </a:graphic>
  </wp:inline>
</w:drawing>

A. To list all images in the document, we can get all Drawing elements first, and then get the Blip
   element in the Drawing element.

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

B. To delete the image, we can delete the Drawing element that contains the Blip element.
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


C. To replace an image in a document, we have to add an ImagePart to the document first,
   and then edit the Blip element to refer to the new ImagePart.

        Public Sub ReplaceImage(ByVal blipElement As Blip, ByVal newImg As FileInfo)
			Dim rid As String = AddImagePart(newImg)
			blipElement.Embed.Value = rid
			Me.OnImagesChanged()
        End Sub

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

D. Because we have set the image as the Image property of the PictureBox, we can just use
   the Image.Save method to export the image to local file.

    picView.Image.Save(dialog.FileName, ImageFormat.Jpeg)
/////////////////////////////////////////////////////////////////////////////
References:

Welcome to the Open XML SDK 2.0 for Microsoft Office
http://msdn.microsoft.com/en-us/library/bb448854.aspx

WordprocessingDocument Class
http://msdn.microsoft.com/en-us/library/documentformat.openxml.packaging.wordprocessingdocument.aspx

ImagePart Class
http://msdn.microsoft.com/en-us/library/documentformat.openxml.packaging.imagepart.aspx
/////////////////////////////////////////////////////////////////////////////