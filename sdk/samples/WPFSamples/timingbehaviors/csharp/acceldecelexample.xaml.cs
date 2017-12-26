using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Data;


namespace Microsoft.Samples.Animation.TimingBehaviors
{


    public partial class AccelDecelExample : Page
    {
    
        
        public AccelDecelExample()
        {
            
        }
        
        private void stateInvalidated(object sender, EventArgs args)
        {
            if (sender != null){
                elapsedTime.Clock = (Clock)sender; 
            }
        }
        
    }


}
