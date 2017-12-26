<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETStripHtmlCode.Default" ValidateRequest="false" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    
        <a href="SourcePage.aspx">View the SourcePage.aspx</a><br />
    
        <asp:TextBox ID="tbResult" runat="server" Height="416px" Width="534px" 
            TextMode="MultiLine"></asp:TextBox>
        <br />
        <asp:Button ID="btnRetrieveAll" runat="server"  
            Text="Retrieve entire Html" onclick="btnRetrieveAll_Click" />
    
        <asp:Button ID="btnRetrievePureText" runat="server"  
            Text="Retrieve pure text" onclick="btnRetrievePureText_Click"  />
    
        <asp:Button ID="btnRetrieveSriptCode" runat="server"  
            Text="Retrieve sript code" onclick="btnRetrieveSriptCode_Click"   />
    
        <asp:Button ID="btnRetrieveImage" runat="server"  
            Text="Retrieve images" onclick="btnRetrieveImage_Click"   />
    
        <asp:Button ID="btnRetrievelink" runat="server"  
            Text="Retrieve links" onclick="btnRetrievelink_Click"   />
    
    </div>
    </form>
</body>
</html>
