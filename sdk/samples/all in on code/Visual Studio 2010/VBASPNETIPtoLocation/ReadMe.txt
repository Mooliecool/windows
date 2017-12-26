=============================================================================
                  VBASPNETIPtoLocation Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This project illustrates how to get the geographical location from an IP
address via a db file named "Location.mdf". You need install Sqlserver 
Express for run the web applicaiton. The code-sample only supports Internet
Protocol version 4 (IPv4).

/////////////////////////////////////////////////////////////////////////////
Demo the Sample.

Step1: Browse the Default.aspx from the sample project and you can find your 
IP address displayed on the page. If you are running the sample locally, you 
may get "127.0.0.1" (or "::1" if IPv6 is enabled) as your client and the 
server is the same machine. When you deploy this demo to a host server, you 
will get your real IP address.

[Note]
If you get "::1" of client address, it's the IPv6 version of your IP address. 
If you want to disable or enable IPv6, please refer to this KB article: 
http://support.microsoft.com/kb/929852
[/Note]

Step2: Enter an IPv4 address (e.g. 207.46.131.43) in the TextBox and click 
the Submit button. You will get the basic geographical location information, 
including country code and country name for the specified IP address.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a VB.NET ASP.NET Empty Web Application in Visual Studio 2010.

Step2: Add a Default ASP.NET page into the application.

Step3: Add a Label, a TextBox and a Button control to the page. The Label
is used to show the client IP address. TextBox is for IP address inputting,
and then user can click the Button to get the location info based on that
input.

Step4: Write code to get the client IP address.

    Dim ipAddress As String
    ipAddress = Request.ServerVariables("HTTP_X_FORWARDED_FOR")
    If String.IsNullOrEmpty(ipAddress) Then
        ipAddress = Request.ServerVariables("REMOTE_ADDR")
    End If

Step5: Create a class and name it as "IPConvert", this class use to convert
IP address string to an IP number. The code like this:

	Public Shared Function ConvertToIPRange(ByVal ipAddress As String) As String
        Try
            Dim ipArray As String() = ipAddress.Split("."c)
            Dim number As Integer = ipArray.Length
            Dim ipRange As Double = 0
            If number <> 4 Then
                Return "error ipAddress"
            End If
            For i As Integer = 0 To 3
                Dim numPosition As Integer = Integer.Parse(ipArray(3 - i).ToString())
                If i = 4 Then
                    ipRange += numPosition
                Else
                    ipRange += ((numPosition Mod 256) * (Math.Pow(256, (i))))
                End If
            Next
            Return ipRange.ToString()
        Catch generatedExceptionName As Exception
            Return "error"
        End Try
    End Function

Step6: Write code to get the location info from the Location.mdf file

        ' Get the IP address string and calculate IP number.
        Dim ipRange As String = IPConvert.ConvertToIPRange(ipAddress)
        Dim tabLocation As New DataTable()

        ' Create a connection to Sqlserver
        Using sqlConnection As New SqlConnection(ConfigurationManager.ConnectionStrings("ConectString").ToString())
            Dim selectCommand As String = "select * from IPtoLocation where CAST(" & ipRange & " as bigint) between BeginingIP and EndingIP"
            Dim sqlAdapter As New SqlDataAdapter(selectCommand, sqlConnection)
            sqlConnection.Open()
            sqlAdapter.Fill(tabLocation)
        End Using

        ' Store IP infomation into Location entity class
        If tabLocation.Rows.Count = 1 Then
            locationInfo.BeginIP = tabLocation.Rows(0)(0).ToString()
            locationInfo.EndIP = tabLocation.Rows(0)(1).ToString()
            locationInfo.CountryTwoCode = tabLocation.Rows(0)(2).ToString()
            locationInfo.CountryThreeCode = tabLocation.Rows(0)(3).ToString()
            locationInfo.CountryName = tabLocation.Rows(0)(4).ToString()
        Else
            Response.Write("<strong>Cannot find the location based on the IP address [" & ipAddress & "].</strong> ")
            Return
        End If

Step7: Write code to display the info on the page.

/////////////////////////////////////////////////////////////////////////////
References:

# SQLServer Express
http://msdn.microsoft.com/en-us/library/dd981032.aspx

/////////////////////////////////////////////////////////////////////////////