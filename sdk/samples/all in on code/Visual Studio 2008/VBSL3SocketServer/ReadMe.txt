========================================================================
    SILVERLIGHT APPLICATION : VBSL3SocketServer Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project create a socket server sample, which could serve both sivlerlight
and normal socket client.
This socket server will accept client connection, then receive string message and 
send back the respone message.     
    

/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBSL3SocketServer <--> VBSL3SocketClient
VBSL3SocketClient is Silverlight Socket client.


/////////////////////////////////////////////////////////////////////////////
Demo:

To run the socket sample, please try the following steps:
1. To start socket server
	a. Open CSSLSocketServer solution with Administrator account, compile.
	b. Run the project.
2. To run the silverlight socket client
	a. Open VBSL3SocketClient solution, compile and run.
	b. When Silverlight application loaded, follow the instructions displayed on page:
		1) Click "connect" to connect to socket server.
		2) Input text in textbox and click "send" button.
		3) Server will receive and handle string message, then send back after 1 second.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. What's the difference between socket server for Silverlight and normal socket server?
	Due to security reason, before Silverlight client connect to socket server, it will
	try to connect server 943 port to request access policy. This project create two
	socket listener, one for serving client communication, another is listening to 943
	port to respond silverlight client's policy request.
	For details about how Socket server works, please refer to another socket server 
	sample project: CSSocketServer
   
/////////////////////////////////////////////////////////////////////////////
References:

Socket Class
http://msdn.microsoft.com/en-us/library/system.net.sockets.socket.aspx

Network Security Access Restrictions in Silverlight
http://msdn.microsoft.com/en-us/library/cc645032(VS.95).aspx

/////////////////////////////////////////////////////////////////////////////
