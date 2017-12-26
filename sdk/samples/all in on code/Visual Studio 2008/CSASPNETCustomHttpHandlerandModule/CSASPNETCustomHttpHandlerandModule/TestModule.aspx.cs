using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace CSASPNETCustomHttpHandlerandModule
{
    public partial class TestModule : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            Response.Write("<h1><font color=green>This is wrote in .aspx page's PageLoad event. </font></h1>");
        }
    }
}
