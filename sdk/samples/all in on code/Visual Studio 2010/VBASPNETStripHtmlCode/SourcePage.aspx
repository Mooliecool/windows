<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="SourcePage.aspx.vb" Inherits="VBASPNETStripHtmlCode.Source" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title></title>
</head>
<script type="text/javascript">
    function click_client() {
        var text = document.getElementById('textDisplay');
        var checkbox = document.getElementById('Checkbox1');
        text.value = "this is a client click";
        if (checkbox.checked == true) {
            return true;
        }
        else {
            //alert("a");
            return false;
        }
    }
</script>

<script type="text/javascript">
    function click_client2() {
        var text = document.getElementById('textDisplay');
        var checkbox = document.getElementById('Checkbox1');
        text.value = "this is a client click";
        if (checkbox.checked == true) {
            return true;
        }
        else {
            alert("a");
        }
    }
</script>
<body>
    <form id="form1" runat="server">
    <div>
        Hello everybody:<br />
        <a href="http:\\www.microsoft.com" type="text/html">www.microsoft.com</a><br />
        <a href="http:\\www.asp.net">www.asp.net</a><br />
        
        <input type="text" id="textDisplay" runat="server" /><asp:Button id="Button1" runat="server" Text="Submit" 
            OnClientClick="return click_client()" />
        <input id="Checkbox1" type="checkbox" value="Check" /><br />
    </div>
    <img alt="Image/asp.jpg" src="Image/asp.jpg" />
    <img alt="Image/asp.jpg" src="Image/asp.jpg" width="100"/>
    
    </form>
</body>
</html>
