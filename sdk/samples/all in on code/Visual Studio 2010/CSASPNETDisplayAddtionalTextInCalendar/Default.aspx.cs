/****************************** Module Header ******************************\
* Module Name: Default.aspx.cs
* Project:     CSASPNETDisplayAddtionalTextInCalendar
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to display additional text in a calendar control.
* As we know, people may want to add different customize text when date of 
* Calendar has been selected. Here we give an easy way to set Calendar's specified
* text, background color, border properties and make Calendar looks better.
* 
* Calendar control host in this page.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*****************************************************************************/




using System;
using System.Collections.Generic;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Drawing;

namespace CSASPNETDisplayAddtionalTextInCalendar
{
    public partial class Default : System.Web.UI.Page
    {
        // Define a list of DateTime to store selected date.
        List<DateTime> SelectDates = new List<DateTime>();
        protected void Page_Load(object sender, EventArgs e)
        {
            SelectDates = this.GetDateTimeList;
        }

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


    }
}