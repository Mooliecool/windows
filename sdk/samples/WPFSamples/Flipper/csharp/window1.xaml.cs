using System;
using System.Data;
using System.Windows;
using System.Windows.Data;
using System.Configuration;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Windows.Media.Animation;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Threading;
using DemoDev;

namespace Ribbon
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        private void OnLoaded(object sender, EventArgs e)
        {
            // setup trackball for moving the model around
            _trackball = new Trackball();
            _trackball.Attach(this);
            _trackball.Servants.Add(myViewport3D);
            _trackball.Enabled = true;

            // Get the mesh objects for changing the material
            ModelVisual3D mv3d = myViewport3D.Children[0] as ModelVisual3D;
            Model3DGroup m3dgBase = mv3d.Content as Model3DGroup;

            Model3DGroup m3dg = m3dgBase.Children[0] as Model3DGroup;
            _TopPlane = m3dg.Children[2] as GeometryModel3D;
            _BottomPlane = m3dg.Children[3] as GeometryModel3D;

            m3dg = m3dgBase.Children[1] as Model3DGroup;
            _FrontSpinPlane = m3dg.Children[0] as GeometryModel3D;
            _BackSpinPlane = m3dg.Children[1] as GeometryModel3D;

            AnimateToNextPicture();
        }

        #region Events

        private void OnToggleAutoRun(object sender, RoutedEventArgs e)
        {
            if (_autorun == true)
            {
                _autorun = false;
            }
            else
            {
                _autorun = true;
                AnimateToNextPicture();
            }
        }

        private void OnSingleStep(object sender, RoutedEventArgs e)
        {
            AnimateToNextPicture();
        }

        #endregion

        #region Private Methods

        private void AnimateToNextPicture()
        {
            int nextPic = _currentPic + 1;

            if (nextPic > _MAX_PICS)
                nextPic = 1;

            DiffuseMaterial df1 = this.FindResource("pic_01" + _currentPic.ToString()) as DiffuseMaterial;
            DiffuseMaterial df2 = this.FindResource("pic_01" + nextPic.ToString()) as DiffuseMaterial;

            if ((df1 == null) || (df2 == null))
                return;

            _BottomPlane.Material = df1;
            _FrontSpinPlane.Material = df1;
            _TopPlane.Material = df2;
            _BackSpinPlane.Material = df2;

            _currentPic++;
            if (_currentPic > _MAX_PICS)
                _currentPic = 1;

            Storyboard s = (Storyboard)this.FindResource("FlipPicTimeline");
            this.BeginStoryboard(s);
        }

        private void OnFlipPicTimeline(object sender, EventArgs e)
        {
            Clock clock = (Clock)sender;

            if (clock.CurrentState == ClockState.Active)  // Begun case
            {
                return;
            }

            if (clock.CurrentState != ClockState.Active)  // Ended case
            {
                if (_autorun == true)
                {
                    AnimateToNextPicture();
                }
                
            }
        }

        #endregion

        #region Globals

        // Geometry models
        GeometryModel3D _TopPlane;
        GeometryModel3D _BottomPlane;
        GeometryModel3D _FrontSpinPlane;
        GeometryModel3D _BackSpinPlane;

        Trackball _trackball;
        int _currentPic = 1;
        const int _MAX_PICS = 6;

        bool _autorun = false;
       
        #endregion
    }

   
}



