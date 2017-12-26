/*******************************************************************************
 *
 * File: UIAutoFetcher.cs
 *
 * Description: A Class that implements UI Automation functionality on a separate thread.
 * 
 * For a full description of the sample, see FetchTimerForm.cs.
 *
 *     
 *  This file is part of the Microsoft Windows SDK Code Samples.
 * 
 *  Copyright (C) Microsoft Corporation.  All rights reserved.
 * 
 * This source code is intended only as a supplement to Microsoft
 * Development Tools and/or on-line documentation.  See these other
 * materials for detailed information regarding Microsoft code samples.
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * 
 *******************************************************************************/
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Windows.Automation;
using System.Diagnostics;
using System.Threading;

namespace FetchTimer
{
    class UIAutomationFetcher
    {
        // Member variables

        // Application form, for output.
        FetchTimerForm appForm;

        // Number of properties fetched for each test.
        const int numberOfFetches = 5;

        TreeScope treeScope;
        AutomationElementMode mode;
        int currentPropCount;
        int cachedPropCount;
        int elementCount;
        System.Windows.Point targetPoint;


        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="form">The application form.</param>
        /// <param name="targetPoint">The screen coordinates of the cursor.</param>
        /// <param name="scope">The TreeScope for caching.</param>
        /// <param name="mode">The mode for caching.</param>
        public UIAutomationFetcher(FetchTimerForm form,
                System.Windows.Point targetPt,
                TreeScope scope, AutomationElementMode cacheMode)
        {
            appForm = form;
            treeScope = scope;
            targetPoint = targetPt;
            mode = cacheMode;
        }

        public void DoWork()
        {
            long timeToGetUncached = 0;
            long timeToGetCached = 0;

            // Create a System.Diagnostics.Stopwatch.
            Stopwatch stopWatchTimer = new Stopwatch();


            // TEST 1: Get the target element without caching, and retrieve
            //  current properties.

            stopWatchTimer.Start();
            AutomationElement targetNoCache = null;
            try
            {
                targetNoCache = AutomationElement.FromPoint(targetPoint);
            }
            catch (ElementNotAvailableException)
            {
                OutputLine("Could not retrieve element.");
                return;
            }

            // Get current properties.
            currentPropCount = 0;
            GetCurrentProperties(targetNoCache, 0);
            stopWatchTimer.Stop();
            timeToGetUncached = stopWatchTimer.Elapsed.Ticks;


            // TEST 2: Get the target element with caching, and retrieve
            //   cached properties.

            // Create CacheRequest.
            CacheRequest fetchRequest = new CacheRequest();

            // Add properties to fetch.
            fetchRequest.Add(AutomationElement.NameProperty);
            fetchRequest.Add(AutomationElement.AutomationIdProperty);
            fetchRequest.Add(AutomationElement.ControlTypeProperty);
            fetchRequest.Add(AutomationElement.FrameworkIdProperty);
            fetchRequest.Add(AutomationElement.IsContentElementProperty);

            // Set options.
            fetchRequest.AutomationElementMode = mode;
            fetchRequest.TreeScope = treeScope;
            fetchRequest.TreeFilter = Automation.RawViewCondition;

            // Activate the CacheRequest and fetch the target.
            AutomationElement targetCached = null;
            using (fetchRequest.Activate())
            {
                stopWatchTimer.Reset();
                stopWatchTimer.Start();
                try
                {
                    targetCached = AutomationElement.FromPoint(targetPoint);
                }
                catch (InvalidOperationException)
                {
                    OutputLine("InvalidOperationException. Could not retrieve element.");
                    return;
                }
                catch (ElementNotAvailableException)
                {
                    OutputLine("ElementNotAvailableException. Could not retrieve element.");
                    return;
                }
            } // CacheRequest is now inactive.

            // Get cached properties.
            GetCachedProperties(targetCached, true);
            stopWatchTimer.Stop();
            timeToGetCached = stopWatchTimer.Elapsed.Ticks;

            // TEST 3: Get updated cache.

            stopWatchTimer.Reset();
            stopWatchTimer.Start();
            AutomationElement updatedTargetCached = null;
            bool cacheUpdated = false;
            if (mode == AutomationElementMode.Full)
            {
                updatedTargetCached = targetCached.GetUpdatedCache(fetchRequest);
                GetCachedProperties(updatedTargetCached, false);
                // Fetches were counted again, so divide count by 2.
                cachedPropCount /= 2;
                cacheUpdated = true;
                stopWatchTimer.Stop();
            }
            long updateTicks = stopWatchTimer.Elapsed.Ticks;

            // END OF TESTS. 
            
            // Display results

            string nameProperty = targetNoCache.Current.Name;
            string framework = targetNoCache.Current.FrameworkId;
            OutputLine("Name: " + nameProperty);
            OutputLine("Framework: " + framework);
            OutputLine(elementCount.ToString() + " cached element(s).");

            OutputLine(timeToGetUncached.ToString("N0") + " Ticks to retrieve element(s) and get "
                + currentPropCount.ToString() + " current properties.");
            OutputLine(timeToGetCached.ToString("N0") + " Ticks to retrieve element(s) and get "
                + cachedPropCount.ToString() + " cached properties.");

            // Show ratio between current and cached performance.
            float ratio = (float)timeToGetUncached / (float)timeToGetCached;
            if (ratio > 2)
            {
                OutputLine("Current:Cached = " + ratio.ToString("N0") + ":1");
            }
            else
            {
                // Print with decimal.
                OutputLine("Current:Cached = " + ratio.ToString("N1") + ":1");
            }
            if (cacheUpdated)
            {
                OutputLine(updateTicks.ToString("N0") + " Ticks to update cache and get properties.");
            }
            else
            {
                OutputLine("Cannot update cache in None mode.");
            }
            OutputLine("");
        }

