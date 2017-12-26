<%@ Page Language="VB" AutoEventWireup="false" CodeFile="Default.aspx.vb" Inherits="_Default" %>

<%@ Register Assembly="ImagePreviewExtender" Namespace="ImagePreviewExtender" TagPrefix="cc1" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title></title>
    <style type="text/css">
        .ThumbnailCss
        {
            width: 50px;
            height: 50px;
            border: 2px solid black;
            padding: 5px;
            background-color: White;
            cursor:pointer;
        }
    </style>
    <script type="text/javascript">
    
    </script>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:ScriptManager ID="ScriptManager1" runat="server">
        </asp:ScriptManager>
        <fieldset>
            <legend>Extend an Image control</legend>
            <asp:Image ID="Image1" runat="server" ImageUrl="http://i.microsoft.com/global/en/publishingimages/sitebrand/microsoft.gif" />
            <cc1:ImagePreviewControl ID="ImagePreviewControl1" ThumbnailCssClass="ThumbnailCss"
                TargetControlID="Image1" runat="server" />
        </fieldset>
        <fieldset>
            <legend>Extend a Panel control which contains some Image controls</legend>
        <asp:Panel runat="server" ID="ImageBox">
            <asp:Image ID="Image2" runat="server" ImageUrl="images/test1.jpg" />
            <asp:Image ID="Image3" runat="server" ImageUrl="images/test2.jpg" />
            <asp:Image ID="Image4" runat="server" ImageUrl="images/test3.jpg" />
        </asp:Panel>
        </fieldset>
        <cc1:ImagePreviewControl ID="ImagePreviewControl2" ThumbnailCssClass="ThumbnailCss"
            TargetControlID="ImageBox" runat="server" />
    </div>
    </form>
</body>
</html>
