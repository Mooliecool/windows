<%@ Page Language="c#" CodeBehind="Login.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.Login" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>

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
                                                <img align="left" height="32" width="60" src="images/1x1.gif">Sign Into Your Account
                                                <br>
                                            </td>
                                        </tr>
                                    </table>
                                    <img align="left" height="1" width="92" src="images/1x1.gif">
                                    <table height="100%" cellspacing="0" cellpadding="0" border="0">
                                        <tr valign="top">
                                            <td width="550">
                                                <asp:Label id="Message" class="ErrorText" runat="server" />
                                                <br>
                                                <br>
                                                &nbsp;<span class="NormalBold">Email</span>
                                                <br>
                                                &nbsp;<asp:TextBox size="25" id="email" runat="server" />&nbsp;
                                                <asp:RequiredFieldValidator id="emailRequired" ControlToValidate="email" Display="dynamic" Font-Name="verdana" Font-Size="9pt" ErrorMessage="'Name' must not be left blank." runat="server" />
                                                <asp:RegularExpressionValidator id="emailValid" ControlToValidate="email" ValidationExpression="[\w\.-]+(\+[\w-]*)?@([\w-]+\.)+[\w-]+" Display="Dynamic" ErrorMessage="Must use a valid email address." runat="server" />
                                                <br>
                                                <br>
                                                &nbsp;<span class="NormalBold">Password</span>
                                                <br>
                                                &nbsp;<asp:TextBox id="password" textmode="password" size="25" runat="server" />&nbsp;
                                                <asp:RequiredFieldValidator id="passwordRequired" ControlToValidate="password" Display="Static" Font-Name="verdana" Font-Size="9pt" ErrorMessage="'Password' must not be left blank." runat="server" />
                                                <br>
                                                <br>
                                                <asp:checkbox id="RememberLogin" runat="server" />
                                                <span class="NormalBold">Remember My Sign-In Across Browser Restarts</span>
                                                <br>
                                                <br>
                                                <br>
                                                <asp:ImageButton id="LoginBtn" ImageURL="images/sign_in_now.gif" runat="server" />
                                                <br>
                                                <br>
                                                <span class="Normal">&nbsp;If you are a new user and you don't have an account with IBuySpy, then register for one now.</span>
                                                <br>
                                                <br>
                                                <a href="register.aspx"><img border="0" src="images/register.gif"></a>
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
