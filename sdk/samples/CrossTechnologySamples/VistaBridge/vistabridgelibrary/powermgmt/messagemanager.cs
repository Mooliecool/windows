using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.ComponentModel;
using System.Collections;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Diagnostics;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement
{
    /// <summary>
    /// This class generates .NET events based on Windows messages.  
    /// The PowerRegWindow class processes the messages from Windows.
    /// </summary>
    internal static class MessageManager
    {
        private static object _lock = new object();
        private static PowerRegWindow _window;

        #region Public static methods

        /// <summary>
        /// Registers a callback for a power event.
        /// </summary>
        /// <param name="eventId">Guid for the event.</param>
        /// <param name="eventToRegister">Event handler for the specified event.</param>
        public static void RegisterPowerEvent(Guid eventId, EventHandler eventToRegister)
        {
            EnsureInitialized();
            _window.RegisterPowerEvent(eventId, eventToRegister);
        }

        /// <summary>
        /// Unregisters an event handler for a power event.
        /// </summary>
        /// <param name="eventId">Guid for the event.</param>
        /// <param name="eventToUnregister">Event handler to unregister.</param>
        public static void UnregisterPowerEvent(Guid eventId, EventHandler eventToUnregister)
        {
            EnsureInitialized();
            _window.UnregisterPowerEvent(eventId, eventToUnregister);
        }

        #endregion

        /// <summary>
        /// Ensures that the hidden window is initialized and 
        /// listening for messages.
        /// </summary>
        private static void EnsureInitialized()
        {
            if (_window == null)
            {
                lock (_lock)
                {
                    if (_window == null)
                    {
                        using (ManualResetEvent mre = new ManualResetEvent(false))
                        {
                            // Start the windowless message pump
                            // and make sure it is active before the
                            // main thread resumes.
                            Thread t = new Thread((ThreadStart)delegate
                            {
                                _window = new PowerRegWindow();
                                mre.Set();
                                _window.MessageLoop();
                                
                            });
                            t.Name = "Power Management message loop";
                            t.IsBackground = true;
                            t.Start();
                            mre.WaitOne();
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Catch Windows messages and generates events for power specific
        /// messages.
        /// </summary>

        internal class PowerRegWindow : IDisposable
        {
            IntPtr _handle;
            int mm;
            bool disposed;

            SafeNativeMethods.WNDPROC del;
            internal PowerRegWindow()
            {
                // Register the window class.
                del = new SafeNativeMethods.WNDPROC(this.WndProc);
                IntPtr hInst = Marshal.GetHINSTANCE(
                Assembly.GetEntryAssembly().GetModules()[0]);
                SafeNativeMethods.WNDCLASSEX wndclass =
                    new SafeNativeMethods.WNDCLASSEX();

                wndclass.cbSize = (uint)Marshal.SizeOf(
                    typeof(SafeNativeMethods.WNDCLASSEX));
                wndclass.style = 11;
                wndclass.lpfnWndProc = del;
                wndclass.cbClsExtra = 0;
                wndclass.cbWndExtra = 0;
                wndclass.hInstance = hInst;
                wndclass.lpszMenuName = null;
                wndclass.lpszClassName = "MessageWnd2";

                mm = NativeMethods.RegisterClassEx(ref wndclass);
                int hr = Marshal.GetLastWin32Error();
                Marshal.ThrowExceptionForHR(hr);

                // Create the windowless window that provides
                // the message pump needed to receive windows messages.
                _handle = NativeMethods.CreateWindowEx(
                    0x08000000,
                    mm,
                    "mymessagewindow2",
                    0, 0, 0, 0, 0, SafeNativeMethods.HWND_MESSAGE,
                    0, hInst, IntPtr.Zero);

                hr = Marshal.GetLastWin32Error();
                if (hr == 0)
                {
                    throw new Win32Exception("Error creating message pump window.");
                }
                Marshal.ThrowExceptionForHR(hr);
            }
            // Implements the message loop for the hidden window.
            internal void MessageLoop()
            {
                SafeNativeMethods.MSG msg = new SafeNativeMethods.MSG();
                int bRet; 
                
                // Start the message loop. 
                while ((bRet = NativeMethods.GetMessage(out msg, _handle, 0, 0)) != 0)
                {
                    if (bRet == -1)
                    {
                        throw new Win32Exception("Error in GetMessage.");
                    }
                    // Pass messages to the window's WndProc.
                    NativeMethods.TranslateMessage(ref msg);
                    NativeMethods.DispatchMessage(ref msg);
                }
                
                return;
            }

           //Implement IDisposable.
           public void Dispose() 
           {
             Dispose(true);
              GC.SuppressFinalize(this); 
           }

           protected virtual void Dispose(bool disposing) 
           {
               if (!disposed)
               {
                   NativeMethods.DestroyWindow(_handle);
                   _handle = IntPtr.Zero;
               }
               disposed = true;
           }

           ~PowerRegWindow()
           {
              Dispose (false);
           }

            private Hashtable _eventList = new Hashtable();
            private ReaderWriterLock _lock = new ReaderWriterLock();

            #region Internal Methods

            /// <summary>
            /// Adds an event handler to call when Windows sends 
            /// a message for an evebt.
            /// </summary>
            /// <param name="eventId">Guid for the event.</param>
            /// <param name="eventToRegister">Event handler for the event.</param>
            internal void RegisterPowerEvent(Guid eventId, EventHandler eventToRegister)
            {
                _lock.AcquireWriterLock(Timeout.Infinite);
                if (!_eventList.Contains(eventId))
                {
                    Power.RegisterPowerSettingNotification(_handle.ToInt64(), eventId);
                    ArrayList newList = new ArrayList();
                    newList.Add(eventToRegister);
                    _eventList.Add(eventId, newList);
                }
                else
                {
                    ArrayList currList = (ArrayList)_eventList[eventId];
                    currList.Add(eventToRegister);
                }
                _lock.ReleaseWriterLock();
            }

            /// <summary>
            /// Removes an event handler.
            /// </summary>
            /// <param name="eventId">Guid for the event.</param>
            /// <param name="eventToUnregister">Event handler to remove.</param>
            /// <exception cref="InvalidOperationException">Cannot unregister 
            /// a function that is not registered.</exception>
            internal void UnregisterPowerEvent(Guid eventId, EventHandler eventToUnregister)
            {
                _lock.AcquireWriterLock(Timeout.Infinite);
                if (_eventList.Contains(eventId))
                {
                    ArrayList currList = (ArrayList)_eventList[eventId];
                    currList.Remove(eventToUnregister);
                }
                else
                {
                    throw new InvalidOperationException(
                        "The specified event handler has not been registered.");
                }
                _lock.ReleaseWriterLock();
            }

            #endregion

            /// <summary>
            /// Executes any registered event handlers.
            /// </summary>
            /// <param name="eventHandlerList">ArrayList of event handlers.</param>
            private static void ExecuteEvents(ArrayList eventHandlerList)
            {
                ArrayList tempList = (ArrayList)eventHandlerList.Clone();
                foreach (EventHandler handler in tempList)
                {
                    try
                    {
                        if (handler != null)
                            handler.Invoke(null, new EventArgs());
                    }
                    // Don't crash if an event handler throws an exception.
                    catch { ;}
                }
            }

            /// <summary>
            /// This method is called when a Windows message 
            /// is sent to this window.
            /// The method calls the registered event handlers.
            /// </summary>
            /// <param name="uMessage">The incoming windows message.</param>
            /// <param name="hWnd">The window handle.</param>
            /// <param name="wParam">The wParam value sent for the message.</param>
            /// <param name="lParam">The lParam value sent for the message.</param>

            private int WndProc(IntPtr hWnd, 
                uint uMessage, IntPtr wParam, IntPtr lParam)
            {
                // Make sure it is a Power Management message.
                if (uMessage == SafeNativeMethods.WM_POWERBROADCAST && (int)wParam == SafeNativeMethods.PBT_POWERSETTINGCHANGE)
                {
                    SafeNativeMethods.PowerBroadcastSetting ps =
                         (SafeNativeMethods.PowerBroadcastSetting)Marshal.PtrToStructure(
                             lParam, typeof(SafeNativeMethods.PowerBroadcastSetting));
                    IntPtr pData = (IntPtr)((int)lParam + Marshal.SizeOf(ps));
                    Guid currentEvent = ps.PowerSetting;

                    // Update the appropriate Property.
                    // Power Personality
                    if (ps.PowerSetting == EventManager.PowerPersonalityChange && 
                        ps.DataLength == Marshal.SizeOf(typeof(Guid)))
                    {
                        Guid newPersonality =
                            (Guid)Marshal.PtrToStructure(pData, typeof(Guid));

                        PowerManager.powerPersonality = PersonalityGuids.GuidToEnum(newPersonality);
                        // Tell PowerManager that is now safe to 
                        // read the powerPersonality member.
                        EventManager.personalityReset.Set();
                    }
                    // Power Source
                    else if (ps.PowerSetting == EventManager.PowerSourceChange &&
                         ps.DataLength == Marshal.SizeOf(typeof(Int32)))
                    {
                        Int32 powerSrc = (Int32)Marshal.PtrToStructure(pData, typeof(Int32));
                        PowerManager.powerSource = (PowerSource)powerSrc;
                        EventManager.powerSrcReset.Set();
                    }
                    // Battery capacity
                    else if (ps.PowerSetting == EventManager.BatteryCapacityChange &&
                        ps.DataLength == Marshal.SizeOf(typeof(Int32)))
                    {
                        Int32 battCapacity = (Int32)Marshal.PtrToStructure(pData, typeof(Int32));
                        PowerManager.batteryLifePercent = battCapacity;
                        EventManager.batteryLifeReset.Set();
                    }
                    // IsMonitorOn
                    else if (ps.PowerSetting == EventManager.MonitorPowerStatus &&
                        ps.DataLength == Marshal.SizeOf(typeof(Int32)))
                    {
                        Int32 monitorStatus = (Int32)Marshal.PtrToStructure(pData, typeof(Int32));
                        PowerManager.isMonitorOn = monitorStatus == 0 ? false : true;
                        EventManager.monitorOnReset.Set();
                    }

                    if (!EventManager.IsMessageCaught(currentEvent))
                        ExecuteEvents((ArrayList)_eventList[currentEvent]);
                }
                return (int)hWnd;
            }
        }
    }
}
