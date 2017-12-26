<?php
/***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: index.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: 07-23-2013
  * DETAILS: This is the XML GUI interface for Program O
  ***************************************/


    $e_all = defined('E_DEPRECATED') ? E_ALL & ~E_DEPRECATED : E_ALL;
    error_reporting($e_all);
    ini_set('log_errors', true);
    ini_set('error_log', '../../logs/gui.xml.error.log');
    ini_set('html_errors', false);
    ini_set('display_errors', false);

  // Experimental code
  $base_URL  = 'http://' . $_SERVER['HTTP_HOST'];                                   // set domain name for the script
  $this_path = str_replace(DIRECTORY_SEPARATOR, '/', realpath(dirname(__FILE__)));  // The current location of this file, normalized to use forward slashes
  $this_path = str_replace($_SERVER['DOCUMENT_ROOT'], $base_URL, $this_path);       // transform it from a file path to a URL
  $url = str_replace('gui/xml', 'chatbot/conversation_start.php', $this_path);      // and set it to the correct script location
/*
  Example URL's for use with the chatbot API
  $url = 'http://api.program-o.com/v2.3.1/chatbot/';
  $url = 'http://localhost/Program-O/Program-O/chatbot/conversation_start.php';
  $url = 'chat.php';
*/

  $display = "The URL for the API is currently set as:<br />\n$url.<br />\n";
  $display .= 'Please make sure that you edit this file to change the value of the variable $url in this file to reflect the correct URL address of your chatbot, and to remove this message.' . PHP_EOL;
  #$display = '';
  $display_template = <<<end_display
      <span class="user_name">[user_name]: </span><span class="user_say">[input]</span><br>
      <span class="bot_name">[bot_name]: </span><span class="bot_say">[response]</span><br>

end_display;

  $options = array(
    'say'       => FILTER_SANITIZE_STRING,
    'format'    => FILTER_SANITIZE_STRING,
    'bot_id'    => FILTER_SANITIZE_STRING,
    'convo_id'  => array(
      'filter'    => FILTER_CALLBACK,
      'options'   => 'validateConvoId'
    )
  );
  $post_vars = filter_input_array(INPUT_POST, $options);
  $get_vars = filter_input_array(INPUT_GET, $options);
  $request_vars = array_merge((array)$get_vars, (array)$post_vars);
  echo ("<!-- POST vars:\n" . print_r($_POST, true) . "\nGET vars:\n" . print_r($_GET, true) . "\$request_vars:\n" . print_r($request_vars, true) . "\n-->\n");
  $convo_id = (isset ($request_vars['convo_id'])) ? $request_vars['convo_id'] : get_convo_id();
  $bot_id = (isset ($request_vars['bot_id'])) ? $request_vars['bot_id'] : 1;
  if (!empty ($post_vars))
  {
    $options = array(
      CURLOPT_USERAGENT => 'Program_O_XML_API',
      CURLOPT_RETURNTRANSFER => true,
      CURLOPT_POST => true,
      //CURLOPT_CONNECTTIMEOUT => 3,
    );
    $ch = curl_init($url);
    curl_setopt_array($ch, $options);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $request_vars);
    $data = curl_exec($ch);
    curl_close($ch);
    $xml = new SimpleXMLElement($data);
    $display = '';
    $success = $xml->status->success;
    if (isset($xml->status->message))
    {
      $message = (string) $xml->status->message;
      $display = 'There was an error in the script. Message = ' . $message;
    }
    else
    {
      $user_name = (string) $xml->user_name;
      $bot_name = (string) $xml->bot_name;
      $chat = $xml->chat;
      $lines = $chat->xpath('line');
      foreach ($lines as $line)
      {
        $input = (string) $line->input;
        $response = (string) $line->response;
        $tmp_row = str_replace('[user_name]', $user_name, $display_template);
        $tmp_row = str_replace('[bot_name]', $bot_name, $tmp_row);
        $tmp_row = str_replace('[input]', $input, $tmp_row);
        $tmp_row = str_replace('[response]', $response, $tmp_row);
        $display .= $tmp_row;
      }
    }

  }

  /**
   * Function get_convo_id
   *
   *
   * @return string
   */
  function get_convo_id()
  {
    if (isset($_COOKIE['Program_O_XML_API'])) $convo_id = $_COOKIE['Program_O_XML_API'];
    else
    {
      session_name('Program_O_XML_API');
      session_start();
      $convo_id = session_id();
      session_destroy();
    }
    return $convo_id;
  }

  function validateConvoId($convo_id)
  {
    $id = htmlentities($convo_id);
    return ($id === $convo_id) ? $convo_id : get_convo_id();
  }

?>
<!doctype html>
<html>
  <head>
    <meta charset="utf-8">
    <link rel="icon" href="./favicon.ico" type="image/x-icon" />
    <link rel="shortcut icon" href="./favicon.ico" type="image/x-icon" />
    <title>Program O AIML Chatbot</title>
    <meta name="Description" content="A Free Open Source AIML PHP MySQL Chatbot called Program-O. Version2" />
    <meta name="keywords" content="Open Source, AIML, PHP, MySQL, Chatbot, Program-O, Version2" />
    <style type="text/css">
      h3 {
        text-align: center;
      }
      .user_name {
        color: rgb(16, 45, 178);
      }
      .bot_name {
        color: rgb(204, 0, 0);
      }
      #shameless_plug {
        position: absolute;
        right: 10px;
        bottom: 10px;
        border: 1px solid red;
        box-sizing: border-box;
        -moz-box-sizing: border-box;
        box-shadow: 2px 2px 2px 0 #808080;
        padding: 5px;
        border-radius: 5px;
      }
    </style>
  </head>
  <body onload="document.forms[0].say.focus();">
    <h3>Program O XML GUI</h3>
    <form accept-charset="utf-8" method="post" action="index.php">
      <p>
        <input type="text" name="say" id="say" size="70" />
        <input id="bot_id" type="hidden" name="bot_id" value="<?php echo $bot_id ?>">
        <input id="convo_id" type="hidden" name="convo_id" value="<?php echo $convo_id ?>">
        <input id="format" type="hidden" name="format" value="xml">
        <input type="submit" value="Chat" />
      </p>
    </form>
    <div id="response">
<?php echo $display ?>
    </div>
    <div id="shameless_plug">
      To get your very own chatbot, visit <a href="http://www.program-o.com">program-o.com</a>!
    </div>
  </body>
</html>
