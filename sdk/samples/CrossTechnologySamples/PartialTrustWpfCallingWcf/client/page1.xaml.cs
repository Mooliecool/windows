using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Deployment.Application;
using System.Security.Policy;

namespace PartialTrustWpfCallingWcf {
  public partial class Page1 : Page {

    public Page1() {
      InitializeComponent();

      // Keep the page alive between navigations
      // to keep the proxy alive between navigations
      KeepAlive = true;
    }

    protected override void OnInitialized(EventArgs e) {
      base.OnInitialized(e);

      // Handle button click
      _equalsButton.Click += _equalsButton_Click;

      // Handle async completions
      proxy.AddCompleted += proxy_AddCompleted;
      proxy.SubtractCompleted += proxy_SubtractCompleted;
      proxy.MultiplyCompleted += proxy_MultiplyCompleted;
      proxy.DivideCompleted += proxy_DivideCompleted;

      // In partial trust, only allowed to call back into the originating server
      proxy.Url = GetUpdateLocationUrl(proxy.Url);

      // TODO: Reclaim resources when the page is finally unloaded
      //((Window)this.Parent).Closed += new EventHandler(Page1Parent_Closed);

      _numberTextBlock.Text = proxy.Url;
    }

    void Page1Parent_Closed(object sender, EventArgs e) {
      // Clean up proxy when Window closes
      proxy.Dispose();
    }

    // Generated with wsdl.exe to enable partial trust access
    // Keep proxy for lifetime of Window
    CalcPlusService proxy = new CalcPlusService();

    // Update an URL to use the host of the ClickOnce update location to enable
    // partial-trust clients to call back to their originating server
    // (the only place they're allowed to call by default), e.g. if this app was launched
    // from "http://itweb/app.application" and the URL baked into the proxy is for localhost,
    // then "http://localhost/foo.svc" becomes "http://itweb/foo.svc"
    static string GetUpdateLocationUrl(string url) {
      // If not launched using ClickOnce, return the original URL
      if( !ApplicationDeployment.IsNetworkDeployed ) {
        return url;
      }

      // Extract the host from the update location
      Uri updateLocation = ApplicationDeployment.CurrentDeployment.UpdateLocation;
      string host = Site.CreateFromUrl(updateLocation.AbsoluteUri).Name;

      // Update service URL to use update location host
      UriBuilder updatedUrl = new UriBuilder(url);
      updatedUrl.Host = host;
      return updatedUrl.ToString();
    }

    void _equalsButton_Click(object sender, RoutedEventArgs e) {
      double operand1 = double.Parse(_operand1.Text);
      double operand2 = double.Parse(_operand2.Text);
      string opcode = ((TextBlock)_op.SelectedValue).Text;

      // Start the call to the web service
      switch( opcode ) {
        case "+":
        proxy.AddAsync(operand1, true, operand2, true);
        break;

        case "-":
        proxy.SubtractAsync(operand1, true, operand2, true);
        break;

        case "*":
        proxy.MultiplyAsync(operand1, true, operand2, true);
        break;

        case "/":
        proxy.DivideAsync(operand1, true, operand2, true);
        break;
      }

      // Let the user know
      _numberTextBlock.Text = "";
      _wordsTextBlock.Text = "(getting results)";
    }

    void proxy_AddCompleted(object sender, AddCompletedEventArgs e) {
      _numberTextBlock.Text = e.Result.Number.ToString();
      _wordsTextBlock.Text = e.Result.Words;
    }

    void proxy_SubtractCompleted(object sender, SubtractCompletedEventArgs e) {
      _numberTextBlock.Text = e.Result.Number.ToString();
      _wordsTextBlock.Text = e.Result.Words;
    }

    void proxy_MultiplyCompleted(object sender, MultiplyCompletedEventArgs e) {
      _numberTextBlock.Text = e.Result.Number.ToString();
      _wordsTextBlock.Text = e.Result.Words;
    }

    void proxy_DivideCompleted(object sender, DivideCompletedEventArgs e) {
      _numberTextBlock.Text = e.Result.Number.ToString();
      _wordsTextBlock.Text = e.Result.Words;
    }

  }
}
