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
using System.IO;
using System.Xml;
namespace FXTest.Log
{
    public class Log
    {
        private static readonly string Scenario = "Scenario";
        private static readonly string Testcase = "Testcase";
        private static readonly string Result = "Result";
        private static readonly string FinalResults = "FinalResults";
        private static readonly string ResultComments = "ResultComments";
        private ScenarioResult tcr = new ScenarioResult();
        private string currentScenario;
        public virtual ScenarioResult TestResults {
            get { return tcr; }
        }
        public void WriteTag(string tagName) {
            WriteTag(tagName, false, null, null);
        }
        public void WriteTag(string tagName, bool closeToo) {
            WriteTag(tagName, closeToo, null, null);
        }
        public void WriteTag(string tagName, bool closeToo, LogAttribute data) {
            if ( data == null )
                WriteTag(tagName, closeToo, null, null);
            else
                WriteTag(tagName, closeToo, null, new LogAttribute[] { data });
        }
        public void WriteTag(string tagName, bool closeToo,  LogAttribute[] data) {
            WriteTag(tagName, closeToo, null, data);
        }
        public void WriteTag(string tagName, bool closeToo, string elementText,  LogAttribute[] data) {
            Console.WriteLine(tagName);
            if (data != null) {
                foreach (LogAttribute a in data) {
                    Console.WriteLine(" " + a.Name + ": " + a.Value);
                }
            }
            Console.WriteLine("  " + elementText);
        }
        public void CloseTag() {
        }
        public virtual void StartTest(string testName) {
            WriteTag(Testcase, false, new LogAttribute("name", testName));
        }
        public virtual void StartScenario(string scenarioName, string comments) {
            WriteTag(Scenario, false, comments, new LogAttribute[] { new LogAttribute("name", scenarioName) });
            currentScenario = scenarioName;
        }
        public virtual void StartScenario(string scenarioName) {
            StartScenario(scenarioName, null);
        }
        public virtual void EndScenario(bool result) {
            string failString = "scenario \"" + currentScenario + "\" failed.";
            EndScenario(result, result ? null : failString);
        }
        public virtual void EndScenario(bool result, string comments) {
            EndScenario(new ScenarioResult(result, result ? null : comments));
        }
        public virtual void EndScenario(ScenarioResult result) {
            ResultsBlock(Result, result);
            if (result == ScenarioResult.Pass)
                tcr.PassCount++;
            else {
                tcr.FailCount++;
                if (result.Comments != null)
                    tcr.Comments = result.Comments;
            }
            CloseTag();
        }
        public virtual void EndTest() {
            ResultsBlock(FinalResults, tcr);
            CloseTag();
        }
        private void ResultsBlock(string tagName, ScenarioResult sr) {
            if ( sr.Comments == null )
                WriteTag(tagName, true, null, sr.GetResultLogAttributes());
            else {
                WriteTag(tagName, false, null, sr.GetResultLogAttributes());
                    WriteTag(ResultComments, true, sr.Comments, null);
                CloseTag();
            }
        }
        public virtual void Write(string data) {
            Console.Write(data);
        }
        public virtual void WriteLine() {
            Console.WriteLine("");
        }
        public virtual void WriteLine(string data) {
            Console.WriteLine(data);
        }
        public virtual void WriteLine(string data, object arg0) {
            WriteLine(String.Format(data, arg0));
        }
        public virtual void WriteLine(string data, object arg0, object arg1) {
            WriteLine(String.Format(data, arg0, arg1));
        }
        public virtual void WriteLine(string data, object arg0, object arg1, object arg2) {
            WriteLine(String.Format(data, arg0, arg1, arg2));
        }
        public virtual void WriteLine(string data, params object[] args) {
            WriteLine(String.Format(data, args));
        }
    }
    public class LogAttribute {
        public string Name;
        public string Value;
        public LogAttribute(string name, string value) {
            Name = name;
            Value = value;
        }
    }
}
