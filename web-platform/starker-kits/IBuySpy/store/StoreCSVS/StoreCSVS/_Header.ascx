<%@ Control CodeBehind="_Header.ascx.cs" Language="c#" AutoEventWireup="false" Inherits="IBuySpy.C_Header" %>

<%--

    This user control form the header for each page in the Store.

--%>

<table cellspacing="0" cellpadding="0" width="100%" border="0">
    <tr>
        <td colspan="2" background="images/grid_background.gif" nowrap>
            <table cellspacing="0" cellpadding="0" width="100%" border="0">
                <tr>
                    <td colspan="2">
                        <img src="images/most_secretive_place.gif">
                    </td>
                    <td align="right" nowrap>
                        <table cellpadding="0" cellspacing="0" border="0">
                            <tr valign="top">
                                <td align="center" width="65">
                                    <a href="Login.aspx" class="SiteLinkBold"><img src="images/sign_in.gif" border="0">
                                        Sign In</a>
                                </td>
                                <td align="center" width="75">
                                    <a href="OrderList.aspx" class="SiteLinkBold"><img src="images/account.gif" border="0">
                                        Account</a>
                                </td>
                                <td align="center" width="55">
                                    <a href="ShoppingCart.aspx" class="SiteLinkBold"><img src="images/cart.gif" border="0">
                                        Cart</a>
                                </td>
                                <td align="center" width="65">
                                    <a href="InstantOrder.asmx" class="SiteLinkBold"><img src="images/services.gif" border="0">
                                        Services</a>
                                </td>
                            <tr>
                        </table>
                    </td>
                    <td width="10">
                        &nbsp;
                    </td>
                </tr>
            </table>
        </td>
    </tr>
    <tr>
        <td colspan="2" nowrap>
            <form method="post" action="SearchResults.aspx" id="frmSearch" name="frmSearch">
                <table cellspacing="0" cellpadding="0" width="100%" border="0">
                    <tr bgcolor="#9D0000">
                        <td background="images/modernliving_bkgrd.gif">
                            <img align="left" src="images/modernliving.gif">
                        </td>
                        <td width="94" align="right" bgcolor="#9D0000">
                            <img src="images/search.gif">
                        </td>
                        <td width="120" align="right" bgcolor="#9D0000">
                            <input type="text" name="txtSearch" ID="txtSearch" SIZE="20">
                        </td>
                        <td align="left" bgcolor="#9D0000">
                            &nbsp;<input type="image" src="images/arrowbutton.gif" border="0" id="image1" name="image1">&nbsp;
                        </td>
                    </tr>
                </table>
            </form>
        </td>
    </tr>
</table>
