//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------
using System;
using System.Reflection; // For Missing.Value and BindingFlags
using System.Runtime.InteropServices; // For COMException
using Microsoft.Office.Interop.Excel;

class AutoExcel {
	public static int Main() {
	
		Console.WriteLine ("Creating new Excel.Application");
		Application app = new Application();
		if (app == null) {
			Console.WriteLine("ERROR: EXCEL couldn't be started!");
			return 0;
		}
		
		Console.WriteLine ("Making application visible");		
		app.Visible = true;
		
		Console.WriteLine ("Getting the workbooks collection");
		Workbooks workbooks = app.Workbooks;

		Console.WriteLine ("Adding a new workbook");
		
		_Workbook workbook = workbooks.Add(XlWBATemplate.xlWBATWorksheet);

		Console.WriteLine ("Getting the worksheets collection");
		Sheets sheets = workbook.Worksheets;

		_Worksheet worksheet = (_Worksheet) sheets.get_Item(1);
		if (worksheet == null) {
			Console.WriteLine ("ERROR: worksheet == null");
		}
		
		Console.WriteLine ("Setting the value for cell");
		
		// This paragraph puts the value 5 to the cell G1
	    Range range1 = worksheet.get_Range("G1", Missing.Value);
		if (range1 == null) {
			Console.WriteLine ("ERROR: range == null");
		}
		const int nCells = 5;
		range1.Value2 = nCells;
		
		// This paragraph sends single dimension array to Excel
	    Range range2 = worksheet.get_Range("A1", "E1");
		int[] array2 = new int [nCells];
		for (int i=0; i < array2.GetLength(0); i++) {
			array2[i] = i+1;
		}
		range2.Value2 = array2;

		// This paragraph sends two dimension array to Excel
	    Range range3 = worksheet.get_Range("A2", "E3");
		int[,] array3 = new int [2, nCells];
		for (int i=0; i < array3.GetLength(0); i++) {
			for (int j=0; j < array3.GetLength(1); j++) {
				array3[i, j] = i*10 + j;
			}
		}
		range3.Value2 = array3;

		// This paragraph reads two dimension array from Excel
	    Range range4 = worksheet.get_Range("A2", "E3");
		Object[,] array4;
		array4 = (Object[,])range4.Value2;
		
		for (int i=array4.GetLowerBound(0); i <= array4.GetUpperBound(0); i++) {
			for (int j=array4.GetLowerBound(1); j <= array4.GetUpperBound(1); j++) {
				if ((double)array4[i, j] != array3[i-1, j-1]) {
					Console.WriteLine ("ERROR: Comparison FAILED!");
					return 0;
				}
			}
		}

		// This paragraph fills two dimension array with points for two curves and sends it to Excel
	    Range range5 = worksheet.get_Range("A5", "J6");
		double[,] array5 = new double[2, 10];
		for (int j=0; j < array5.GetLength(1); j++) {
			double arg = Math.PI/array5.GetLength(1) * j;
			array5[0, j] = Math.Sin(arg);
			array5[1, j] = Math.Cos(arg);
		}
		range5.Value2 = array5;
		
		// The following code draws the chart
		range5.Select();
		ChartObjects chartobjects = (ChartObjects) worksheet.ChartObjects(Missing.Value);
		
		ChartObject chartobject = (ChartObject) chartobjects.Add(10 /*Left*/, 100 /*Top*/, 450 /*Width*/, 250 /*Height*/);
		_Chart chart = (_Chart) chartobject.Chart;
		
		// Call to chart.ChartWizard() is shown using late binding technique solely for the demonstration purposes
		Object[] args7 = new Object[11];
		args7[0] = range5; // Source
		args7[1] = XlChartType.xl3DColumn; // Gallery
		args7[2] = Missing.Value; // Format
		args7[3] = XlRowCol.xlRows; // PlotBy
		args7[4] = 0; // CategoryLabels
		args7[5] = 0; // SeriesLabels
		args7[6] = true; // HasLegend
		args7[7] = "Sample Chart"; // Title
		args7[8] = "Sample Category Type"; // CategoryTitle
		args7[9] = "Sample Value Type"; // ValueTitle
		args7[10] = Missing.Value; // ExtraTitle
		chart.GetType().InvokeMember("ChartWizard", BindingFlags.InvokeMethod, null, chart, args7);
	    
		Console.WriteLine ("Press ENTER to finish the sample:");
		Console.ReadLine();		
		
		try {
			// If user interacted with Excel it will not close when the app object is destroyed, so we close it explicitely
			workbook.Saved = true;
			app.UserControl = false;
			app.Quit();
		} catch (COMException) {
			Console.WriteLine ("User closed Excel manually, so we don't have to do that");
		}
		
		Console.WriteLine ("Sample successfully finished!");
		return 100;
	}
}
