=============================================================================
              VBASPNETLimitDownloadSpeed Project Overview
=============================================================================

Use:

 This project illustrates how to limit the download speed via coding. 

/////////////////////////////////////////////////////////////////////////////
Note:

Please kindly note that IIS7 has an extension called Bit Rate Throttling can
do this feature for us with very simple option settings. For more info about 
Bit Rate Throttling, please refer to: http://www.iis.net/download/BitRateThrottling

If you are not using IIS7 with Windows Server 2008, we strongly recommend you 
to upgrade your server as soon as possible. If you are already using IIS7, 
please use the Bit Rate Throttling feature instead of what this code sample 
demos to limit the download speed. Thanks.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample.

Step1: Browse the Default.aspx from the sample and select a limited speed
from the DropDownList.

Step2: Click the Download button to download the file.

Step3: You can find the download speed is limited to the selected speed.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a VB.NET ASP.NET Web Application in Visual Studio 2008.

Step2: Add a Default ASP.NET page into the application.

Step3: Add these two namespaces to the code behind of Default.aspx.

	Imports System.IO
    Imports System.Threading
	
NOTE: System.IO is used for FileStream and BinaryReader class as well as
some enum type. System.Threading is used for Thread.Sleep() method. 

Step4: Navigate to the Page_Load event handler and write code to create 
a temporary big file for the download test.

    Protected Sub Page_Load(ByVal sender As Object, 
                            ByVal e As System.EventArgs) 
                            Handles Me.Load
        Dim length As Integer = 1024 * 1024 * 1
        Dim buffer As Byte() = New Byte(length - 1) {}

        Dim filepath As String = Server.MapPath("~/bigFileSample.dat")
        Using fs As New FileStream(filepath, 
                                   FileMode.Create, 
                                   FileAccess.Write)
            fs.Write(buffer, 0, length)
        End Using
    End Sub
    
Step5: Add a DropDownList control and a Button control to the page. 

    <asp:DropDownList ID="ddlDownloadSpeed" runat="server">
		<asp:ListItem Value="20">20 Kb/s</asp:ListItem>
		<asp:ListItem Value="50">50 Kb/s</asp:ListItem>
		<asp:ListItem Value="80">80 Kb/s</asp:ListItem>
    </asp:DropDownList>
    <asp:Button ID="btnDownload" runat="server" Text="Download" />

Step6: Write the DownloadFileWithLimitedSpeed() method. Please refer to the
NOTE for understanding of this method.

    Public Sub DownloadFileWithLimitedSpeed(ByVal fileName As String, 
                                            ByVal filePath As String, 
                                            ByVal downloadSpeed As Long)
        '...
	End Sub
    
NOTE: This method is the key of this sample. The basic logic to achieve the 
download speed limit feature in this method is to force the response thread 
sleep for an appropriate time each time it sends a file packs. 

For a simplest instance, if the size of the file pack is 1 Kb, we can make 
the response thread sleep for 1 second after it sends each file pack, so that
we will get a controlled download speed as 1 Kb/s. To have different download
speed, we just need to make the thread sleep shorter or longer.

So obviously, to decide the sleep time is the most important thing here. 

Well, let's say if the file pack is 1 Kb, and we need a download speed limit 
of 50 Kb/s, how long shoud the sleep time be? The answer is 20 millisecond.
50 Kb/s means that we need to send 50 file packs in a single second. As there 
are 1000 millisecond in a second, we can cut them to 50 parts and each part 
is 20 millisecond. 20 ms * 50 =  1000 ms = 1 second.

So, the formula of the sleep time should be: 
sleep = 1000 / (downloadspeed / pack)

Follow the sample above, downloadspeed = 1024 * 50 and pack = 1024 and we get:
sleep = 1000 / (50 * 1024 / 1024) = 1000 / 50 = 20.

For a better understanding, we may think that the result of (1000 / sleep) is
how many times that the thread should send the file packs in a single second. 
Like if sleep = 20, that means the thread sends 1000 / 20 = 50 file packs each 
second. So, the download speed equals to 50 file packs per second.


MORE: You may find that sometimes the download speed doesn't meet the selected
value accurately, especially when it comes to a higher speed. This is caused 
by the IO consumption. Higher speed stands for shorter sleep time, which also 
means the code below is fired more times than a lower speed in a second.

    If Response.IsClientConnected Then
        Response.BinaryWrite(br.ReadBytes(pack))
        Thread.Sleep(sleep)
    End If
                        
However, the code itself will spend some time to run, maybe 1 or 2 milliseconds.
It will be not apparent if the sleep time is 100 ms or more. You see, 100 ms is
not far different between 101 ms. But, if the sleep time is 10 ms or less, this
percentage error will take much effect to the download speed.

Step7: Add Button.Click event handler to call the method.

    Protected Sub btnDownload_Click(ByVal sender As Object, 
                                    ByVal e As EventArgs) 
                                    Handles btnDownload.Click
                                    
        Dim outputFileName As String = "bigFileSample.dat"
        Dim filePath As String = Server.MapPath("~/bigFileSample.dat")

        Dim value As String = ddlDownloadSpeed.SelectedValue

        Dim downloadSpeed As Integer = 1024 * Integer.Parse(value)
        Response.Clear()

        Try
            DownloadFileWithLimitedSpeed(outputFileName, 
                                         filePath, 
                                         downloadSpeed)
        Catch ex As Exception
            Response.Write(ex.Message)
        End Try
        Response.End()
    End Sub


/////////////////////////////////////////////////////////////////////////////
References:

MSDN:
# HttpResponse.AddHeader Method
http://msdn.microsoft.com/en-us/library/system.web.httpresponse.addheader.aspx

# HttpResponse.BinaryWrite Method
http://msdn.microsoft.com/en-us/library/system.web.httpresponse.binarywrite.aspx

# Thread.Sleep Method 
http://msdn.microsoft.com/en-us/library/d00bd51t.aspx

/////////////////////////////////////////////////////////////////////////////