<%@ Page Language="C#" AutoEventWireup="true"  CodeFile="Default.aspx.cs" Inherits="_Default" ValidateRequest="false" %>
<%@ OutputCache Location="None" %>

<%@ Register assembly="Microsoft.IdentityModel,  Version=3.5.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35" namespace="Microsoft.IdentityModel.Web.Controls" tagprefix="idfx" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>ASP.NET Security Token Service Web Site</title>
    <style type="text/css">
        .style1
        {
            font-size: large;
        }
    </style>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <b><span class="style1">Windows Identity Foundation - ASP.NET Security Token Service Web Site</span><br 
            class="style1" />
        </b>
        <b>Note : This test STS uses Forms based authentication with no password validation. Use more secure authentication mode for production scenarios.</b>        
    </div>
    </form>
</body>
</html>
