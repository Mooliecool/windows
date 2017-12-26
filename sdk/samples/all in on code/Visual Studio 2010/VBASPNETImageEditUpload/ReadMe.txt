=============================================================================
         ASP.NET APPLICATION : VBASPNETImageEditUpload Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

  The project demonstrates how to insert,edit and update a database with an 
  Image type field.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Create a VB ASP.NET Web Application in Visual Studio 2010 /
Visual Web Developer 2010 and name it as VBASPNETImageEditUpload.

[ NOTE: You can download the free Web Developer from:
 http://www.microsoft.com/express/Web/ ]

[ NOTE2: You can also download the free Sql2008 from:
 http://www.microsoft.com/express/Database/ ]

Step2. Delete the following default folders and files created automatically 
by Visual Studio.

    Account folder
    Script folder
    Style folder
    About.aspx file
    Default.aspx file
    Global.asax file
    Site.Master file

Step3. Create a folder called "DefaultImage", where you can put an image
as a default photo when no picture for a new comer's registration or update.

Step4. Create another folder called "ImageHandler", this will read out byte
collection to show a picture stored in the specific datatable "db_Persons".
Your codes should be something like this:

 Public Class ImageHandler
    Implements System.Web.IHttpHandler

    Sub ProcessRequest(ByVal context As HttpContext) Implements IHttpHandler.ProcessRequest
        Using cmd As New SqlCommand()
            cmd.Connection = New SqlConnection(
                ConfigurationManager.ConnectionStrings("db_PersonsConnectionString").ConnectionString)
            cmd.Connection.Open()
            cmd.CommandText = "select PersonImage,PersonImageType from tb_personInfo where id=" +
                context.Request.QueryString("id")

            Dim reader As SqlDataReader = cmd.ExecuteReader(CommandBehavior.CloseConnection _
                                                            Or CommandBehavior.SingleRow)
            If (reader.Read) Then
                Dim imgbytes() As Byte = Nothing
                Dim imgtype As String = Nothing

                If (reader.GetValue(0) IsNot DBNull.Value) Then
                    imgbytes = CType(reader.GetValue(0), Byte())
                    imgtype = reader.GetString(1)
                Else
                    imgbytes = File.ReadAllBytes(
                        context.Server.MapPath("~/DefaultImage/DefaultImage.JPG"))
                    imgtype = "image/pjpeg"
                End If
                context.Response.ContentType = imgtype
                context.Response.BinaryWrite(imgbytes)
            End If

            reader.Close()
            context.Response.End()
        End Using
    End Sub

    ReadOnly Property IsReusable() As Boolean Implements IHttpHandler.IsReusable
        Get
            Return False
        End Get
    End Property
End Class

Step5: Drag and drop a GridView control inside the Default.aspx page, and bind
it with SqlDataSource. Your codes finally look like this:

    <asp:SqlDataSource ID="SqlDSPersonOverView" runat="server" 
        ConnectionString="<%$ ConnectionStrings:db_PersonsConnectionString %>" 
        SelectCommand="SELECT [Id], [PersonName] FROM [tb_personInfo]">
    </asp:SqlDataSource>
    <asp:GridView ID="gvPersonOverView" runat="server" CellPadding="4" 
        EnableModelValidation="True" ForeColor="#333333" GridLines="None" Width="70%" 
        AutoGenerateColumns="False" DataKeyNames="Id" DataSourceID="SqlDSPersonOverView" 
        onselectedindexchanged="gvPersonOverView_SelectedIndexChanged">
        <AlternatingRowStyle BackColor="White" ForeColor="#284775" />
        <Columns>
            <asp:BoundField DataField="Id" HeaderText="Id" InsertVisible="False"
                ReadOnly="True" SortExpression="Id" />
            <asp:BoundField DataField="PersonName" HeaderText="PersonName" 
                SortExpression="PersonName" />
            <asp:CommandField ShowSelectButton="True" HeaderText="Click to see Details" 
                SelectText="Details..." />
        </Columns>
        <EditRowStyle BackColor="#999999" />
        <EmptyDataTemplate>
            No Data Available, Please Insert data with the help of the FormView...<br />
        </EmptyDataTemplate>
        <FooterStyle BackColor="#5D7B9D" Font-Bold="True" ForeColor="White" />
        <HeaderStyle BackColor="#5D7B9D" Font-Bold="True" ForeColor="White" />
        <PagerStyle BackColor="#284775" ForeColor="White" HorizontalAlign="Center" />
        <RowStyle BackColor="#F7F6F3" ForeColor="#333333" HorizontalAlign="Center"
            VerticalAlign="Middle" />
        <SelectedRowStyle BackColor="#E2DED6" Font-Bold="True" ForeColor="#333333" />
    </asp:GridView>

[NOTE] You can:
1) Drag and drop a SqlDataSource from "Data" panel, or just click the right
arrow of the GridView,choose SqlDataSource as datasource for GridView.
2) Then according to the Wizard, bind the database "db_Persons" to the SqlDataSource.
3) At last bind the SqlDataSource to the GridView.
4) Modify your Sql connection string in the web.config, "connectionStrings" node
if necessary (If you do not know how to write the string, please switch to the SqlDataSource
and see the property panel, copy the ConnectionString there).

