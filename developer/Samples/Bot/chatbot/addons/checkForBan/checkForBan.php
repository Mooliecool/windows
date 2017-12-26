<?php
  /***************************************************/
  /*                 checkForBan.php                 */
  /*             Program O Addon Script              */
  /* Checks the user's IP address against a list of  */
  /* address that were banned, aborting the script   */
  /* if the address is found, and notifying the user */
  /* of the ban.                                     */
  /* �2011 Geek Cave Creations - All rights Reserved */
  /*                  Coded by Dave                  */
  /***************************************************/

  ####################################################
  # I'm planning on integrating this addon into the  #
  # database, at some point, rather than making use  #
  # of a text file. This will increase security of   #
  # script, overall, but will require the addition   #
  # of a new table in the database, which means that #
  # an install script will also need to be created,  #
  # along with an uninstall script, so that if the   #
  # addon is ever removed, it won't leave anything   #
  # behind, such as a 'Banned Users' table.          #
  ####################################################

  /**
   * Function checkIP
   *
   * * @param $convoArr
   * @return mixed
   */
  function checkIP($convoArr) {
    global $debugemail;
    $pathInfo = pathinfo(realpath(__FILE__));
    $thisDir = $pathInfo['dirname'];
    $banList = $thisDir . DIRECTORY_SEPARATOR . 'banList.txt';
    $IP = $_SERVER['REMOTE_ADDR'];
    if (!file_exists($banList)) $x = file_put_contents($banList,'');
    $ipList = file($banList, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    $l = print_r($ipList, true);
    if (in_array($IP,$ipList)) {
      $convoArr['send_to_user'] = <<<endMessage
You have been banned from using this chat interface.
If you feel that this is in error, please contact
<a href="mailto:$debugemail">$debugemail</a>.
endMessage;
      $convoArr['abort'] = true;
      exit($convoArr['send_to_user']);
    }
    return $convoArr;
  }

  /**
   * Function add_to_ban
   *
   * * @param $ip
   * @return void
   */
  function add_to_ban($ip)
  {
    $pathInfo = pathinfo(realpath(__FILE__));
    $thisDir = $pathInfo['dirname'];
    $banList = $thisDir . DIRECTORY_SEPARATOR . 'banList.txt';
    $x = file_put_contents($banList, $ip . PHP_EOL, FILE_APPEND);
  }

?>
