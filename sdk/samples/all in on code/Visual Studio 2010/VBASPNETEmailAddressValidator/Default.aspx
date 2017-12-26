<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="Default.aspx.vb" Inherits="VBASPNETEmailAddressValidator._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>VBASPNETEmailAddressValidator</title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <asp:Wizard ID="ValidationWizard" runat="server" Width="700px" ActiveStepIndex="0"
            BackColor="#F7F6F3" BorderColor="#CCCCCC" BorderStyle="Solid" BorderWidth="1px"
            Font-Names="Verdana" Font-Size="0.8em" OnNextButtonClick="ValidationWizard_OnNextButtonClick">
            <HeaderStyle BackColor="#5D7B9D" BorderStyle="Solid" Font-Bold="True" Font-Size="0.9em"
                ForeColor="White" HorizontalAlign="Left" />
            <HeaderTemplate>
                <h2>
                    Validate E-Mail Address By Validation Mail</h2>
            </HeaderTemplate>
            <NavigationButtonStyle BackColor="#FFFBFF" BorderColor="#CCCCCC" BorderStyle="Solid"
                BorderWidth="1px" Font-Names="Verdana" Font-Size="0.8em" ForeColor="#284775" />
            <SideBarButtonStyle BorderWidth="0px" Font-Names="Verdana" ForeColor="White" />
            <SideBarStyle BackColor="#7C6F57" BorderWidth="0px" Font-Size="0.9em" VerticalAlign="Top" />
            <StepStyle BorderWidth="0px" ForeColor="#5D7B9D" />
            <WizardSteps>
                <asp:WizardStep ID="WizardStep1" runat="server" Title="Login SMTP Server">
                    <table>
                        <tr>
                            <td align="right" style="width: 200px;">
                                Send Email Host:
                            </td>
                            <td style="width: 300px;">
                                <asp:TextBox ID="tbHost" runat="server" Text="smtp.live.com"></asp:TextBox>
                            </td>
                        </tr>
                        <tr>
                            <td align="right" style="width: 200px;">
                                Send Email Address:
                            </td>
                            <td style="width: 300px;">
                                <asp:TextBox ID="tbSendMail" runat="server" Text="@hotmail.com"></asp:TextBox>
                            </td>
                        </tr>
                        <tr>
                            <td align="right">
                                Send Email Password:
                            </td>
                            <td>
                                <asp:TextBox ID="tbSendMailPassword" TextMode="Password" runat="server"></asp:TextBox>
                            </td>
                        </tr>
                        <tr>
                            <td align="right">
                                Use SSL:
                            </td>
                            <td>
                                <asp:CheckBox ID="chkUseSSL" Checked="true" runat="server" />
                            </td>
                        </tr>
                        <tr>
                            <td colspan="2">
                                Description:<br />
                                This is the SMTP server and your email address which you will used to send the validation
                                email.
                            </td>
                        </tr>
                    </table>
                </asp:WizardStep>
                <asp:WizardStep ID="WizardStep2" runat="server" Title="Validation">
                    <table width="100%">
                        <tr>
                            <td>
                                <asp:Label runat="server" ID="lbMessage"></asp:Label>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                The Email address which you want to validate:<br />
                                <asp:TextBox runat="server" ID="tbValidateEmail" Width="100%"></asp:TextBox>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <asp:Button runat="server" ID="btnValidate" Text="Start to Validate the E-Mail Address"
                                    OnClick="btnValidate_Click" />
                                <asp:Button runat="server" ID="btnSendAgain" Visible="false" Text="Send E-Mail Address again"
                                    OnClick="btnSendEmailAgain_Click" />
                            </td>
                        </tr>
                    </table>
                </asp:WizardStep>
            </WizardSteps>
        </asp:Wizard>
    </div>
    </form>
</body>
</html>
