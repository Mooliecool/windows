<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="CommonEncryption.aspx.cs"
    Inherits="CSASPNETEncryptAndDecryptConfiguration.CommonEncryption" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <table style="font-family: Calibri; width: 100%">
            <tr>
                <td>
                    <asp:Label ID="lbwords" runat="server" Text="Enter Some Words:"></asp:Label>
                </td>
                <td colspan="3">
                    <asp:TextBox ID="tbData" runat="server" Width="300px"></asp:TextBox>
                </td>
            </tr>
            <tr>
                <td>
                    <asp:Label ID="lbEncryptData" runat="server" Text="Encrypted Data:"></asp:Label>
                </td>
                <td>
                    <asp:Button ID="btnEncrypt" runat="server" Text="Encrypt it" OnClick="Encrypt_Click"
                        Height="30px" Width="90px" />
                    <br />
                    <asp:TextBox ID="tbEncryptData" runat="server" TextMode="MultiLine" Height="200px"
                        Width="400px" ReadOnly="true"></asp:TextBox>
                    <br />
                </td>
                <td>
                    <asp:Label ID="lbDecryptedData" runat="server" Text="Decrypted Data:"></asp:Label>
                </td>
                <td>
                    <asp:Button ID="btnDecrypt" runat="server" Text="Decrypt it" OnClick="Decrypt_Click"
                        Height="30px" Width="90px" />
                    <br />
                    <asp:TextBox ID="tbDecryptData" runat="server" TextMode="MultiLine" Height="200px"
                        Width="400px" ReadOnly="true"></asp:TextBox>
                    <br />
                </td>
            </tr>
        </table>
    </div>
    </form>
</body>
</html>
