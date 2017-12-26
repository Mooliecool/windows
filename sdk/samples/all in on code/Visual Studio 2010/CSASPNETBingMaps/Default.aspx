<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETBingMaps.Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <script type="text/javascript" src="http://ecn.dev.virtualearth.net/mapcontrol/mapcontrol.ashx?v=6.3"></script>
    <script type="text/javascript">
        var map = null;
        var style = VEMapStyle.Road;

        function LoadMap() {
            map = new VEMap('pnlBingMap');

            var LA = new VELatLong(34.0540, -118.2370);
            map.LoadMap(LA, 12, style, false, VEMapMode.Mode2D, true, 1);
        }

        function SetMap() {
            var lat = document.getElementById("txtLatitude").value;
            var lng = document.getElementById("txtLongitude").value;

            if (lng == "" | lat == "") {
                alert("You need to input both Latitude and Longitude first.");
                return;
            }

            var ddlzoom = document.getElementById("ddlZoomLevel");
            var zoom = ddlzoom.options[ddlzoom.selectedIndex].value;

            map.SetCenter(new VELatLong(lat, lng));
            map.SetMapStyle(style);
            map.SetZoomLevel(zoom);
        }

        function FindLoc() {
            var loc = document.getElementById("txtLocation").value;
            try {
                map.Find(null, loc);
            } catch (e) {
                alert(e.message);
            }
        }

        function SetStyle(s) {
            if (s == "r") {
                style = VEMapStyle.Road;
            }
            else {
                style = VEMapStyle.Aerial;
            }
        }

    </script>
    <style type="text/css">
        .map
        {
            position: absolute;
            width: 700px;
            height: 500px;
            border: #555555 2px solid;
        }
    </style>
</head>
<body onload="LoadMap();">
    <form id="form1" runat="server">
    <div>
        <table>
            <tr>
                <td style="width: 740px; vertical-align: text-top">
                    <b>Bing Maps</b>
                    <br />
                    <asp:Panel ID="pnlBingMap" CssClass="map" runat="server">
                    </asp:Panel>
                </td>
                <td>
                    <asp:Panel ID="pnlSearch" runat="server" DefaultButton="btnLocation">
                        <b>Find a Location:</b><br />
                        Location:
                        <asp:TextBox ID="txtLocation" runat="server"></asp:TextBox>
                        <br />
                        <asp:Button ID="btnLocation" runat="server" Text="Submit" OnClientClick="FindLoc();return false;" />
                    </asp:Panel>
                    <br />
                    <asp:Panel ID="pnlDisplayOption" runat="server">
                        <b>Show a Map:</b>
                        <br />
                        View Style:
                        <asp:RadioButtonList ID="rdlViewStyle" runat="server" RepeatDirection="Horizontal"
                            RepeatLayout="Flow">
                            <asp:ListItem Selected="True" onclick="SetStyle('r')">Road</asp:ListItem>
                            <asp:ListItem onclick="SetStyle('a')">Aerial</asp:ListItem>
                        </asp:RadioButtonList>
                        <br />
                        Zoom Level:
                        <asp:DropDownList ID="ddlZoomLevel" runat="server">
                            <asp:ListItem>1</asp:ListItem>
                            <asp:ListItem>2</asp:ListItem>
                            <asp:ListItem>3</asp:ListItem>
                            <asp:ListItem>4</asp:ListItem>
                            <asp:ListItem>5</asp:ListItem>
                            <asp:ListItem>6</asp:ListItem>
                            <asp:ListItem>7</asp:ListItem>
                            <asp:ListItem>8</asp:ListItem>
                            <asp:ListItem>9</asp:ListItem>
                            <asp:ListItem>10</asp:ListItem>
                            <asp:ListItem>11</asp:ListItem>
                            <asp:ListItem Selected="True">12</asp:ListItem>
                            <asp:ListItem>13</asp:ListItem>
                            <asp:ListItem>14</asp:ListItem>
                            <asp:ListItem>15</asp:ListItem>
                            <asp:ListItem>16</asp:ListItem>
                            <asp:ListItem>17</asp:ListItem>
                            <asp:ListItem>18</asp:ListItem>
                            <asp:ListItem>19</asp:ListItem>
                        </asp:DropDownList>
                        <br />
                        <asp:Panel ID="pnlLatLng" runat="server" DefaultButton="btnLatLng">
                            Lat:
                            <asp:TextBox ID="txtLatitude" runat="server" Text="34.0540"></asp:TextBox>
                            <br />
                            Lng:
                            <asp:TextBox ID="txtLongitude" runat="server" Text="-118.2370"></asp:TextBox>
                            <br />
                            <asp:Button ID="btnLatLng" runat="server" Text="Submit" OnClientClick="SetMap();return false;" />
                        </asp:Panel>
                    </asp:Panel>
                    <br />
                </td>
            </tr>
        </table>
        <br />
        <br />
    </div>
    </form>
</body>
</html>
