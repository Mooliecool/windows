<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSASPNETDataPager._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>    
</head>
<body>
    <form id="form1" runat="server">
    <div>    
        <asp:ListView ID="lvPerson" runat="server">
            <LayoutTemplate>
            <table style="width: 100%; height: 100%; background-color: #5D549C; font-family: Arial; font-size: x-large; color: #FFFFFF;">
                <tr>
                    <td align="center">Person List</td>
                </tr>
            </table>
            <asp:PlaceHolder ID= "itemPlaceHolder" runat ="server" ></asp:PlaceHolder>
            </LayoutTemplate>
                            
            <ItemTemplate>
            <table style="width: 100%; height: 100%; background-color: #E3E6FE; font-family: Arial; font-size:12px; color: #5D549C;">
                <tr>
                    <td>
                    <br />
                    PersonID:&nbsp;<b><asp:Label runat="server" ID="lbPersonID" Text='<%# Eval("PersonID") %>'></asp:Label></b>
                    <br /><br />
                    First Name:&nbsp;<b><asp:Label runat="server" ID="lbFirstName" Text='<%# Eval("FirstName") %>'></asp:Label></b>&nbsp;
                    Last Name:&nbsp;<b><asp:Label runat="server" ID="lbLastName" Text='<%# Eval("LastName") %>'></asp:Label></b>
                    <br /><br />
                    </td>
                </tr>
            </table>
            </ItemTemplate>
            
            <AlternatingItemTemplate>
            <table style="width: 100%; height: 100%; background-color: #F0F3F7; font-family: Arial; font-size:12px; color: #5D549C;">
                <tr>
                    <td>
                    <br />
                    PersonID:&nbsp;<b><asp:Label runat="server" ID="lbPersonID" Text='<%# Eval("PersonID") %>'></asp:Label></b>
                    <br /><br />
                    First Name:&nbsp;<b><asp:Label runat="server" ID="lbFirstName" Text='<%# Eval("FirstName") %>'></asp:Label></b>&nbsp;
                    Last Name:&nbsp;<b><asp:Label runat="server" ID="lbLastName" Text='<%# Eval("LastName") %>'></asp:Label></b>
                    <br /><br />
                    </td>
                </tr>
            </table>
            </AlternatingItemTemplate>
        </asp:ListView>
        <br />
        <asp:DataPager ID="dpPerson" runat="server" PagedControlID="lvPerson" 
            OnPreRender="dpPerson_PreRender">
            <Fields>
                <asp:TemplatePagerField>
                    <PagerTemplate>
                        Page
                        <asp:Label runat="server" ID="lbCurrentPage" 
                          Text="<%# Container.TotalRowCount>0 ? (Container.StartRowIndex / Container.PageSize) + 1 : 0 %>" />
                        of
                        <asp:Label runat="server" ID="lbTotalPages" 
                          Text="<%# Math.Ceiling ((double)Container.TotalRowCount / Container.PageSize) %>" />
                        (Total:
                        <asp:Label runat="server" ID="lbTotalItems" 
                          Text="<%# Container.TotalRowCount%>" />
                        records)
                    <br /><br />                 
                    </PagerTemplate>
                </asp:TemplatePagerField>
                
                
                <asp:NextPreviousPagerField ButtonType ="Button" ShowFirstPageButton="True" ShowPreviousPageButton="True" ShowNextPageButton="True" ShowLastPageButton="True" />
               
                <asp:TemplatePagerField OnPagerCommand="TemplateNextPrevious_OnPagerCommand">
                    <PagerTemplate>                                            
                        <br /><br />
                                                
                        <asp:LinkButton ID="lbtnFirst" runat="server" CommandName="First" 
                        Text="First" Visible='<%# Container.StartRowIndex > 0 %>' />

                        <asp:LinkButton ID="lbtnPrevious" runat="server" CommandName="Previous" 
                        Text='<%# (Container.StartRowIndex - Container.PageSize + 1) + " - " + (Container.StartRowIndex) %>'
                        Visible='<%# Container.StartRowIndex > 0 %>' />
                        
                        <asp:Label ID="lbtnCurrent" runat="server"
                        Text='<%# (Container.StartRowIndex + 1) + "-" + (Container.StartRowIndex + Container.PageSize > Container.TotalRowCount ? Container.TotalRowCount : Container.StartRowIndex + Container.PageSize) %>' />
                       
                        <asp:LinkButton ID="lbtnNext" runat="server" CommandName="Next"
                        Text='<%# (Container.StartRowIndex + Container.PageSize + 1) + " - " + (Container.StartRowIndex + Container.PageSize*2 > Container.TotalRowCount ? Container.TotalRowCount : Container.StartRowIndex + Container.PageSize*2) %>' 
                        Visible='<%# (Container.StartRowIndex + Container.PageSize) < Container.TotalRowCount %>' />
 
                        <asp:LinkButton ID="lbtnLast" runat="server" CommandName="Last" 
                        Text="Last" Visible='<%# (Container.StartRowIndex + Container.PageSize) < Container.TotalRowCount %>' />                        
                    </PagerTemplate>
                </asp:TemplatePagerField>
                
                <asp:TemplatePagerField OnPagerCommand = "TemplateGoTo_OnPagerCommand">            
                    <PagerTemplate>
                        <br /><br />
                        <asp:TextBox ID="tbPageNumber" runat="server" Width="30px" onKeyUp = "value = value.replace(/[^0-9]/g,'')"
                        Text="<%# Container.TotalRowCount>0 ? (Container.StartRowIndex / Container.PageSize) + 1 : 0 %>" ></asp:TextBox>
                        <asp:Button ID="btnGoTo" runat="server" Text="GoTo" />
                    </PagerTemplate>            
                </asp:TemplatePagerField>
            </Fields>
        </asp:DataPager>
    </div>
    </form>
</body>
</html>
