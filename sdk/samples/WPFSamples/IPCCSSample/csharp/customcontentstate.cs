using System;
using System.ComponentModel;
using System.Windows.Navigation;
using System.Windows.Data;
using System.Windows.Controls;

namespace IPCCSSampleCSharp
{
    [Serializable]
    public class CustomPageContentState : CustomContentState
    {
        double verticalOffset;
        int readCount;

        public CustomPageContentState(double verticalOffset, int readCount)
        {
            // "Remember" vertical offset and read count
            this.verticalOffset = verticalOffset;
            this.readCount = readCount;

            // Note: The Value of the zoom scrollbar is automatically saved because
            // the dependency property has the Journal metadata flag, and the control is
            // placed in the primary logical tree.
            // Neither ScrollViewer nor Label, however, is not automatically journaled.
        }

        public override void Replay(NavigationService navigationService, NavigationMode mode)
        {
            // Apply previous scroll offset
            ScrollViewer scrollViewer = ((HomePage)navigationService.Content).documentScrollViewer;
            scrollViewer.ScrollToVerticalOffset(this.verticalOffset);

            // Increment previous read count and apply
            Label label = ((HomePage)navigationService.Content).readCountLabel;
            label.Content = (++this.readCount);
        }
    }
}
