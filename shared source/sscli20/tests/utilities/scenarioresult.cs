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
namespace FXTest.Log
{
    public class ScenarioResult
    {
        private int pass, fail;
        private bool _type = false;
        private String comments;
        public ScenarioResult() {
            pass = fail = 0;
        }
        public ScenarioResult(bool b) : this() {
            this._type = b;
        }
        public ScenarioResult(bool b, string comments) : this(b) {
            if (!b)
                Comments = comments;
        }
        public ScenarioResult(bool b, string comments, Log log) : this(b, comments) {
            if ( !b && log != null )
                log.WriteLine(comments);
        }
        public static ScenarioResult Pass {
            get {
                return new ScenarioResult(true);
            }
        }        
        public static ScenarioResult Fail {
            get {
                return new ScenarioResult(false);
            }
        }
        public virtual String Comments {
            get {
                return comments;
            }
            set {
                this.comments = value;
            }
        }
        public virtual int PassCount {
            get {
                return pass;
            }
            set {
                pass = value;
                _type = (FailCount == 0);
            }
        }
        public virtual int FailCount {
            get {
                return fail;
            }
            set {
                fail = value;
                _type = (FailCount == 0);
            }
        }
        public virtual int TotalCount {
            get {
                return pass + fail;
            }
        }
        public void IncCounters (bool b) {
            if ( b )
                ++this.pass;
            else
                ++this.fail;
            _type = (FailCount == 0);
        }
        public void IncCounters (bool b, String comments) {
            IncCounters(b);
            if ( !b )
                Comments = comments;
        }
        public void IncCounters (bool b, String comments, Log log) {
            IncCounters(b, comments);
            if ( !b && log != null )
                log.WriteLine(Comments);
        }
        public void IncCounters(ScenarioResult result) {
            if ( result == null )
                throw new ArgumentNullException("result can't be null");
            if ( result.TotalCount == 0 )
                PassCount++;
            else
                PassCount += result.PassCount;
            if ( !result._type ) {                  
                Comments = result.Comments;
                if ( result.TotalCount == 0 )
                    FailCount++;
                else
                    FailCount += result.FailCount;
            }
        }
        public override bool Equals(object obj) {
            if ( obj is ScenarioResult )
                return this == (ScenarioResult)obj;     
            return false;
        }
        public static bool operator ==(ScenarioResult a, ScenarioResult b) {
            if ( (object)a == null && (object)b == null )
                return true;
            if ( (object)a == null || (object)b == null )
                return false;
            return a._type == b._type;
        }
        public static bool operator !=(ScenarioResult a, ScenarioResult b) {
            return !(a == b);
        }
        public override int GetHashCode() {
            return base.GetHashCode();
        }
        private static String NameValuePair(String name, String value)
        {
            return name + "=\"" + value + "\"";
        }        
        public override String ToString() {
            String result = NameValuePair("type", _type ? "Pass" : "Fail");
            if (TotalCount != 0) {
                String total = NameValuePair("total", TotalCount.ToString());
                String failed = NameValuePair("fail", FailCount.ToString());
                result += " " + total + " " + failed;
            }
            return result;
        }
        internal LogAttribute[] GetResultLogAttributes() {
            LogAttribute type = new LogAttribute("type", _type ? "Pass" : "Fail");
            if ( TotalCount == 0 )
                return new LogAttribute[] { type };
            else {
                return new LogAttribute[] {
                    type,
                    new LogAttribute("total", TotalCount.ToString()),
                    new LogAttribute("fail", FailCount.ToString())
                };
            }
        }
    }
}
