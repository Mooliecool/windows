<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="Default.aspx.vb" Inherits="VBASPNETImageMap._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <p>
            Please select the planet from the image to navigate the iframe</p>
        <asp:ImageMap ID="imgMapSolarSystem" runat="server" ImageUrl="~/solarsystem.jpg"
            HotSpotMode="PostBack">
            <asp:RectangleHotSpot PostBackValue="Sun" AlternateText="Sun" Top="0" Left="0" Right="110"
                Bottom="258" HotSpotMode="PostBack" />
            <asp:CircleHotSpot PostBackValue="Mercury" AlternateText="Mercury" X="189" Y="172"
                Radius="3" HotSpotMode="PostBack" />
            <asp:CircleHotSpot PostBackValue="Venus" AlternateText="Venus" X="227" Y="172" Radius="10"
                HotSpotMode="PostBack" />
            <asp:CircleHotSpot PostBackValue="Earth" AlternateText="Earth" X="277" Y="172" Radius="10"
                HotSpotMode="PostBack" />
            <asp:CircleHotSpot PostBackValue="Mars" AlternateText="Mars" X="324" Y="172" Radius="8"
                HotSpotMode="PostBack" />
            <asp:CircleHotSpot PostBackValue="Jupiter" AlternateText="Jupiter" X="410" Y="172"
                Radius="55" HotSpotMode="PostBack" />
            <asp:PolygonHotSpot PostBackValue="Saturn" AlternateText="Saturn" Coordinates="492,235,471,228,522,179,540,133,581,126,593,134,657,110,660,126,615,167,608,203,563,219,542,214"
                HotSpotMode="PostBack" />
            <asp:CircleHotSpot PostBackValue="Uranus" AlternateText="Uranus" X="667" Y="172"
                Radius="21" HotSpotMode="PostBack" />
            <asp:CircleHotSpot PostBackValue="Neptune" AlternateText="Neptune" X="736" Y="172"
                Radius="18" HotSpotMode="PostBack" />
        </asp:ImageMap>
        <p>
            <asp:Label ID="lbDirection" runat="server"></asp:Label>
        </p>
        <iframe id="ifSelectResult" runat="server" width="895" height="500" src="http://en.wikipedia.org/wiki/Solar_System">
        </iframe>
    </div>
    </form>
</body>
</html>
