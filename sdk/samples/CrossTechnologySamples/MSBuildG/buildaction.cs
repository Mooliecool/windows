// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // A class for showing information of all the Actions (Targets) that can be taken. 
    // Used by the ComboBoxes in the Designer
    
    public class BuildAction : ItemsControl
    {
        Microsoft.Build.BuildEngine.Target m_Target;

        public BuildAction(Microsoft.Build.BuildEngine.Target target)
        {
            if (target == null)
            {
                throw new ArgumentNullException("target");
            }

            //Save a reference to the child target
            m_Target = target;

            //Create the main layout panels

            StackPanel mainLayout = new StackPanel();
            mainLayout.Orientation = Orientation.Vertical;

            StackPanel itemLayout = new StackPanel();
            itemLayout.Orientation = Orientation.Horizontal;

            //Determine whether this Target is dependent on antoher
            if (target.DependsOnTargets.Length > 0)
            {
                BitmapImage dependsOnSource=new BitmapImage(SettingsSystem.GetIcon("Warning.ico"));

                Image dependsOn = new Image();
                dependsOn.Source = dependsOnSource;
                dependsOn.Width = 16;
                dependsOn.Height = 16;

                StackPanel targetPanel = new StackPanel();
                targetPanel.Orientation = Orientation.Vertical;

                TextBlock titleBlock = new TextBlock();
                titleBlock.Text = "Target Depends On:";
                titleBlock.TextTrimming = TextTrimming.CharacterEllipsis;
                titleBlock.Foreground = Brushes.Black;
                titleBlock.Background = Brushes.PaleGreen;

                targetPanel.Children.Add(titleBlock);

                foreach (string dependsTargetName in target.DependsOnTargets.Split(';'))
                {
                    TextBlock tBlock = new TextBlock();
                    tBlock.TextTrimming = TextTrimming.CharacterEllipsis;
                    tBlock.Text = dependsTargetName.Trim();
                    tBlock.Foreground = Brushes.Black;

                    targetPanel.Children.Add(tBlock);
                }

                ToolTip dependsTip = new ToolTip();
                dependsTip.Content = targetPanel;

                dependsOn.ToolTip = dependsTip;

                itemLayout.Children.Add(dependsOn);
            }

            //Determine whether this target has a condition
            if (target.Condition.Length > 0)
            {
                BitmapImage conditionSource = new BitmapImage(SettingsSystem.GetIcon("Warning.ico"));

                Image condition = new Image();
                condition.Source = conditionSource;
                condition.Width = 16;
                condition.Height = 16;

                StackPanel conditionPanel = new StackPanel();
                conditionPanel.Orientation = Orientation.Vertical;

                TextBlock titleBlock = new TextBlock();
                titleBlock.TextTrimming = TextTrimming.CharacterEllipsis;
                titleBlock.Text = "Target Condition:";
                titleBlock.Foreground = Brushes.Black;
                titleBlock.Background = Brushes.Wheat;

                conditionPanel.Children.Add(titleBlock);

                TextBlock tBlock = new TextBlock();
                tBlock.TextTrimming = TextTrimming.CharacterEllipsis;
                tBlock.Text = target.Condition;
                tBlock.Foreground = Brushes.Black;

                conditionPanel.Children.Add(tBlock);

                ToolTip conditionTip = new ToolTip();
                conditionTip.Content = conditionPanel;

                condition.ToolTip = conditionTip;

                itemLayout.Children.Add(condition);
            }

            //Add the target's name

            TextBlock targetName = new TextBlock();
            targetName.TextTrimming = TextTrimming.CharacterEllipsis;

            targetName.Text = target.Name;

            if (target.IsImported)
            {
                targetName.Foreground = Brushes.SkyBlue;
            }
            else
            {
                targetName.Foreground = Brushes.White;
            }

            itemLayout.Children.Add(targetName);

            mainLayout.Children.Add(itemLayout);

            //Save the layout
            AddChild(mainLayout);
        }

        public string TargetName
        {
            get
            {
                return (m_Target.Name);
            }
        }
    }

}
