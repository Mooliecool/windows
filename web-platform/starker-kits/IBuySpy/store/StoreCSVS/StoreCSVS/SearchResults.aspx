<%@ Page Language="c#" CodeBehind="SearchResults.aspx.cs" AutoEventWireup="false" Inherits="IBuySpy.SearchResults" %>
<%@ Register TagPrefix="IBuySpy" TagName="Header" Src="_Header.ascx" %>
<%@ Register TagPrefix="IBuySpy" TagName="Menu" Src="_Menu.ascx" %>

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
                <td valign="top">
                    <IBuySpy:Menu id="Menu1" runat="server" />
                    <img height="1" src="images/1x1.gif" width="145">
                </td>
                <td align="left" valign="top" width="100%" nowrap>
                    <table height="100%" align="left" cellspacing="0" cellpadding="0" width="100%" border="0">
                        <tr valign="top">
                            <td nowrap>
                                <br>
                                <asp:DataList id="MyList" RepeatColumns="2" runat="server">
                                    <ItemTemplate>
                                        <table border="0" width="300">
                                            <tr>
                                                <td width="25">
                                                    &nbsp;
                                                </td>
                                                <td width="100" valign="middle" align="right">
                                                    <a href='ProductDetails.aspx?productID=<%# DataBinder.Eval(Container.DataItem, "ProductID") %>'>
                                                        <img src='ProductImages/thumbs/<%# DataBinder.Eval(Container.DataItem, "ProductImage") %>' width="100" height="75" border="0">
                                                    </a>
                                                </td>
                                                <td width="200" valign="middle">
                                                    <a href='ProductDetails.aspx?productID=<%# DataBinder.Eval(Container.DataItem, "ProductID")%>'>
                                                        <span class="ProductListHead">
                                                            <%# DataBinder.Eval(Container.DataItem, "ModelName")%>
                                                        </span>
                                                        <br>
                                                    </a>
                                                    <span class="ProductListItem"><b>Special Price: </b>
                                                        <%# DataBinder.Eval(Container.DataItem, "UnitCost", "{0:c}")%>
                                                    </span>
                                                    <br>
                                                    <a href='AddToCart.aspx?productID=<%# DataBinder.Eval(Container.DataItem, "ProductID")%>'>
                                                        <font color="#9D0000"><b>Add To Cart</b></font></a>
                                                </td>
                                            </tr>
                                        </table>
                                    </ItemTemplate>
                                </asp:DataList>
                                <img height="1" width="30" src="Images/1x1.gif">
                                <asp:Label id="ErrorMsg" class="ErrorText" runat="server" />
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
    </body>
</HTML>
