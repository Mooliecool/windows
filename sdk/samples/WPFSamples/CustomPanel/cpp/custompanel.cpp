//CustomPanel.cpp file

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Media;
using namespace System::Windows::Shapes;
using namespace System::Threading;

namespace Custom_Panel {

   public ref class app : Application {
   public: 
      ref class CustomPanel : Panel {

      public: 
         CustomPanel () 
         {};
      protected: 
         virtual System::Windows::Size MeasureOverride (System::Windows::Size constraint) override 
         {
            System::Windows::Size curLineSize = Size();
            System::Windows::Size panelSize = Size();
            UIElementCollection^ children = InternalChildren;

            for (
               int i = 0;
               i < children->Count;
            i++)
            {
               UIElement^ child = (UIElement^)children[i];
               child->Measure(constraint);
               Size sz = child->DesiredSize;
               if (curLineSize.Width + sz.Width > constraint.Width) //need to switch to another line
               {
                  panelSize.Width = Math::Max(curLineSize.Width, panelSize.Width);
                  panelSize.Height += curLineSize.Height;
                  curLineSize = sz;

                  if (sz.Width > constraint.Width) // if the element is wider then the constraint - give it a separate line                    
                  {
                     panelSize.Width = Math::Max(sz.Width, panelSize.Width);
                     panelSize.Height += sz.Height;
                     curLineSize = Size();
                  }
               }
               else //continue to accumulate a line
               {
                  curLineSize.Width += sz.Width;
                  curLineSize.Height = Math::Max(sz.Height, curLineSize.Height);
               }
            }

            panelSize.Width = Math::Max(curLineSize.Width, panelSize.Width);
            panelSize.Height += curLineSize.Height;
            return panelSize;
         };

      protected: 
         virtual System::Windows::Size ArrangeOverride (System::Windows::Size arrangeBounds) override 
         {
            int firstInLine = 0;
            System::Windows::Size curLineSize = System::Windows::Size();
            double accumulatedHeight = 0;
            UIElementCollection^ children = InternalChildren;

            for (
               int i = 0;
               i < children->Count;
            i++)
            {
               System::Windows::Size sz = children[i]->DesiredSize;
               if (curLineSize.Width + sz.Width > arrangeBounds.Width)
               {
                  arrangeLine(accumulatedHeight, curLineSize.Height, firstInLine, i);
                  accumulatedHeight += curLineSize.Height;
                  curLineSize = sz;
                  if (sz.Width > arrangeBounds.Width)
                  {
                     arrangeLine(accumulatedHeight, sz.Height, i, ++i);
                     accumulatedHeight += sz.Height;
                     curLineSize = System::Windows::Size();
                  }
                  firstInLine = i;
               } else
               {
                  curLineSize.Width += sz.Width;
                  curLineSize.Height = Math::Max(sz.Height, curLineSize.Height);
               }
            }

            if (firstInLine < children->Count)
            {
               arrangeLine(accumulatedHeight, curLineSize.Height, firstInLine, children->Count);
            }
            return arrangeBounds;
         };

      private: 
         void arrangeLine (double y, double lineHeight, int start, int end) 
         {
            double x = 0;
            UIElementCollection^ children = InternalChildren;

            for (
               int i = start;
               i < end;
            i++)
            {
               System::Windows::UIElement^ child = children[i];
               child->Arrange(System::Windows::Rect(x, y, child->DesiredSize.Width, lineHeight));
               x += child->DesiredSize.Width;
            }

         };
      };
   private: 
      System::Windows::Window^ mainWindow;
      Custom_Panel::app::CustomPanel^ customPanel1;
      Rectangle^ rect1;
      Rectangle^ rect2;
      Rectangle^ rect3;
      TextBlock^ txt1;

   protected: 
      virtual void OnStartup (System::Windows::StartupEventArgs^ e) override 
      {
         Application::OnStartup(e);
         CreateAndShowMainWindow();
      };

   private: 
      void CreateAndShowMainWindow () 
      {
         // Create the application's main window and instantiate a CustomPanel

         customPanel1 = gcnew Custom_Panel::app::CustomPanel();
         customPanel1->Width = 450;
         customPanel1->Height = 450;

         // Add elements to populate the CustomPanel

         rect1 = gcnew Rectangle();
         rect2 = gcnew Rectangle();
         rect3 = gcnew Rectangle();
         rect1->Width = 200;
         rect1->Height = 200;
         rect1->Fill = Brushes::Blue;
         rect2->Width = 200;
         rect2->Height = 200;
         rect2->Fill = Brushes::Purple;
         rect3->Width = 200;
         rect3->Height = 200;
         rect3->Fill = Brushes::Red;

         // Add a TextBlock to show flowing and clip behavior

         txt1 = gcnew TextBlock();
         txt1->Text = "Text is clipped when it reaches the edge of the container";
         txt1->FontSize = 25;

         // Add the elements defined above as children of the CustomPanel

         customPanel1->Children->Add(rect1);
         customPanel1->Children->Add(rect2);
         customPanel1->Children->Add(rect3);
         customPanel1->Children->Add(txt1);

         // Add the CustomPanel as a Child of the MainWindow and show the Window

         mainWindow = gcnew System::Windows::Window();
         mainWindow->Content = customPanel1;
         mainWindow->Title = "Custom Panel Sample";
         mainWindow->Show();


      };
   };
   private ref class EntryClass {

   public: 
      [System::STAThread()]
      static void Main () 
      {
         Custom_Panel::app^ app = gcnew Custom_Panel::app();
         app->Run();
      };
   };
};



//Entry Point:
[System::STAThreadAttribute()]
void main ()
{
   return Custom_Panel::EntryClass::Main();
}
