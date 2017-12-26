<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="CSExcelLikeGridView._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>CSExcelLikeGridView</title>
    <script src="http://ajax.microsoft.com/ajax/jquery/jquery-1.4.4.min.js" type="text/javascript">
    </script>
    <script type="text/javascript">

        //Function to read HidState (JSON) to keep the state color
        function ResetColors(color1, color2) {
            var contents = $(":hidden:last").val();

            // If not "[]", meaning something changed, reset colors.
            if (contents.toString() != "[]") {
                // Convert to JSON object.
                var objectc = eval(contents);

                // To reset colors depending on whether it's changed
                for (var i = 0; i < objectc.length; ++i) {
                    if (objectc[i].Color != '') {
                        $("td:eq(" + objectc[i].Index + ")").css("background-color", objectc[i].Color);
                    }

                    var checked = objectc[i].Deleted == "True";

                    if (parseInt(objectc[i].Index) % 2 == 0) {
                        $("td:eq(" + parseInt(objectc[i].Index - 2) + ")").find(":checkbox").attr("checked", checked);
                    }
                    else {
                        $("td:eq(" + parseInt(objectc[i].Index - 3) + ")").find(":checkbox").attr("checked", checked);
                    }

                    // If a checkbox checked, change the row color
                    if (checked) {
                        $("td:eq(" + objectc[i].Index + ")").parent().css("background-color", "red");
                    }
                }
            }

        }

        //Validation for Save
        function SaveValidate() {
            //first tell whether you've missed "Name" to be ful-filled...

            if (Page_ClientValidate('Fill')) {
                return confirm('Do you really want to save all these changes together?');
            }
            else {
                alert("Attention! You cannot leave a name blank!");
            }
        }

        //Validation for Insert
        function InsertValidate() {
            //first tell whether you've missed "Name" to be ful-filled...

            if (!Page_ClientValidate('Insert')) {
                alert("Attention! You cannot insert a blank name!");
            }
        }

        // Add dynamically events for all textboxes 
        // except the footer one to turn the background color.
        function AddEvents() {

            var rowarray = $("tr");
            for (var i = 0; i < rowarray.length - 1; ++i) {
                $(rowarray[i]).find(":text").change(function () {
                    $(this).parent().css("background", "blue");
                });
            }

        }

        $(function () {

            //Keep the original color row for odd
            var color1 = $("tr:eq(1)").css("background-color").valueOf();
            var color2 = $("tr:eq(2)").css("background-color").valueOf();
            var headercolor = $("tr:first").css("background-color").valueOf();
            var footercolor = $("tr:last").css("background-color").valueOf();

            AddEvents();

            // Header checkbox's cascading effect:
            $("#chkAll").click(function () {

                $(":checkbox").attr("checked", $(this).attr("checked"));

                if ($(this).attr("checked")) {
                    $(":checkbox").parent().parent().css("background-color", "red");
                    //Reset the color of header
                    $("tr:first").css("background-color", headercolor);
                }
                else {
                    $("tr:odd").css("background-color", color1);
                    $("tr:even").css("background-color", color2);

                    //Reset the color of header and footer
                    $("tr:first").css("background-color", headercolor);
                    $("tr:last").css("background-color", footercolor);
                }
            });

            //Single checkbox checked event
            $(":checkbox").click(function () {
                if ($(this).attr("checked")) {
                    $(this).parent().parent().css("background-color", "red");
                }
                else {
                    if ($(this).parent().parent().index() % 2 == 0) {
                        $(this).parent().parent().css("background-color", color2);
                    }
                    else {
                        $(this).parent().parent().css("background-color", color1);
                    }
                }

                //Reset the header color
                $("tr:first").css("background-color", headercolor);
            });

            ResetColors(color1, color2);
        })

    </script>
</head>
<body>
    <form id="form1" runat="server">
    <h1>
        Demo for Batching Actions</h1>
    <span style="color: Red">red row to be deleted</span>
    <br />
    <span style="color: green">green row to be added</span>
    <br />
    <span style="color: blue">blue cell to be modified</span>
    <br />
    <hr />
    <div>
        <asp:GridView ID="GridView1" runat="server" Width="70%" Height="50%" AutoGenerateColumns="False"
            CellPadding="4" ForeColor="#333333" GridLines="None" ShowFooter="True">
            <AlternatingRowStyle BackColor="White" />
            <Columns>
                <asp:TemplateField HeaderText="Delete State">
                    <HeaderTemplate>
                        <input id="chkAll" type="checkbox" />
                        Delete
                    </HeaderTemplate>
                    <ItemTemplate>
                        <asp:CheckBox ID="chkDelete" runat="server" />
                    </ItemTemplate>
                </asp:TemplateField>
                <asp:TemplateField HeaderText="Id">
                    <ItemTemplate>
                        <%#Eval("Id") %>
                    </ItemTemplate>
                    <FooterTemplate>
                        Name:<asp:TextBox ID="tbNewName" runat="server"></asp:TextBox>
                        <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" ControlToValidate="tbNewName"
                            ErrorMessage="You cannot insert a blank name!" ForeColor="#FFFF66" ValidationGroup="Insert"></asp:RequiredFieldValidator>
                    </FooterTemplate>
                </asp:TemplateField>
                <asp:TemplateField HeaderText="Name">
                    <ItemTemplate>
                        <asp:TextBox ID="tbName" runat="server" Text='<%#Eval("PersonName") %>'>
                        </asp:TextBox>
                        <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" ControlToValidate="tbName"
                            ErrorMessage="You cannot leave a name blank!" ValidationGroup="Fill"></asp:RequiredFieldValidator>
                    </ItemTemplate>
                    <FooterTemplate>
                        Address:<asp:TextBox ID="tbNewAddress" runat="server"></asp:TextBox>
                    </FooterTemplate>
                </asp:TemplateField>
                <asp:TemplateField HeaderText="Address">
                    <ItemTemplate>
                        <asp:TextBox ID="tbAddress" runat="server" Text='<%#Eval("PersonAddress") %>'>
                        </asp:TextBox>
                    </ItemTemplate>
                    <FooterTemplate>
                        <asp:Button ID="btnAdd" runat="server" Text="Add a new row" OnClick="btnAdd_Click"
                            ValidationGroup="Insert" OnClientClick="InsertValidate()" />
                    </FooterTemplate>
                </asp:TemplateField>
            </Columns>
            <FooterStyle BackColor="#990000" Font-Bold="True" ForeColor="White" HorizontalAlign="Center"
                VerticalAlign="Middle" />
            <HeaderStyle BackColor="#990000" Font-Bold="True" ForeColor="White" HorizontalAlign="Center"
                VerticalAlign="Middle" />
            <PagerStyle BackColor="#FFCC66" ForeColor="#333333" HorizontalAlign="Center" />
            <RowStyle BackColor="#FFFBD6" ForeColor="#333333" HorizontalAlign="Center" VerticalAlign="Middle" />
            <SelectedRowStyle BackColor="#FFCC66" Font-Bold="True" ForeColor="Navy" />
        </asp:GridView>
    </div>
    <asp:Button ID="btnSaveAll" runat="server" Height="30px" Text="Save All Changes"
        Width="149px" OnClick="btnSaveAll_Click" OnClientClick="SaveValidate()" ValidationGroup="Fill" />
    <asp:HiddenField ID="HidState" runat="server" Value="[]" />
    </form>
</body>
</html>
