<?php
/***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: index.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: FEB 01 2016
  * DETAILS: This is the interface for the Program O JSON API
  ***************************************/
  $cookie_name = 'Program_O_JSON_GUI';
  $botId = filter_input(INPUT_GET, 'bot_id');
  $convo_id = (isset($_COOKIE[$cookie_name])) ? $_COOKIE[$cookie_name] : jq_get_convo_id();
  $bot_id = (isset($_COOKIE['bot_id'])) ? $_COOKIE['bot_id'] :($botId !== false && $botId !== null) ? $botId : 1;
  setcookie('bot_id', $bot_id);
  // Experimental code
  $base_URL  = 'http://' . $_SERVER['HTTP_HOST'];                                   // set domain name for the script
  $this_path = str_replace(DIRECTORY_SEPARATOR, '/', realpath(dirname(__FILE__)));  // The current location of this file, normalized to use forward slashes
  $this_path = str_replace($_SERVER['DOCUMENT_ROOT'], $base_URL, $this_path);       // transform it from a file path to a URL
  $root_url = str_replace('gui/jquery', '', $this_path);   // and set it to the correct script location
  $url = $root_url . 'chatbot/conversation_start.php';   // and set it to the correct script location
/*
  Example URL's for use with the chatbot API
  $url = 'http://api.program-o.com/v2.3.1/chatbot/';
  $url = 'http://localhost/Program-O/Program-O/chatbot/conversation_start.php';
  $url = 'chat.php';
*/

  $display = "The URL for the API is currently set as:<br />\n$url.<br />\n";
  $display .= 'Please make sure that you edit this file to change the value of the variable $url in this file to reflect the correct URL address of your chatbot, and to remove this message.' . PHP_EOL;
  #$display = '';

  /**
   * Function jq_get_convo_id
   *
   *
   * @return string
   */
  function jq_get_convo_id()
  {
    global $cookie_name;
    session_name($cookie_name);
    session_start();
    $convo_id = session_id();
    session_destroy();
    setcookie($cookie_name, $convo_id);
    return $convo_id;
  }

