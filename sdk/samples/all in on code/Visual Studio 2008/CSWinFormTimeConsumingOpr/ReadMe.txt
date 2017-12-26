================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormTimeConsumingOpr Project Overview
       
                        Time-consuming Operation Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Time-consuming Operation sample demonstrates how to use the BackgroundWorker 
component to execute a time-consuming operation in the background.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

To execute a time-consuming operation in the background, create a BackgroundWorker 
and listen for events that report the progress of your operation and signal when 
your operation is finished. You can create the BackgroundWorker programmatically 
or you can drag it onto your form from the Components tab of the Toolbox. 
If you create the BackgroundWorker in the Windows Forms Designer, it will appear 
in the Component Tray, and its properties will be displayed in the Properties window.

To set up for a background operation, add an event handler for the DoWork event. 
Call your time-consuming operation in this event handler. To start the operation, 
call RunWorkerAsync. To receive notifications of progress updates, 
handle the ProgressChanged event. To receive a notification when the operation 
is completed, handle the RunWorkerCompleted event. 


/////////////////////////////////////////////////////////////////////////////
References:

1. BackgroundWorker Class
   http://msdn.microsoft.com/en-us/library/system.componentmodel.backgroundworker.aspx
   
2. Windows Forms General FAQ.
   http://social.msdn.microsoft.com/Forums/en-US/winforms/thread/77a66f05-804e-4d58-8214-0c32d8f43191
   

/////////////////////////////////////////////////////////////////////////////
