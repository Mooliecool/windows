using System;
using System.Web.UI.Design;
using System.IO;
using System.Web.UI;

namespace AspNetForums.Controls.Design {

    public class ForumRepeaterDesigner : ControlDesigner {
    
        protected override string GetEmptyDesignTimeHtml() {
            return GetDesignTimeHtml();
        }

        public override string GetDesignTimeHtml() {

            StringWriter sw = new StringWriter();
            HtmlTextWriter writer = new HtmlTextWriter(sw);

            writer.Write("<table bgcolor=silver width=150px cellpadding=2 cellspacing=0 border=0>");
            writer.Write("<tr><td valign='top'>");
            writer.Write("<div>" + this.Component.ToString() + "<br>ASP.NET Server Control</div>");
            writer.Write("</td><tr>");
            writer.Write("<tr><td valign='top'>");
            writer.Write("<div>http://www.asp.net/forums</div>");
            writer.Write("</td><tr>");
            writer.Write("</td></tr></table>");

            return sw.ToString();

        }
    }
}
