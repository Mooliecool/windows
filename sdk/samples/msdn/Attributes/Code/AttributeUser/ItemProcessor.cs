using System;
using System.Threading;
using System.Diagnostics;
using System.Runtime.Remoting.Messaging;
using AttributeSample.TraceLibrary;

namespace AttributeUser
{
	/// <summary>
	/// The ItemProcessor class uses the custom attribute defined in the TraceLibrary
	/// project to provide trace output services to the class based on the trace
	/// level defined in the application configuration file.
	/// </summary>
	[Trace()]
	internal sealed class ItemProcessor : ContextBoundObject
	{
    private Random randomNumber = new Random();

    /// <summary>
    /// This method simulates calls to process various items and is used to
    /// demonstrate the TraceAttribute's functionality.  Infrequently, this
    /// method will throw exceptions, and at other times will supply additional
    /// trace output in the form of Warning and Verbose messages.
    /// </summary>
    /// <remarks>
    /// Note that even if this class remained unaware that the TraceOutput object
    /// was available through the call context, the TraceAttribute would still
    /// provide tracing functionality.
    /// </remarks>
    /// <param name="itemName">The name of the item to be processed</param>
    /// <param name="total">A "ref" parameter whose value is shown (before and after the call) in the trace output</param>
    /// <returns>A new string based on the item name and the wait time generated within the method</returns>
    internal string ProcessItem(string itemName, ref int total)
    {
      // Simulate a wait time for items to be processed between 10 ms and 2 seconds.
      int simulatedWaitTime = 10 * randomNumber.Next(1, 21);
      int options = randomNumber.Next(10);

      Thread.Sleep(simulatedWaitTime);

      if (options >= 9)
        throw new ApplicationException();

      TraceOutput tracer = (TraceOutput)CallContext.GetData(TraceOutput.ContextName);
      if (options >= 6)
        tracer.WriteLine(TraceLevel.Verbose, "This a verbose message");
      else
        if (options >= 4)
          tracer.WriteLine(TraceLevel.Warning, "This is a warning message");

      // Update total to see it reflected in our trace output.
      total += simulatedWaitTime;

      // Return a value to be displayed in the trace output.
      return string.Format("{0} processed with wait-time of {1} milliseconds",
        itemName, simulatedWaitTime);
    }
	}
}
