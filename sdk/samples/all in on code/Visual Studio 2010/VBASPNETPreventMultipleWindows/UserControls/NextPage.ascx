<%@ Control Language="vb" AutoEventWireup="false" CodeBehind="NextPage.ascx.vb" Inherits="VBASPNETPreventMultipleWindows.NextPage" %>
<script type="text/javascript">
    //If this window name not equal to sessions,will be goto InvalidPage
    if (window.name != "<%=GetWindowName()%>") {
        window.name = "InvalidPage";
        window.open("InvalidPage.aspx", "_self");
    }
</script>