?>
<!DOCTYPE html>
<html>
  <head>
    <link rel="stylesheet" type="text/css" href="main.css" media="all" />
    <link rel="icon" href="./favicon.ico" type="image/x-icon" />
    <link rel="shortcut icon" href="./favicon.ico" type="image/x-icon" />
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Program O AIML PHP Chatbot</title>
    <meta name="Description" content="A Free Open Source AIML PHP MySQL Chatbot called Program-O. Version2" />
    <meta name="keywords" content="Open Source, AIML, PHP, MySQL, Chatbot, Program-O, Version2" />
    <style type="text/css">
      h3 {
        text-align: center;
      }
      hr {
        width: 80%;
        color: green;
        margin-left: 0;
      }

      .user_name {
        color: rgb(16, 45, 178);
      }
      .bot_name {
        color: rgb(204, 0, 0);
      }
      #shameless_plug, #urlwarning {
        position: absolute;
        right: 10px;
        bottom: 10px;
        border: 1px solid red;
        box-sizing: border-box;
        box-shadow: 2px 2px 2px 0 #808080;
        padding: 5px;
        border-radius: 5px;
      }
      #urlwarning {
        right: auto;
        left: 10px;
        width: 50%;
        font-size: large;
        font-weight: bold;
        background-color: white;
      }
      .leftside {
        text-align: right;
        float: left;
        width: 48%;
      }
      .rightside {
        text-align: left;
        float: right;
        width: 48%;
      }
      .centerthis {
        width: 90%;
      }
      #chatdiv {
        margin-top: 20px;
        text-align: center;
        width: 100%;
      }
      #chatlog {
        box-sizing: border-box;
        min-height: 300px;
        max-height: 500px;
        position: relative;
        min-width: 700px;
        max-width: 90%;
        margin: 10px auto;
        background-color: transparent;
        overflow: auto;
        border: 3px inset #C66;
        border-radius: 9px;
        padding: 12px;
      }
      .userTitle {
        color: rgb(77, 38, 204);
        font-weight: bold;
      }
      .botTitle {
        color: rgb(61, 176, 23);
        font-weight: bold;
      }
      #sbBot_id {
        margin-right: 35px;
      }

    </style>
  </head>
  <body>
    <h3>Program O XML GUI</h3>
    <p>
      This is a simple example of how to access the Program O chatbot using the JSON API. Feel free to change the HTML
      code for this page to suit your specific needs. For more advanced uses, please visit us at <a href="http://www.program-o.com/">
        Program O</a>.
    </p>
    <div class="clearthis"></div>
    <div class="centerthis">
      <form method="post" name="talkform" id="talkform" action="index.php">
        <div id="chatdiv">
          Choose a chatbot:
          <select id="sbBot_id" name="bot_id">
            <option value="1">Unknown</option>
          </select>
          <label for="submit">Say:</label>
          <input type="text" name="say" id="say" size="60"/>
          <input type="submit" name="submit" id="submit" class="submit"  value="say" />
          <input type="hidden" name="convo_id" id="convo_id" value="<?php echo $convo_id;?>" />
          <input type="hidden" name="format" id="format" value="json" />
        </div>
      </form>
    </div>
    <div class="centerthis">
      <div id="chatlog">
        <span class="botTitle">chatbot: </span>Hey!<br>
      </div>
    </div>
    <div id="shameless_plug">
      To get your very own chatbot, visit <a href="http://www.program-o.com">program-o.com</a>!
    </div>
    <div id="urlwarning"><?php echo $display ?></div>
    <script type="text/javascript" src="//ajax.google_apis.com/ajax/libs/jquery/1.11.0/jquery.min.js"></script>
    <script>
      window.jQuery || document.write('<script type="text/javascript" src="jquery-1.9.1.min.js">\x3C/script>');
    </script>
    <script type="text/javascript" >
      var gbURL = '<?php echo $root_url ?>getbots.php';
      var botTitle = '<span class="botTitle">';
      var userTitle = '<span class="userTitle">';
      var endSpan = '</span>';
      $(document).ready(function() {
        // Load multiple chatbots into the selectbox
        $.getJSON(gbURL, function(data){
          $('#sbBot_id').html("\n");
          $.each(data.bots, function(bot_id,bot_name){
            $('#sbBot_id').append('            <option value="' + bot_id + '">' + bot_name + "</option>\n");
          });
        });
        $('#sbBot_id').on('change', function(){
          var bn = $('#sbBot_id option:selected').text();
          //$('.botTitle').html(bn + ": ");
          $('#chatlog').html('Now chatting with <span class="botTitle">' + bn + endSpan + "<br>\n");
        });

        // Form submission - This is where the magic happens!
        $('#talkform').submit(function(e) {
          e.preventDefault();
          var bot_name = $('#sbBot_id option:selected').text();
          var user = $('#say').val();
          var userSaid = userTitle + 'User: ' + endSpan + user + "<br>\n";
          $('#chatlog').html($('#chatlog').html() + userSaid);
          var botSaid = botTitle + bot_name + endSpan;
          var formdata = $("#talkform").serialize();
          $('#say').val('')
          $('#say').focus();
          $.post('<?php echo $url ?>', formdata, function(data){
            var b = data.botsay;
            if (b.indexOf('[img]') >= 0) {
              b = showImg(b);
            }
            if (b.indexOf('[link') >= 0) {
              b = makeLink(b);
            }
            var usersay = data.usersay;

            $('#chatlog').html($('#chatlog').html() + botSaid + b + "<br>\n");
          }, 'json').fail(function(xhr, textStatus, errorThrown){
            $('#urlwarning').html("Something went wrong! Error = " + errorThrown);
          });
          return false;
        });
      });
      function showImg(input) {
        var regEx = /\[img\](.*?)\[\/img\]/;
        var repl = '<br><a href="$1" target="_blank"><img src="$1" alt="$1" width="150" /></a>';
        var out = input.replace(regEx, repl);
        console.log('out = ' + out);
        return out
      }
      function makeLink(input) {
        var regEx = /\[link=(.*?)\](.*?)\[\/link\]/;
        var repl = '<a href="$1" target="_blank">$2</a>';
        var out = input.replace(regEx, repl);
        console.log('out = ' + out);
        return out;
      }
    </script>
  </body>
</html>
