<%@ Page EnableSessionState="False" EnableViewState="False" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.IO" %>

<script language="C#" runat="server">

    String width = "100%";
    String path;
    String font;

    void Page_Load(Object sender, EventArgs e) {

        // Handle .src Path
        
        path = Request.QueryString["path"];
        PopulateNavigationList(path);
    }

    void PopulateNavigationList(String path) {

        FileStream fs = new FileStream(Server.MapPath(path), FileMode.Open, FileAccess.Read);
        StreamReader sr = new StreamReader(fs);
        String fileName;

        char [] c1 = { ':' };
        char [] c2 = { ',' };
        String line;

        while ( (line = sr.ReadLine()) != null ) {

            String [] list = line.Split(c1);

            if (list.Length > 1) {

                TableRow row = new TableRow();

                // Construct Group Label for Items

                String sourcegroup = list[0];

                TableCell groupCell = new TableCell();
                groupCell.Text = "<nobr>" + sourcegroup + ": </nobr>";
                groupCell.CssClass ="SourceListBold";
                row.Cells.Add(groupCell);


                // Construct List of Items

                String [] sourcelist = list[1].Split(c2);

                if ((sourcelist.Length>0)) {
                    fileName = Server.MapPath("~/docs/" + ParseItem(sourcelist[0]).HRef);
                }

                StringBuilder sb = new StringBuilder();

                for (int i=0; i< sourcelist.Length; i++) {

                    ItemDetails item = ParseItem(sourcelist[i]);
                    sb.Append("<a class='white' target=docFrame href='" + Request.ApplicationPath + "/docs/" + item.HRef + "'>");
                    sb.Append(item.Description);
                    sb.Append("</a>&nbsp;&nbsp; ");
                }

                TableCell itemCell = new TableCell();
                itemCell.Text = sb.ToString();
                itemCell.CssClass ="SourceList";
                row.Cells.Add(itemCell);

                SourceTable.Rows.Add(row);
            }
        }

        fs.Close();
    }

    public ItemDetails ParseItem(String item) {

        char [] c3 = { '|' };

        String [] itemParts = item.Split(c3);

        if (itemParts == null) {
            return null;
        }

        ItemDetails itemDetails = new ItemDetails();

        if (itemParts.Length > 1) {
            itemDetails.Description = itemParts[0];
            itemDetails.HRef = itemParts[1];
        }
        else {
            itemDetails.Description = itemParts[0];
            itemDetails.HRef = itemParts[0];
        }

        return itemDetails;
    }

    public class ItemDetails {
        public String Description;
        public String HRef;
    }

</script>
<html>
    <head>
        <link rel="stylesheet" href="style.css">
    </head>
    <body leftmargin="0" bottommargin="0" rightmargin="0" topmargin="0" marginheight="0" marginwidth="0">
        <form runat="server">
            <table width="100%" cellspacing="0" cellpadding="0" class="SourceViewHeaderBg" border="0">
                <tr valign="top">
                    <td>
                        <table cellpadding="5" cellspacing="0" background="grid_background.gif" width="100%">
                            <tr>
                                <td class="TitleBar">
                                    <span id="Title" class="SourceViewTitle" runat="server">IBuySpy Store Source Code Viewer</span>
                                </td>
                            </tr>
                        </table>
                        <asp:Table id="SourceTable" cellpadding="2" cellspacing="2" EnableViewState="true" runat="server" />
                    </td>
                </tr>
            </table>
        </form>
    </body>
</html>
