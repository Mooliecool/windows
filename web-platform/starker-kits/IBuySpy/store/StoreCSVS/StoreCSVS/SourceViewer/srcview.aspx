<%@ Page %>
<script language="C#" runat=server>

    void Page_Load(Object sender, EventArgs e) {

        int numRows = 4;

        if (Request.QueryString["rows"] != null) {
            numRows = Int32.Parse(Request.QueryString["rows"]);
        }

        int tabHeight = (numRows * 22) + 40;

        Frameset.Attributes["rows"] = "" + tabHeight + ",*";

        srcFrame.Attributes["src"] = "tabview.aspx?path=" + Request.QueryString["path"];

        String newPath = Request.QueryString["file"].Replace('/', '_');
        newPath = Request.ApplicationPath + "/docs/" + newPath.Replace('.', '_') + ".htm";
        docFrame.Attributes["src"] = newPath;
    }

</script>

<frameset id="Frameset" BORDER=0 ROWS="130,*" HEIGHT=100% FRAMEBORDER=0 FRAMESPACING=0 runat=server>
    <frame id="srcFrame"  NORESIZE SRC="tabview.aspx" runat=server/>
    <frame id="docFrame" name="docFrame" height="100%" runat=server/>
</frameset>