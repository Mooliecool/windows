<%@ Page Language="vb" AutoEventWireup="true" CodeBehind="HtmlClient.aspx.vb" Inherits="AzureBingMaps.WebRole.HtmlClient" ValidateRequest="false" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>Plan My Travel</title>
    <script type="text/javascript" src="http://ajax.microsoft.com/ajax/jquery/jquery-1.4.2.min.js"></script>
    <script type="text/javascript" src="http://ajax.microsoft.com/ajax/jquery.ui/1.8.5/jquery-ui.min.js"></script>
    <script type="text/javascript" src="http://ecn.dev.virtualearth.net/mapcontrol/mapcontrol.ashx?v=7.0"></script>
    <script type="text/javascript" src="http://ajax.microsoft.com/ajax/jquery.templates/beta1/jquery.tmpl.min.js"></script>
    <script type="text/javascript" src="Scripts/HtmlClient.js"></script>

    <script id="travelTemplate" type="text/x-jquery-tmpl">
    <li>
        <div class="ItemContainer" onclick="SelectItem(this)">
            <div class="PlaceDiv">${Value.Place}</div>
            <div>Location: ${Value.Latitude}, ${Value.Longitude}</div>
            <div>Time: <input type="text" class="DatePicker" value="${formatDate(Value.Time)}" readonly/></div>
            <input type="button" value="Delete" onclick="DeleteItem(this)"/>
            <input type="hidden" value="${Value.PartitionKey}"/>
            <input type="hidden" value="${Value.RowKey}"/>
        </div>
    </li>
    </script>

    <link href="Styles/Site.css" rel="stylesheet" type="text/css" />
    <link rel="Stylesheet" type="text/css" href="Styles/jquery.ui.datepicker.css" />
    <link rel="Stylesheet" type="text/css" href="Styles/jquery-ui-1.8.2.custom.css" />
</head>
<body>
    <form id="form1" runat="server">
    <asp:HyperLink ID="LoginLink" runat="server" NavigateUrl="LoginPage.aspx?returnpage=HtmlClient.aspx" Text="Login to manage your custom travel." />
    <asp:Label ID="UserNameLabel" runat="server" />
    <div id="ErrorInformation"></div>
    <div id="OperationsArea">
        <input id="DataButton" type="button" value="Save" onclick="SaveListButton_Click()" />
        <ul id="TravelList" />
    </div>
    <div id="MapArea">
        <div id="MainMap" style="width: 100%; height: 600px; position: absolute">
            <div id="PushpinPopup">
                <img src="Images/Cloud Dialog.png" alt="Pushpin Popup" style="position:absolute"/>
                <div id="PushpinText"></div>
            </div>
        </div>
    </div>
    </form>
</body>
</html>