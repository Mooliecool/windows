<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Default.aspx.cs" Inherits="_Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>CSASPNETAJAXConsumeExternalWebService</title>
    <script type="text/javascript">
        // This function is used to call the service by Ajax Extension.
        function GetServerDateTime() {
            $get("Result").innerHTML = "Please wait a moment...";
            BridgeWebService.GetServerTime(onSuccess, onFailed);
        }
        // This function will be executed when get a response 
        // from the service.
        function onSuccess(result) {
            $get("Result").innerHTML = "Server DateTime is : " + result.toLocaleString();
        }

        // This function will be executed when get an exception
        // from the service.
        function onFailed(args) {
            alert("Server Return Error:\n" +
            "Message:" + args.get_message() + "\n" +
            "Status Code:" + args.get_statusCode() + "\n" +
            "Exception Type:" + args.get_exceptionType());
        }
    </script>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:ScriptManager ID="ScriptManager1" runat="server">
            <Services>
                <asp:ServiceReference Path="~/BridgeWebService.asmx" />
                <%--
                <!-- 
                     We will get exception if we add
                     the external service reference directly like this: 
                -->
                <asp:ServiceReference 
                    Path="http://localhost:43224/ExternalWebSite/ExternalWebService.asmx" />
                --%>
            </Services>
        </asp:ScriptManager>

        <div id="Result" style="width: 100%; height: 100px; background-color: Black; color: White">
        </div>
        <input type="button" value="Get Server Time From External Web Service" onclick="GetServerDateTime()" />
    </div>
    </form>
</body>
</html>
