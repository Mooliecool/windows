<%@ Page Language="C#"  Debug="true" ValidateRequest="false" %>
<%@ Import Namespace="System.IdentityModel.Claims" %>
<%@ Import Namespace="Microsoft.IdentityModel.TokenProcessor" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<script runat="server">

    protected void ShowError(string text)
    {
        fields.Visible = false;
        errors.Visible = true;
        errtext.Text = text;
    }
    protected void Page_Load(object sender, EventArgs e)
    {
        string xmlToken;
        xmlToken = Request.Params["xmlToken"];
        if (xmlToken == null || xmlToken.Equals(""))
        {
            ShowError("Token presented was null");
        }
        else
        {
            Token token= new Token(xmlToken);
            givenname.Text = token.Claims[ClaimTypes.GivenName];
            surname.Text = token.Claims[ClaimTypes.Surname];
            email.Text = token.Claims[ClaimTypes.Email];
        }
        
    }
</script>

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id="Head1" runat="server">
    <title>Login Page</title>
</head>
<body>
    <form id="form1" runat="server">
    <div runat="server" id="fields">
        Given Name:<asp:Label ID="givenname" runat="server" Text=""></asp:Label><br/>
        Surname:<asp:Label ID="surname" runat="server" Text=""></asp:Label><br/>
        Email Address:<asp:Label ID="email" runat="server" Text=""></asp:Label><br/>
    </div>
    <div runat="server" id="errors" visible="false">
        Error:<asp:Label ID="errtext" runat="server" Text=""></asp:Label><br/>
    </div>
        
    </form>
</body>
</html>
