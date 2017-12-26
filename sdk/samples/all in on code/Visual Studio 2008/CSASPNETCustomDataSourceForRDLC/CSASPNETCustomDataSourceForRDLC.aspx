<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="CSASPNETCustomDataSourceForRDLC.aspx.cs"
    Inherits="CSASPNETCustomDataSourceForRDLC.CSASPNETCustomDataSourceForRDLC" %>

<%@ Register Assembly="Microsoft.ReportViewer.WebForms, Version=9.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a"
    Namespace="Microsoft.Reporting.WebForms" TagPrefix="rsweb" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>CSASPNETCustomDataSourceForRDLC</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <p>
            Country:<asp:TextBox ID="txt_country" runat="server"></asp:TextBox>
            <asp:Button ID="btn_search" runat="server" Text="Filter the records via country" OnClick="btn_search_Click" />
        </p>
        <rsweb:ReportViewer ID="ReportViewer1" runat="server" Font-Names="Verdana" Font-Size="8pt"
            Height="400px" Width="100%">
            <LocalReport ReportPath="TestReport.rdlc">
                <DataSources>
                    <rsweb:ReportDataSource DataSourceId="ObjectDataSource1" Name="Customer" />
                </DataSources>
            </LocalReport>
        </rsweb:ReportViewer>
        <asp:ObjectDataSource ID="ObjectDataSource1" runat="server" SelectMethod="GetCustomers"
            TypeName="CSASPNETCustomDataSourceForRDLC.GetPersons"></asp:ObjectDataSource>
    </div>
    </form>
</body>
</html>
