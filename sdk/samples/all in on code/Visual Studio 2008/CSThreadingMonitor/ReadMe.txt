============================================================================
    Console Application : CSThreadingMonitor Project Overview
============================================================================

/////////////////////////////////////////////////////////////////////////////
Demo:

After running the sample, you execpt to see the following output,

Produce: 1
Consume: 1
Produce: 2
Consume: 2
Produce: 3
Consume: 3
Produce: 4
Consume: 4
Produce: 5
Consume: 5
Produce: 6
Consume: 6
Produce: 7
Consume: 7
Produce: 8
Consume: 8
Produce: 9
Consume: 9
Produce: 10
Consume: 10
Produce: 11
Consume: 11
Produce: 12
Consume: 12
Produce: 13
Consume: 13
Produce: 14
Consume: 14
Produce: 15
Consume: 15
Produce: 16
Consume: 16
Produce: 17
Consume: 17
Produce: 18
Consume: 18
Produce: 19
Consume: 19
Produce: 20
Consume: 20



/////////////////////////////////////////////////////////////////////////////
Use:

The following example shows how synchronization can be accomplished using the
 C# lock keyword and the Pulse method of the Monitor object. The Pulse method
notifies a thread in the waiting queue of a change in the object's state 
(for more details on pulses, see the Monitor.Pulse Method). 
http://msdn.microsoft.com/en-us/library/system.threading.monitor.pulse(VS.71).aspx


/////////////////////////////////////////////////////////////////////////////
Codes Logic:

The example creates a Cell object that has two methods: ReadFromCell and 
WriteToCell. Two other objects are created from classes CellProd and
CellCons; these objects both have a method ThreadRun whose job is to call 
ReadFromCell and WriteToCell. Synchronization is accomplished by waiting for
"pulses" from the Monitor object, which come in order. That is, first an 
item is produced (the consumer at this point is waiting for a pulse), then
a pulse occurs, then the consumer consumes the production (while the producer
is waiting for a pulse), and so on.


/////////////////////////////////////////////////////////////////////////////
Reference:

Monitor Pulse
http://msdn.microsoft.com/en-us/library/system.threading.monitor.pulse.aspx


////////////////////////////////////////////////////////////////////////////
