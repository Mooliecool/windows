using System;
using System.Activities;
using System.Activities.Statements;


namespace CSWF4LongRunningHost
{
    class Program
    {
        static void Main(string[] args)
        {
            LongRunningWFHost host = new LongRunningWFHost(new Workflow1());
            host.Run();
        }
    }
}
