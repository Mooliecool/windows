========================================================================
    SILVERLIGHT APPLICATION : CSSL3LocalMessage Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project create a whiteboard application demonstrating how to use local message
in silverlight 3. To test this local messaging sample, open TestPage.html in two
browsers, draw on one of the application, another one would keep synchronous.

/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. How does this sample working?
    1. When starting application, use localmessagereceiver and localmessagesender
       to create a duplex communication channel.
    2. When drawing a stroke, serialize stroke object to a string, use localmessagesender
       send string to another applciation.
    3. When localmessagereceiver received stroke string, deserialize to stroke object,
       add to InkPresenter.

2. How to establish duplex communication channel between two application?
    1. Preassign two names as LocalMessageReceiver name.
    2. Use one of its name to create LocalMessageReeciver, register messagereceived event,
       start listening by calling LocalMessageRecevier.Listen(). if got exception, it means 
       another application with same name in domain has started listening already, try use 
       another preset name to create receiver.
    3. When initializing LocalMessageReceiver successful, create LocalMessageSender targeting
       to another application's recever. Register messagesended event, handling the message send
       state and response message there. 

3. How to serialize/deserialize object for transfering by local message?
    Local message only accept text format message, to transfer object, we could use Xml or JSON
    Serializer. This sample use DataContractJsonSerializer to serialize/deserialize object.
    To serialize Stroke object to json string:
    
        // Serialize stroke object to string.
        var stream = new MemoryStream();
        _jsonserializer.WriteObject(stream, _newStroke);
        stream.Flush();
        stream.Position = 0;
        var obstring = new StreamReader(stream).ReadToEnd();
        stream.Close();
        
    To deserialize string to Stroke object:
    
        // Deserialize json string to stroke object.
        var stream = new MemoryStream();
        var streamwriter = new StreamWriter(stream);
        streamwriter.Write(e.Message);
        streamwriter.Flush();
        var receivedstroke = _jsonserializer.ReadObject(stream) as Stroke;
        stream.Close();
    
    For details about DataContractJsonSerializer, please check msdn article
    http://msdn.microsoft.com/en-us/library/system.runtime.serialization.json.datacontractjsonserializer(VS.95).aspx
    
4. How to use implement drawing function?
    The drawing function is implemented by using InkPresenter.
    1. When mouseleftbuttondown, create new Stroke as currentstroke, add to InkPresenter.
    2. While mousemoving, if currentstroke is not null, add current position as new StylusPoints
       to currentstroke.
    3. When mouseleftbuttondown, set currentstroke to null.
    
    For creating more complex drawing function with inkprsenter, you could check  
    http://msdn.microsoft.com/en-us/magazine/cc721604.aspx

/////////////////////////////////////////////////////////////////////////////
References:

Communication Between Local Silverlight-Based Applications
http://msdn.microsoft.com/en-us/library/dd833063(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
