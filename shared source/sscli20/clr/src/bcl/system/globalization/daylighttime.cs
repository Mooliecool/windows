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
namespace System.Globalization {
   
	using System;
    // This class represents a starting/ending time for a period of daylight saving time.
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public class DaylightTime
    {
    	internal DateTime m_start;
    	internal DateTime m_end;
    	internal TimeSpan m_delta;

        private DaylightTime() {
        }

    	public DaylightTime(DateTime start, DateTime end, TimeSpan delta) {
            m_start = start;
            m_end = end;
            m_delta = delta;
        }    

    	// The start date of a daylight saving period.
        public DateTime Start {
            get {
                return m_start;
            }
        }

    	// The end date of a daylight saving period.
        public DateTime End {
            get {
                return m_end;
            }
        }

    	// Delta to stardard offset in ticks.
        public TimeSpan Delta {
            get {
                return m_delta;
            }
        }
    
    }

}
