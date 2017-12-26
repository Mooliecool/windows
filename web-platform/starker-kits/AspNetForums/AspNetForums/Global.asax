<%@ Import Namespace="AspNetForums" %>

<script language="C#" runat="server">

    //*********************************************************************
    //
    // Application_AuthenticateRequest Event
    //
    // If the client is authenticated with the application, then determine
    // which security roles he/she belongs to and replace the "User" intrinsic
    // with a custom IPrincipal security object that permits "User.IsInRole"
    // role checks within the application
    //
    // Roles are cached in the browser in an in-memory encrypted cookie.  If the
    // cookie doesn't exist yet for this session, create it.
    //
    //*********************************************************************
    void Application_AuthenticateRequest(Object sender, EventArgs e) {
      UserRoles forumRoles = new UserRoles();
      forumRoles.GetUserRoles();
    }

</script>