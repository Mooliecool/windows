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
#ifndef TIMELINE_H_
#define TIMELINE_H_ 1

//*****************************************************************************
// Timeline.h
//
// Simple timed manual event log, for profiling purposes.
//
//*****************************************************************************

#ifndef ENABLE_TIMELINE


#endif

class Timeline
{
 public:
    enum
    {
        GENERAL             = 0x00000001,
        LOADER              = 0x00000002,
        ZAP                 = 0x00000004,
        FUSIONBIND          = 0x00000008,
        STARTUP             = 0x00000010,
        STARTUP_PRESTUB     = 0x00000020,
        // Add more categories here
    } Category;

    enum
    {
        LEVEL_MAJOR = 1,
        MAX_LEVEL = 20
    };

#if ENABLE_TIMELINE
    static Timeline g_Timeline;
#endif

    static void Startup();
    static void Shutdown();

    void Init();
    void Destroy();

    BOOL Enabled(int category) { return (m_enabled&category) != 0; }
    void Stamp(int level);
    void Event(LPCSTR formatString, ...);
    void EventStart(LPCSTR formatString, ...);
    void EventEnd(LPCSTR formatString, ...);

    class Monitor
    {
#if ENABLE_TIMELINE
    public:
        int m_level;
        LPCSTR m_string;

        Monitor() 
        { 
            m_level = Timeline::g_Timeline.m_lastLevel; 
            m_string = "";
        }

        ~Monitor() 
        { 
            while (m_level < Timeline::g_Timeline.m_lastLevel) 
                g_Timeline.EventEnd(m_string); 
        }
#else   // ENABLE_TIMELINE
    public:
        Monitor() { }
        ~Monitor() { }
#endif  // ENABLE_TIMELINE
    };

 private:

    typedef LARGE_INTEGER timestamp;

    int         m_enabled;
    timestamp   m_frequency;
    timestamp   m_start;
    int         m_lastLevel;    
 friend Timeline::Monitor::Monitor(void);
 friend Timeline::Monitor::~Monitor(void);
    timestamp   m_lastTime[MAX_LEVEL];

    FILE        *m_out;
};

#if ENABLE_TIMELINE

#define TIMELINE(c, a) \
        do { if ((Timeline::g_Timeline.Enabled(Timeline::c))) Timeline::g_Timeline.Event a; } while (0)
#define TIMELINE_START(c, a) \
        do { if ((Timeline::g_Timeline.Enabled(Timeline::c))) Timeline::g_Timeline.EventStart a; } while (0)
#define TIMELINE_END(c, a) \
        do { if ((Timeline::g_Timeline.Enabled(Timeline::c))) Timeline::g_Timeline.EventEnd a; } while (0)

#define TIMELINE_START_SAFE(c, a)                   \
        Timeline::Monitor __timelinemonitor;        \
        TIMELINE_START(c, a)

#define TIMELINE_AUTO(c, s)                         \
        Timeline::Monitor __timelineauto;           \
        __timelineauto.m_string = (s);              \
        TIMELINE_START(c, (s))

#else

#define TIMELINE(c, a)
#define TIMELINE_START(c, a)
#define TIMELINE_END(c, a)
#define TIMELINE_START_SAFE(c, a)
#define TIMELINE_AUTO(c, a)

inline void Timeline::Startup() {}
inline void Timeline::Shutdown() {}
inline void Timeline::Init() {}
inline void Timeline::Destroy() {}
inline void Timeline::Event(LPCSTR formatString, ...) {}
inline void Timeline::EventStart(LPCSTR formatString, ...) {}
inline void Timeline::EventEnd(LPCSTR formatString, ...) {}

#endif

#endif // TIMELINE_H_




