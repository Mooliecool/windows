using System;
using System.Diagnostics;
using System.Runtime.Remoting.Contexts;
using System.Runtime.Remoting.Messaging;

namespace AttributeSample.TraceLibrary
{
	/// <summary>
	/// The TraceOutputProperty describes a specific behavior that can be applied
	/// to a ContextBoundObject.  In this sample, it is the "glue" between the
	/// custom TraceAttribute and the functionality of TraceOutput.
	/// </summary>
	public class TraceOutputProperty : IContextProperty, IContributeObjectSink
	{
    private TraceLevel _traceLevel;

    /// <summary>
    /// Creates an instance of the TraceOutputProperty with the specified TraceLevel.
    /// </summary>
    /// <param name="traceLevel">The level of tracing desired</param>
    public TraceOutputProperty(TraceLevel traceLevel)
    {
      _traceLevel = traceLevel;
    }

    /// <summary>
    /// Returns an instance of TraceOutput
    /// </summary>
    /// <param name="obj"></param>
    /// <param name="nextSink">The next sink in the chain of "interested" code</param>
    /// <returns></returns>
    public IMessageSink GetObjectSink(MarshalByRefObject obj, IMessageSink nextSink)
    {
      return new TraceOutput(_traceLevel, nextSink);
    }

    /// <summary>
    /// Provided as an implementation of the IContextProperty interface
    /// </summary>
    /// <param name="newCtx"></param>
    /// <returns></returns>
    public bool IsNewContextOK(Context newCtx)
    {
      return true;
    }

    /// <summary>
    /// Provided as an implementation of the IContextProperty interface
    /// </summary>
    /// <param name="newCtx"></param>
    public void Freeze(Context newCtx)
    {
    }

    /// <summary>
    /// Provided as an implementation of the IContextProperty interface
    /// </summary>
    public string Name
    {
      get
      {
        return this.GetType().Name;
      }
    }
	}
}
