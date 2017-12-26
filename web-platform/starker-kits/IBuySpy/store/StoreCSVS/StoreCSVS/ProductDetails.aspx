<%@ Page Language="c#" EnableViewState="false" CodeBehind="ProductDetails.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.ProductDetailsPage" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="AlsoBought" Src="_AlsoBought.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="ReviewList" Src="_ReviewList.ascx" %>
<%@ OutputCache Duration="60" VaryByParam="ProductID" %>

<HTML>
  <HEAD>
        <link href="IBuySpy.css" type="text/css" rel="stylesheet">
  </HEAD>
    <body bottomMargin="0" leftMargin="0" background="images/sitebkgrd.gif" topMargin="0" rightMargin="0" marginwidth="0" marginheight="0">
        <table cellSpacing="0" cellPadding="0" width="100%" border="0">
            <tr>
                <td colSpan="2">
                    <IBuySpy:Header id="Header1" runat="server" />
                </td>
            </tr>
            <tr>
                <td vAlign="top" width=145>
                    <IBuySpy:Menu id="Menu1" runat="server" />
                    <img height="1" src="images/1x1.gif" width="145">
                </td>
                <td vAlign="top" align="left">
                    <table height="100%" cellSpacing="0" cellPadding="0" width="620" align="left" border="0">
                        <tr vAlign="top">
                            <td>
                                <br>
                                <img src="images/1x1.gif" width="24" align="left">
                                <table cellSpacing="0" cellPadding="0" width="100%" border="0">
                                    <tr>
                                        <td class="ContentHead">
                                            <img height="32" src="images/1x1.gif" width="60" align="left"><asp:label id="ModelName" runat="server" />
                                            <br>
                                        </td>
                                    </tr>
                                </table>
                                <table cellSpacing="0" cellPadding="0" width="100%" border="0" valign="top">
                                    <tr vAlign="top">
                                        <td rowspan="2">
                                            <img height="1" width="24" src="images/1x1.gif">
                                        </td>
                                        <td width="309">
                                            <img height="15" src="images/1x1.gif">
                                            <br>
                                            <asp:image id="ProductImage" runat="server" height="185" width="309" border="0" />
                                            <br>
                                            <br>
                                            <img height="20" src="images/1x1.gif" width="72"><span class="UnitCost"><b>Your Price:</b>&nbsp;<asp:label id="UnitCost" runat="server" /></span>
                                            <br>
                                            <img height="20" src="images/1x1.gif" width="72"><span class="ModelNumber"><b>Model Number:</b>&nbsp;<asp:label id="ModelNumber" runat="server" /></span>
                                            <br>
                                            <img height="30" src="images/1x1.gif" width="72"><asp:hyperlink id="addToCart" runat="server" ImageUrl="images/add_to_cart.gif" />
                                        </td>
                                        <td>
                                            <table width="300" border="0">
                                                <tr>
                                                    <td vAlign="top">
                                                        <asp:label class="NormalDouble" id="desc" runat="server"></asp:label>
                                                        <br>
                                                    </td>
                                                </tr>
                                            </table>
                                            <img height="30" src="images/1x1.gif">
                                            <IBuySpy:AlsoBought id="AlsoBoughtList" runat="server" />
                                        </td>
                                    </tr>
                                    <tr>
                                    </tr>
                                </table>
                                <table border="0">
                                    <tr>
                                        <td>
                                            <img src="images/1x1.gif" width="89" height="20">
                                        </td>
                                        <td width="100%">
                                            <IBuySpy:ReviewList id="ReviewList" runat="server" />
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
