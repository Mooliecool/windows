'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
option strict off
imports System
imports System.Reflection ' For Missing.Value and BindingFlags
imports System.Runtime.InteropServices ' For COMException
Imports Microsoft.Office.Interop.Excel

class AutoExcel
	public shared sub Main()	
	
		Console.WriteLine ("Creating new Excel Application")
        Dim app As Application
        app = New Application()
		if app is nothing then
			Console.WriteLine("ERROR: EXCEL couldn't be started!")
			environment.exitcode = 0
			exit sub
		end if
		
		Console.WriteLine ("Making application visible")
		app.Visible = true
		
		Console.WriteLine ("Getting the workbooks collection")
        Dim workbooks As Workbooks
		workbooks = app.Workbooks

		Console.WriteLine ("Adding a new workbook")
        Dim workbook As _Workbook
        workbook = workbooks.Add(XlWBATemplate.xlWBATWorksheet)

		Console.WriteLine ("Getting the worksheets collection")
        Dim sheets As Sheets
		sheets = workbook.Worksheets

        Dim worksheet As _Worksheet
		worksheet = sheets.Item(1)
		if worksheet is nothing then
			Console.WriteLine ("ERROR: worksheet == null")
		end if
		
		Console.WriteLine ("Setting the value for cell")
	    dim range as Object 'Range 
	    range = worksheet.Range("A10", Missing.Value)
		if range is nothing then
			Console.WriteLine ("ERROR: range == null")
		end if
	    range.Value2 = 5
	    
		' This paragraph sends single dimension array to Excel
	    Dim range2 as Object
	    range2 = worksheet.Range("A1", "E1")
	    Dim array2(5) as Integer
	    Dim i As Integer
	    For i = 0 To 4
	    	array2(i) = i+1
	    Next i
		range2.Value2 = array2
	    
		' This paragraph sends two dimension array to Excel
	    Dim range3 as Object
	    range3 = worksheet.Range("A2", "E3")
	    Dim array3(2, 5) as Integer
	    Dim j As Integer
	    For i = 0 To 1
		    For j = 0 To 4
		    	array3(i, j) = i*10+j
		    Next j
	    Next i
		range3.Value2 = array3
	    
		' This paragraph reads two dimension array from Excel
	    Dim range4 as Object 
	    range4 = worksheet.Range("A2", "E3")
	    Dim array4(,) as Object
		array4 = range4.Value2
		
	    For i = array4.GetLowerBound(0) To array4.GetUpperBound(0)
		    For j = array4.GetLowerBound(1) To array4.GetUpperBound(1)
				if CDbl(array4(i, j)) <> array3(i-1, j-1) then 
				environment.exitcode = 0
				Console.WriteLine ("ERROR: Comparison FAILED!")
				exit sub
				end if
		    Next j
	    Next i
		
		' This paragraph sends two dimension array to Excel
	    Dim range5 as Object 
	    range5 = worksheet.Range("A5", "J6")
	    Dim array5(2, 10) as Double
		For j = 0 To array5.GetUpperBound(1)
			Dim arg as Double
			arg = Math.PI/array5.GetLength(1) * j
			array5(0, j) = Math.Sin(arg)
			array5(1, j) = Math.Cos(arg)
		Next j
		range5.Value2 = array5

		' The following code draws the chart
		
		range5.Select
		
		Dim chartobjects as Object
		chartobjects = worksheet.ChartObjects(Missing.Value)

		Dim chartobject as Object
		chartobject = chartobjects.Add(10, 100, 450, 250)
		
		Dim chart as Object
		chart = chartobject.Chart
		
        chart.ChartWizard(range5, XlChartType.xl3DColumn, Missing.Value, XlRowCol.xlRows, 0, 0, True, "Sample Chart", _
  "Sample Category Type", "Sample Value Type")

		Console.WriteLine ("Press ENTER to finish the sample:")
		Console.ReadLine()
		
		Try
			' If user interacted with Excel it will not close when the app object is destroyed, so we close it explicitely
			workbook.Saved = true
			app.UserControl = false
			app.Quit()
		Catch Outer As COMException	
			Console.WriteLine ("User closed Excel manually, so we don't have to do that")
		End Try

		environment.exitcode = 100
		Console.WriteLine ("Sample successfully finished!")
end sub
end class
