'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBSparseFile
' Copyright (c) Microsoft Corporation.
' 
' VBSparseFile demonstrates the common operations on sparse files. A sparse 
' file is a type of computer file that attempts to use file system space more 
' efficiently when blocks allocated to the file are mostly empty. This is 
' achieved by writing brief information (metadata) representing the empty 
' blocks to disk instead of the actual "empty" space which makes up the 
' block, using less disk space. You can find in this example the creation of 
' sparse file, the detection of sparse attribute, the retrieval of sparse 
' file size, and the query of sparse file layout.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 7/7/2009 11:34 PM Jialiang Ge Created
'***************************************************************************'

#Region "Imports directives"

Imports System.IO

#End Region


Module MainModule

    Sub Main()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Determine if the volume support sparse streams.
        ' 

        If Not SparseFile.VolumeSupportsSparseFiles("C:\") Then
            Console.WriteLine("Volume {0} does not support sparse streams", _
                              "C:\")
            Return
        End If


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Create a sparse file.
        ' 

        Dim fileName As String = "SparseFile.tmp"
        Console.WriteLine("Create sparse file: {0}", fileName)

        Using fs As FileStream = SparseFile.Create(fileName)

            ' Write a large block of data
            Const blockLength As Integer = 512 * 1024 ' 512KB
            Dim block As Byte() = New Byte(blockLength - 1) {}
            For i As Integer = 0 To blockLength - 1
                block(i) = &HFF
            Next i

            fs.Write(block, 0, blockLength)

            ' Set some sparse ranges in the block

            SparseFile.SetSparseRange(fs.SafeFileHandle, 0, &H10000)
            SparseFile.SetSparseRange(fs.SafeFileHandle, &H20000, &H20000)

            ' Set sparse block at the end of the file

            ' 1GB sparse zeros are extended to the end of the file
            fs.SetLength(fs.Length + &H40000000)

        End Using


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Determine if a file is sparse.
        ' 

        Dim isSparse As Boolean = SparseFile.IsSparseFile(fileName)
        Console.WriteLine("The file is{0} sparse", IIf(isSparse, "", " not"))


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Get file size.
        ' 

        SparseFile.GetSparseFileSize(fileName)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query the sparse file layout.
        ' 

        SparseFile.GetSparseRanges(fileName)

    End Sub

End Module
