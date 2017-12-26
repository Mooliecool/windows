<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="NextLink2.aspx.vb" Inherits="VBASPNETPreventMultipleWindows.NextLink2" %>

<%@ Register src="UserControls/NextPage.ascx" tagname="NextPage" tagprefix="uc1" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
      NextLink2's link
    </div>
    <uc1:NextPage ID="NextPage1" runat="server" />
    </form>
</body>
</html>
