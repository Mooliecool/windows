========================================================================
            WPF APPLICATION : VBWPFThreading Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBWPFThreading sample project illustrates two WPF threading models. The
first one divides a long-running process into many snippets of workitems.  
Then the dispather of WPF will pick up the workitems one by one from the 
queue by their priority. The background workitem does not affect the UI 
operation, so it just looks like the background workitem is processed by 
another thread. But actually, all of them are executed in the same thread. 
This trick is very useful if you want single threaded GUI application, and 
also want to keep the GUI responsive when doing expensive operations in the 
UI thread. 

The second model is similar to the traditional WinForm threading model. The 
background work item is executed in another thread and it calls the 
Dispatcher.BeginInvoke method to update the UI.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

Long-Running Calculation in UI Thread (Tab1)

1.When the start button is firstly clicked, the continueCalculating is false,
so the codes call Dispatcher.BeginInvoke to execute a first workitem for the
CheckNextNumber.

2.In the CheckNextNumber function, it judge if the current number is a prime
number. If yes, it updates the UI directly. If not, it call BeginInvoke to 
execute the CheckNextNumber again for the next odd number.

3.In the CheckNextNumber function, because the first parameter passed into 
BeginInvoke is DispatcherPriority.SystemIdle, all of the CheckNextNumber
workitem will not break the UI operation. 

4.When the calculation goes on, we can draw in the UI's InkCanvas, this proves
that the UI thread is not affected by the "background" long calculation.


Blocked Operation in Worker Thread (Tab2)

1.When the Retrieve Data from Server button is clicked, the click handle
retrieveData function is called.

2.Then our codes use delegate.BeginInvoke to start a thread from the thread
pool. This thread is used to perform the long operation of retrieving data.

3.We use Thread.Sleep(5000) to simulate a 5 seconds delay here.

4.The codes generate 4 random numbers as data and update them to the UI by
calling the Dispatcher.BeginInvoke()

5.Since the retrieving operation is executed in another thread, we can draw
in the InkCanvas normally.


/////////////////////////////////////////////////////////////////////////////
References:

WPF Threading Model
http://msdn.microsoft.com/en-us/library/ms741870.aspx


/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Start Without Debugging, and the mian window of the project will show.

Step3. In the first tap, click the start button and the the prime number runs 
in the textbox; meanwhile, draw in the canvase.

Step4. Select the second tap, Retrieve Data from Server button, and meanwhile 
draw in the canvase, after 5 seconds it shows 4 random number less than 1000.
/////////////////////////////////////////////////////////////////////////////