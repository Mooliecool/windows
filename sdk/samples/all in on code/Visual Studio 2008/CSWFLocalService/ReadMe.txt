=============================================================================
    CONSOLE APPLICATION : CSWFLocalService
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

How to play the game:
When the game start,workflow instance will generate a random number,then you 
input a number between 1 and 10,if your input larger than the random number,
the game will ask you to input a smaller number. if your input number smaller 
than the random number,the game will ask you to input a larger one.Until your 
input number equel to the ramdon number, the game will tell you your input hit 
the answer - show a message:"right".

I create the sample to demonstrate:
1. How to send a message from workflow instance to local service?
   When the game start and every time you input a number,game workflow instance 
will send out a message to the player(local service),we use 
CallExternalMethodActivity to do it.More info about CallExternalMethodActivity
please check:
http://xhinker.com/2009/10/27/WFWhatIsCallExternalMethodActivityAndHowToUseIt.aspx
2. How to send a message from workflow host to workflow instance?
   When you input a number,how could the host send the input number to workflow
instance?We use HandleExternalEventActivity to do it,for a example usage of
HandleExternalEventActivity,please check:
http://xhinker.com/2009/10/29/WFHandleExternalEventActivity.aspx
3. How to send a message from local service to workflow host?
   We use CallExternalMethodActivity to send data from the workflow to local
service.In this game,send message to local service(I mean GuessNumberGameService)
is not enough.To interact with player, we should let the message end at workflow
host. We can use event to do it.declare a event in GuessNumberGameService,handler
the event in workflow host(Program.cs),warp message with MessageEventArgs.


/////////////////////////////////////////////////////////////////////////////
Prerequisite

1. Visual Studio 2008
2. .NET Framework 3.5


/////////////////////////////////////////////////////////////////////////////
Creation:

1.Create a Event Arg - MessageEventArgs.cs
2.Create Local Service - IGuessNumberGameService.cs;GuessNumberGameService.cs
3.Create WF - GuessNumberGameWF.cs
4.Host the workflow - Program.cs

For more detail info, please check:
http://xhinker.com/2009/11/01/WFBuildAGuessNumberGameWithWindowWorkflowFoundation35.aspx


/////////////////////////////////////////////////////////////////////////////
References:

http://xhinker.com/2009/11/01/WFBuildAGuessNumberGameWithWindowWorkflowFoundation35.aspx


/////////////////////////////////////////////////////////////////////////////