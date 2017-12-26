<%@ Page Language="c#" CodeBehind="OrderList.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.OrderList" %>
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
                                    <img align="left" width="24" src="images/1x1.gif">
                                    <table cellspacing="0" cellpadding="0" width="100%" border="0">
                                        <tr>
                                            <td class="ContentHead">
                                                <img align="left" height="32" width="60" src="images/1x1.gif">Account History
                                                <br>
                                            </td>
                                        </tr>
                                    </table>
                                    <img align="left" height="4" width="110" src="images/1x1.gif"> <font color="red">
                                        <asp:Label id="MyError" class="ErrorText" runat="Server" />
                                    </font>
                                    <br>
                                    <img align="left" height="15" width="84" src="images/1x1.gif" border="0">
                                    <table height="100%" cellspacing="0" cellpadding="0" width="550" border="0">
                                        <tr valign="top">
                                            <td width="100%">
                                                <asp:DataGrid id="MyList" width="90%" BorderColor="black" GridLines="Vertical" cellpadding="4" cellspacing="0" Font-Name="Verdana" Font-Size="8pt" ShowFooter="true" HeaderStyle-CssClass="CartListHead" FooterStyle-CssClass="cartlistfooter" ItemStyle-CssClass="CartListItem" AlternatingItemStyle-CssClass="CartListItemAlt" AutoGenerateColumns="false" runat="server">
                                                    <Columns>
                                                        <asp:BoundColumn HeaderText="Order ID" DataField="OrderID" />
                                                        <asp:BoundColumn HeaderText="Order Date" DataField="OrderDate" DataFormatString="{0:d}" />
                                                        <asp:BoundColumn HeaderText="Order Total" DataField="OrderTotal" DataFormatString="{0:c}" />
                                                        <asp:BoundColumn HeaderText="Ship Date" DataField="ShipDate" DataFormatString="{0:d}" />
                                                        <asp:HyperLinkColumn HeaderText="Show Details" Text="Show Details" DataNavigateUrlField="OrderID" DataNavigateUrlFormatString="orderdetails.aspx?OrderID={0}" />
                                                    </Columns>
                                                </asp:DataGrid>
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
