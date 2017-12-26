<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETDataListImageGallery._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    <table style="height:300px; width:500px">
      <tr style="height:200px; width:500px">
        <td>
          <asp:Image ID="Image1" runat="server" ImageUrl="~/null.jpg" />
        </td>
      </tr>
      <tr style="height:100px; width:500px">
        <td>
          <asp:DataList ID="DataList1" runat="server" RepeatColumns="5" RepeatDirection="Horizontal" >
          <ItemTemplate>
            <asp:ImageButton ID="imgBtn" runat="server" ImageUrl='<%# "/Image/" + Eval("Url") %>' Width="100px" Height="100px" OnClick="imgBtn_Click" CommandArgument='<%# Container.ItemIndex %>' />
          </ItemTemplate>
          <SelectedItemStyle BorderColor="Red" BorderWidth="1px" />
          </asp:DataList>
        </td>
      </tr>
    </table>
    <table>
      <tr style="height:50px; width:500px">
        <td align="center">
          <asp:LinkButton ID="lbnFirstPage" Text="First" CommandName="first" OnCommand="Page_OnClick" runat="server" Width="125px" />
          <asp:LinkButton ID="lbnPrevPage" Text="Prev" CommandName="prev" OnCommand="Page_OnClick" runat="server" Width="125px" />
          <asp:LinkButton ID="lbnNextPage" Text="Next" CommandName="next" OnCommand="Page_OnClick" runat="server" Width="125px" />
          <asp:LinkButton ID="lbnLastPage" Text="Last" CommandName="last" OnCommand="Page_OnClick" runat="server" Width="125px" />
        </td>
      </tr>
    </table>
    </div>
    </form>
</body>
</html>
