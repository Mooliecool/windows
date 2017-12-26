//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
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
using namespace System;
using namespace System::Reflection; // For Missing.Value and BindingFlags
using namespace System::Runtime::InteropServices; // For COMException
using namespace Microsoft::Office::Core;
using namespace Microsoft::Office::Interop::Excel;


void main () {
    Console::WriteLine ("Creating new Excel.Application");
    Application^ app = safe_cast<Application ^>( gcnew ApplicationClass());
    if (app == nullptr) {
        Console::WriteLine("ERROR: EXCEL couldn't be started!");
        return;
    }

    Console::WriteLine ("Making application visible");		
    app->Visible = true;

    Console::WriteLine ("Getting the workbooks collection");
    Workbooks^ workbooks = app->Workbooks;

    Console::WriteLine ("Adding a new workbook");

    _Workbook^ workbook = workbooks->Add(Missing::Value);

    Console::WriteLine ("Getting the worksheets collection");
    Sheets^ sheets = workbook->Worksheets;

    _Worksheet^ worksheet = safe_cast<_Worksheet^>(sheets->Item[ (Object^)1 ]);
    if (worksheet == nullptr) {
        Console::WriteLine ("ERROR: worksheet == null");
    }

    Console::WriteLine ("Setting the value for cell");
    // This paragraph puts the value 5 to the cell G1
    Range^ range1 = worksheet->Range["G1", Missing::Value];
    if (range1 == nullptr) {
        Console::WriteLine ("ERROR: range == null");
    }
    range1->Value2 = 5;

    // This paragraph sends single dimension array to Excel
    Range^ range2 = worksheet->Range["A1", "E1"];
    array<int>^ array2 = gcnew array<int>(5);
    for (int i=0; i < array2->GetLength(0); i++) {
        array2[i] = i+1;
    }
    range2->Value2 = array2;

    // This paragraph sends two dimension array to Excel
    Range^ range3 = worksheet->Range["A2", "E3"];
    array<int, 2>^ array3 = gcnew array<int, 2>(2,5);
    for (int i=0; i < array3->GetLength(0); i++) {
        for (int j=0; j < array3->GetLength(1); j++) {
            array3[i, j] = i*10 + j;
        }
    }
    array<Object^>^ args3 = gcnew array<Object^>(1);
    args3[0] = array3;
    range3->Value2 = array3;

    // This paragraph reads two dimension array from Excel
    Range^ range4 = worksheet->Range["A2", "E3"];
    array<Object^, 2>^ array4;
    array4 =  safe_cast<array<Object^,2>^> (range4->Value2);

    for (int i=array4->GetLowerBound(0); i <= array4->GetUpperBound(0); i++) {
        for (int j=array4->GetLowerBound(1); j <= array4->GetUpperBound(1); j++) {
            double d = *safe_cast<Double^>(array4[i,j]);
            if (d != array3[i-1, j-1]) {
                Console::WriteLine ("Test FAILED!");
                return;
            }
        }
    }

    // This paragraph sends two dimension array to Excel
    Range^ range5 = worksheet->Range["A5", "J6"];
    array<double, 2>^ array5 = gcnew array<double,2>(2,10);
    for (int j=0; j < array5->GetLength(1); j++) {
        double arg = Math::PI/array5->GetLength(1) * j;
        array5[0, j] = Math::Sin(arg);
        array5[1, j] = Math::Cos(arg);
    }
    range5->Value2 = array5;

    // The following code draws the chart
    range5->Select();

    ChartObjects^ chartobjects = safe_cast<ChartObjects^>(worksheet->ChartObjects(Missing::Value));

    ChartObject^ chartobject = safe_cast<ChartObject^>(chartobjects->Add(10 /*Left*/, 100 /*Top*/, 450 /*Width*/, 250 /*Height*/));
    _Chart^ chart = safe_cast<_Chart^>(chartobject->Chart);

    // Call to chart.ChartWizard() is shown using late binding technique solely for the demonstration purposes
    array<Object^>^ args7 = gcnew array<Object^>(11);
    args7[0] = range5; // Source
    args7[1] = XlChartType::xl3DColumn; // Gallery - note, we're using implicit boxing here
    args7[2] = Missing::Value; // Format
    args7[3] = XlRowCol::xlRows; // PlotBy - note, we're using implicit boxing here
    args7[4] = nullptr; // CategoryLabels
    args7[5] = nullptr; // SeriesLabels
    args7[6] = true; // HasLegend - note, we're using implicit boxing here
    args7[7] = "Sample Chart"; // Title
    args7[8] = "Sample Category Type"; // CategoryTitle
    args7[9] = "Sample Value Type"; // ValueTitle
    args7[10] = Missing::Value; // ExtraTitle
    // The last parameter is lcid, but as we use late binding here it should be omited
    //args7[11] = 0; // lcid
    chart->GetType()->InvokeMember("ChartWizard", BindingFlags::InvokeMethod, nullptr, chart, args7);

    Console::WriteLine ("Press ENTER to finish the sample:");
    Console::ReadLine();		

    try {
        // If user interacted with Excel it will not close when the app object is destroyed, so we close it explicitely
        workbook->Saved = true; // To be able to close it without being asked to save it
        app->UserControl = false;
        app->Quit();
    } catch (COMException^) {
        Console::WriteLine ("User closed Excel manually, so we don't have to do that");
    }

    // The following line is necessary to be sure that app object is released when we finish, because
    // the Garbage Collector is non-deterministic and by default can leave app unreleased. 
    GC::Collect();

    Console::WriteLine ("Test successfully passed!");
    return;
}
