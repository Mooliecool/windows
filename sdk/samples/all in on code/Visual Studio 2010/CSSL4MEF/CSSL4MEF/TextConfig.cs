/****************************** Module Header ******************************\
* Module Name:                TextConfig.cs
* Project:                    CSSL4MEF
* Copyright (c) Microsoft Corporation.
* 
* DataModel for text style configuration
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Windows.Media;
using System.Windows.Shapes;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;

namespace CSSL4MEF
{
    /// <summary>
    /// Datamodel, contains properties of text style.
    /// Implement INotifyPropertyChanged for updating text
    /// style dynamicly.
    /// </summary>
    public class TextConfig : INotifyPropertyChanged
    {
        string _text;
        public string Text
        {
            set
            {
                if (_text != value)
                {
                    _text = value;
                    OnPropertyChanged("Text");
                }
            }
            get
            {
                return _text;
            }
        }

        int _fontsize = 10;
        [Range(8, 72)]
        public int FontSize
        {
            set
            {
                if (_fontsize != value)
                {
                    _fontsize = value;
                    OnPropertyChanged("FontSize");
                }
            }
            get
            {
                return _fontsize;
            }
        }

        Color _color;
        public Color FontColor
        {
            set
            {
                if (_color != value)
                {
                    _color = value;
                    OnPropertyChanged("FontColor");
                }
            }
            get
            {
                return _color;
            }
        }

        EnumEffect _effect;
        public EnumEffect Effect
        {
            set
            {
                if (_effect != value)
                {
                    _effect = value;
                    OnPropertyChanged("Effect");
                }
            }
            get
            {
                return _effect;
            }
        }

        void OnPropertyChanged(string property)
        {
            if (PropertyChanged != null)
                PropertyChanged(this,
                    new PropertyChangedEventArgs(property));
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }


    public enum EnumEffect
    {
        None = 0,
        Blur,
        Shadow
    }
}
