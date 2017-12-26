Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Navigation

Namespace dockpanel_dockprop_vb

    '@ <summary>
    '@ Interaction logic for Pane1_xaml.xaml
    '@ </summary>
    Partial Class Pane1

        Private Sub OnClick1(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect1, System.Windows.Controls.Dock.Left)
            Txt1.Text = "The Dock property of the LightCoral Rectangle is set to Left"
        End Sub
        
        Private Sub OnClick2(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect1, System.Windows.Controls.Dock.Right)
            Txt1.Text = "The Dock property of the LightCoral Rectangle is set to Right"

        End Sub

        Private Sub OnClick3(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect1, System.Windows.Controls.Dock.Top)
            Txt1.Text = "The Dock property of the LightCoral Rectangle is set to Top"

        End Sub

        Private Sub OnClick4(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect1, System.Windows.Controls.Dock.Bottom)
            Txt1.Text = "The Dock property of the LightCoral Rectangle is set to Bottom"

        End Sub

        Private Sub OnClick5(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect2, System.Windows.Controls.Dock.Left)
            Txt2.Text = "The Dock property of the LightSkyBlue Rectangle is set to Left"

        End Sub

        Private Sub OnClick6(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect2, System.Windows.Controls.Dock.Right)
            Txt2.Text = "The Dock property of the LightSkyBlue Rectangle is set to Right"

        End Sub

        Private Sub OnClick7(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect2, System.Windows.Controls.Dock.Top)
            Txt2.Text = "The Dock property of the LightSkyBlue Rectangle set to Top"

        End Sub

        Private Sub OnClick8(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            System.Windows.Controls.DockPanel.SetDock(rect2, System.Windows.Controls.Dock.Bottom)
            Txt2.Text = "The Dock property of the LightSkyBlue Rectangle is set to Bottom"

        End Sub

        Private Sub OnClick9(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            myDP.LastChildFill = True
            Txt3.Text = "The LastChildFill property is set to True (default)"

        End Sub

        Private Sub OnClick10(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            myDP.LastChildFill = False
            Txt3.Text = "The LastChildFill property is set to False"

        End Sub

    End Class

End Namespace