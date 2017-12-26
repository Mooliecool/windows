using System;
using System.Text;
using System.Diagnostics;
using System.Runtime.Remoting.Messaging;

namespace AttributeSample.TraceLibrary
{
	/// <summary>
	/// The TraceOutput class defines the core behavior implied by the TraceAttribute.
	/// For each call to an object that the TraceAttribute is applied to, this class
	/// will intercept method calls to and from that object, and will write out
	/// trace information appropriate to the level setting currently in effect.
	/// </summary>
	public sealed class TraceOutput : IMessageSink
	{
    private const int TRACE_OUTPUT_BUFFER_SIZE = 2048;

    private IMessageSink _nextSink;
    private TraceLevel _traceLevel;
    private DateTime _timeOfMethodCall;

		internal TraceOutput(TraceLevel traceLevel, IMessageSink nextSink)
		{
      _traceLevel = traceLevel;
      // Store the next sink to respond to calls to IMessageSink.NextSink
      _nextSink = nextSink;
		}

    /// <summary>
    /// This property is used by the remoting framework to obtain the next
    /// bit of behavior that is attached to the ContextBoundObject.
    /// </summary>
    public IMessageSink NextSink
    {
      get
      {
        return _nextSink;
      }
    }

    /// <summary>
    /// This method intercepts method calls to the target object and adds on
    /// the trace output behavior desired.
    /// </summary>
    /// <param name="msg"></param>
    /// <returns></returns>
    public IMessage SyncProcessMessage(IMessage msg)
    {
      TraceMethodCall(msg);
      // Finish processing the incoming message by passing it to the various
      // bits of code that are "interested" in the call.
      IMessage replyMsg = _nextSink.SyncProcessMessage(msg);
      // Process the outgoing message and return the reply to the caller or
      // the next sink in the chain.
      TraceMethodReturn(msg, replyMsg);
      return replyMsg;
    }

    /// <summary>
    /// Trace logging for asynchronous calls is not supported in this sample.
    /// </summary>
    /// <param name="msg"></param>
    /// <param name="replySink"></param>
    /// <returns></returns>
    public IMessageCtrl AsyncProcessMessage(IMessage msg, IMessageSink replySink)
    {
      throw new InvalidOperationException();
    }

    /// <summary>
    /// The ContextName is used by the objects to which this behavior is attached
    /// to obtain a reference to the behavior.  This allows the object to participate
    /// in the behaviors that surround it.
    /// </summary>
    public static string ContextName
    {
      get
      {
        return "TraceOutput";
      }
    }

    /// <summary>
    /// Provide trace output for the incoming method call.
    /// </summary>
    /// <param name="msg">The message representing the incoming method call.</param>
    private void TraceMethodCall(IMessage msg)
    {
      IMethodMessage methodCall = msg as IMethodMessage;

      // Process method calls only
      if (methodCall == null) return;

      // If the TraceLevel is set to "Info" or "Verbose", log the
      // method call.
      if (((int)_traceLevel) >= ((int)TraceLevel.Info))
      {
        // Store the time of the call, this will be used later to show elapsed time.
        _timeOfMethodCall = DateTime.Now;

        // Add information about the call to the trace output.
        StringBuilder textOutput = new StringBuilder(TRACE_OUTPUT_BUFFER_SIZE);
        textOutput.Append(Type.GetType(methodCall.TypeName).Name);
        textOutput.Append(".");
        textOutput.Append(methodCall.MethodName);
        textOutput.Append("(");
        for (int index = 0; index < methodCall.ArgCount; index++)
        {
          if (index > 0) textOutput.Append(", ");
          textOutput.Append(methodCall.GetArgName(index));
          textOutput.Append(" = ");
          textOutput.Append(methodCall.GetArg(index).ToString());
        }
        textOutput.Append(")");

        // TODO: Write Trace Output.
        Trace.WriteLine(textOutput.ToString(), "Method Call");
        Trace.Indent();
      }

      // Add this object to the call context.
      methodCall.LogicalCallContext.SetData(ContextName, this);
    }

    /// <summary>
    /// This functionality is used by the ContextBoundObject to which this
    /// behavior is attached.
    /// </summary>
    /// <param name="minimumTraceLevel">The minimum trace level for which
    /// the output is intended</param>
    /// <param name="value">The value to write to the trace output</param>
    public void Write(TraceLevel minimumTraceLevel, object @value)
    {
      Write(minimumTraceLevel, @value.ToString());
    }

    /// <summary>
    /// This functionality is used by the ContextBoundObject to which this
    /// behavior is attached.
    /// </summary>
    /// <param name="minimumTraceLevel">The minimum trace level for which
    /// the output is intended</param>
    /// <param name="value">The value to write to the trace output</param>
    public void WriteLine(TraceLevel minimumTraceLevel, object @value)
    {
      WriteLine(minimumTraceLevel, @value.ToString());
    }

