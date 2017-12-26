using System;
using System.Diagnostics;
using System.Configuration;
using System.Runtime.Remoting.Contexts;
using System.Runtime.Remoting.Activation;

namespace AttributeSample.TraceLibrary
{
	/// <summary>
	/// This class defines the custom attribute in this sample.
	/// </summary>
	[AttributeUsage(AttributeTargets.Class)]
	public class TraceAttribute : ContextAttribute
	{
    private const TraceLevel defaultTraceLevel = TraceLevel.Info;
    private string defaultTraceLog = Process.GetCurrentProcess().MainModule.FileName + ".tracelog";

    /// <summary>
    /// Create a new instance of this attribute. Classes that inherit from ContextBoundObject
    /// and apply this attribute will make this call automatically.
    /// </summary>
		public TraceAttribute() : base("TraceOutput")
		{
		}

    /// <summary>
    /// Add a context property to the context bound object.  This context property
    /// defines an "aspect" in AOP that can modify the behavior of the object or
    /// provide additional services to the object.
    /// </summary>
    /// <param name="ctorMsg">The construction call to the object this attribute is
    /// applied to</param>
    public override void GetPropertiesForNewContext(IConstructionCallMessage ctorMsg)
    {
      string traceLog = defaultTraceLog;
      TraceLevel traceLevel = defaultTraceLevel;

      // Read in the settings for TraceLevel and TraceLog.  If not found, or incorrect,
      // then use the default settings.
      string traceLevelSetting = ConfigurationSettings.AppSettings["TraceLevel"];
      if (traceLevelSetting != null)
      {
        if (Enum.IsDefined(typeof(TraceLevel), traceLevelSetting))
          traceLevel = (TraceLevel)Enum.Parse(typeof(TraceLevel), traceLevelSetting, true);
        else
          traceLevel = defaultTraceLevel;
      }

      string traceLogSetting = ConfigurationSettings.AppSettings["TraceLog"];
      if (traceLogSetting != null)
        traceLog = traceLogSetting;

      if (Trace.Listeners["TraceOutput"] == null)
        Trace.Listeners.Add(new TextWriterTraceListener(traceLog, "TraceOutput"));

      // Add the TraceOutputProperty to the context of the newly created object.
      ctorMsg.ContextProperties.Add(new TraceOutputProperty(traceLevel));
    }
	}
}
