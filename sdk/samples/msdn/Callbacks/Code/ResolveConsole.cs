using System;
using System.Net;

namespace CallbackSample
{
  /// <summary>
  /// The ResolveConsole class demonstrates various types of callbacks and
  /// asynchronous calls.
  /// </summary>
	internal sealed class ResolveConsole : IResolveCallback
	{
    private bool resultsDisplayed = false;

    // This class can only be created by the Main static method.
    private ResolveConsole()
    {
    }

    /// <summary>
    /// This method implements the IResolveCallback interface and allows
    /// a called object to inform the caller of any results.
    /// </summary>
    /// <param name="hostInformation">The host information resulting from
    /// a DNS resolution</param>
    public void HostResolved(IPHostEntry hostInformation)
    {
      DisplayResults(hostInformation);
    }

    /// <summary>
    /// This method is called by the Main static method to perform the
    /// DNS resolutions requested by the user demonstrating the callback
    /// options specified.
    /// </summary>
    /// <param name="host">An IP address or host name to resolve</param>
    /// <param name="methodToUse">The callback option to demonstrate</param>
    private void DoResolve(string host, CallbackOption methodToUse)
    {
      resultsDisplayed = false;
      IPHostEntry hostInformation = null;
      Resolver resolver = new Resolver();

      switch (methodToUse)
      {
        case CallbackOption.UseInterface:

        {
          Console.WriteLine("Resolving...");

          // By passing an interface that this object supports, the called
          // method can notify this object of the results
          try
          {
            resolver.Resolve(host, (IResolveCallback)this);
          }
          catch (System.Net.Sockets.SocketException)
          {
            Console.WriteLine("Bad host name (SocketException)");
            resultsDisplayed = true;
          }
          break;
        }

        case CallbackOption.UseSynchronousDelegate:

        {
          Console.WriteLine("Resolving...");

          // By passing a delegate wrapping the HostResolved method, the
          // called object can notify this object of the result in a synchronous
          // or asynchronous manner, depending on how it is constructed.
          ResolveCallbackDelegate cb = new ResolveCallbackDelegate(HostResolved);
          resolver.Resolve(host, cb);
          break;
        }

        case CallbackOption.UseAsynchronousDelegateWithWait:

        {
          Console.Write("Resolving");

          // By wrapping a synchronous long-running method (DNS resolution)
          // with a delegate, this object can call that method
          // asynchronously and show progress (in this case) or do other
          // work while it executes.  In this scenario, it waits on the
          // result using the WaitHandle provided by IAsyncResult.
          ResolveDelegate synchMethod = new ResolveDelegate(resolver.Resolve);
          // The BeginInvoke method is supplied by the C# compiler...
          // The IntelliSense engine does not display this at design time.
          IAsyncResult ar = synchMethod.BeginInvoke(host, null, null);

          // Write another period for each 100ms interval of wait time.
          while (!ar.AsyncWaitHandle.WaitOne(100, false))
            Console.Write(".");
          Console.WriteLine();

          // If any exceptions are raised by the called method, they won't
          // be thrown until the results are obtained.
          try
          {
            // The EndInvoke method is supplied by the C# compiler...
            // The IntelliSense engine does not display this at design time.
            hostInformation = synchMethod.EndInvoke(ar);
            DisplayResults(hostInformation);
          }
          catch (System.Net.Sockets.SocketException)
          {
            Console.WriteLine("Bad host name (SocketException)");
            resultsDisplayed = true;
          }
          break;
        }

        case CallbackOption.UseAsynchronousDelegateWithCallback:

        {
          Console.WriteLine("Resolving...");
          
          ResolveDelegate synchMethod = new ResolveDelegate(resolver.Resolve);
          AsyncCallback cb = new AsyncCallback(this.AsyncCustomCallbackMethod);
          // Begin the method's execution...when finished, the callback will be
          // called.
          IAsyncResult ar = synchMethod.BeginInvoke(host, cb, null);
          break;
        }

        case CallbackOption.UseFrameworkSuppliedSynchronousMethod:

        {
          Console.WriteLine("Resolving...");

          // This calls the synchronous version of a framework-defined class
          // that also explicitly supports asynchronous invocation.
          try
          {
            hostInformation = Dns.Resolve(host);
            DisplayResults(hostInformation);
          }
          catch (System.Net.Sockets.SocketException)
          {
            Console.WriteLine("Bad host name (SocketException)");
            resultsDisplayed = true;
          }
          break;
        }

        case CallbackOption.UseFrameworkSuppliedAsynchronousMethodWithWait:

        {
          Console.Write("Resolving");

          IAsyncResult ar = Dns.BeginResolve(host, null, null);

          // Write another period for each 100ms interval of wait time.
          while (!ar.AsyncWaitHandle.WaitOne(100, false))
            Console.Write(".");
          Console.WriteLine();

          // If any exceptions are raised by the called method, they won't
          // be thrown until the results are obtained.
          try
          {
            hostInformation = Dns.EndResolve(ar);
            DisplayResults(hostInformation);
          }
          catch (System.Net.Sockets.SocketException)
          {
            Console.WriteLine("Bad host name (SocketException)");
            resultsDisplayed = true;
          }
          break;
        }

        case CallbackOption.UseFrameworkSuppliedAsynchronousMethodWithCallback:

        {
          Console.WriteLine("Resolving...");

          AsyncCallback cb = new AsyncCallback(this.AsyncFrameworkCallbackMethod);
          // Begin the call...when it is finished, the callback method will be
          // called.
          IAsyncResult ar = Dns.BeginResolve(host, cb, null);
          break;
        }

        default:
          Console.WriteLine("Not Implemented Yet");
          break;
      }

      // If this method ends now, there is no guarantee that the host information
      // will be displayed before the next prompt to the user for more hosts to
      // resolve is shown.  In order to force the wait, put the thread to sleep
      // for 100ms intervals until the output has been displayed.
      while (!resultsDisplayed)
      {
        // For the synchronous options, this will never get executed
        // because the results will have been displayed before execution
        // reaches this point.
        System.Threading.Thread.Sleep(100);
      }
    }

