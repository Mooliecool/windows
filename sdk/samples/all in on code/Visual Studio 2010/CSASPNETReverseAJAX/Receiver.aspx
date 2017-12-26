<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Receiver.aspx.cs" Inherits="CSASPNETReverseAJAX.Receiver" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">

    <!-- Login -->
    <asp:Label ID="lbNotification" runat="server" ForeColor="Red" Text="Please login:"></asp:Label><br />
    <asp:TextBox ID="tbUserName" runat="server"></asp:TextBox>
    <asp:Button ID="btnLogin" runat="server" Text="Click to login" onclick="btnLogin_Click" />

    <!-- Receive messages -->
    <asp:ScriptManager ID="ScriptManager1" runat="server" AsyncPostBackTimeout="2147483647">
        <Services>
            <asp:ServiceReference Path="~/Dispatcher.asmx" />
        </Services>
    </asp:ScriptManager>
    <script type="text/javascript">

        // This method will persist a http request and wait for messages.
        function waitEvent() {

            CSASPNETReverseAJAX.Dispatcher.WaitMessage("<%= Session["userName"] %>", 
            function (result) {
                
                displayMessage(result);

                // Keep looping.
                setTimeout(waitEvent, 0);
            }, function () {

                // Keep looping.
                setTimeout(waitEvent, 0);
            });
        }

        // Append a message content to the result panel.
        function displayMessage(message) {
            var panel = document.getElementById("<%= lbMessages.ClientID %>");

            panel.innerHTML += currentTime() + ": " + message + "<br />";
        }

        // Return a current time string.
        function currentTime() {
            var currentDate = new Date()
            return currentDate.getHours() + ":" + currentDate.getMinutes() + ":" + currentDate.getSeconds();
        }
    </script>

    <h3>Messages:</h3>
    <asp:Label ID="lbMessages" runat="server" ForeColor="Red"></asp:Label>

    </form>
</body>
</html>
