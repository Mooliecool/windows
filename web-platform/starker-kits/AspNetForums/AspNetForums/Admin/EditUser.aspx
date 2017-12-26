<%@ Page SmartNavigation="true" Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>

<%@ Import Namespace="AspNetForums" %>
<%@ Import Namespace="AspNetForums.Components" %>

<HTML>
    <HEAD>
        <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
    </HEAD>
    <body>
        <form runat="server" ID="Form1">
            <AspNetForums:NavigationMenu id="NavigationMenu2" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET" />
            <p>
                <AspNetForums:EditUserProfile 
                	AdminMode="Admin" 
                	runat="server" 
                	ID="Userinfo1" 
                	NAME="Userinfo1" />
            <p>
            <a class="normalTextSmall" href="default.aspx">Return to the Administration page</a>
        </form>
    </body>
</HTML>