        /// <summary>
        /// Walks the tree and gets properties from all elements found. Recursive.
        /// </summary>
        /// <param name="element">Node to walk.</param>
        /// <param name="depth">Depth of this iteration (distance from initial node).
        /// </param>
        /// <remarks>
        /// Nothing is done with the objects retrieved.
        /// </remarks>
        private void GetCurrentProperties(AutomationElement element, int depth)
        {
            if ((treeScope == TreeScope.Element) && (depth > 0))
            {
                return;
            }
            if (((treeScope & TreeScope.Descendants) == 0) && (depth > 1))
            {
                return;
            }
            string name = element.Current.Name;
            string id = element.Current.AutomationId;
            ControlType controlType = element.Current.ControlType;
            string framework = element.Current.FrameworkId;
            currentPropCount += numberOfFetches;

            TreeWalker walker = TreeWalker.ContentViewWalker;
            AutomationElement elementChild = walker.GetFirstChild(element);
            while (elementChild != null)
            {
                GetCurrentProperties(elementChild, depth+1);
                elementChild = walker.GetNextSibling(elementChild);
            }
        }



        /// <summary>
        /// Gets a set of cached properties. Recursive.
        /// </summary>
        /// <param name="element">The target element.</param>
        /// <remarks>
        /// Nothing is done with the objects retrieved.  
        /// </remarks>
        private void GetCachedProperties(AutomationElement element, bool updateCount)
        {
            if (updateCount)
            {
                elementCount++;
            }
            string name = element.Cached.Name;
            string s = element.Cached.AutomationId;
            ControlType controlType = element.Cached.ControlType;
            string frame = element.Cached.FrameworkId;
            cachedPropCount += numberOfFetches;

            try
            {
                foreach (AutomationElement child in element.CachedChildren)
                {
                    GetCachedProperties(child, updateCount);
                }
            }
            catch (System.InvalidOperationException)
            {
                // Expected; there might be no CachedChildren, in which case an 
                //exception is raised when the property is accessed.
            }
        }



        /// <summary>
        /// Prints a line of text to the textbox.
        /// </summary>
        /// <param name="outputStr">The string to print.</param>
        /// <remarks>
        /// Must use Invoke so that UI is not being called directly from this thread.
        /// </remarks>
        private void OutputLine(string outputStr)
        {
            appForm.Invoke(appForm.outputMethodInstance, 
                new Object[] { outputStr + Environment.NewLine } );
        }
    } // UIAutoWorker class.
}
