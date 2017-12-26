========================================================================
            CSASPNETDisplayAddtionalTextInCalendar Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to display additional text in a calendar control.
As we know, people may want to add different customize text when date of 
Calendar has been selected. Here we give an easy way to set Calendar's specified
text, background color, border properties and make Calendar looks better.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the CSASPNETDisplayAddtionalTextInCalendar.sln.

Step 2: Expand the CSASPNETDisplayAddtionalTextInCalendar web application and press 
        Ctrl + F5 to show the Default.aspx.

Step 3: You can find a Calendar control on the Default.aspx page. Please Click
        to select one of dates.

Step 4: You will see "your text" in the bottom of every selected date.

Step 5: Validation finished.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Create a C# "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "CSASPNETDisplayAddtionalTextInCalendar".

Step 2. Add one web form in the root directory, name it as "Default.aspx".

Step 3. Add a Calendar control and a button on default page, The calendar control
        display the selected dates and additional text, The Button is use to output
		all selected dates.

Step 4. Define a list of DateTime instances for storing selected dates.
        The List<DateTime> variable store in ViewState.
		[code]
		// Define a list of DateTime to store selected date.
        List<DateTime> SelectDates = new List<DateTime>();

		/// <summary>
        /// The selected dates is stored in ViewState.
        /// </summary>
        public List<DateTime> GetDateTimeList
        {
            get
            {
                if (ViewState["SelectDate"] == null)
                {
                    List<DateTime> selectdates = new List<DateTime>();
                    selectdates.Add(DateTime.MaxValue);
                    ViewState["SelectDate"] = selectdates;
                }
                return (List<DateTime>)ViewState["SelectDate"];
            }
            set
            {
                ViewState["SelectDate"] = value;
            }
        }
		[/code]	

Step 5  Add Calendar SelectionChanged, PreRender, DayRender event to add
        date time in selected date list or remove date from it. 
		[code]
        /// <summary>
        /// Calendar SelectionChanged event, add new date in SelectedDate and 
        /// remove the same date.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void myCalendar_SelectionChanged(object sender, EventArgs e)
        {
            DateTime dateSeletion = myCalendar.SelectedDate;
            if (SelectDates.Contains(dateSeletion))
            {
                SelectDates.Remove(dateSeletion);
            }
            else
            {
                SelectDates.Add(dateSeletion);
            }
        }

        /// <summary>
        /// Clear selected dates.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void myCalendar_PreRender(object sender, EventArgs e)
        {
            myCalendar.SelectedDates.Clear();
        }

        /// <summary>
        /// Add your customize text in Calendar control.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void myCalendar_DayRender(object sender, DayRenderEventArgs e)
        {
            foreach (DateTime time in SelectDates)
            {
                myCalendar.SelectedDates.Add(time);
                if (e.Day.Date == time)
                {
                    e.Cell.BorderWidth = 1;
                    e.Cell.BackColor = Color.SlateBlue;
                    e.Cell.Controls.Add(new LiteralControl("<br />your text"));
                }
            }
        }
	    [/code] 

Step 6. Add button's click event, the button will display the selected dates
        on default page, actually we may store them in database or xml files.
		Here we only output them. 
		[code]
		/// <summary>
        /// render selected dates.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void btnCheck_Click(object sender, EventArgs e)
        {
            foreach (DateTime date in SelectDates)
            {
                if (date != DateTime.MaxValue)
                {
                    Response.Write(date.ToShortDateString());
                    Response.Write("<br />");
                }
            }
        }
		[/code]

Step 7. Build the application and you can debug it.
/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Calendar Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.calendar(VS.80).aspx

MSDN: Calendar.SelectionChanged Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.calendar.selectionchanged.aspx

MSDN: Control.PreRender Event
http://msdn.microsoft.com/en-us/library/system.web.ui.control.prerender.aspx

MSDN: Calendar.DayRender Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.calendar.dayrender.aspx
/////////////////////////////////////////////////////////////////////////////