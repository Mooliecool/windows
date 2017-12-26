//---------------------------------------------------------------------------
//
// <copyright file="Trackball.cs" company="Microsoft">
//    Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// 
//
// Description: A trackball controls that allows rotation and scaling
// of a 3D scene.  It will require further work to be a general
// component.
//
//---------------------------------------------------------------------------


using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Media3D;

public class Trackball
{
    public Trackball()
    {
        Reset();
    }
    
    public void Attach(FrameworkElement element)
    {
        element.MouseMove += this.MouseMoveHandler;
        element.MouseLeftButtonDown += this.MouseDownHandler;
        element.MouseLeftButtonUp += this.MouseUpHandler;
    }
    
    public void Detach(FrameworkElement element)
    {
        element.MouseMove -= this.MouseMoveHandler;
        element.MouseLeftButtonDown -= this.MouseDownHandler;
        element.MouseLeftButtonUp -= this.MouseUpHandler;
    }
    
    public List<Viewport3D> Servants
    {
        get
        {
            if (_servants == null)
                _servants = new List<Viewport3D>();

            return _servants;
        }
        set
        {
            _servants = value;
        }
    }

    public bool Enabled
    {
        get
        {
            return _enabled && (_servants != null) && (_servants.Count > 0);
        }
        set
        {
            _enabled=value;
        }
    }

    // Updates the matrices of the slaves using the rotation quaternion.
    private void UpdateServants(Quaternion q, double s)
    {
        RotateTransform3D rotation = new RotateTransform3D();//IB: changed this
        
        
        QuaternionRotation3D quatRotation = new QuaternionRotation3D(q);
        rotation.Rotation = quatRotation;
        //rotation.Rotation = new Rotation3D(q);
        rotation.CenterX = _center.X;
        rotation.CenterY = _center.Y;

        ScaleTransform3D scale = new ScaleTransform3D(new Vector3D(s,s,s));
        Transform3DCollection rotateAndScale = new Transform3DCollection();

        rotateAndScale.Add(scale);//IB: moved out of the constructor above
        rotateAndScale.Add(rotation);


        if (_servants != null)
        {
            foreach (Viewport3D i in _servants)
            {
                // Note that we don't copy constantly here, we copy the first time someone tries to
                // trackball a frozen Models, but we replace it with a ChangeableReference
                // and so subsequent interactions go through without a copy.
                /* mijacobs: commenting out
                if (i.Models.Transform.IsFrozen)
                {
                    Model3DGroup mutableCopy = i.Models.Copy();
                    //mutableCopy.StatusOfNextUse = UseStatus.ChangeableReference; IB: no longer necessary I need to architect this out if time permits
                    i.Models = mutableCopy;
                }*/

                Transform3DGroup myTransformGroup = new Transform3DGroup();//IB: added transformGroup
                 myTransformGroup.Children = rotateAndScale;
                
                // mijacobs old: i.Models.Transform = myTransformGroup;
                ((Model3DGroup)((ModelVisual3D)i.Children[0]).Content).Transform = myTransformGroup;
            }
        }
    }
    

    private void MouseMoveHandler(object sender, System.Windows.Input.MouseEventArgs e)
    {
        if (!Enabled) return;
        e.Handled = true;
        
        UIElement el = (UIElement)sender;
        if (el.IsMouseCaptured)
        {
            Vector delta = _point - e.MouseDevice.GetPosition(el);
            delta /= 2;
            // We can redefine this 2D mouse delta as a 3D mouse delta
            // where "into the screen" is Z
            Vector3D mouse = new Vector3D(delta.X,-delta.Y,0);
            Vector3D axis = Vector3D.CrossProduct(mouse, new Vector3D(0,0,1));
            double len = axis.Length;
            if (len < 0.00001 || _scaling)
                _rotationDelta = new Quaternion(new Vector3D(0,0,1),0);
            else
                _rotationDelta = new Quaternion(axis,len);
            Quaternion q = _rotationDelta*_rotation;
            if (_scaling)
            {
                _scaleDelta = Math.Exp(delta.Y/10);
            }
            else
            {
                _scaleDelta = 1;
            }
            UpdateServants(q,_scale*_scaleDelta);
        }
    }

    private void MouseDownHandler(object sender, MouseButtonEventArgs e)
    {
        if (!Enabled) return;
        e.Handled = true;

        if ( e.ClickCount == 2 )
        {
            Reset();
            return;
        }

        UIElement el = (UIElement)sender;
        _point = e.MouseDevice.GetPosition(el);
        // Initialize the center of rotation to the lookatpoint
        if (!_centered)
        {
            ProjectionCamera camera = (ProjectionCamera)_servants[0].Camera;
            _center = (Point3D)camera.LookDirection;
            _centered = true;
        }
        _scaling = (e.RightButton == MouseButtonState.Pressed);

        el.CaptureMouse();
    }

    private void MouseUpHandler(object sender, MouseButtonEventArgs e)
    {
        if (!_enabled) return;
        e.Handled = true;
        
        // Stuff the current initial + delta into initial so when we next move we
        // start at the right place.
        _rotation = _rotationDelta*_rotation;
        _scale = _scaleDelta*_scale;
        UIElement el = (UIElement)sender;
        el.ReleaseMouseCapture();
    }
    
    private void MouseDoubleClickHandler(object sender, MouseButtonEventArgs e)
    {
        Reset();
    }

    private void Reset()
    {
        _rotation = Quaternion.Identity;
        _scale = 1;
        // Clear delta too, because if reset is called because of a double click then the mouse
        // up handler will also be called and this way it won't do anything.
        _rotationDelta = Quaternion.Identity;
        _scaleDelta = 1;
        UpdateServants(_rotation,_scale);
    }

    // The state of the trackball
    private bool _enabled;
    private Point3D _center;
    private bool _centered;     // Have we already determined the rotation center?
    private double _scale;
    private Quaternion _rotation;
    private List<Viewport3D> _servants;
    
    // The state of the current drag
    private bool _scaling;              // Are we scaling?  NOTE otherwise we're rotating
    private double _scaleDelta;          // Change to scale because of this drag
    private Quaternion _rotationDelta;  // Change to rotation because of this drag
    private System.Windows.Point _point; // Initial point of drag
}