    private void AsyncFrameworkCallbackMethod(IAsyncResult ar)
    {
      // If any exceptions are raised by the called method, they won't
      // be thrown until the results are obtained.
      try
      {
        IPHostEntry hostInformation = Dns.EndResolve(ar);
        DisplayResults(hostInformation);
      }
      catch (System.Net.Sockets.SocketException)
      {
        Console.WriteLine("Bad host name (SocketException)");
        resultsDisplayed = true;
      }
    }

    [System.Runtime.Remoting.Messaging.OneWay()]
    private void AsyncCustomCallbackMethod(IAsyncResult ar)
    {
      // If any exceptions are raised by the called method, they won't
      // be thrown until the results are obtained.
      try
      {
        // Unwrap the delegate so that the EndInvoke method can be called.
        ResolveDelegate synchMethod = (ResolveDelegate)((System.Runtime.Remoting.Messaging.AsyncResult)ar).AsyncDelegate;
        IPHostEntry hostInformation = synchMethod.EndInvoke(ar);
        DisplayResults(hostInformation);
      }
      catch (System.Net.Sockets.SocketException)
      {
        Console.WriteLine("Bad host name (SocketException)");
        resultsDisplayed = true;
      }
    }

    /// <summary>
    /// Display the host information from an IPHostEntry object
    /// </summary>
    /// <param name="hostInformation">An IPHostEntry object containing host information</param>
    private void DisplayResults(IPHostEntry hostInformation)
    {
      Console.WriteLine("Host Information:\nName: \"{0}\"", 
        hostInformation.HostName);
      if (hostInformation.Aliases.Length > 0)
      {
        Console.WriteLine("Aliases Found:");
        foreach (string alias in hostInformation.Aliases)
          Console.WriteLine("  {0}", alias);
      }
      if (hostInformation.AddressList.Length > 0)
      {
        Console.WriteLine("IP Addresses:");
        foreach (IPAddress ipAddress in hostInformation.AddressList)
          Console.WriteLine("  {0}", ipAddress.ToString());
      }
      // Set the resultsDisplayed value.
      resultsDisplayed = true;
    }

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
      ResolveConsole app = new ResolveConsole();

      string userInput;
      do
      {
        Console.WriteLine("Please enter an IP address or host name to " +
          "resolve, or the letter 'x' to quit.");
        userInput = Console.ReadLine().ToLower().Trim();

        // If the user wants to quit, then exit the main loop.
        if (userInput == "x")
          break;

        // Validate the user input, and reprompt the user if invalid.
        UriHostNameType hostType = Uri.CheckHostName(userInput);
        if (hostType == UriHostNameType.Dns || hostType == UriHostNameType.IPv4 ||
          hostType == UriHostNameType.IPv6)
        {
          string host = userInput;

          // Prompt the user for the method to use when resolving the host.
          // The methods supported are defined in the CallbackOption enumeration.
          object enumValue;
          do 
          {
            // First, show the user what valid options exist.
            string[] names = Enum.GetNames(typeof(CallbackOption));
            int[] values = (int[])Enum.GetValues(typeof(CallbackOption));

            Console.WriteLine("Valid Options Are:");
            for (int index = 0; index < values.Length; index++)
            {
              Console.WriteLine("{0,2} = {1}", values[index], names[index]);
            } // end option enumeration (for loop)

            // Prompt the user
            Console.Write("Please indicate the method you would like " +
              "to use to receive results: ");
            userInput = Console.ReadLine().Trim();

            // Cast to an int value, if possible...otherwise retain string input.
            try 
            {
              enumValue = int.Parse(userInput);
            } // end try
            catch (Exception)
            {
              enumValue = userInput;
            } // end catch

            // Validate the user input and reprompt if invalid.
          } while (!Enum.IsDefined(typeof(CallbackOption), enumValue)); // end option loop

          // Parse the input to an entry in the CallbackOption enumeration.
          CallbackOption methodToUse = (CallbackOption)Enum.Parse(
            typeof(CallbackOption), userInput, true
            );

          app.DoResolve(host, methodToUse);
        } // end valid host entered.
        else
        {
          Console.WriteLine("The IP address or host name you entered was " +
            "formatted incorrectly, please try again.");
        } // end invalid host entered.

        // Continue looping until the user wants to quit.
      } while (true); // end primary loop.

      app = null;
		} // end Main method
	} // end class
} // end namespace
