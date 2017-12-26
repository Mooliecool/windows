========================================================================
    SILVERLIGHT APPLICATION : VBSL3SocketClient Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project create a silverlight socket client sample, which could send and
receive string message asynchronously from(to) socket server.     


/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBSL3SocketClient <--> VBSL3SocketServer
VBSL3SocketServer is the socket server which can serve silverlight socket client.


/////////////////////////////////////////////////////////////////////////////
Demo:

To run the socket sample, please try the following steps:
1. To start socket server
	a. Open VBSLSocketServer solution with Administrator account, compile.
	b. Run the project.
2. To run the silverlight socket client
	a. Open VBSL3SocketClient solution, compile and run.
	b. When Silverlight application loaded, follow the instructions displayed on page:
		1) Click "connect" to connect to socket server.
		2) Input text in textbox and click "send" button.
		3) Server will receive and handle string message, then send back after 1 second.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/silverlight3/


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. How to use socket to connect to socket server asynchronously?
	Silverlight only support async pattern calls. To connect remote socket:
	1. Create a SocketAsyncEventArgs, set SocketAsyncEventArgs.RemoteEndPoint 
	to socket server's endpoint	address, and register Completed event.
    2. Create a Socket instance, and call Socket.ConnectAsync method with initialized
    SocketAsyncEventArgs.
    3. When SocketAsyncEventArgs.Completed fired, check SocketAsyncEventArgs.SocketError
    Property, if equal to SocketError.Success, it means socket has connected successfully.

    Code sample can be found at SocketClient.vb "Socket async connect" region

2. How to separate socket bytes array into string message?
	There are several ways to separate message, This project use a predefined char as message
	spliter. One thing should be note that UTF8 is variable-length encoding. While decoding, 
	we need check and recover the char which be separated by byte array.
	
    Code sample at SocketClient.vb "String Decoding" region

3. How to receive string by socket?
	1. Create a SocketAsyncEventArgs, assign bytes array to SocketAsyncEventArgs.Buffer
	as receive buffer, and then register Completed event.
	2. use the connected socket to receive bytes by calling Socket.ReceiveAsync method.
	3. When SocketAsyncEventArgs.Completed fired, check SocketAsyncEventArgs.SocketError
    Property, if equal to SocketError.Success, it means socket has connected successfully.
    4. Use UTF8 decoder to decode SocketAsyncEventArgs.Buffer to string.
    
    Code sample can be found at SocketClient.vb "Socket async Receive" region

4. How to send string by socket?
	1. Use UTF8 encoder to encode string to bytes array.
	2. Create a SocketAsyncEventArgs, assign encoded bytes array to SocketAsyncEventArgs.Buffer
	as send buffer, and then register Completed event.
	3. When SocketAsyncEventArgs.Completed fired, check SocketAsyncEventArgs.SocketError
    Property, if equal to SocketError.Success, it means socket has connected successfully.

    Code sample can be found at SocketClient.vb "Socket async Send" region
    
    
/////////////////////////////////////////////////////////////////////////////
References:

Socket Class
http://msdn.microsoft.com/en-us/library/system.net.sockets.socket.aspx


/////////////////////////////////////////////////////////////////////////////
