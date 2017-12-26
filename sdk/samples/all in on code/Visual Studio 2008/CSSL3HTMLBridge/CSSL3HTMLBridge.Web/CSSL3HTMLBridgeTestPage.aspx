<%@ Page Language="C#" AutoEventWireup="true" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>CSSL3HTMLBridge</title>
    <style type="text/css">
        html, body
        {
            height: 100%;
            overflow: auto;
        }
        body
        {
            padding: 0;
            margin: 0;
        }
        #silverlightControlHost
        {
            height: 100%;
            text-align: center;
        }
        .style1
        {
            font-size: 32pt;
            font-weight: bold;
            color: #FF0000;
        }
    </style>

    <script type="text/javascript" src="Silverlight.js"></script>

    <script type="text/javascript">

        // register silverlight event.
        function onSilverlightLoaded() {
            var silverlight1 = document.getElementById('silverlight1');
            silverlight1.Content.silverlightPage.TextChanged = function(sender, e) {
                document.getElementById('Text4').value = e.Text;
            }
        }
    
        // change textbox1 text.
        function changetext(name) {
            document.getElementById('Text1').value = name;
        }

        function ontext3keydown() {
            var text = document.getElementById('Text3').value;
            // calling managed code method.
            var silverlight1 = document.getElementById('silverlight1');
            silverlight1.Content.silverlightPage.ChangeTB3Text(text);
        }

        function onSilverlightError(sender, args) {
            var appSource = "";
            if (sender != null && sender != 0) {
                appSource = sender.getHost().Source;
            }

            var errorType = args.ErrorType;
            var iErrorCode = args.ErrorCode;

            if (errorType == "ImageError" || errorType == "MediaError") {
                return;
            }

            var errMsg = "Unhandled Error in Silverlight Application " + appSource + "\n";

            errMsg += "Code: " + iErrorCode + "    \n";
            errMsg += "Category: " + errorType + "       \n";
            errMsg += "Message: " + args.ErrorMessage + "     \n";

            if (errorType == "ParserError") {
                errMsg += "File: " + args.xamlFile + "     \n";
                errMsg += "Line: " + args.lineNumber + "     \n";
                errMsg += "Position: " + args.charPosition + "     \n";
            }
            else if (errorType == "RuntimeError") {
                if (args.lineNumber != 0) {
                    errMsg += "Line: " + args.lineNumber + "     \n";
                    errMsg += "Position: " + args.charPosition + "     \n";
                }
                errMsg += "MethodName: " + args.methodName + "     \n";
            }

            throw new Error(errMsg);
        }
    </script>

</head>
<body>
    <form id="form1" runat="server">
    <div id="silverlightControlHost" style="width: 400px; height: 400px; position: absolute;
        top: 0px; left: 0px;">
        <object id="silverlight1" data="data:application/x-silverlight-2," type="application/x-silverlight-2"
            width="100%" height="100%">
            <param name="source" value="ClientBin/CSSL3HTMLBridge.xap" />
            <param name="onError" value="onSilverlightError" />
            <param name="onLoad" value="onSilverlightLoaded" />
            <param name="background" value="white" />
            <param name="minRuntimeVersion" value="3.0.40624.0" />
            <param name="autoUpgrade" value="true" />
            <a href="http://go.microsoft.com/fwlink/?LinkID=149156&v=3.0.40624.0" style="text-decoration: none">
                <img src="http://go.microsoft.com/fwlink/?LinkId=108181" alt="Get Microsoft Silverlight"
                    style="border-style: none" />
            </a>
        </object>
        <iframe id="_sl_historyFrame" style="visibility: hidden; height: 0px; width: 0px;
            border: 0px"></iframe>
    </div>
    <div style="border: 10px solid #FF0000; position: absolute; top: 0px; left: 400px;
        width: 400px; height: 380px;">
        <span class="style1">Html</span><table style="width: 100%">
            <tr>
                <td style="height: 23px" valign="top">
                </td>
            </tr>
            <tr>
                <td valign="top" style="height: 68px">
                    <input id="Text1" type="text" readonly="readonly" style="width: 100%" />
                </td>
            </tr>
            <tr>
                <td valign="top" style="height: 68px">
                    <input id="Text2" type="text" style="width: 100%" />
                </td>
            </tr>
            <tr>
                <td valign="top" style="height: 68px">
                    <input id="Text3" type="text" onchange="ontext3keydown()" style="width: 100%" />
                </td>
            </tr>
            <tr>
                <td valign="top">
                    <input id="Text4" type="text" readonly="readonly" style="width: 100%" />
                </td>
            </tr>
        </table>
    </div>
    </form>
</body>
</html>
