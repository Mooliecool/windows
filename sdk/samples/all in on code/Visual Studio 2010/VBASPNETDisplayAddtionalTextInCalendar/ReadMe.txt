========================================================================
            VBASPNETDisplayAddtionalTextInCalendar Overview
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

Step 1: Open the VBASPNETDisplayAddtionalTextInCalendar.sln.

Step 2: Expand the VBASPNETDisplayAddtionalTextInCalendar web application and press 
        Ctrl + F5 to show the Default.aspx.

Step 3: You can find a Calendar control on the Default.aspx page. Please Click
        to select one of dates.

Step 4: You will see "your text" in the bottom of every selected date.

Step 5: Validation finished.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Create a VB "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "VBASPNETDisplayAddtionalTextInCalendar".

Step 2. Add one web form in the root directory, name it as "Default.aspx".

Step 3. Add a Calendar control and a button on default page, The calendar control
        display the selected dates and additional text, The Button is use to output
		all selected dates.

Step 4. Define a list of DateTime instances for storing selected dates.
        The List<DateTime> variable store in ViewState.
		[code]
    ' Define a list of DateTime to store selected date.
    Dim SelectDates As New List(Of DateTime)()

    ''' <summary>
    ''' The selected dates is stored in ViewState.
    ''' </summary>
    Public Property GetDateTimeList() As List(Of DateTime)
        Get
            If ViewState("SelectDate") Is Nothing Then
                Dim selectdates As New List(Of DateTime)()
                selectdates.Add(DateTime.MaxValue)
                ViewState("SelectDate") = selectdates
            End If
            Return DirectCast(ViewState("SelectDate"), List(Of DateTime))
        End Get
        Set(ByVal value As List(Of DateTime))
            ViewState("SelectDate") = value
        End Set
    End Property
		[/code]	

Step 5  Add Calendar SelectionChanged, PreRender, DayRender event to add
        date time in selected date list or remove date from it. 
		[code]
    ''' <summary>
    ''' Calendar SelectionChanged event, add new date in SelectedDate and 
    ''' remove the same date.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub myCalendar_SelectionChanged(ByVal sender As Object, ByVal e As EventArgs)
        Dim dateSeletion As DateTime = myCalendar.SelectedDate
        If SelectDates.Contains(dateSeletion) Then
            SelectDates.Remove(dateSeletion)
        Else
            SelectDates.Add(dateSeletion)
        End If
    End Sub

    ''' <summary>
    ''' Clear selected dates.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub myCalendar_PreRender(ByVal sender As Object, ByVal e As EventArgs)
        myCalendar.SelectedDates.Clear()
    End Sub

    ''' <summary>
    ''' Add your customize text in Calendar control.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub myCalendar_DayRender(ByVal sender As Object, ByVal e As DayRenderEventArgs)
        For Each time As DateTime In SelectDates
            myCalendar.SelectedDates.Add(time)
            If e.Day.[Date] = time Then
                e.Cell.BorderWidth = 1
                e.Cell.BackColor = Color.SlateBlue
                e.Cell.Controls.Add(New LiteralControl("<br />your text"))
            End If
        Next
    End Sub
	    [/code] 

Step 6. Add button's click event, the button will display the selected dates
        on default page, actually we may store them in database or xml files.
		Here we only output them. 
		[code]
    ''' <summary>
    ''' render selected dates.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub btnCheck_Click(ByVal sender As Object, ByVal e As EventArgs)
        For Each [date] As DateTime In SelectDates
            If [date] <> DateTime.MaxValue Then
                Response.Write([date].ToShortDateString())
                Response.Write("<br />")
            End If
        Next
    End Sub
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