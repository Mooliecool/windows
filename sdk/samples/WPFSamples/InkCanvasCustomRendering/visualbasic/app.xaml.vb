' Interaction logic for App.xaml
Partial Public Class App
    Inherits System.Windows.Application

    '  Rather than render ink in this sample, pieces (strokes from the stylus) of the image are
    ' rendered on the InkCanvas. ImageSource is the place from where the information to do this
    ' is drawn.
    Public Shared myImageSource As ImageSource

    ''' <summary>
    ''' Static constructor to instance our static ImageSource
    ''' </summary>
    Shared Sub New()

        Dim thisExe As System.Reflection.Assembly = System.Reflection.Assembly.GetExecutingAssembly()
        Dim stream As System.IO.Stream = thisExe.GetManifestResourceStream("InkCanvasCustomRenderingSample.Winter Leaves.jpg")

        Dim codec As JpegBitmapDecoder = New JpegBitmapDecoder(stream, BitmapCreateOptions.None, BitmapCacheOption.Default)
        myImageSource = codec.Frames(0)
        myImageSource.Freeze()

    End Sub
End Class
