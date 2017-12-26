//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Data;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Collections;


namespace InputLibrary
{
    /// <summary>
    /// Class responsible for handling all input.
    /// </summary>
    public class Input
    {
        public ArrayList HardwareKeys { get { return hardwareKeysValue; } }
        ArrayList hardwareKeysValue = new ArrayList(7);


        /// <summary>
        /// Bitmask used to access if a button is currently pressed.
        /// </summary>
        const byte DownMask = 0x01;

        /// <summary>
        /// Equal to ~DownMask.
        /// </summary>
        const byte NotDownMask = 0xfe;

        /// <summary>
        /// Bitmask used to access if a button has been pressed and released.
        /// </summary>
        const byte PressedMask = 0x02;

        /// <summary>
        /// Equal to ~PressedMask.
        /// </summary>
        const byte NotPressedMask = 0xfd;

        /// <summary>
        /// Bitmask used to determine if a button is registered.
        /// </summary>
        const byte RegisteredMask = 0x80;

        /// <summary>
        /// Equal to ~RegisteredMask.
        /// </summary>
        const byte NotRegisteredMask = 0x7f;

        /// <summary>
        /// Number of keys to track.
        /// </summary>
        const int NumberKeys = 256;

        /// <summary>
        /// Array of key states.  These states are tracked using the various
        /// bitmasks defined in this class.
        /// </summary>
        byte[] keyStates = new byte[NumberKeys];

        /// <summary>
        /// MessageWindow instance used by Input to intercept hardware
        /// button presses.
        /// </summary>
        Control msgWindow = null;

        /// <summary>
        /// Creates an instance of Input.
        /// </summary>
        public Input(Control ctrl)
        {
            msgWindow = ctrl;
            msgWindow.KeyDown += new KeyEventHandler(msgWindow_KeyDown);
            msgWindow.KeyUp += new KeyEventHandler(msgWindow_KeyUp);

#if SMARTPHONE
            hardwareKeysValue.Add(193);
            hardwareKeysValue.Add(112);
            hardwareKeysValue.Add(113);
            hardwareKeysValue.Add(198);
            hardwareKeysValue.Add(197);
            hardwareKeysValue.Add(27);
            hardwareKeysValue.Add(13);
#elif MAINSTONE
            // quit
            hardwareKeysValue.Add(35);
            // reset
            hardwareKeysValue.Add(8);
            // debug switch
            hardwareKeysValue.Add(Keys.A);
            // shoot
            hardwareKeysValue.Add(43);
            hardwareKeysValue.Add(197);
#elif DESKTOP
            hardwareKeysValue.Add(Keys.Q);
            hardwareKeysValue.Add(Keys.W);
            hardwareKeysValue.Add(Keys.A);
            hardwareKeysValue.Add(Keys.S);
            hardwareKeysValue.Add(Keys.D);
#else // PPC
            hardwareKeysValue.Add(193);
            hardwareKeysValue.Add(194);
            hardwareKeysValue.Add(195);
            hardwareKeysValue.Add(134);
            hardwareKeysValue.Add(197);
#endif

            // Initialize each key state
            for (int i = 0; i < NumberKeys; i++)
            {
                keyStates[i] = 0x00;
            }
        }

        /// <summary>
        /// Register all of the hardware keys, including the directional pad.
        /// It is required to register a key in order to receive state
        /// information on it.
        /// </summary>
        public void RegisterAllHardwareKeys()
        {
            foreach (int i in hardwareKeysValue)
            {
                RegisterKey(i);
            }

            RegisterKey((int)Keys.Up);
            RegisterKey((int)Keys.Down);
            RegisterKey((int)Keys.Left);
            RegisterKey((int)Keys.Right);
        }

        /// <summary>
        /// Register every possible key. It is required to register a key
        /// in order to receive state information on it.
        /// </summary>
        public void RegisterAllKeys()
        {
            for (int i = 0; i < NumberKeys; i++)
            {
                RegisterKey(i);
            }
        }

        /// <summary>
        /// Unregister all of the hardware keys, including the directional
        /// pad.
        /// </summary>
        public void UnregisterAllHardwareKeys()
        {
            foreach (int i in hardwareKeysValue)
            {
                UnregisterKey(i);
            }

            UnregisterKey((int)Keys.Up);
            UnregisterKey((int)Keys.Down);
            UnregisterKey((int)Keys.Left);
            UnregisterKey((int)Keys.Right);
        }

        /// <summary>
        /// Unregister every key.
        /// </summary>
        public void UnregisterAllKeys()
        {
            for (int i = 0; i < NumberKeys; i++)
            {
                UnregisterKey(i);
            }
        }

        /// <summary>
        /// Register the key specified for input.
        /// </summary>
        /// <param name="virtualKey">Virtual key code</param>
        public void RegisterKey(int virtualKey)
        {
            keyStates[virtualKey] |= RegisteredMask;
        }

        /// <summary>
        /// Unregister the key specified.
        /// </summary>
        /// <param name="virtualKey">Virtual key code</param>
        public void UnregisterKey(int virtualKey)
        {
            keyStates[virtualKey] &= NotRegisteredMask;
        }

        /// <summary>
        /// Check if any registered key has been pressed
        /// </summary>
        /// <param name="virtualKey">Virtual key code</param>
        /// <returns>true if any registered key is down</returns>
        public bool AnyKeyPressed()
        {
            for (int i = 0; i < keyStates.Length; i++)
            {
                if (((keyStates[i] & RegisteredMask) != 0) &&
                    ((keyStates[i] & PressedMask) != 0))
                {
                    return true;
                }
            }

            return false;
        }

        /// <summary>
        /// Clears any existing key presses so that KeyPressed()
        /// and AnyKeyPressed() will return false until a new key
        /// has been pressed
        /// </summary>
        public void ClearKeyPresses()
        {
            for (int i = 0; i < keyStates.Length; i++)
                keyStates[i] &= NotPressedMask;
        }

        /// <summary>
        /// Check if the key has been pressed and released since the last
        /// call to this function.
        /// </summary>
        /// <param name="virtualKey">Virtual key code</param>
        /// <returns>true if the key has been pressed and released
        /// since the last call to this function</returns>
        public bool KeyPressed(int virtualKey)
        {
            if ((keyStates[virtualKey] & PressedMask) != 0)
            {
                keyStates[virtualKey] &= NotPressedMask;
                return true;
            }

            return false;
        }


        /// <summary>
        /// Check if the key is currently down.
        /// </summary>
        /// <param name="virtualKey">Virtual key code</param>
        /// <returns>true if down, false otherwise</returns>
        public bool KeyDown(int virtualKey)
        {
            if ((keyStates[virtualKey] & DownMask) != 0)
                return true;

            return false;
        }

        /// <summary>
        /// Check if the key is currently released.
        /// </summary>
        /// <param name="virtualKey">Virtual key code</param>
        /// <returns>true if released, false otherwise</returns>
        public bool KeyReleased(int virtualKey)
        {
            if ((keyStates[virtualKey] & DownMask) == 0)
                return true;

            return false;
        }

        private void msgWindow_KeyDown(object sender, KeyEventArgs e)
        {
            keyStates[e.KeyValue] |= DownMask;
        }

        private void msgWindow_KeyUp(object sender, KeyEventArgs e)
        {
            if ((keyStates[e.KeyValue] & DownMask) != 0)
                keyStates[e.KeyValue] |= PressedMask;
            keyStates[e.KeyValue] &= NotDownMask;
        }
    }
}