Step6: Drag and drop FormView onto the Default.aspx page, bind it with another SqlDataSource
with the same table as mentioned below, modify it as this:

    <asp:FormView ID="fvPersonDetails" runat="server" Width="50%"
        DataSourceID="SqlDSPersonDetails" EnableModelValidation="True" DataKeyNames="Id"
        DataMember="DefaultView" OnItemInserting="fvPersonDetails_ItemInserting" 
        onitemupdating="fvPersonDetails_ItemUpdating" BackColor="#DEBA84" 
        BorderColor="#DEBA84" BorderStyle="None" BorderWidth="1px" CellPadding="3" 
        CellSpacing="2" GridLines="Both" 
        onitemupdated="fvPersonDetails_ItemUpdated" 
        onitemdeleted="fvPersonDetails_ItemDeleted" 
        onitemdeleting="fvPersonDetails_ItemDeleting" 
        oniteminserted="fvPersonDetails_ItemInserted" 
        onmodechanging="fvPersonDetails_ModeChanging">
        <ItemTemplate>
            <table width="100%">
                <tr>
                    <th>
                        Person Name:
                    </th>
                    <td colspan="2">
                        <%#Eval("PersonName") %>
                    </td>
                </tr>
                <tr>
                    <th>
                        Person Image:
                    </th>
                    <td colspan="2">
                        <img src='ImageHandler/ImageHandler.ashx?id=<%#Eval("Id") %>' width="200" alt=""
                            height="200" />
                    </td>
                </tr>
                <tr>
                    <td align="center">
                        <asp:LinkButton ID="lnkEdit" runat="server" CommandName="Edit" Text="Edit" />
                    </td>
                    <td align="center">
                        <asp:LinkButton ID="lnkDelete" runat="server" CommandName="Delete" Text="Delete" 
                        OnClientClick="return confirm('Are you sure to delete it completely?');" />
                    </td>
                    <td align="center">
                        <asp:LinkButton ID="lnkNew" runat="server" CommandName="New" Text="New" />
                    </td>
                </tr>
            </table>
        </ItemTemplate>
        <EditItemTemplate>
            <table width="100%">
                <tr>
                    <th>
                        Person Name:
                    </th>
                    <td>
                        <asp:TextBox ID="txtName" runat="server" Text=' <%#Bind("PersonName") %>' 
                        MaxLength="20" />
                        <asp:RequiredFieldValidator ID="reqName" runat="server" 
                            ControlToValidate="txtName" ErrorMessage="Name is required!">
                            *
                            </asp:RequiredFieldValidator>
                    </td>
                </tr>
                <tr>
                    <th>
                        Person Image:
                    </th>
                    <td>
                        <asp:FileUpload ID="fupEditImage" runat="server" />
                        <asp:CustomValidator ID="cmvImageType" runat="server" 
                            ControlToValidate="fupEditImage" ErrorMessage="File is invalid!" 
                            OnServerValidate="CustomValidator1_ServerValidate">
                            </asp:CustomValidator>
                    </td>
                </tr>
                <tr>
                    <td align="center">
                        <asp:LinkButton ID="lnkUpdate" runat="server" CommandName="Update" 
                        Text="Update" />
                    </td>
                    <td align="center">
                        <asp:LinkButton ID="lnkCancel" runat="server" CommandName="Cancel" 
                            Text="Cancel" CausesValidation="False" />
                    </td>
                </tr>
            </table>
        </EditItemTemplate>
        <EditRowStyle BackColor="#738A9C" Font-Bold="True" ForeColor="White" />
        <FooterStyle BackColor="#F7DFB5" ForeColor="#8C4510" />
        <HeaderStyle BackColor="#A55129" Font-Bold="True" ForeColor="White" />
        <InsertItemTemplate>
            <table width="100%">
                <tr>
                    <th>
                        Person Name:
                    </th>
                    <td>
                        <asp:TextBox ID="txtName" runat="server" MaxLength="20" 
                        Text='<%#Bind("PersonName") %>'></asp:TextBox>
                        <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 
                        ControlToValidate="txtName" ErrorMessage="Name is required!">
                        *
                        </asp:RequiredFieldValidator>
                    </td>
                </tr>
                <tr>
                    <th>
                        Person Image:
                    </th>
                    <td>
                        <asp:FileUpload ID="fupInsertImage" runat="server" />
                        <asp:CustomValidator ID="cmvImageType" runat="server" 
                        ControlToValidate="fupInsertImage" ErrorMessage="File is invalid!" 
                        OnServerValidate="CustomValidator1_ServerValidate">
                        </asp:CustomValidator>
                    </td>
                </tr>
                <tr>
                    <td align="center">
                        <asp:LinkButton ID="lnkInsert" runat="server" CommandName="Insert"
                         Text="Insert" />
                    </td>
                    <td align="center">
                        <asp:LinkButton ID="lnkInsertCancel" runat="server" CommandName="Cancel" 
                            Text="Cancel" CausesValidation="False" />
                    </td>
                </tr>
            </table>
        </InsertItemTemplate>
        <PagerStyle HorizontalAlign="Center" ForeColor="#8C4510" />
        <RowStyle BackColor="#FFF7E7" ForeColor="#8C4510" />
    </asp:FormView>

    <asp:SqlDataSource ID="SqlDSPersonDetails" runat="server" 
    ConnectionString="<%$ ConnectionStrings:db_PersonsConnectionString %>"
        DeleteCommand="DELETE FROM tb_personInfo WHERE (Id = @Id)" 
        InsertCommand="INSERT INTO tb_personInfo(PersonName, PersonImage, PersonImageType) 
        VALUES (@PersonName, @PersonImage, @PersonImageType)"
        SelectCommand="SELECT [Id], [PersonName] FROM [tb_personInfo] where id=@id" 
        UpdateCommand="UPDATE tb_personInfo SET PersonName = @PersonName,
         PersonImage = @PersonImage, PersonImageType = @PersonImageType WHERE (Id = @Id)">
        <DeleteParameters>
            <asp:Parameter Name="Id" Type="Int32" />
        </DeleteParameters>
        <InsertParameters>
            <asp:Parameter Name="PersonName" Type="String" />
            <asp:Parameter Name="PersonImage" DbType="Binary" ConvertEmptyStringToNull="true" />
            <asp:Parameter Name="PersonImageType" Type="String" ConvertEmptyStringToNull="true" />
        </InsertParameters>
        <UpdateParameters>
            <asp:Parameter Name="PersonName" Type="String" />
            <asp:Parameter Name="PersonImage" DbType="Binary" ConvertEmptyStringToNull="true" />
            <asp:Parameter Name="PersonImageType" Type="String" ConvertEmptyStringToNull="true" />
            <asp:Parameter Name="Id" Type="Int32" />
        </UpdateParameters>
        <SelectParameters>
            <asp:ControlParameter Name="id" Type="Int32" ControlID="gvPersonOverView" 
            PropertyName="SelectedValue" DefaultValue="0" />
        </SelectParameters>
    </asp:SqlDataSource>

