<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="CSASPNETReportViewerExport.aspx.cs"
    Inherits="CSASPNETReportViewerExport.CSASPNETReportViewerExport" %>

<%@ Register Assembly="Microsoft.ReportViewer.WebForms, Version=9.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a"
    Namespace="Microsoft.Reporting.WebForms" TagPrefix="rsweb" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Untitled Page</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <rsweb:ReportViewer ID="ReportViewer1" runat="server" Font-Names="Verdana" Font-Size="8pt"
            Height="400px" ShowToolBar="False" Width="100%">
            <LocalReport ReportPath="SampleReport.rdlc">
            </LocalReport>
        </rsweb:ReportViewer>
        <asp:Button ID="btn_ExportExcel" runat="server" Text="ExportToEXCEL" OnClick="btn_ExportExcel_Click" />
        <asp:Button ID="btn_ExportPDF" runat="server" Text="ExportToPDF" OnClick="btn_ExportPDF_Click" />
    </div>
    </form>
</body>
</html>
