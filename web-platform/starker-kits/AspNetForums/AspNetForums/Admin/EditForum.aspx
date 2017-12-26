<%@ Page Language="C#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="Admin" Namespace="AspNetForums.Controls.Admin" Assembly="AspNetForums" %>
<HTML>
    <HEAD>
        <AspNetForums:StyleSkin runat="server" ID="Styleskin1" />
    </HEAD>
    <body>
        <form runat="server" ID="Form1">
            <AspNetForums:NavigationMenu id="NavigationMenu2" title="ASP.NET Discussion Forum" runat="server" Description="A free discussion system for ASP.NET" />
            <br>
            <Admin:EditForum id="objEditForum" runat="server" CreateEditForum-RedirectUrl="Forums.aspx" />
            <P>
                <a class="normalTextSmall" href="default.aspx">Return to the Administration page</a>
        </form>
    </body>
</HTML>
