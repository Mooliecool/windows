/****************************** Module Header ******************************\
* Module Name:  CalendarCellControl
* Project:      CSASPNETAjaxScriptControl
* Copyright (c) Microsoft Corporation.
* 
* CellContainer of Schedule Template
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 2009/10/22 5:00 PM Vince Xu Created
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Web.UI;
namespace CSASPNETAjaxScriptControl
{
    public class CalendarCellControl : Table,INamingContainer
    {  
        public CalendarCellControl(){ }
        private TableCell _header,_footer;
        private Panel _content;
        public virtual TableCell Header
        {
            get{
                if (_header == null) 
                    _header = new TableCell();
                return _header;
            }
        }
        public virtual Panel Content
        {
            get{
                if (_content == null) 
                    _content = new Panel();
                return _content;
            }
        }
        public virtual TableCell Footer
        {
            get{
                if (_footer == null) 
                    _footer = new TableCell();
                return _footer;
            }
        }
        public override string CssClass
        {
            get{return base.CssClass;}
            set{base.CssClass = value;}
        }
        private DateTime _relatedDate;
        public DateTime RelatedDate
        {
            get{
                if (_relatedDate == null || _relatedDate == Convert.ToDateTime(null))
                    _relatedDate = DateTime.Now;
                return _relatedDate;
            }
            set{
                _relatedDate = value;
            }
        }
        protected override void CreateChildControls()
        {
            this.Controls.Clear();
            TableRow tr_header = new TableRow();
            tr_header.Cells.Add(Header);
            this.Controls.Add(tr_header);
            TableRow tr_content = new TableRow();
            TableCell td_content = new TableCell();
            td_content.Controls.Add(Content);
            tr_content.Cells.Add(td_content);
            this.Controls.Add(tr_content);
            base.CreateChildControls();
        }
        protected override void AddAttributesToRender(System.Web.UI.HtmlTextWriter writer)
        {
            base.AddAttributesToRender(writer);
            Header.CssClass = Header.CssClass == "" ? "calendarCellHeader" : Header.CssClass;
            Content.CssClass = Content.CssClass == "" ? "calendarCellContent" : Content.CssClass;
            Footer.CssClass = Footer.CssClass == "" ? "calendarCellFooter" : Footer.CssClass;
        }
        public virtual DropPanel GenerateDropPanel(string id, DateTime date,string title, string description)
        {
            DropPanel dropPanel = new DropPanel();
            dropPanel.ID = "dropPanel"+id;
            dropPanel.FromContextKey = new { key = id, datetime = date.ToString("yyyy-M-d") };
            dropPanel.HeaderText = title;
            dropPanel.ToolTip = title;
            dropPanel.ContentText = description;
            Content.Controls.Add(dropPanel);
            return dropPanel;
        }
        protected override void OnPreRender(EventArgs e)
        {
            base.OnPreRender(e);
            this.CellPadding = 0;
            this.CellSpacing = 0;
            this.CssClass = this.CssClass == "" ? "calendarCell" : this.CssClass;
            LiteralControl lc = new LiteralControl(RelatedDate.Day.ToString());
            Header.Controls.Add(lc);
        }
        protected override void Render(System.Web.UI.HtmlTextWriter writer)
        {
            base.Render(writer);
        }
    }
}
