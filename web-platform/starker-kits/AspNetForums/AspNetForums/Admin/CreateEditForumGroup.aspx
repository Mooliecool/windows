<%@ Page language="c#" %>
<%@ Register TagPrefix="AspNetForums" Namespace="AspNetForums.Controls" Assembly="AspNetForums" %>
<%@ Register TagPrefix="Admin" Namespace="AspNetForums.Controls.Admin" Assembly="AspNetForums" %>
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
      <Admin:CreateEditForumGroup runat="server" RedirectUrl="default.aspx" Mode="CreateForum" />
      <P>
      <a class="normalTextSmall" href="default.aspx">Return to the Administration page</a>
    </form>
  </body>
</HTML>
