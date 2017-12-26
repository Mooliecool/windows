===========================================================================
            VBASPNETAddControlDynamically Project Overview
===========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

 This project shows how to add controls dynamically in ASP.NET pages. It 
 imitates a scenario that customers need to input more than one address info 
 without max limit. So we use button to add new address input TextBoxes.
 After a customer inputs all the addresses, we also use button to update 
 these info into database, which is run as displaying these addresses in 
 this sample.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample.

Step1: Browse the Default.aspx page from the sample and you will find two 
buttons called "Add a New Address" and "Save These Addresses".

Step2: Click on the "Add a New Address" button to add a field after these
two buttons. It will contain a Lable, a TextBox and a "Check" button.

Step3: Input something into the TextBox and click on the "Add a New Address" 
button again to add another field after the first.

Step4: Enter some strings in the second TextBox and then try to click the
"Check" button. A msg will pop-up telling the value in the TextBox next
to the "Check" button.

Step5: Click on the "Save These Addresses" button. Now, the list of the 
addresses inputted will be displayed on the top of the page.

Step6: Click on the "Add a New Address" button to react this process. You can
add as many such fields as you want to test this demo.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a VB ASP.NET Empty Web Application in Visual Studio 2010.

Step2: Add a Default ASP.NET page into it as the demo page.

Step3: Add two Buttons and a Panel into the page as the HTML code below.

	<asp:Button ID="btnAddAddress" runat="server" Text="Add a New Address" />
	<asp:Button ID="btnSave" runat="server" Text="Save These Addresses" />
	<br />
	<br />
	<asp:Panel ID="pnlAddressContainer" runat="server">
	</asp:Panel>
	<br />

Step4: Open the VB behind code view to write a funciton of adding components.
You can find the complete version in the Default.aspx.vb file.

    Protected Sub AddAdress(ByVal id As String)
        Dim lb As New Label()
        lb.Text = "Address" & id & ": "

        Dim tb As New TextBox()
        tb.ID = "TextBox" & id
		tb.Text = Request.Form(tb.ID)

        Dim btn As New Button()
        btn.Text = "Check"
        btn.ID = "Button" & id

        AddHandler btn.Click, AddressOf ClickEvent

        pnlAddressContainer.Controls.Add(lb)
        pnlAddressContainer.Controls.Add(tb)
        pnlAddressContainer.Controls.Add(btn)
    End Sub

Step5: Write the function for "Check" button's click event. Some code in this
function will need skills on working along with JavaScript in ASP.NET page.

    Protected Sub ClickEvent(ByVal sender As Object, ByVal e As EventArgs)
        Dim btn As Button = TryCast(sender, Button)
        Dim tb As TextBox = TryCast(pnlAddressContainer.FindControl(btn.ID.Replace("Button", "TextBox")), TextBox)
        Dim address As String = If(tb.Text = "", "Empty", tb.Text)

        Dim sb As New System.Text.StringBuilder()
        sb.Append("<script type=""text/javascript"">")
        sb.Append("alert(""Address" & btn.ID.Replace("Button", "") & " is " & address & "."");")
        sb.Append("</script>")

        If Not ClientScript.IsClientScriptBlockRegistered(Me.GetType(), "AlertClick") Then
            ClientScript.RegisterClientScriptBlock(Me.GetType(), "AlertClick", sb.ToString())
        End If
    End Sub

Step6: Edit click event handler of the "Add New Address" button to call the 
funciton above to add address components.

/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET Forum: 
# Why do dynamic controls disappear on postback and not raise events?
http://forums.asp.net/t/1186195.aspx

MSDN:
# ASP.NET Page Life Cycle Overview
http://msdn.microsoft.com/en-us/library/ms178472.aspx

/////////////////////////////////////////////////////////////////////////////