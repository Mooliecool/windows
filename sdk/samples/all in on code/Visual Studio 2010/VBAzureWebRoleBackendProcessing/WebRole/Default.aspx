<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="Default.aspx.vb" Inherits="VBAzureWebRoleBackendProcessing.WebRole._Default" %>
<%@ Import Namespace="VBAzureWebRoleBackendProcessing.Common" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>WebRole Backend Processing Sample</title>
</head>
<body>
    <form id="form1" runat="server">

Please add some words to the table. Then the backend processor will process these words and transform them to upper case.<br />
<asp:TextBox ID="tbContent" runat="server" Height="50px" Width="350px" TextMode="MultiLine"></asp:TextBox><br />
<asp:Button ID="btnProcess" runat="server" Text="Process" />

<h3>Results</h3>

<!-- A countdown timer. -->
Next refresh <span id="remaining_seconds_span">5</span> seconds.
<script type="text/javascript">
    function Countdown() {
        var remain = GetCounter();
        if (remain == 0) {
            remain = 5;
            __doPostBack('<%= UpdatePanelResult.ClientID %>', '');
        }
        SetCounter(remain);
    }
    setInterval(Countdown, 1000);

    // Helper functions

    function GetCounter() {
        var container = document.getElementById("remaining_seconds_span");
        if (container) {
            if (document.all) { //IE
                return parseInt(container.innerText) - 1;
            } else { // FireFox
                return parseInt(container.textContent) - 1;
            }
        }
        return 0;
    }
    function SetCounter(value) {
        var container = document.getElementById("remaining_seconds_span");
        if (container) {
            if (document.all) { //IE
                container.innerText = value;
            } else { // FireFox
                container.textContent = value;
            }
        }
    }
</script>

<asp:ScriptManager ID="ScriptManager1" runat="server">
</asp:ScriptManager>

<asp:UpdatePanel ID="UpdatePanelResult" runat="server">
    <ContentTemplate>
        <asp:DataList ID="dlResult" runat="server">
            <ItemTemplate>
                <asp:Image ID="imgProcessing" runat="server" ImageUrl="~/Images/Processing.gif" Visible='<%# Not DirectCast(Container.DataItem, WordEntry).IsProcessed %>' />
                <asp:Image ID="imgDone" runat="server" ImageUrl="~/Images/Done.gif" Visible='<%# DirectCast(Container.DataItem, WordEntry).IsProcessed %>' />
                <%# Eval("Content") %><hr />
            </ItemTemplate>
        </asp:DataList>
    </ContentTemplate>
</asp:UpdatePanel>

    </form>
</body>
</html>
