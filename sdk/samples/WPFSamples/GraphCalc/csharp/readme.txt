/*****************************
 *  
 * Program: GraphCalc - 12/31/04
 * 
 *
 * Bugs listed at the bottom
 *
 *****************************/

	Hopefully the GUI is mostly self explanatory.  I just wanted to point out the graph
viewing features that aren't obvious.

	There is a bug in one of the Registry APIs I'm using (to load the default options
and equations).  I added a workaround to my final version, but a bug in deployment makes
it so that the old version is run if you don't run the hostingcleanup drt first.


- 2D Graphs (y= and parametric):

	- Zoom In:
		Left click the mouse and drag a rectangle over the area you want to zoom in
		to.  The viewport will automatically resize to the size of the rectangle
		drawn.  If you do not want to zoom in after a left click has been processed,
		right click the mouse to make the rectangle disappear (just like rectangle
		selection in mspaint).

	- Zoom Out:
		Right click the mouse to zoom out by a factor of 2 (the zoom happens from
		the center of the screen, not the mouse cursor's position)


- 3D Graphs:

	- Rotate the Mesh:
		Left click the mouse and drag the cursor around.  It is a trackball-like
		control

	- Zoom In/Out:
		Press and hold the right mouse button, then press and hold the left mouse
		button.  While the two buttons are held, move the mouse up or down to zoom
		in and out.

	- Camera Reset:
		Double click on the screen to reset the camera to its default position


- Parser nuances:

	- "sin" "cos" and "tan":
		The parser requires that the expressions operated upon by these functions be
		surrounded by parentheses.  Therefore, "sin 34" won't work, but "sin( 34 )"
		will.

	- "^":
		"x^y" means x to the yth power.  It's not an XOR.

	- Implicit multiplication:
		The parser allows short-hand constructs like 2x and 7sin(3) for user
		convenience.  To get the same effect with variables and functions, a simple
		space will do.  For example: "u v" will be parsed as u*v, but "uv" will be
		parsed as a single variable "uv"

	- Case-insensitive variables:
		Capital "X" is the same variable as lower-case "x"

	- PI:
		Pi can be accessed by using the variable "pi"

	- Last answer:
		In non-graphing mode, the calculator saves the last valid answer in a variable
		called "ans"
