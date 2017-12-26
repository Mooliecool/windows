<%@ Page Language="vb" AutoEventWireup="false" %>


<%@ Register Assembly="AjaxControlToolkit" Namespace="AjaxControlToolkit" TagPrefix="asp" %>
<%@ Register Assembly="HTMLEditorExtender" Namespace="HTMLEditorExtender" TagPrefix="asp" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>VBASPNETHTMLEditorExtender</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <script type="text/javascript">
            function insertStyle(openTag, closeTag, control) {

                var editor = Sys.Extended.UI.HTMLEditor.LastFocusedEditPanel.get_activePanel();
                var sel = editor._getSelection();
                var range = editor._createRange(sel);
                var parent = Sys.Extended.UI.HTMLEditor.getSelParent(editor);

                var currentParentNode = parent;
                while (parent.tagName != null && parent.tagName.toLowerCase() != "body") {
                    currentParentNode = parent;
                    parent = parent.parentNode;
                }

                if (parent != null && currentParentNode != null) {
                    currentParentNode.outerHTML = openTag + currentParentNode.innerText + closeTag;
                }

            }

        </script>
        
        <asp:ToolkitScriptManager runat="server" ID="ToolkitScriptManager1">
        </asp:ToolkitScriptManager>
        <asp:MyEditor runat="server" ID="MyEditor1" />
    </div>
    </form>
</body>
</html>
