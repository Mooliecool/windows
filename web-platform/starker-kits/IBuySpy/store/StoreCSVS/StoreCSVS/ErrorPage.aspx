<%@ Page CodeBehind="ErrorPage.aspx.cs" Language="c#" AutoEventWireup="false" Inherits="IBuySpy.ErrorPage" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>

<html>
    <head>
        <link rel="stylesheet" type="text/css" href="IBuySpy.css">
    </head>
    <body background="images/sitebkgrdnogray.gif" leftmargin="0" topmargin="0" rightmargin="0" bottommargin="0" marginheight="0" marginwidth="0">
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
                                <img align="left" width="32" src="images/1x1.gif">
                                <table cellspacing="0" cellpadding="0" width="100%">
                                    <tr>
                                        <td>
                                            <table cellspacing="0" cellpadding="0" width="100%">
                                                <tr>
                                                    <td class="HomeHead">
                                                        <h3>
                                                            We are sorry, but an error occured during the
                                                            <br>
                                                            processing of your last request.
                                                            <br>
                                                            <br>
                                                            This could be a result of either illegal input
                                                            <br>
                                                            values, or a bug in our code. Sorry for the inconvenience.
                                                        </h3>
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
</html>
