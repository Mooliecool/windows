/******************************************************************************
 * File: Target.xaml.cs
 *
 * Description: 
 * This is the code-behind file for the target application used in the 
 * test script generator sample. It contains seven interdependent controls; 
 * a series of four 'navigation' buttons, a slider, and a list box. 
 * 
 * The buttons can be used to increment or decrement the slider's range value by 
 * either its small or large change value. In turn, when the slider's range value 
 * is changed, the selected index of the list box changes to a corresponding 
 * value. The reverse is also true. 
 * 
 * See Client.cs in the ScriptGeneratorClient project for a full description 
 * of the sample code.
 *      
 * This file is part of the Microsoft Windows SDK Code Samples.
 * 
 * Copyright (C) Microsoft Corporation.  All rights reserved.
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
 *****************************************************************************/
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;


namespace ScriptGeneratorTarget
{
    ///--------------------------------------------------------------------
    /// <summary>
    /// Interaction logic for Target.xaml
    /// </summary>
    ///--------------------------------------------------------------------
    public partial class Target : Window
    {
        ///--------------------------------------------------------------------
        /// <summary>
        /// Constructor
        /// </summary>
        /// <remarks>
        /// Initializes components.
        /// </remarks>
        ///--------------------------------------------------------------------
        public Target()
        {
            InitializeComponent();
            SetControls();
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Enables or disables buttons based on slider value.
        /// </summary>
        ///--------------------------------------------------------------------
        private void SetControls()
        {
            if (targetSlider.Minimum == targetSlider.Value)
            {
                SmallDecrement.IsEnabled = false;
                LargeDecrement.IsEnabled = false;
                SmallIncrement.IsEnabled = true;
                LargeIncrement.IsEnabled = true;
            }
            else if (targetSlider.Maximum == targetSlider.Value)
            {
                SmallIncrement.IsEnabled = false;
                LargeIncrement.IsEnabled = false;
                SmallDecrement.IsEnabled = true;
                LargeDecrement.IsEnabled = true;
            }
            else
            {
                SmallIncrement.IsEnabled = true;
                LargeIncrement.IsEnabled = true;
                SmallDecrement.IsEnabled = true;
                LargeDecrement.IsEnabled = true;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Adjusts Slider range value based on button click.
        /// </summary>
        /// <param name="src">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void ButtonClicked(object src, RoutedEventArgs e)
        {
            Button srcButton = src as Button;
            switch(srcButton.Name)
            {
                case "LargeDecrement":
                    targetSlider.Value = 
                        targetSlider.Value - targetSlider.LargeChange;
                    break;
                case "SmallDecrement":
                    targetSlider.Value = 
                        targetSlider.Value - targetSlider.SmallChange;
                    break;
                case "SmallIncrement":
                    targetSlider.Value = 
                        targetSlider.Value + targetSlider.SmallChange;
                    break;
                case "LargeIncrement":
                    targetSlider.Value = 
                        targetSlider.Value + targetSlider.LargeChange;
                    break;
            }
            SetControls();
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Adjusts ListBox selected index based on Slider range value change.
        /// </summary>
        /// <param name="src">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void SliderValueChanged(object src, RoutedEventArgs e)
        {
            targetListBox.SelectedIndex = (int)targetSlider.Value;
            SetControls();
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Adjusts Slider range value based on ListBox selected index change.
        /// </summary>
        /// <param name="src">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void SelectionItemChanged(object src, RoutedEventArgs e)
        {
            targetSlider.Value = targetListBox.SelectedIndex;
            SetControls();
        }
    }
}