Step7: Switch to design mode, double click the blank place, in the Page_Load event
please write this following:

    Partial Public Class _Default
        Inherits System.Web.UI.Page

        ''' <summary>
        ''' Static types of common images for checking.
        ''' </summary>
        Private Shared imgytpes As New List(Of String)() From { _
         ".BMP", _
         ".GIF", _
         ".JPG", _
         ".PNG" _
        }

        Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
            If Not IsPostBack Then
                gvPersonOverView.DataBind()
                If gvPersonOverView.Rows.Count > 0 Then
                    gvPersonOverView.SelectedIndex = 0
                    fvPersonDetails.ChangeMode(FormViewMode.[ReadOnly])
                    fvPersonDetails.DefaultMode = FormViewMode.[ReadOnly]
                Else
                    fvPersonDetails.ChangeMode(FormViewMode.Insert)
                    fvPersonDetails.DefaultMode = FormViewMode.Insert
                End If
            End If
        End Sub
    End Class
  
Step8: Now you can turn to gridview, switch to the event panel and double click 
the event name "SelectIndexChanged", and say like this:

    Protected Sub gvPersonOverView_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs)
        fvPersonDetails.ChangeMode(FormViewMode.[ReadOnly])
        fvPersonDetails.DefaultMode = FormViewMode.[ReadOnly]
    End Sub

[Note] Your other controls' events can be added like Step9, for detailed codes 
and information, please see the Default.aspx.cs (Open by clicking the symbol 
"+" to expand the Default.aspx page).


/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET QuickStart Torturial:
http://quickstarts.asp.net/QuickStartv20/aspnet/doc/ctrlref/data/gridview.aspx

MSDN: Serving Dynamic Content with HTTP Handlers
http://msdn.microsoft.com/en-us/library/ms972953.aspx


/////////////////////////////////////////////////////////////////////////////