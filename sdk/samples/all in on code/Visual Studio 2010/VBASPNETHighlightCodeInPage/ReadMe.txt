=============================================================================
            VBASPNETHighlightCodeInPage Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Sometimes we input code like VB or HTML in our post and we need these code 
to be highlighted for a better reading experience.This project illustrates 
how to highlight the code in a page. 


/////////////////////////////////////////////////////////////////////////////
Demo the Sample.

Step1: Browse the HighlightCodePage.aspx page from the sample and you will
find a dropdownlist control which is used to let user choose the language of 
code and a textbox control which is used to let user paste the code in.

Step2: Chose a type of language in the dropdownlist control and paste the code
in the textbox control, then click the HighLight button. If the user does not 
choose a type of language or does not paste the code in the textbox control
before clicking the HighLight button, page will show the error message beside
the HighLight button.

Step3: After the user clicking the HighLight button, the highlighted code will
be displayed at the right side of the page. 


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a VB ASP.NET Empty Web Application in Visual Studio 2010.

Step2: Add a VB class file which named as 'CodeManager' in Visual Studio 2010.
In this file, we use a Hashtable variable to store the different languages of 
code and their related regular expressions with matching options.Then add the
style object to the matching string of code. You can find the complete code in
CodeManager.cs file.

Step3: Add an ASP.NET page into the Web Application as the page which used to
let user highlight the code.

Step4: Add a dropdownlist control ,two label controls ,a textbox control and a
button control into the page as below.

	<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title></title>
    <link href="Styles/HighlightCode.css" rel="stylesheet" type="text/css" />
</head>
<body>
    <form id="form1" runat="server">
    <table border="1" style="height: 98%">
        <tr>
            <td style="width: 50%; font-size: 12px">
                <strong>Please paste the code in textbox control and choose 
				the language before clicking
                    the HighLight button </strong>
            </td>
            <td style="width: 50%; font-size: 12px">
                <strong>Result: </strong>
            </td>
        </tr>
        <tr>
            <td>
                Please choose the language:<asp:DropDownList ID="ddlLanguage"
				 runat="server">
                    <asp:ListItem Value="-1">-Please select-</asp:ListItem>
                    <asp:ListItem Value="cs">C#</asp:ListItem>
                    <asp:ListItem Value="vb">VB.NET</asp:ListItem>
                    <asp:ListItem Value="js">JavaScript</asp:ListItem>
                    <asp:ListItem Value="vbs">VBScript</asp:ListItem>
                    <asp:ListItem Value="sql">Sql</asp:ListItem>
                    <asp:ListItem Value="css">CSS</asp:ListItem>
                    <asp:ListItem Value="html">HTML/XML</asp:ListItem>
                </asp:DropDownList>
                <br />
                please paste your code here:<br />
                <asp:TextBox ID="tbCode" runat="server" TextMode="MultiLine" 
				Height="528px" Width="710px"></asp:TextBox>
                <br />
                <asp:Button ID="btnHighLight" runat="server" Text="HighLight"
				 OnClick="btnHighLight_Click" /><asp:Label
                    ID="lbError" runat="server" Text="Label" ForeColor="Red">
					</asp:Label>
            </td>
            <td>
                <div id="DivCode">
                    <asp:Label ID="lbResult" runat="server" Text=""></asp:Label>
                </div>
            </td>
        </tr>
    </table>
    </form>
</body>
</html>

Step5: Open the VB code behind view of the page to write the highlight code function.
You can find the complete version in the HighlightCodePage.aspx.vb file.

      Protected Sub btnHighLight_Click(ByVal sender As Object, _
                                     ByVal e As EventArgs)
        Dim _error As String = String.Empty

        ' Check the value of user's input data.
        If CheckControlValue(Me.ddlLanguage.SelectedValue,
                             Me.tbCode.Text, _error) Then
            ' Initialize the Hashtable variable which used to
            ' store the different languages of code and their 
            ' related regular expressions with matching options.
            Dim _htb As Hashtable = CodeManager.Init()

            ' Initialize the suitable collection object.
            Dim _rg As New RegExp()
            _rg = DirectCast(_htb(Me.ddlLanguage.SelectedValue), RegExp)
            Me.lbResult.Visible = True
            If Me.ddlLanguage.SelectedValue <> "html" Then
                ' Display the highlighted code in a label control.
                Me.lbResult.Text = CodeManager.Encode(
                    CodeManager.HighlightCode(
                        Server.HtmlEncode(
                            Me.tbCode.Text).
                        Replace("&quot;", """").
                        Replace("&#39;", "'"),
                        Me.ddlLanguage.SelectedValue, _rg))
            Else
                ' Display the highlighted code in a label control.
                Me.lbResult.Text = CodeManager.Encode(
                    CodeManager.HighlightHTMLCode(Me.tbCode.Text, _htb))
            End If
        Else
            Me.lbError.Visible = True
            Me.lbError.Text = _error
        End If
    End Sub

Step6: Create a new directory, "Styles". We need to create a style sheet file.
Add a style sheet file which named as 'HighlightCode' in the Styles folder.
In this file ,define some of the styles which used to highlight the code.You can
find the complete version in the HighlightCode.css file.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: 
# struct
http://msdn.microsoft.com/en-us/library/ah19swz4(VS.71).aspx

MSDN:
# Hashtable Class
http://msdn.microsoft.com/en-us/library/system.collections.hashtable.aspx

MSDN:
# ArrayList Class
http://msdn.microsoft.com/en-us/library/system.collections.arraylist.aspx

MSDN:
# Regex Class
http://msdn.microsoft.com/en-us/library/system.text.regularexpressions.regex.aspx

MSDN:
# String.Replace Method 
http://msdn.microsoft.com/en-us/library/system.string.replace.aspx

MSDN:
# GroupCollection Class
http://msdn.microsoft.com/en-us/library/system.text.regularexpressions.groupcollection.aspx

MSDN:
# Match Class
http://msdn.microsoft.com/en-us/library/system.text.regularexpressions.match.aspx


/////////////////////////////////////////////////////////////////////////////