'****************************** Module Header ******************************\
' Module Name:    Default.aspx.vb
' Project:        VBASPNETChart
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to use the new Chart control to create an chart
' in the web page. Two important properties of the Chart class are the Series 
' and ChartAreas properties, both of which are collection properties. The
' Series collection property stores Series objects, which are used to store 
' data that is to be displayed, along with attributes of that data. The 
' ChartAreas collection property stores ChartArea objects, which are primarily
' used to draw one or more charts using one set of axes.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' 3/17/2010 4:30 PM Bravo Yang Created
'*****************************************************************************/

Imports System.Web.UI.DataVisualization.Charting

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim dt As DataTable
        dt = CreateDataTable()

        'Set the DataSource property of the Chart control to the DataTabel
        Chart1.DataSource = dt

        'Give two columns of data to Y-axle
        Chart1.Series(0).YValueMembers = "Volume1"
        Chart1.Series(1).YValueMembers = "Volume2"

        'Set the X-axle as date value
        Chart1.Series(0).XValueMember = "Date"

        'Bind the Chart control with the setting above
        Chart1.DataBind()

    End Sub

    Private Function CreateDataTable() As DataTable

        'Create a DataTable as the data source of the Chart control
        Dim dt As New DataTable
        dt.Columns.Add("Date")
        dt.Columns.Add("Volume1")
        dt.Columns.Add("Volume2")

        Dim dr As DataRow

        'Add rows to the table which contains some random data for demonstration
        dr = dt.NewRow()
        dr("Date") = "Jan"
        dr("Volume1") = 3731
        dr("Volume2") = 4101
        dt.Rows.Add(dr)

        dr = dt.NewRow()
        dr("Date") = "Feb"
        dr("Volume1") = 6024
        dr("Volume2") = 4324
        dt.Rows.Add(dr)

        dr = dt.NewRow()
        dr("Date") = "Mar"
        dr("Volume1") = 4935
        dr("Volume2") = 2935
        dt.Rows.Add(dr)

        dr = dt.NewRow()
        dr("Date") = "Apr"
        dr("Volume1") = 4466
        dr("Volume2") = 5644
        dt.Rows.Add(dr)

        dr = dt.NewRow()
        dr("Date") = "May"
        dr("Volume1") = 5117
        dr("Volume2") = 5671
        dt.Rows.Add(dr)

        dr = dt.NewRow()
        dr("Date") = "Jun"
        dr("Volume1") = 3546
        dr("Volume2") = 4646
        dt.Rows.Add(dr)

        Return dt
    End Function



End Class