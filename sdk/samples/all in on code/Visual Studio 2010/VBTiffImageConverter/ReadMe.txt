========================================================================================
       WINDOWS APPLICATION : VBTiffImageConverter Project Overview
========================================================================================

////////////////////////////////////////////////////////////////////////////////////////
Summary: 

This sample demonstrates how to convert JPEG images into TIFF images and vice versa. This
sample also allows to create single multipage TIFF images from selected JPEG images.

TIFF (originally standing for Tagged Image File Format) is a flexible, adaptable file 
format for handling images and data within a single file, by including the header tags
(size, definition, image-data arrangement, applied image compression) defining the 
image's geometry. For example, a TIFF file can be a container holding compressed (lossy)
JPEG and (lossless) PackBits compressed images. A TIFF file also can include a 
vector-based clipping path (outlines, croppings, image frames). 


////////////////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the TIFF image converter sample.

Step 1: Build and run the sample solution in Visual Studio 2010

Step 2: Check the checkbox "check to create multipage tiff (single) file" if 
        multipage tiff file is to be created.

Step 3: Convert the images format from Jpeg to Tiff.
        
		Step 3_1. Click on the "Convert to Tiff" button to browse the jpeg images.
		          (Multiselect supported.)
		
		Step 3_2. Click "Ok" after selecting the Jpeg images, which will initiate the 
		          conversion process.

Step 4: Convert the images format from Tiff to Jpeg.

        Step 4_1. Click on the "Convert to Jpeg" button to browse the tiff image.

		Step 4_2. Click "Ok" after selecting the tiff image, which will initiate the
		          conversion process.


/////////////////////////////////////////////////////////////////////////////////////////
Implementation:

A. Converting TIFF to JPEG
(See: TiffImageConverter.ConvertTiffToJpeg)

The basic code logic is as follows:

  1. load the TIFF image with Image
  2. get the number of frames in the TIFF image.
  3. select each frame, and save it as a new jpg image file.

    Public Shared Function ConvertTiffToJpeg(fileName As String) As String()
        Using imageFile As Image = Image.FromFile(fileName)
            Dim frameDimensions As New FrameDimension(imageFile.FrameDimensionsList(0))

            ' Gets the number of pages from the tiff image (if multi-page)
            Dim frameNum As Integer = imageFile.GetFrameCount(frameDimensions)
            Dim jpegPaths As String() = New String(frameNum) {}

            Dim frame As Integer = 0
            While frame < frameNum
                ' Selects one frame at a time and save as jpeg.
                imageFile.SelectActiveFrame(frameDimensions, frame)
                Using bmp As New Bitmap(imageFile)
                    jpegPaths(frame) = [String].Format("{0}\{1}{2}.jpeg",
                                       Path.GetDirectoryName(fileName),
                                       Path.GetFileNameWithoutExtension(fileName), frame)
                    bmp.Save(jpegPaths(frame), ImageFormat.Jpeg)
                End Using
                System.Math.Max(System.Threading.Interlocked.Increment(frame), frame - 1)
            End While

            Return jpegPaths
        End Using
    End Function

B. Converting JPEG(s) to TIFF
(See: TiffImageConverter.ConvertJpegToTiff)

The basic code logic is as follows:

  1. if user checked "check to create multipage tiff (single) file".

    1) initialize the first frame of the multipage tiff using the first selected 
	   jpeg file.
    2) add additional frames from the rest jpeg files.
    3) when it is the last frame, flush the resources and close it.

  2. if user did not check "check to create multipage tiff (single) file".

    1) load each jpeg file
    2) save it as a single-frame tiff file.

    Public Shared Function ConvertJpegToTiff(fileNames As String(),
                                             isMultipage As Boolean) As String()
        Using encoderParams As New EncoderParameters(1)
            Dim tiffCodecInfo As ImageCodecInfo = ImageCodecInfo.GetImageEncoders(). _
                First(Function(ie) ie.MimeType = "image/tiff")

            Dim tiffPaths As String() = Nothing
            If isMultipage Then
                tiffPaths = New String(1) {}
                Dim tiffImg As Image = Nothing
                Try
                    Dim i As Integer = 0
                    While i < fileNames.Length
                        If i = 0 Then
                            tiffPaths(i) = [String].Format("{0}\{1}.tiff",
                                        Path.GetDirectoryName(fileNames(i)),
                                        Path.GetFileNameWithoutExtension(fileNames(i)))

                            ' Initialize the first frame of multi-page tiff.
                            tiffImg = Image.FromFile(fileNames(i))
                            encoderParams.Param(0) = New EncoderParameter(
                                Encoder.SaveFlag, DirectCast(EncoderValue.MultiFrame, 
                                EncoderParameterValueType))
                            tiffImg.Save(tiffPaths(i), tiffCodecInfo, encoderParams)
                        Else
                            ' Add additional frames.
                            encoderParams.Param(0) = New EncoderParameter(
                                Encoder.SaveFlag,
                                DirectCast(EncoderValue.FrameDimensionPage, 
                                EncoderParameterValueType))
                            Using frame As Image = Image.FromFile(fileNames(i))
                                tiffImg.SaveAdd(frame, encoderParams)
                            End Using
                        End If

                        If i = fileNames.Length - 1 Then
                            ' When it is the last frame, flush the resources and closing.
                            encoderParams.Param(0) = New EncoderParameter(
                               Encoder.SaveFlag,
                               DirectCast(EncoderValue.Flush, EncoderParameterValueType))
                            tiffImg.SaveAdd(encoderParams)
                        End If
                        System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                    End While
                Finally
                    If tiffImg IsNot Nothing Then
                        tiffImg.Dispose()
                        tiffImg = Nothing
                    End If
                End Try
            Else
                tiffPaths = New String(fileNames.Length) {}

                Dim i As Integer = 0
                While i < fileNames.Length
                    tiffPaths(i) = [String].Format("{0}\{1}.tiff",
                                       Path.GetDirectoryName(fileNames(i)),
                                       Path.GetFileNameWithoutExtension(fileNames(i)))

                    ' Save as individual tiff files.
                    Using tiffImg As Image = Image.FromFile(fileNames(i))
                        tiffImg.Save(tiffPaths(i), ImageFormat.Tiff)
                    End Using
                    System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                End While
            End If

            Return tiffPaths
        End Using
    End Function

/////////////////////////////////////////////////////////////////////////////////////////
References:

Tagged Image File Format
http://en.wikipedia.org/wiki/Tagged_Image_File_Format


/////////////////////////////////////////////////////////////////////////////////////////