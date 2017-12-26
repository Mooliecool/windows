<%@ Page Language="c#" CodeBehind="Register.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.Register" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>

<HTML>
  <HEAD>
        <link rel="stylesheet" type="text/css" href="IBuySpy.css">
  </HEAD>
    <body background="images/sitebkgrd.gif" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginheight="0" marginwidth="0">
        <table cellspacing="0" cellpadding="0" width="100%" border="0">
            <tr>
                <td colspan="2">
                    <IBuySpy:Header ID="Header1" runat="server" />
                </td>
            </tr>
            <tr>
                <td valign="top">
                    <IBuySpy:Menu id="Menu1" runat="server" />
                    <img height="1" src="images/1x1.gif" width="145">
                </td>
                <td align="left" valign="top" width="100%" nowrap>
                    <table height="100%" align="left" cellspacing="0" cellpadding="0" width="100%" border="0">
                        <tr valign="top">
                            <td nowrap>
                                <br>
                                <form runat="server">
                                    <img align="left" width="24" height="1" src="images/1x1.gif">
                                    <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                        <tr>
                                            <td class="ContentHead">
                                                <img align="left" height="32" width="60" src="images/1x1.gif">Create a New Account
                                                <br>
                                            </td>
                                        </tr>
                                    </table>
                                    <img align="left" height="1" width="92" src="images/1x1.gif">
                                    <asp:Label id="MyError" CssClass="ErrorText" EnableViewState="false" runat="Server" />
                                    <table height="100%" cellspacing="0" cellpadding="0" width="500" border="0">
                                        <tr valign="top">
                                            <td width="550">
                                                <br>
                                                <br>
                                                <span class="NormalBold">Full Name</span>
                                                <br>
                                                <asp:TextBox size="25" id="Name" runat="server" />
                                                &nbsp;
                                                <asp:RequiredFieldValidator ControlToValidate="Name" Display="dynamic" Font-Name="verdana" Font-Size="9pt" ErrorMessage="'Name' must not be left blank." runat="server" id=RequiredFieldValidator1></asp:RequiredFieldValidator>
                                                <br>
                                                <br>
                                                <span class="NormalBold">Email</span>
                                                <br>
                                                <asp:TextBox size="25" id="Email" runat="server" />
                                                &nbsp;
                                                <asp:RegularExpressionValidator ControlToValidate="Email" ValidationExpression="[\w\.-]+(\+[\w-]*)?@([\w-]+\.)+[\w-]+" Display="Dynamic" Font-Name="verdana" Font-Size="9pt" ErrorMessage="Must use a valid email address." runat="server" id=RegularExpressionValidator1></asp:RegularExpressionValidator>
                                                <asp:RequiredFieldValidator ControlToValidate="Email" Display="dynamic" Font-Name="verdana" Font-Size="9pt" ErrorMessage="'Email' must not be left blank." runat="server" id=RequiredFieldValidator2></asp:RequiredFieldValidator>
                                                <br>
                                                <br>
                                                <span class="NormalBold">Password</span>
                                                <br>
                                                <asp:TextBox size="25" id="Password" TextMode="Password" runat="server" />
                                                &nbsp;
                                                <asp:RequiredFieldValidator ControlToValidate="Password" Display="dynamic" Font-Name="verdana" Font-Size="9pt" ErrorMessage="'Password' must not be left blank." runat="server" id=RequiredFieldValidator3></asp:RequiredFieldValidator>
                                                <br>
                                                <br>
                                                <span class="NormalBold">Confirm Password</span>
                                                <br>
                                                <asp:TextBox size="25" id="ConfirmPassword" TextMode="Password" runat="server" />
                                                &nbsp;
                                                <asp:RequiredFieldValidator ControlToValidate="ConfirmPassword" Display="dynamic" Font-Name="verdana" Font-Size="9pt" ErrorMessage="'Confirm' must not be left blank." runat="server" id=RequiredFieldValidator4></asp:RequiredFieldValidator>
                                                <asp:CompareValidator ControlToValidate="ConfirmPassword" ControlToCompare="Password" Display="Dynamic" Font-Name="verdana" Font-Size="9pt" ErrorMessage="Password fields do not match." runat="server" id=CompareValidator1></asp:CompareValidator>
                                                <br>
                                                <br>
                                                <asp:ImageButton id=RegisterBtn ImageUrl="images/submit.gif" runat="server" />
                                                <br>
                                            </td>
                                        </tr>
                                    </table>
                                </form>
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
    </body>
</HTML>
