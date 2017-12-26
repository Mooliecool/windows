<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="Default.aspx.vb" Inherits="VBASPNETFormViewUpload._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>    
        <asp:FormView ID="fvPerson" runat="server">
        <EditItemTemplate>
                <table style="width: 400px;" >
                    <tr>
                        <td style="width: 128px; height: 128px;" rowspan="2">
                            <asp:Image ID="imgPicture" runat="server" 
                            ImageUrl ='<%# Eval("PersonID","Image.aspx?PersonID={0}") %>' Width="128px" 
                            Height="128px" /> 
                        </td>
                        <td valign="top">
                            Person ID:
                            <asp:Label ID="lblPersonID" runat="server" Text='<%# Eval("PersonID") %>' />
                            <br />
                            First name:
                            <asp:TextBox ID="tbFirstName" runat="server" 
                            Text='<%# Bind("FirstName") %>' />
                            <br />
                            Last name:
                            <asp:TextBox ID="tbLastName" runat="server" 
                            Text='<%# Bind("LastName") %>' />
                            <br />                     
                            Picture path:
                            <asp:FileUpload ID="uploadPicture" runat="server" />
                        </td>
                    </tr>
                    <tr>
                        <td valign="bottom">
                            <asp:LinkButton ID="UpdateButton" runat="server" CausesValidation="True" 
                            CommandName="Update" Text="Update" />
                            &nbsp;<asp:LinkButton ID="UpdateCancelButton" runat="server" 
                        CausesValidation="False" CommandName="Cancel" Text="Cancel" />
                        </td>
                    </tr>
                </table>
            </EditItemTemplate>
            <InsertItemTemplate>
                <table style="width: 400px;" >
                    <tr>
                        <td height="100px" valign="top">
                            First name:
                            <asp:TextBox ID="tbFirstName" runat="server" 
                            Text='<%# Bind("FirstName") %>' />
                            <br />
                            Last name:
                            <asp:TextBox ID="tbLastName" runat="server" 
                            Text='<%# Bind("LastName") %>' />
                            <br />                     
                            Picture path:
                            <asp:FileUpload ID="uploadPicture" runat="server" />
                        </td>
                    </tr>
                    <tr>
                        <td height="28px" valign="bottom">
                             <asp:LinkButton ID="InsertButton" runat="server" CausesValidation="True" 
                            CommandName="Insert" Text="Insert" />
                            &nbsp;<asp:LinkButton ID="InsertCancelButton" runat="server" 
                            CausesValidation="False" CommandName="Cancel" Text="Cancel" />
                        </td>
                    </tr>
                </table>              
            </InsertItemTemplate>
            <ItemTemplate>
                <table style="width: 400px;">                
                    <tr>
                        <td style="width: 128px; height: 128px;" rowspan="2">
                            <asp:Image ID="imgPicture" runat="server" 
                            ImageUrl ='<%# Eval("PersonID","Image.aspx?PersonID={0}") %>' Width="128px" 
                            Height="128px" /> 
                        </td>
                        <td valign="top">
                            Person ID:
                            <asp:Label ID="lblPersonID" runat="server" Text='<%# Eval("PersonID") %>' />
                            <br />
                            First name:
                            <asp:Label ID="lblFirstName" runat="server" Text='<%# Eval("FirstName") %>' />
                            <br />
                            Last name:
                            <asp:Label ID="lblLastName" runat="server" Text='<%# Eval("LastName") %>' />                            
                        </td>
                    </tr>
                    <tr>
                        <td valign="bottom">
                            <asp:LinkButton ID="EditButton" runat="server" CausesValidation="False" 
                            CommandName="Edit" Text="Edit" />
                            &nbsp;<asp:LinkButton ID="DeleteButton" runat="server" CausesValidation="False" 
                            CommandName="Delete" Text="Delete" />
                            &nbsp;<asp:LinkButton ID="NewButton" runat="server" CausesValidation="False" 
                            CommandName="New" Text="New" />
                        </td>
                    </tr>                     
                </table>             
            </ItemTemplate>            
        </asp:FormView>    
    </div>
    </form>
</body>
</html>
