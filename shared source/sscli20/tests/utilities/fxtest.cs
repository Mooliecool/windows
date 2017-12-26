// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
using System;
using System.Collections;
using System.Reflection;
using System.Diagnostics;
using System.Security;
using System.Security.Permissions;
using System.Security.Policy;
using FXTest.Log;
namespace FXTest.FXQATestLib
{
    public class FXQATestBase  
    {
        private FXTest.Log.Log log = null;        
        protected static String testName = "defaultTestName";
        protected static MethodInfo[] tests;
        private bool baseInitTestCalled = false;
        private bool initPassed = false;
        protected FXQATestBase() : base()
        {
            if (log == null)
                log = new FXTest.Log.Log();        
            testName = this.GetType().Name;
            log.StartTest(testName);
            log.WriteTag("TestInitialize", false);
            try {
                InitTest();
                if ( baseInitTestCalled )
                    initPassed = true;
                else {
                    initPassed = false;
                    log.WriteLine("*****ERROR: FXQATestBase.InitTest() was not called!");
                    log.WriteLine("*****All test classes which override InitTest() must call base.InitTest().");
                }
            }
            catch (Exception e) {
                log.WriteLine("Test initialization failed :");
                log.WriteLine();
                log.WriteLine(e.ToString());
            }
            log.CloseTag();
        }    
        [EnvironmentPermission(SecurityAction.Assert, Unrestricted=true)]
        public string GetCurrentDirectory(){
                return Environment.CurrentDirectory ;
        }
        public void RunTestCases()
        {
            if ( initPassed )
                TestEngine();
            else
                TestIsDone();        
        }
        public FXTest.Log.Log Log
        {
                get
                {
                        return log ;
                } 
        }
        protected virtual void InitTest() {
            baseInitTestCalled = true;
            WriteLine("Start time: " + DateTime.Now.ToString(null, null));
        }
        protected virtual bool ExecuteScenario(MethodInfo mi)
        {
            String sTestDesc;
            sTestDesc = mi.Name ;
            WriteLine("");
            log.StartScenario(sTestDesc);
            ScenarioResult testResult = InvokeMethod(mi);
            if ( (object)testResult == null )
                testResult = new ScenarioResult(false, "***Test returned null ScenarioResult!***");
            log.EndScenario(testResult);
            return (testResult == ScenarioResult.Pass);
        }
        private ScenarioResult InvokeMethod(MethodInfo mi)
        {
            ScenarioResult sr = new ScenarioResult(false,
                    "ScenarioResult was not set in FAXQATestBase.InvokeMethod()");
            try
            {
                try {
                    sr = (ScenarioResult)(mi.Invoke(this, null));
                }
                finally {
                }
                if ((object)sr == (object)ScenarioResult.Fail)
                    sr = new ScenarioResult(false);
                if ((sr == ScenarioResult.Fail) && (sr.Comments == null))
                    sr.Comments = mi.Name + " FAILED";
            }
            catch(Exception ex)
            {
                sr = new ScenarioResult(false, mi.Name + " excepted: ");
                if (ex is TargetInvocationException)
                    ex = ((TargetInvocationException)ex).InnerException;
                sr.Comments = sr.Comments + ex.GetType().ToString() + ": " + ex.Message;
                WriteLine(sr.Comments + "\r\n" + ex.StackTrace);
            }
            return sr;
        }
        protected virtual void TestEngine()
        {
            try
            {
                tests = GetAllScenarios(this);
                bool bPass = true;
                for (int j = 0; j < tests.Length; j++)
                {
                    WriteLine("");
                    log.WriteTag("ScenarioGroup", false, new LogAttribute("name", "All"));
                    bool b = ExecuteScenario(tests[j]);
                    if (!b) {
                        Console.WriteLine("FAILED");
                        Environment.Exit(1234);
                    }
                    bPass = b && bPass;        
                }
                TestIsDone();
            }
            catch (Exception ex)
            {
                if (ex is TargetInvocationException)
                {
                    TargetInvocationException ite;
                    ite = (TargetInvocationException)ex;
                    ex = ite.InnerException;
                }
                WriteLine(ex.ToString());
                TestIsDone();
            }
        }
        protected virtual void TestIsDone()
        {           
            log.EndTest();
        }
        [ReflectionPermission(SecurityAction.Assert, Unrestricted=true)]       
        public static MethodInfo[] GetAllScenarios(Object o)
        {           
            ArrayList l = new ArrayList();
            Type typ = o.GetType();
            MethodInfo[] mi = typ.GetMethods( BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.DeclaredOnly);
            for (int i=0; i<mi.Length; i++)
            {
                if (mi[i].ReturnType == typeof(ScenarioResult))
                {
                    l.Add(mi[i]);
                }
            }
            return (MethodInfo[])l.ToArray(typeof(MethodInfo));            
        }
        private void WriteLine(String strMessage){
            log.WriteLine( strMessage );                        
        }
    }    
}
