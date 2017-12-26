=============================================================================
              Windows APPLICATION: VBExtractAudioFile Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to extract and convert audio file formats, which 
include wav, mp3 and mp4 files.

The sample is used to extract music file formats. We usually play music with 
Windows Media Player or other music-playing software. If we find our favorite 
music clip, we can use the function of this sample to extract it and convert 
it into another file format. All technology mentioned above is based on 
Expression Encoder SDK 4.0. When you install Expression Encoder 4.0, you can 
use Visual Studio 2010 to add a reference to it. In this way, you don't have 
to install the SDK installation kits individually.

The sample uses Expression Encoder SDK 4 to output *.mp4 or *.wma file. The 
.mp3 audio format is currently not supported as output format in Expression 
Encoder 4.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Please install a free version, called "Microsoft Expression Encoder 4" at 
the link as follows:
http://www.microsoft.com/downloads/en/details.aspx?displaylang=en&FamilyID=75402be0-c603-4998-a79c-becdd197aa79

The free version is a feature-filled VC-1 encoding application that supports 
the following: 

    - High performance multi-core encoding 
    - Crop/scale/de-interlace operation 
    - Multi-clip editing 
    - A/B compare 
    - Live encoding 
    - Up to 10 minutes of screen capture 
    - Smart encoding 
    - Silverlight templates 
    - Multi-channel audio import and export 
    - Rich metadata support 
    - Presets and custom plug-ins as well as 
    - Full access to the .NET SDK for all above features 
    - VC-1 Smooth Streaming (new for v4) 

The Professional version of Microsoft Expression Encoder 4 adds:
    - H.264 encoding (both MP4 and Smooth Streaming) 
    - Live Smooth Streaming encoding 
    - DRM 
    - Native support of MP4/H.264, TS, M2TS, AVCHD, MPEG-2, ISM, ISMV, AAC 
      and AC-3 files
    - Unlimited screen capture durations 

So if you like these features provided by this software application, please 
purchase the professional version to support more advanced features.


/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Install Microsoft Expression Encoder 4. 

Step2. Build and run the sample project in Visual Studio 2010. 

Step3. Prepare a .wma or .mp4 or .mp3 audio file.  Click the button which is 
       written as "Choose Audio File..." and select the audio file. 

Step4. Click the play button and drag the block on the progress bar to 
       seek the position that you want to start extracting.  Then, you click 
       the "Set Start Point" button.
       Do the same operation to set the end of extract file.  It's important 
       that you don't set the extract end point before the start point.

Step5. Select the audio output file format between WMA and MP4. Set the output 
       directory.

Step6. Click the "Extract" button to extract the audio file.  If it succeeds, 
       you will be informed the output file path.  You can play the output 
       audio file to check if the audio clip is correct. 


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. The sample uses the Windows Media Player control to play the source audio 
   file.  The following MSDN article introduces how to embed the Windows 
   Media Player ActiveX control in a Windows Form.
   http://msdn.microsoft.com/en-us/library/dd562851.aspx
   
   We play a specified audio file in the Windows Media Player control by 
   setting its URL property and calling AxWindowsMediaPlayer.Ctlcontrols.play().

    Me.player.URL = openAudioFileDialog.FileName
    Me.player.Ctlcontrols.play()

   When the form is being closed, we call the AxWindowsMediaPlayer.close() 
   method to release Windows Media Player resources.

    Private Sub MainForm_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs) Handles MyBase.FormClosing
        ' Releases Windows Media Player resources.
        Me.player.close()
    End Sub

   In order to get the current playing position in the media item, and save 
   the start and end points, we use the 
   AxWindowsMediaPlayer.Ctlcontrols.currentPosition property. It contains the 
   current position in the media item in seconds from the beginning:

    If btnSetBeginEndPoints.Tag.Equals("SetStartPoint") Then
     ' Save the startpoint.
     ' player.Ctlcontrols.currentPosition contains the current 
     ' position in the media item in seconds from the beginning.
     Me.tbStartpoint.Text = (player.Ctlcontrols.currentPosition * 1000).ToString("0")
     Me.tbEndpoint.Text = ""

     Me.btnSetBeginEndPoints.Text = "Set End Point"
     Me.btnSetBeginEndPoints.Tag = "SetEndPoint"
    ElseIf btnSetBeginEndPoints.Tag.Equals("SetEndPoint") Then
     ' Check if the startpoint is in front of the endpoint.
       Dim endPoint As Integer = CInt(Fix(player.Ctlcontrols.currentPosition * 1000))
       If endPoint <= Integer.Parse(Me.tbStartpoint.Text) Then
         MessageBox.Show("Audio endpoint is overlapped. Please reset the endpoint.")
         Else
          ' Save the endpoint.
          Me.tbEndpoint.Text = endPoint.ToString()

          Me.btnSetBeginEndPoints.Text = "Set Start Point"
          Me.btnSetBeginEndPoints.Tag = "SetStartPoint"
       End If
   End If
      
2. The sample uses following helper function to cut the audio file from 
   startpoint to endpoint, and output the clip as the selected audio format.  

    Public Shared Function ExtractAudio(ByVal sourceAudioFile As String, ByVal outputDirectory As String, ByVal outputType As OutputAudioType, ByVal startpoint As Double, ByVal endpoint As Double) As String
        Using [job] As New Job()
            Dim src As New MediaItem(sourceAudioFile)
            Select Case outputType
                Case OutputAudioType.MP4
                    src.OutputFormat = New MP4OutputFormat()
                    src.OutputFormat.AudioProfile = New AacAudioProfile()
                    src.OutputFormat.AudioProfile.Codec = AudioCodec.AAC
                    src.OutputFormat.AudioProfile.BitsPerSample = 24
                Case OutputAudioType.WMA
                    src.OutputFormat = New WindowsMediaOutputFormat()
                    src.OutputFormat.AudioProfile = New WmaAudioProfile()
                    src.OutputFormat.AudioProfile.Bitrate = New VariableConstrainedBitrate(128, 192)
                    src.OutputFormat.AudioProfile.Codec = AudioCodec.WmaProfessional
                    src.OutputFormat.AudioProfile.BitsPerSample = 24
            End Select

            Dim spanStart As TimeSpan = TimeSpan.FromMilliseconds(startpoint)
            src.Sources(0).Clips(0).StartTime = spanStart
            Dim spanEnd As TimeSpan = TimeSpan.FromMilliseconds(endpoint)
            src.Sources(0).Clips(0).EndTime = spanEnd

            [job].MediaItems.Add(src)
            [job].OutputDirectory = outputDirectory
            [job].Encode()

            Return [job].MediaItems(0).ActualOutputFileFullPath
        End Using
    End Function
   End Class

   btnExtract_Click in MainForm.cs calls the helper method to extract the audio file.

   Dim outputType As OutputAudioType = CType(Me.cmbOutputAudioType.SelectedValue, OutputAudioType)
   Dim outputFileName As String = ExpressionEncoderWrapper.ExtractAudio(sourceAudioFile, outputDirectory, outputType, Double.Parse(startpoint), Double.Parse(endpoint))

           

//////////////////////////////////////////////////////////////////////////////////////
References:

Expression Encoder SDK Programming Reference 
http://msdn.microsoft.com/en-us/library/ff396833(v=Expression.40).aspx

Microsoft Expression Encoder 4 FAQ 
http://social.expression.microsoft.com/Forums/en/encoder/thread/3eabf903-b49f-4f92-b508-f28a795d6c90


//////////////////////////////////////////////////////////////////////////////////////
