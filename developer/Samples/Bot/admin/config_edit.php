<?php
  $e_all = defined('E_DEPRECATED') ? E_ALL & ~E_DEPRECATED : E_ALL;
  error_reporting($e_all);
  ini_set('display_errors', 1);
  $thisFile = __FILE__;
  require_once('../config/global_config.php');
  $all_vars = get_defined_vars();
  $contents = file_get_contents('../config/config.template.php');
  $contents = str_replace('<?php', '', $contents);
  $contents = str_replace('?>', '', $contents);
  $search = '~\[(\w+?)\]~';
  $contents = preg_replace_callback($search, 'make_input', $contents);


  /**
   * Function make_input
   *
   * * @param $matches
   * @return string
   */
  function make_input($matches)
  {
    global $all_vars;
    $my_var = (in_array($matches[1], array_keys($all_vars))) ? $all_vars[$matches[1]] : '';
    $replace = '<input name="' . $matches[1] . '" value="' . $my_var . '" />';
    return $replace;
  }
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>Program O Global Config File Editor</title>
    <style type="text/css">
      h3 {
        text-align: center;
      }
      #config_div {
        width: 95%;
        max-height: 500px;
        min-height: 250px;
        height: 80%;
        overflow: auto;
      }
      #save_btn {
        text-align: center;
      }
    </style>
  </head>
  <body>
    <h3>Program O Config File Editor</h3>
    <p>
      Use this (somewhat unconventional) form to edit your chatbot's confituration
      file directly. Only the settings that you <strong>should</strong> need to change are
      available here. If you need to change other settings, then you'll need to
      make those changes in a text editor.
    </p>
    <form name="config" action="config_edit.php">
      <hr />
      <div id="config_div">
        <pre>
<?php echo $contents ?>
        </pre>
      </div>
      <hr />
      <div id="save_btn"><input type="submit" name="save" value="Save" /></div>
    </form>
  </body>
</html>