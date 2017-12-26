==============================================================================
 ASP.NET APPLICATION : VBASPNETReverseAJAX Project Overview
==============================================================================

//////////////////////////////////////////////////////////////////////////////
Summary:

Reverse Ajax is also known as Comet Ajax, Ajax Push, Two-way-web and HTTP server push. 
This technique persists a HTTP request to allow the server push data to a browser, 
without requesting the server in a individual time.

This sample shows how to use this technique in ASP.NET Ajax.

//////////////////////////////////////////////////////////////////////////////
Demo the Sample:

1. Open Receiver.aspx page in one or more windows (not browser tabs). 
   Log in with different user names.

2. Open Sender.aspx page. Input recipient name and message content.
   Click the send button.

//////////////////////////////////////////////////////////////////////////////
Code Logical:

1. The Message class represents a message entity.

2. The Client class represents a client that can receive messages from server. 
   Also the server can send a message to the client. The class contains two 
   private members and two public methods. 
   
    Public Class Client
        Private messageEvent As New ManualResetEvent(False)
        Private messageQueue As New Queue(Of Message)()

        Public Sub EnqueueMessage(ByVal message As Message)
            SyncLock messageQueue
                messageQueue.Enqueue(message)
                messageEvent.[Set]()
            End SyncLock
        End Sub
        Public Function DequeueMessage() As Message
            messageEvent.WaitOne()
            SyncLock messageQueue
                If messageQueue.Count = 1 Then
                    messageEvent.Reset()
                End If
                Return messageQueue.Dequeue()
            End SyncLock
        End Function
    End Class

   EnqueueMessage method is designed for the sender to send a message to that 
   client, as well as DequeueMessage is designed for the recipient to receive 
   messages.

3. The ClientAdapter class is used to manage multiple clients. The presentation 
   layer can easily call its methods to send and receive messages.

4. Dispatcher.asmx web service is designed to be called by Ajax to receive messages.

5. The Receiver.aspx page contains a JavaScript function waitEvent. When the function
   is called, it will persist a request until a new message. And then it will persist 
   the next request immediately.

    function waitEvent() {

        VBASPNETReverseAJAX.Dispatcher.WaitMessage("<%= Session("userName") %>", 
        function (result) {
            displayMessage(result);
            setTimeout(waitEvent, 0);
        }, function () {
            setTimeout(waitEvent, 0);
        });
    }

//////////////////////////////////////////////////////////////////////////////
References:

ManualResetEvent Class
http://msdn.microsoft.com/en-us/library/system.threading.manualresetevent.aspx

ScriptManager Control Overview
http://msdn.microsoft.com/en-us/library/bb398863.aspx

Web Services in ASP.NET AJAX
http://msdn.microsoft.com/en-us/library/bb398785(v=VS.90).aspx