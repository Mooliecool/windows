<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETServerClock.Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
    <script type="text/javascript">
        function doing() {
            var xmlHttp;
            try {
                xmlHttp = new XMLHttpRequest();
            }
            catch (e) {
                try {
                    xmlHttp = new ActiveXObject("Msxml2.XMLHTTP");
                }
                catch (e) {
                    try {
                        xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
                    }
                    catch (e) {
                        alert("Error");
                        return false;
                    }
                }
            }
            xmlHttp.onreadystatechange = function () {
                if (xmlHttp.readyState == 4) {
                    var strResult = xmlHttp.responseText;
                    document.getElementById("time").innerText = strResult;
                }
            }
            xmlHttp.open("GET", "Clock.aspx", true);
            xmlHttp.send(null);
        }

        function gettingTime() {
            setInterval(doing, 1000);
        } 
    </script>
</head>
<body onload="gettingTime();">
    <form id="form1" runat="server">
    <div>
        The server time is now：<span id="time" />
    </div>
    </form>
</body>
</html>
