<?php
  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: bugs.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: 12-12-2014
    * DETAILS: Allows admins to file bug reports directly to the Program O dev team
    ***************************************/

  $upperScripts = $template->getSection('UpperScripts');
  $post_vars = filter_input_array(INPUT_POST);
  if (count($post_vars) > 0)
  {
    foreach ($post_vars as $key => $value) {
      $$key = $value;
    }
  }
  $func = (isset($post_vars['func'])) ? $post_vars['func'] : 'showBugForm';
# Build page sections
# ordered here in the order that the page is constructed
# Only the variables that are different from the
# login page need be set here.
    $topNav        = $template->getSection('TopNav');
    $leftNav       = $template->getSection('LeftNav');
    $main          = $template->getSection('Main');
    
    $navHeader     = $template->getSection('NavHeader');
    
    $FooterInfo    = getFooter();
    $errMsgClass   = (!empty($msg)) ? "ShowError" : "HideError";
    $errMsgStyle   = $template->getSection($errMsgClass);
    $noLeftNav     = '';
    $noTopNav      = '';
    $noRightNav    = $template->getSection('NoRightNav');
    $headerTitle   = 'Actions:';
    $pageTitle     = 'My-Program O - Report a Bug';
    $mainContent   = $func();
    $lowerScripts  .= '      <script type="text/javascript">var fName = document.contactForm.name;fName.focus();</script>';
    $mainTitle     = 'Send a Bug Report';


  /**
   * Function showBugForm
   *
   *
   * @return string
   */
  function showBugForm() {
    global $debugemail, $template;
    $out = $template->getSection('BugForm');
    $out = str_replace('[blank]', '', $out);
    $out = str_replace('[debugemail]', $debugemail, $out);
    return $out;
  }

  /**
   * Function sendMail
   *
   *
   * @return string
   */
  function sendMail() {
    global $email, $name, $subject, $message, $captcha;
    #print "<!-- Ginger message = $message --\n";
    $_SESSION['message'] = $message;
    $rawCap = $captcha;
    $captcha = sha1(strtolower($captcha));
    $capKey = (isset($_SESSION["capKey"])) ? $_SESSION["capKey"] : "";
    $out = "";
    $time = date('h:j:s');
    $date = date('m/d/Y');
    $remoteAddr = $_SERVER['REMOTE_ADDR'];
    $remoteHost = (isset($_SERVER['REMOTE_HOST'])) ? $_SERVER['REMOTE_HOST'] : 'localhost';
    $localServer = $_SERVER['HTTP_HOST'];
    $senderInfo = "\n\nThis message was sent through the Program O Bug Reporting System at $localServer, from $remoteAddr at $time on $date\n";
    $message .= $senderInfo;
    $cba = checkBadAddress($email);
    $cbip = checkBadIP();
	if ($email != "" and $name != "" and $subject != "" and $cba == 0 and $cbip == 0 and $message != "" and ($captcha == $capKey)) {
      //$toAddr = "dmorton@geekcavecreations.com, " . BUGS_EMAIL;
      $toAddr = BUGS_EMAIL;
      $fromAddr = "$email";
      $header = "From: $name <$email>";
      $result = mail ($toAddr, $subject, $message, $header);
      $out .= <<<endThanx
      <p>
      Ok, message sent. Thanx for taking the time to submit your bug report.
      With your help, we can make Program O even better than ever!<br />
      </p>
      <p style="text-align:center">
        <a href="./">Home</a>
      </p>
endThanx;
    }
    Else {
      $description = "";
      $description .= ($cba == 1)              ? "        <li>Your email address is on our ban list.</li>\n" : "";
      $description .= ($cbip == 1)             ? "        <li>Your IP address is on our ban list.</li>\n" : "";
      $description .= ($name == "")            ? "        <li>The name field was left blank.</li>\n" : "";
      $description .= ($subject == "")         ? "        <li>The subject field was left blank.</li>\n" : "";
      $description .= ($message == "")         ? "        <li>The message field was left blank.</li>\n" : "";
      #$description .= ($captcha != $capKey) ? "        <li>The typed CAPTCHA did not match the image (image was $capKey and text was $captcha. Text entered was $rawCap).</li>\n" : "";
      $description .= ($captcha != $capKey) ? "        <li>The typed CAPTCHA did not match the image.</li>\n" : "";
      $insert = ($cba == 1 or $cbip == 1) ? " don't" : "";

      $out .= <<<endOops
      <ul>The following errors need to be addressed:
        $description
      </ul>
      Please$insert try again.<br />      <a href="#" onclick='history.go(-1)'>Back</a>
endOops;
    }
    return $out;
  }

  /**
   * Function checkBadAddress
   *
   * * @param $address
   * @return int
   */
  function checkBadAddress ($address) {
    $out = 0;
	$excluded = array("namecheap2.ehost-services150.com", "rxciales.info", "mail.ru", "rxcilliss.info", "PaulkyLyday@gmail.com");
	foreach ($excluded as $check) {
      $isPresent =  strpos($address, $check);
      if ($isPresent !== false) $out = 1;
	}
	return $out;
  }

  /**
   * Function checkBadIP
   *
   *
   * @return int
   */
  function checkBadIP () {
    $IP = $_SERVER['REMOTE_ADDR'];
    $out = 0;
	$excluded = array("89.28.114", "85.140.66.54");
	foreach ($excluded as $check) {
      $isPresent =  strpos($IP, $check);
      if ($isPresent !== false) $out = 1;
	}
	return $out;
  }
?>