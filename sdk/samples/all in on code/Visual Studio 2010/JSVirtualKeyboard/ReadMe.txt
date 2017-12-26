=============================================================================
                   JSVirtualKeyboard Project Overview
=============================================================================

Use:

 This project illustrates how to make a virtual keyboard on the page. For
 example, if we need a password textbox somewhere and we need it can only
 be inputted through a virtual keyboard on the page, this sample may give
 a quick start to build a solution for this situation.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample.

Step1: Browse the Default.htm from the sample. We will see a TextBox and a 
number keyboard following the TextBox.

Step2: Click the number button to enter the value into the TextBox and click
Backspace button to delete a number in the TextBox.

Step3: The button order of the virtual number keyboard will be changed every
time the page is refreshed.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1: Create a C# / VB.NET ASP.NET Empty Web Application in Visual Studio 2010.

Step2: Add a Default HTML page into the application called Default.htm.

Step3: Add an HTML Input(Text) control to the page. Set its id to "tbInput".

Step4: Add an HTML Div control to the page and insert 11 Input(Button) controls
inside this Div. Ten of these buttons stand for ten numbers and the last one is
for Backspace button. For layout purpose, please add a <br /> tag between every
three buttons as well.

    <div id="VirtualKey">
        <input id="btn1" type="button" value="1"/>
        <input id="btn2" type="button" value="2"/>
        <input id="btn3" type="button" value="3"/>
        <br />
        <input id="btn4" type="button" value="4"/>
        <input id="btn5" type="button" value="5"/>
        <input id="btn6" type="button" value="6"/>
        <br />
        <input id="btn7" type="button" value="7"/>
        <input id="btn8" type="button" value="8"/>
        <input id="btn9" type="button" value="9"/>
        <br />
        <input id="btn0" type="button" value="0"/>
        <input id="btnDel" type="button" value="Backspace" />
    </div>

Step5: Create two JavaScritp functions on the page called input(e) and del(). 
The input(e) function will enter the number into the TextBox based on the 
parameter e. And the del() function will delete a number from the TextBox.

    function input(e) {
        var tbInput = document.getElementById("tbInput
        tbInput.value = tbInput.value + e.value;
    }

    function del() {
        var tbInput = document.getElementById("tbInput");
        tbInput.value = tbInput.value.substr(0, tbInput.value.length - 1);
    }

Step6: Set the onclick event of the ten number buttons equal to input(e) 
function and make the parameter to be the button itself. Also set the onclick
event of the Backspace button to del();.

    <div id="VirtualKey">
        <input id="btn1" type="button" value="1" onclick="input(this);" />
        <input id="btn2" type="button" value="2" onclick="input(this);" />
        <input id="btn3" type="button" value="3" onclick="input(this);" />
        <br />
        <input id="btn4" type="button" value="4" onclick="input(this);" />
        <input id="btn5" type="button" value="5" onclick="input(this);" />
        <input id="btn6" type="button" value="6" onclick="input(this);" />
        <br />
        <input id="btn7" type="button" value="7" onclick="input(this);" />
        <input id="btn8" type="button" value="8" onclick="input(this);" />
        <input id="btn9" type="button" value="9" onclick="input(this);" />
        <br />
        <input id="btn0" type="button" value="0" onclick="input(this);" />
		<input id="btnDel" type="button" value="Backspace" onclick="del();" />
	</div>

NOTE: Till now, we can run this sample and input the number to the TextBox
by click the button of the virtual number keyboard. The steps below will
tell how to make the button order different when refresh the page.

Step7: Set the onload event of the page to load() function to disorder the
number button.

    function load() {
        var array = new Array();

        while (array.length < 10) {
            var temp = Math.round(Math.random() * 9);
            if (!contain(array, temp)) {
                array.push(temp);
            }
        }
        for (i = 0; i < 10; i++) {
            var btn = document.getElementById("btn" + i);
            btn.value = array[i];
        }
    }

NOTE: After the while loop, we will get an array of 10 random number from 0 
to 9 without repeat. For example: (3,0,5,4,8,6,1,2,9,7). Then, we will set 
the value of the number buttons to each item in this array. This will give 
us a disordered virtual number keyboard.

/////////////////////////////////////////////////////////////////////////////
References:

For a more advanced virtual keyboard, please refer to these links:

# Creating a Keyboard with CSS and jQuery 
http://net.tutsplus.com/tutorials/javascript-ajax/creating-a-keyboard-with-css-and-jquery/

# Creating a Virtual jQuery Keyboard
http://designshack.co.uk/articles/javascript/creating-a-virtual-jquery-keyboard

/////////////////////////////////////////////////////////////////////////////