================================================================================
       WPF MVVM Pracice Project Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to implement the MVVM pattern in a WPF application.
   
/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build the sample project in Visual Studio 2008.

Step2. Click on the cells in the grid. 

Step3. If one player has won the game, a messagebox pops up saying "XX has won,Congratulations!".

Step4. If all the cells in the grid are clicked, but no one has won, a messagebox pops up saying "No winner".

Step5. You can change the dimension of the game using the Game menu.

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Model: 
	Cell class - represents a cell in the tic-tac-toe game grid
	PlayerMove class - represents a player move in the game

2. ViewModel:
	TicTacToeViewModel - contains game's logic and data

3. View:
	MainWindow.xaml - contains a Menu and an ItemsControl

4. Others:
	a. Attached behavior
		ChangeDimensionBehavior - connect the MenuItem in the View to the TicTacToeViewModel in order to change the game's dimension
		ShutdownBehavior - contains code to exit the application		
		GameOverBehavior - listen to the GameOver event on the TicTacToeViewModel and show a messagebox reporting the game result.
	b. ValueConverter
		IntToBoolValueConverter - used to check/uncheck a MenItem that is used to change the dimension of the game	
	c.Command
		RelayCommand - provides an implementation of the ICommand interface

/////////////////////////////////////////////////////////////////////////////
References:

WPF Apps With The Model-View-ViewModel Design Pattern
http://msdn.microsoft.com/en-us/magazine/dd419663.aspx

Introduction to Attached Behaviors in WPF
http://www.codeproject.com/KB/WPF/AttachedBehaviors.aspx 

/////////////////////////////////////////////////////////////////////////////


