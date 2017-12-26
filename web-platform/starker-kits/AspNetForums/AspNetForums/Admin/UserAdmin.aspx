<%@ Import Namespace="AspNetForums.Components" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Page SmartNavigation="true" %>
<HTML>
    <HEAD>
        <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
    </HEAD>
    <body>
        <form runat="server" ID="Form1">
            <AspNetForums:NavigationMenu id="NavigationMenu2" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET" />
            <p>
                <AspNetForums:ShowAllUsers SkinFilename="Skin-AdminShowAllUsers.ascx" runat="server" ID="Useradmin1" />
            <p>
                <a class="normalTextSmall" href="default.aspx">Return to the Administration page</a>
        </form>
    </body>
</HTML>
