/****************************** Module Header ******************************\
* Module Name:  Schedule
* Project:      CSASPNETAjaxScriptControl
* Copyright (c) Microsoft Corporation.
* Schedule Control Class
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* History:
* * 2009/10/22 5:00 PM Vince Xu Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.ComponentModel;
using System.Data;
using System.Collections;
[assembly: WebResource("CSASPNETAjaxScriptControl.Common.jquery-1.0.js", "application/x-javascript")]
[assembly: WebResource("CSASPNETAjaxScriptControl.Schedule.Schedule.js", "application/x-javascript")]
[assembly: WebResource("CSASPNETAjaxScriptControl.Schedule.Schedule.css", "text/css", PerformSubstitution = true)]
[assembly: WebResource("CSASPNETAjaxScriptControl.Schedule.backgroundImage.png", "image/png")]
[assembly: WebResource("CSASPNETAjaxScriptControl.Schedule.DropPanel_Skin.header.png", "image/png")]
[assembly: WebResource("CSASPNETAjaxScriptControl.Schedule.DropPanel_Skin.content.png", "image/png")]
[assembly: WebResource("CSASPNETAjaxScriptControl.Schedule.DropPanel_Skin.loading.gif", "image/gif")]
namespace CSASPNETAjaxScriptControl
{
    [ToolboxData("<{0}:Schedule runat=\"server\"></{0}:Schedule>")]
    public class Schedule : DataBoundControl, IScriptControl,INamingContainer 
    {
        public Schedule(){ }
        private DateTime firstDayOfTargetDateTime;
        private LiteralControl CalendarTitle {get;set;}
        private List<CalendarCellControl> CalendarCellControlList = new List<CalendarCellControl>();
        private List<DropPanel> _DropPanelClientIDCollection = new List<DropPanel>();
        int columnNum = 7, rowNum = 5;
        public IEnumerable<ScriptDescriptor> GetScriptDescriptors()
        {
            ScriptControlDescriptor descriptor = new ScriptControlDescriptor("CSASPNETAjaxScriptControl.Schedule", this.ClientID);
            descriptor.AddElementProperty("toolContainer", ToolContainer.ClientID);
            descriptor.AddElementProperty("dateTimePicker", DateTimePicker.ClientID);
            descriptor.AddElementProperty("calendarContainer", CalendarContainer.ClientID);
            descriptor.AddProperty("calendarCellContentCssClass", CalendarCellContentCssClass);
            if (string.IsNullOrEmpty(ServicePath))  throw new Exception("Please set ServicePath property.");
            descriptor.AddProperty("servicePath", ServicePath);
            if (string.IsNullOrEmpty(UpdateServiceMethod)) throw new Exception("Please set UpdateServiceMethod property.");
            descriptor.AddProperty("updateServiceMethod", UpdateServiceMethod);
            if (string.IsNullOrEmpty(DeleteServiceMethod)) throw new Exception("Please set DeleteServiceMethod property.");
            descriptor.AddProperty("deleteServiceMethod", DeleteServiceMethod);        
            List<string> aa = new List<string>();
            for (int i = 0; i < DropPanelClientIDCollection.Count; i++)
            {
                string a = DropPanelClientIDCollection[i].ClientID;
                aa.Add(a);
            }         
            descriptor.AddProperty("dropPanelClientIDCollection", aa);           
            descriptor.AddProperty("dateTimeFieldName", DateTimeFieldName);
            descriptor.AddProperty("titleFieldName", TitleFieldName);
            descriptor.AddProperty("descriptionFieldName", DescriptionFieldName);
            yield return descriptor;
        }
        public IEnumerable<ScriptReference> GetScriptReferences()
        {
            yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.Schedule.Schedule.js"));
        }
        #region properties
        public virtual string CalendarCssClass{get { return "calendar"; }}
        public virtual string CalendarThCssClass{get { return "calendarTh";}}        
        public virtual string CalendarHeaderCssClass{get { return "calendarHeader"; }}
        public virtual string CalendarCellTdCssClass{get { return "calendarCelltd"; }}
        public virtual string DateTimePickerCssClass{get { return "dateTimePicker"; }}
        public virtual string ToolContainerCssClass{get { return "toolContainer"; }}
        public virtual string CalendarCellContentCssClass{get { return "calendarCellContent"; }}
        public virtual string KeyField
        {
            get { return (string)(ViewState["KeyField"] ?? string.Empty); }
            set { ViewState["KeyField"] = value; }
        }
        public virtual string DateTimeFieldName
        {
            get { return (string)(ViewState["DateTimeField"] ?? string.Empty); }
            set { ViewState["DateTimeField"] = value; }
        }
        public virtual string TitleFieldName
        {
            get { return (string)(ViewState["TitleField"] ?? string.Empty); }
            set { ViewState["TitleField"] = value; }
        }
        public virtual string DescriptionFieldName
        {
            get { return (string)(ViewState["DescriptionField"] ?? string.Empty); }
            set { ViewState["DescriptionField"] = value; }
        }
        public DateTime CalendarRelatedDate
        {
            get { return (DateTime)(ViewState["CalendarRelatedDate"] ?? DateTime.Now); }
            set {
                if (CalendarRelatedDate != value)
                {
                    ViewState["CalendarRelatedDate"] = value;
                    DateTimePicker.SelectedDate = value;
                    this.DataBind();
                }
            }
        }
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string ServicePath { get; set; }
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string UpdateServiceMethod { get; set; }
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string DeleteServiceMethod { get; set; }
        private List<DropPanel> DropPanelClientIDCollection {
            get{ 
                return this._DropPanelClientIDCollection; 
            }         
        }
        #endregion
        #region Child Controls
        private Panel _toolContainer;
        private Calendar _dateTimePicker;
        private Table _calendar, _calendarHeader;
        private ImageButton _navigateBackButton, _navigateForwordButton;
        private ButtonList _displayModeSelection;
        public virtual Panel ToolContainer
        {
            get{
                if (_toolContainer == null)
                    _toolContainer = new Panel();
                return _toolContainer;
            }
        }
        public virtual CSASPNETAjaxScriptControl.Calendar DateTimePicker
        {
            get{              
                if (_dateTimePicker == null)
                    _dateTimePicker = new CSASPNETAjaxScriptControl.Calendar();
                return _dateTimePicker;
            }
        }
        public virtual Table CalendarContainer
        {
            get{
                if (_calendar == null)
                    _calendar = new Table();                    
                return _calendar;
            }
        }
        public virtual Table CalendarHeader
        {
            get{
                return _calendarHeader;
            }
        }
        public virtual ImageButton NavigateBackButton
        {
            get{
                if (_navigateBackButton == null)
                    _navigateBackButton = new ImageButton();
                return _navigateBackButton;
            }
        }
        public virtual ImageButton NavigateForwordButton
        {
            get{
                if (_navigateForwordButton == null)
                    _navigateForwordButton = new ImageButton();
                return _navigateForwordButton;
            }
        }
        public virtual ButtonList DisplayModeSelection
        {
            get{
                if (_displayModeSelection == null)
                    _displayModeSelection = new ButtonList();
                return _displayModeSelection;
            }
        }
        #endregion
        #region Create Child Controls
        protected override void CreateChildControls()
        {
            this.Controls.Clear();
            CreateToolContainer();
            CreateCalendarContainer();
            base.CreateChildControls();
        }
        private void CreateToolContainer()
        {
            CreateDateTimePicker();
            ToolContainer.CssClass = ToolContainerCssClass;
            this.Controls.Add(ToolContainer);
        }
        private void CreateDateTimePicker()
        {
            DateTimePicker.TitleStyle.BackColor = System.Drawing.Color.Transparent;
            DateTimePicker.CssClass = DateTimePickerCssClass;
            DateTimePicker.SelectionChanged += new EventHandler(DateTimePicker_SelectionChanged);
            DateTimePicker.VisibleMonthChanged += new MonthChangedEventHandler(DateTimePicker_VisibleMonthChanged);
            ToolContainer.Controls.Add(DateTimePicker);
        }                
        private void CreateCalendarContainer()
        {
            // Calendar header
            TableRow tr_header = new TableRow();
            TableCell tc_header = new TableCell();
            tc_header.ColumnSpan = columnNum;
            if (_calendarHeader == null)
                _calendarHeader = new Table();
            _calendarHeader = GenerateCalendarHeaderChild();
            tc_header.Controls.Add(_calendarHeader);
            tr_header.Cells.Add(tc_header);
            tr_header.CssClass = CalendarHeaderCssClass; 
            CalendarContainer.Rows.Add(tr_header);
            // Calendar th
            TableHeaderRow tr_th = new TableHeaderRow();
            tr_th.CssClass = CalendarThCssClass;
            for (int i = 0; i < columnNum; i++)
            {
                TableHeaderCell tc_th_cell = new TableHeaderCell();
                string dayOfWeekName = Enum.GetNames(typeof(DayOfWeek))[i];
                tc_th_cell.Text = dayOfWeekName.Substring(0,3).ToUpper();
                tr_th.Cells.Add(tc_th_cell);
            }
            CalendarContainer.Rows.Add(tr_th);
            // Calendar cells            
            CreateCalendarCells(CalendarContainer, CalendarRelatedDate);       
            this.Controls.Add(CalendarContainer);
        }
        protected virtual Table GenerateCalendarHeaderChild()
        {
            Table tb = new Table();
            tb.Width = Unit.Percentage(100);
            TableRow tr = new TableRow();
            TableCell tc1 = new TableCell();
            tc1.Width = Unit.Percentage(20);
            TableCell tc2 = new TableCell();
            LiteralControl lc = new LiteralControl();
            CalendarTitle = lc;
            tc2.Controls.Add(lc);
            tc2.Width = Unit.Percentage(67);
            TableCell tc3 = new TableCell();
            DisplayModeSelection.ID ="DisplayModeSelection";
            DisplayModeSelection.AutoPostBack = true;
            // TODO: We will extend the "Day" mode to present.
            DisplayModeSelection.Items.Add("Month");
            DisplayModeSelection.SelectedIndex = 0;
            DisplayModeSelection.SelectedIndexChanged += new EventHandler(DisplayModeSelection_SelectedIndexChanged);
            tc3.Controls.Add(DisplayModeSelection);
            tc3.Width = Unit.Percentage(13);
            tc3.Style["text-align"] = "left";
            tr.Cells.AddRange( new TableCell[]{tc1,tc2,tc3});            
            tb.Rows.Add(tr);
            return tb;
        }
        public virtual void CreateCalendarCells(Table TargetControl, DateTime CalendarRelatedDate)
        {
            for (int i = 0; i < rowNum; i++)
            {
                TableRow tr_cells = new TableRow();
                for (int n = 0; n < columnNum; n++)
                {
                    TableCell tr_cells_cell = new TableCell();
                    CalendarCellControl ccc = new CalendarCellControl();
                    ccc.Content.CssClass = this.CalendarCellContentCssClass;
                    CalendarCellControlList.Add(ccc);
                    tr_cells_cell.Controls.Add(ccc);
                    tr_cells_cell.CssClass = CalendarCellTdCssClass;
                    tr_cells.Cells.Add(tr_cells_cell);     
                }
                TargetControl.Rows.Add(tr_cells);
            } 
        }
        #endregion
        protected void DisplayModeSelection_SelectedIndexChanged(object sender, EventArgs e)
        {// raisemodechange()
        }
        private void DateTimePicker_SelectionChanged(object sender, EventArgs e)
        {
            CalendarRelatedDate = DateTimePicker.SelectedDate;
        }
        protected void DateTimePicker_VisibleMonthChanged(object sender, MonthChangedEventArgs e)
        {
            CalendarRelatedDate = e.NewDate;
        }
        #region Render Phase
        protected override HtmlTextWriterTag TagKey
        {
            get { return HtmlTextWriterTag.Div; }
        }
        private void RenderCssReference()
        {
            string cssUrl = Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.Schedule.Schedule.css");
            HtmlLink link = new HtmlLink();
            link.Href = cssUrl;
            link.Attributes.Add("type", "text/css");
            link.Attributes.Add("rel", "stylesheet");
            Page.Header.Controls.Add(link);
        }
        protected override void AddAttributesToRender(HtmlTextWriter writer)
        {
            this.CssClass = this.CssClass == string.Empty ? "schedule" : this.CssClass;//please set it before base method.
            base.AddAttributesToRender(writer);
            CalendarContainer.CellPadding = 0;
            CalendarContainer.CellSpacing = 0;
            CalendarHeader.CellPadding = 0;
            CalendarHeader.CellSpacing = 0;
            CalendarContainer.CssClass = CalendarCssClass;
        }
        protected override void OnPreRender(EventArgs e)
        {
            base.OnPreRender(e);
            RenderCssReference();
            FillCalendar();
            ScriptManager manager = ScriptManager.GetCurrent(this.Page);
            if (manager == null) throw new InvalidOperationException("A ScriptManager is required on the page.");
            manager.RegisterScriptControl<Schedule>(this);
            manager.Services.Add(new ServiceReference(this.ServicePath));  
        }   
        protected void FillCalendar()
        {
            CalendarTitle.Text = CalendarRelatedDate.ToString("yyyy MM");
            DateTime targetDT = new DateTime(CalendarRelatedDate.Year, CalendarRelatedDate.Month, 1);
            int offsetDays = (int)targetDT.DayOfWeek;
            firstDayOfTargetDateTime = targetDT.AddDays(-offsetDays);
            DateTime _dtInCell = firstDayOfTargetDateTime;
            for (int i = 0; i < columnNum*rowNum; i++)
            {
                CalendarCellControlList[i].RelatedDate = _dtInCell;
                CalendarCellControlList[i].Content.Attributes.Add("relatedDate", _dtInCell.ToString("yyyy-M-d"));
                _dtInCell = _dtInCell.AddDays(+1);
            }
            // To reduce the data source volumn of datasource, it needs DataSource to dynamically genereate the data source according to the CalendarRelatedDate in page load, rather than retrieving all the data from datasource 
            this.GetData().Select(DataSourceSelectArguments.Empty, this.SelectCallback);
        }
        private void SelectCallback(IEnumerable data)
        {
            DataTable dt=((DataView)data).Table;
            FillDropPanel(dt);
        }
        internal void FillDropPanel(DataTable dt)
        {
            if (DateTimeFieldName == string.Empty) throw new Exception("DateTimeFieldName can't be null, set the property DateTimeFieldName");
            string startDate = firstDayOfTargetDateTime.ToString("yyyy-M-d");
            string endDate = firstDayOfTargetDateTime.AddDays(columnNum * rowNum-1).ToString("yyyy-M-d");
            string sqlExpress = DateTimeFieldName+" >= '"+startDate+"' And "+DateTimeFieldName+" <= '"+endDate+"'";
            DataRow[] filterData = dt.Select(sqlExpress, DateTimeFieldName);
            List<string> highLightDayList = new List<string>();
            foreach (DataRow dr in filterData)
            {
                DateTime date = ((DateTime)dr[DateTimeFieldName]);
                string id = dr[KeyField].ToString();
                string title = dr[TitleFieldName].ToString();
                string description = dr[DescriptionFieldName].ToString();
                DropPanel _dropPanel = CalendarCellControlList[(date - firstDayOfTargetDateTime).Days].GenerateDropPanel(id, date, title, "");
                DropPanelClientIDCollection.Add(_dropPanel);
                string dateStr = date.ToString("yyyy-M-d");
                if (!highLightDayList.Contains(dateStr))
                    highLightDayList.Add(dateStr);
            }
            DateTimePicker.HighLightDayList = highLightDayList;
            DateTimePicker.HighLightDayListType = "yyyy-M-d";
        }
        protected override void Render(HtmlTextWriter writer)
        {
            ScriptManager.GetCurrent(this.Page).RegisterScriptDescriptors(this);
            base.Render(writer); 
        }
        #endregion
    }
}
