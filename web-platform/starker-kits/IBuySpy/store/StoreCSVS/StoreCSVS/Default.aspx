<%@ Page Language="c#" CodeBehind="Default.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.CDefault" %>
<%@ Register TagPrefix="IBuySpy" TagName="PopularItems" Src="_PopularItems.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>

<HTML>
  <HEAD>
        <link rel="stylesheet" type="text/css" href="IBuySpy.css">
  </HEAD>
    <body background="images/sitebkgrdnogray.gif" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginheight="0" marginwidth="0">
        <table cellspacing="0" cellpadding="0" width="100%" border="0">
            <tr>
                <td colspan="2">
                    <IBuySpy:Header ID="Header1" runat="server" />
                </td>
            </tr>
            <tr>
                <td valign="top" width=145>
                    <IBuySpy:Menu id="Menu1" runat="server" />
                    <img height="1" src="images/1x1.gif" width="145">
                </td>
                <td align="left" valign="top" width="*" nowrap>
                    <table height="100%" align="left" cellspacing="0" cellpadding="0" width="100%" border="0">
                        <tr valign="top">
                            <td nowrap>
                                <br>
                                <img align="left" width="24" SRC="images/1x1.gif">
                                <table cellspacing="0" cellpadding="0" width="100%">
                                    <tr>
                                        <td>
                                            <table cellspacing="0" cellpadding="0" width="100%">
                                                <tr>
                                                    <td class="HomeHead">
                                                        <asp:Label id="WelcomeMsg" runat="server">Welcome to IBuySpy.com</asp:Label>
                                                    </td>
                                                </tr>
                                            </table>
                                            <table cellspacing="0" cellpadding="2" width="600" border="0">
                                                <tr valign="top">
                                                    <td>
                                                        <table width="300">
                                                            <tr valign="top">
                                                                <td>
                                                                    <span class="NormalDouble">Congratulations, you have successfully infiltrated
                                                                        IBuySpy.com! IBuySpy is a fictitious "click and mortar" retailer that sells
                                                                        ultra-cool spy gear on its Web site. A lighthearted look at some very serious
                                                                        requirements, IBuySpy demonstrates how extraordinarily simple it is to create
                                                                        powerful, scalable applications and services for the .NET platform.
                                                                        <br>
                                                                        <br>
                                                                    </span>
                                                                </td>
                                                            </tr>
                                                        </table>
                                                    </td>
                                                    <td align="left">
                                                        <img border="0" width="309" src="ProductImages/image.gif">
                                                        <br>
                                                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="NormalDouble"><i>Blast off in a <a href='ProductDetails.aspx?productID=373'>
                                                                    <b>Pocket Protector Rocket Pack</b></i></A></span>
                                                    </td>
                                                    <td>
                                                        &nbsp;
                                                    </td>
                                                </tr>
                                                <tr valign="top">
                                                    <td>
                                                        <IBuySpy:PopularItems runat="server" id="PopularItems1" />
                                                    </td>
                                                    <td>
                                                        <br>
                                                        <span class="NormalDouble">To give IBuySpy a test spin, simply starting
                                                            browsing and add any items you want to your shopping cart. Click the <b>Documentation</b>
                                                            link (left) at any point to learn what's going on under the hood.</span>
                                                    </td>
                                                    <td>
                                                        &nbsp;
                                                    </td>
                                                </tr>
                                            </table>
                                        </td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
    </body>
</HTML>
