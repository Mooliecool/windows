' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ServiceModel
Imports System.Threading
Imports System.IO
Imports System.Text
Imports System.Globalization

Namespace Microsoft.ServiceModel.Samples

    ' Define a service contract.
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface ICalculator

        <OperationContract()> _
        Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double

        <OperationContract()> _
        Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double

        'Multiply involves some file I/O so we'll make it Async.
        <OperationContract(AsyncPattern:=True)> _
        Function BeginMultiply(ByVal n1 As Double, ByVal n2 As Double, ByVal callback As AsyncCallback, ByVal state As Object) As IAsyncResult
        Function EndMultiply(ByVal ar As IAsyncResult) As Double

        'Divide involves some file I/O so we'll make it Async.
        <OperationContract(AsyncPattern:=True)> _
        Function BeginDivide(ByVal n1 As Double, ByVal n2 As Double, ByVal callback As AsyncCallback, ByVal state As Object) As IAsyncResult
        Function EndDivide(ByVal ar As IAsyncResult) As Double

    End Interface

    ' Service class which implements the service contract.
    <ServiceBehavior(InstanceContextMode:=InstanceContextMode.PerCall, ConcurrencyMode:=ConcurrencyMode.Multiple)> _
    Public Class CalculatorService
        Implements ICalculator

        Public Function Add(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Add

            Console.WriteLine("Received Add Synchronously on ThreadID {0}:  Sleeping for 3 seconds", Thread.CurrentThread.ManagedThreadId)
            Thread.Sleep(3000)
            Console.WriteLine("Returning Add Result on ThreadID {0}", Thread.CurrentThread.ManagedThreadId)
            Return n1 + n2

        End Function

        Public Function Subtract(ByVal n1 As Double, ByVal n2 As Double) As Double Implements ICalculator.Subtract

            Console.WriteLine("Received Subtract Synchronously on ThreadID {0}:  Sleeping for 3 seconds", Thread.CurrentThread.ManagedThreadId)
            Thread.Sleep(3000)
            Console.WriteLine("Returning Subtract Result on ThreadID {0}", Thread.CurrentThread.ManagedThreadId)
            Return n1 - n2

        End Function

        Public Function BeginMultiply(ByVal n1 As Double, ByVal n2 As Double, ByVal callback As AsyncCallback, ByVal state As Object) As IAsyncResult Implements ICalculator.BeginMultiply

            Console.WriteLine("Asynchronous call: BeginMultiply on ThreadID {0}", Thread.CurrentThread.ManagedThreadId)
            'return an AsyncResult
            Return New MathAsyncResult(New MathExpression(n1, n2, "*"), callback, state)

        End Function

        Public Function EndMultiply(ByVal ar As IAsyncResult) As Double Implements ICalculator.EndMultiply

            Console.WriteLine("EndMultiply called on ThreadID {0}", Thread.CurrentThread.ManagedThreadId)
            'use the AsyncResult to complete that async operation
            Return MathAsyncResult.[End](ar)

        End Function

        Public Function BeginDivide(ByVal n1 As Double, ByVal n2 As Double, ByVal callback As AsyncCallback, ByVal state As Object) As IAsyncResult Implements ICalculator.BeginDivide

            Console.WriteLine("Asynchronous call: BeginDivide on ThreadID {0}", Thread.CurrentThread.ManagedThreadId)
            'return an AsyncResult
            Return New MathAsyncResult(New MathExpression(n1, n2, "/"), callback, state)

        End Function

        Public Function EndDivide(ByVal ar As IAsyncResult) As Double Implements ICalculator.EndDivide

            Console.WriteLine("EndDivide called on ThreadID {0}", Thread.CurrentThread.ManagedThreadId)
            'use the AsyncResult to complete that async operation
            Return MathAsyncResult.[End](ar)

        End Function

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Create a ServiceHost for the CalculatorService type.
            Using serviceHost As New ServiceHost(GetType(CalculatorService))

                ' Open the ServiceHost to create listeners and start listening for messages.
                serviceHost.Open()

                ' The service can now be accessed.
                Console.WriteLine("The service is ready.")
                Console.WriteLine("Press <ENTER> to terminate service.")
                Console.WriteLine()
                Console.ReadLine()

            End Using

        End Sub

    End Class

    Public Class MathExpression

        Private m_n1 As Double
        Private m_n2 As Double
        Private m_operation As String

        Public Sub New(ByVal n1 As Double, ByVal n2 As Double, ByVal operation As String)

            Me.m_n1 = n1
            Me.m_n2 = n2
            Me.m_operation = operation

        End Sub

        Public ReadOnly Property N1() As Double

            Get

                Return m_n1

            End Get

        End Property

        Public ReadOnly Property N2() As Double

            Get

                Return m_n2

            End Get

        End Property

        Public ReadOnly Property Operation() As String

            Get

                Return m_operation

            End Get

        End Property

        Public ReadOnly Property Result() As Double

            Get

                Select Case Operation

                    Case "+"
                        Return N1 + N2
                    Case "-"
                        Return N1 - N2
                    Case "*"
                        Return N1 * N2
                    Case "/"
                        Return N1 / N2
                    Case Else
                        Throw New InvalidOperationException("could not handle " + Operation + " operation.")

                End Select

            End Get

        End Property

        Public Function ToBytes() As Byte()

            Return Encoding.Unicode.GetBytes(String.Format(CultureInfo.InvariantCulture, "{0} {1} {2} = {3}", N1, Operation, N2, Result))

        End Function

    End Class

    ''' <summary>
    ''' Implementation of async Math invocation 
    ''' </summary>
    Class MathAsyncResult
        Inherits Microsoft.ServiceModel.Samples.TypedAsyncResult(Of Double)

        Private fs As FileStream
        Private expr As MathExpression

        Public Sub New(ByVal mathExpression As MathExpression, ByVal callback As AsyncCallback, ByVal state As Object)
            MyBase.New(callback, state)

            expr = mathExpression
            'Turn the expression into an array of bytes
            Dim bytes As Byte() = expr.ToBytes()

            'open a file for writing
            fs = File.OpenWrite(Path.GetRandomFileName() + ".txt")

            'begin writing asynchronously
            Dim result As IAsyncResult = fs.BeginWrite(bytes, 0, bytes.Length, New AsyncCallback(AddressOf OnWrite), Me)

            'if the write did not complete synchronously, we are done setting up this AsyncResult
            If Not result.CompletedSynchronously Then
                Return
            End If

            'If the write did complete synchronously, then we'll complete the AsyncResult
            CompleteWrite(result, True)

        End Sub

        'Completes asynchronous work.
        'cleans up any resources managed by this AsyncResult
        'Signals the base class that all work is finished
        Private Sub CompleteWrite(ByVal result As IAsyncResult, ByVal synchronous As Boolean)

            Try

                'Complete the asynchronous file write
                fs.EndWrite(result)

            Finally

                'Clean up the file resources
                fs.Close()

            End Try

            'Calling Complete on the base AsyncResult signals the WaitHandle
            'And makes the callback if necessary
            MyBase.Complete(expr.Result, synchronous)

        End Sub

        Private Sub OnWrite(ByVal result As IAsyncResult)

            'if we returned synchronously, then CompletWrite will be called directly
            If result.CompletedSynchronously Then
                Return
            End If

            Console.WriteLine("IO thread for {0} operation on ThreadID {1}", expr.Operation, Thread.CurrentThread.ManagedThreadId)

            Try

                'Call CompleteWrite to cleanup resources and complete the AsyncResult
                CompleteWrite(result, False)

            Catch e As Exception

                'if something bad happend, then call the exception overload
                'on the base class.  This will serve up the exception on the
                'AsyncResult
                MyBase.Complete(False, e)

            End Try

        End Sub

    End Class

End Namespace