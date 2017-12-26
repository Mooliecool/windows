//ScrollViewer_Code.cpp file

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Media;
using namespace System::Windows::Shapes;
using namespace System::Threading;

namespace ScrollViewer_Sample {

   public ref class app : Application {

   private: 
      Rectangle^ rect1;
      ScrollViewer^ scrView1;
      StackPanel^ sPanel;
      Window^ mainWindow;
      TextBlock^ txt1;
      TextBlock^ txt2;

   protected: 
      virtual void OnStartup (StartupEventArgs^ e) override 
      {
         Application::OnStartup(e);
         CreateAndShowMainWindow();
      };

   public: 
      void sChanged (System::Object^ sender, ScrollChangedEventArgs^ e) 
      {
         txt1->Text = "ScrollViewer.HorizontalScrollBarVisibility is set to: " + scrView1->ComputedHorizontalScrollBarVisibility.ToString();
         txt2->Text = "ScrollViewer.VerticalScrollBarVisibility is set to: " + scrView1->ComputedVerticalScrollBarVisibility.ToString();
      };

   public: 
      void CreateAndShowMainWindow () 
      {
         // Create the application's main window
         mainWindow = gcnew Window();
         mainWindow->Height = 400;
         mainWindow->Width = 400;


         // Create a ScrollViewer
         scrView1 = gcnew ScrollViewer();
         txt1 = gcnew TextBlock();
         txt2 = gcnew TextBlock();
         scrView1->CanContentScroll = true;
         scrView1->HorizontalScrollBarVisibility = ScrollBarVisibility::Auto;
         scrView1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;

         // Create a StackPanel
         sPanel = gcnew StackPanel();
         sPanel->Orientation = Orientation::Vertical;
         sPanel->Children->Add(txt1);
         sPanel->Children->Add(txt2);

         // Add the first rectangle to the StackPanel
         rect1 = gcnew Rectangle();
         rect1->Stroke = Brushes::Black;
         rect1->Fill = Brushes::SkyBlue;
         rect1->Height = 400;
         rect1->Width = 400;
         sPanel->Children->Add(rect1);

         scrView1->ScrollChanged += gcnew ScrollChangedEventHandler(this,&ScrollViewer_Sample::app::sChanged);
         scrView1->Content = sPanel;
         mainWindow->Content = scrView1;
         mainWindow->Title = "ScrollViewer Sample";
         mainWindow->Show();


      };
   };

   private ref class EntryClass sealed {

   public: 
      [System::STAThread()]
      static void Main () 
      {
         ScrollViewer_Sample::app^ app = gcnew ScrollViewer_Sample::app();
         app->Run();
      };
   };
}

//Entry Point:
[System::STAThreadAttribute()]
void main ()
{
   return ScrollViewer_Sample::EntryClass::Main();
}