    /// <summary>
    /// This functionality is used by the ContextBoundObject to which this
    /// behavior is attached.
    /// </summary>
    /// <param name="minimumTraceLevel">The minimum trace level for which
    /// the output is intended</param>
    /// <param name="message">The message to write to the trace output</param>
    public void Write(TraceLevel minimumTraceLevel, string message)
    {
      if ((((int)_traceLevel) >= ((int)minimumTraceLevel)) &&
        (_traceLevel != TraceLevel.Off))
      {
        Trace.Write(message);
      }
    }

    /// <summary>
    /// This functionality is used by the ContextBoundObject to which this
    /// behavior is attached.
    /// </summary>
    /// <param name="minimumTraceLevel">The minimum trace level for which
    /// the output is intended</param>
    /// <param name="message">The nessage to write to the trace output</param>
    public void WriteLine(TraceLevel minimumTraceLevel, string message)
    {
      if ((((int)_traceLevel) >= ((int)minimumTraceLevel)) &&
        (_traceLevel != TraceLevel.Off))
      {
        Trace.WriteLine(message, "Trace Message");
      }
    }

    /// <summary>
    /// Provide trace output for the method return.  It is at this point that
    /// exceptions and return values will be logged (if available), and the
    /// elapsed time for the method to execute is calculated.
    /// </summary>
    /// <param name="msg">The incoming method call</param>
    /// <param name="returnMsg">The method return</param>
    private void TraceMethodReturn(IMessage msg, IMessage returnMsg)
    {
      IMethodReturnMessage methodReturn = returnMsg as IMethodReturnMessage;

      // Handle method returns only
      if ((methodReturn == null) || !(msg is IMethodMessage)) return;

      // If no tracing is performed, exit.
      if (_traceLevel == TraceLevel.Off) return;

      // If the TraceLevel is set to "Info" or "Verbose", the method call
      // was logged and the indent increased.  Decrease it now.
      if (((int)_traceLevel) >= ((int)TraceLevel.Info))
        Trace.Unindent();

      // Calculate the time spent in the method call.
      TimeSpan elapsedTimeInCall = DateTime.Now - _timeOfMethodCall;

      StringBuilder textOutput = new StringBuilder(TRACE_OUTPUT_BUFFER_SIZE);

      // If an exception occurred, log it to the trace log and return.
      Exception e = methodReturn.Exception;
      if (e != null)
      {
        textOutput.Append(Type.GetType(methodReturn.TypeName).Name);
        textOutput.Append(".");
        textOutput.Append(methodReturn.MethodName);
        textOutput.Append("(");
        for (int index = 0; index < methodReturn.ArgCount; index++)
        {
          if (index > 0) textOutput.Append(", ");
          textOutput.Append(methodReturn.GetArgName(index));
          textOutput.Append(" = ");
          textOutput.Append(methodReturn.GetArg(index).ToString());
        }
        textOutput.Append(")\n");
        textOutput.Append("----- EXCEPTION -----\n");
        textOutput.Append(e.ToString());
        textOutput.Append("\n----- EXCEPTION -----");

        Trace.WriteLine(textOutput.ToString(), string.Format(
          "Method Return - {0} ms", elapsedTimeInCall.TotalMilliseconds));
        return;
      }

      // If the trace level is set to "Info" or "Verbose", then log the
      // return values from the call as well as the time spent in the call.
      if (((int)_traceLevel) >= ((int)TraceLevel.Info))
      {
        textOutput.Append(Type.GetType(methodReturn.TypeName).Name);
        textOutput.Append(".");
        textOutput.Append(methodReturn.MethodName);
        textOutput.Append("(");
        if (methodReturn.OutArgCount > 0)
        {
          textOutput.Append("ref/out parameters[");
          for (int index = 0; index < methodReturn.OutArgCount; index++)
          {
            if (index > 0) textOutput.Append(", ");
            textOutput.Append(methodReturn.GetOutArgName(index));
            textOutput.Append(" = ");
            textOutput.Append(methodReturn.GetOutArg(index).ToString());
          }
          textOutput.Append("]");
        }

        if (methodReturn.ReturnValue.GetType() != typeof(void))
        {
          if (methodReturn.OutArgCount > 0)
            textOutput.Append(", ");
          textOutput.Append("returned [");
          textOutput.Append(methodReturn.ReturnValue.ToString());
          textOutput.Append("]");
        }
        textOutput.Append(")");

        Trace.WriteLine(textOutput.ToString(), string.Format(
          "Method Return - {0} ms", elapsedTimeInCall.TotalMilliseconds));
      }
    }
	}
}
