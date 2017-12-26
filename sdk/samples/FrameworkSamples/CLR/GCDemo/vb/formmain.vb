'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
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
Imports System.Globalization

Public Class formMain

    Private _allocation As Integer
    Private _curCount As Integer
    Private _totCount As Integer
    Private _gcBaseLine As Integer

    Private Sub buttonRun_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonRun.Click
        Cursor = Cursors.WaitCursor

        ' Garbage collection is forced to "clean" the heap, then
        ' we pause while waiting for the separate finalizer thread 
        ' to clear the finalization queue. A final GC.Collect()
        ' call clears out anything that may have been resurrected
        ' by the finalizer.
        GC.Collect()
        GC.WaitForPendingFinalizers()
        GC.Collect()

        Dim ms As MEMORYSTATUSEX = New MEMORYSTATUSEX()

        ms.Init()
        NativeMethods.GlobalMemoryStatusEx(ms)

        Dim pressure As Integer = _allocation

        treeMain.Nodes.Clear()

        For i As Integer = 0 To CInt(numericAllocations.Value)

            Dim nodeElement As Integer = treeMain.Nodes.Add(New TreeNode(String.Format(CultureInfo.InvariantCulture, "Allocating 1/{0} for the {1}{2} time", _allocation, i + 1, GetSuffix(i))))

            Dim m As MemoryHogger = New MemoryHogger(pressure, radioPressureYes.Checked)
            m = New MemoryHogger(pressure, radioPressureYes.Checked)
            Dim tn As TreeNode = New TreeNode()
            If m.Success Then
                tn.Text = "Allocation Succeeded!"
                tn.BackColor = Color.FromArgb(0, 255, 0)
                treeMain.Nodes(nodeElement).Nodes.Add(tn)
            Else

                tn.Text = "Allocation Failed!"
                tn.BackColor = Color.Red
                treeMain.Nodes(nodeElement).Nodes.Add(tn)
            End If
            tn.Parent.Expand()
        Next
        _curCount = GC.CollectionCount(0) - _gcBaseLine - _totCount
        _totCount = GC.CollectionCount(0) - _gcBaseLine
        labelCount.Text = _curCount.ToString(CultureInfo.InvariantCulture)
        labelTotCount.Text = _totCount.ToString(CultureInfo.InvariantCulture)
        buttonClear.Enabled = True
        Cursor = Cursors.Arrow
    End Sub

    Private Sub buttonClear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonClear.Click
        treeMain.Nodes.Clear()
        labelCount.Text = "0"
        labelTotCount.Text = "0"
        _gcBaseLine = _gcBaseLine + _totCount
        _curCount = 0
        _totCount = 0

        buttonClear.Enabled = False
    End Sub

    Private Sub buttonExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonExit.Click
        Me.Close()
    End Sub

    Private Sub formMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        _allocation = 1000
        _curCount = 0
        _totCount = 0
        comboAmount.SelectedIndex = 1
        UpdateDescLabel()
    End Sub

    Private Sub UpdateAllocationLabel()
        labelAllocation.Text = String.Format(CultureInfo.InvariantCulture, "Allocate {0} bytes of available memory", comboAmount.Text)
    End Sub

    Private Sub UpdateDescLabel()
        labelDesc.Text = String.Format(CultureInfo.InvariantCulture, "This demo allocates " & Environment.NewLine & _
                   "{0} bytes of available unmanaged" & Environment.NewLine & _
                  "memory, {1} times", comboAmount.Text, numericAllocations.Value)
    End Sub

    Private Function GetSuffix(ByVal i As Integer) As String
        If (i = 0) Then
            Return "st"
        ElseIf (i = 1) Then
            Return "nd"
        ElseIf (i = 2) Then
            Return "rd"
        Else
            Return "th"
        End If
    End Function

    Private Sub comboAmount_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles comboAmount.SelectedIndexChanged
        _allocation = CInt(comboAmount.Text)

        UpdateDescLabel()
        UpdateAllocationLabel()
    End Sub

    Private Sub numericAllocations_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles numericAllocations.ValueChanged
        UpdateDescLabel()
    End Sub
End Class