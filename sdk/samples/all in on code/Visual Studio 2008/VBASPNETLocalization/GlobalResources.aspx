<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="GlobalResources.aspx.vb" Inherits="VBASPNETLocalization.GlobalResources" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form2" runat="server">
    <div>
        <h2>GlobalResources.aspx</h2>
        <p>
            Manually Culture Selection:&nbsp;&nbsp;
            
            <asp:DropDownList ID="ddlLanguage" runat="server" AutoPostBack="True">
                <asp:ListItem Value="">Auto Select</asp:ListItem>
                <asp:ListItem Value="en">English</asp:ListItem>
                <asp:ListItem Value="fr">Français</asp:ListItem>
                <asp:ListItem Value="de">Deutsch</asp:ListItem>
            </asp:DropDownList>
        </p>
        <p>
        <asp:Button ID="Button1" runat="server" 
                    Text="Back to LocalResources.aspx" 
                    PostBackUrl="~/LocalResources.aspx" />
        </p>
        <hr />
        <p>Use Local Resources specially for page LocalResources.aspx:</p>
        <p>
            <strong>
                <asp:Label ID="Label1" runat="server" Text="Welcome" 
                           meta:resourceKey="lblLocal">
                </asp:Label>
            </strong>
        </p>
        <hr />
        <p>Use Global Resources:</p>
        <p>
            <strong>
                <asp:Label ID="lblGlobal" runat="server" 
                           BackColor="<%$ Resources:GlobalResources, lblGlobalBackColor%>"
                           Text="<%$ Resources:GlobalResources, lblGlobalText%>">
                </asp:Label>
            </strong>
        </p>
    </div>
    </form>
</body>
</html>
