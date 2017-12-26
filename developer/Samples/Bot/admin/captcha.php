<?php
     /*******************************************************/
    /*                     CAPTCHA.php                     */
   /* Creates a simple security image, for bot prevention */
  /*           ©2009-2015 Geek Cave Creations            */
 /*                Coded by Dave Morton                 */
/*******************************************************/

  require_once('../config/global_config.php');
  chdir( dirname ( __FILE__ ) );
  $thisFolder = dirname( realpath( __FILE__ ) ) . DIRECTORY_SEPARATOR;
  $log_path = str_replace('admin' . DIRECTORY_SEPARATOR, 'logs' . DIRECTORY_SEPARATOR, $thisFolder);
  $captcha_path = $thisFolder . 'captcha-images' . DIRECTORY_SEPARATOR;
  define('CAPTCHA_PATH', $captcha_path);

  $e_all = defined('E_DEPRECATED') ? E_ALL & ~E_DEPRECATED : E_ALL;
  error_reporting($e_all);
  ini_set('log_errors', 1);
  ini_set('display_errors', 0);
  ini_set('error_log', _LOG_PATH_ . 'CAPTCHA.error.log');

  // Set session config
  $session_name = 'PGO_Admin';
  session_name($session_name);
  session_start();
/*
captcha.php comments/help

This particular CAPTCHA script takes a slightly different approach to proving that
the entity is human,rather than a bot, by combining both a simple question and a
small, randomly chosen image to create the CAPTCHA image. Initially, this form of
test can be "broken" with "supervised training" of the AI, due to the limited number
of test images employed, but it can easily be augmented by either adding more images
to the included data file and directory, or perhaps by "scraping" Google Image Search
for certain descriptive terms. This is an avenue I wish to explore in greater detail
later.


To use this script, simply create an image tag, with this file as the SRC. This
script has a few options available for use in the query string, which are listed below:

b (flag) (no value) causes the CAPTCHA image to have a black background
g (flag) (no value) causes the CAPTCHA image to use a gradient background
w (int) (200) Sets the width of the image
h (int) (75) Sets the height of the image

Flag variables don't require a value in the query string. Simply using the variable
alone is sufficient to trigger it's use. For example, the following will create
an image 300 pixels wide, by 100 pixels tall, using a black background:

<img src="captcha.php?b&amp;w=300&amp;h=100" />

The minimum size for the CAPTCHA image is 200x75, to allow for both the describing
image, and the question text. Larger image sizes are allowed, and both the font
size for the question text and the describing image should scale accordingly.

The CAPTCHA script will randomly pick from an array of valid keywords, then generate
the image based on that word, and store the SHA1 value for that word. CAPTCHA
validation in your script then needs to compare the session variable
$_SESSION['capKey'] with the SHA1 of the submitted response from the user. A
match should allow the user access, while failure should be handled accordingly.


  */
  $minX = 200;
  $minY = 75;
  $X = (isset($_GET['w'])) ? (int)$_GET['w'] : $minX; // Determine width of image
  $Y = (isset($_GET['h'])) ? (int)$_GET['h'] : $minY; // Determine height of image
  $X = ($X < $minX) ? $minX : $X;
  $Y = ($Y < $minY) ? $minY : $Y;
  $b = (isset($_GET['b'])) ? true : false; // determine whether a black background is desired
  $g = (isset($_GET['g'])) ? true : false; // determine whether a gradient background is desired
  $defaultAspectRatio = 200 / 75;
  $aspectRatio = $X / $Y;
  $fsl = 20 * floor((200 / $X) * ($aspectRatio / $defaultAspectRatio));
  $fsh = 26 * floor((200 / $X) * ($aspectRatio / $defaultAspectRatio));
  $vCenter = floor($Y / 2);
  $capString = ""; // This is the 'question string' to present to the user, and is built later
  $capKeys = array ('red','green','blue','man','woman','cat','dog','fish','one','two','three','four','circle','square','triangle');
  $capKeyCount = count($capKeys);
  $capNum = rand(0,$capKeyCount - 1);
  $capKey = $capKeys[$capNum];
  $objectWord = '';
  $fn = '';
  $number = '';
  switch ($capKey) {
    case 'red':
    case 'green':
    case 'blue':
      $fn = '[objectWord] [capKey]';
      $capString = "What color is this [objectWord]?";
      $objectNum = rand(1,5);
      switch ($objectNum) {
        case 1:
          $objectWord = 'man';
          $capString = "What color is this man's shirt?";
          break;
        case 2:
          $objectWord = 'woman';
          $capString = "What color is this woman's blouse?";
          break;
        case 3:
          $objectWord = 'cat';
          break;
        case 4:
          $objectWord = 'dog';
          break;
        case 5:
          $objectWord = 'fish';
          break;
      }
      break;
    case 'one':
    case 'two':
    case 'three':
    case 'four':
      $fn = '[capKey]';
      $capString = "How many lines do you see?";
      break;
    case 'circle':
    case 'square':
    case 'triangle':
      $fn = '[capKey]';
      $capString = "What shape do you see?";
      break;
    case 'man':
    case 'woman':
      $fn = '[capKey] [objectWord]';
      $capString = "Is this a man or a woman?";
      $objectNum = rand(1,3);
      switch ($objectNum) {
        case 1:
          $objectWord = 'red';
          break;
        case 2:
          $objectWord = 'green';
          break;
        case 3:
          $objectWord = 'blue';
          break;
      }
      break;
    case 'cat':
    case 'dog':
    case 'fish':
      $fn = '[capKey] [objectWord]';
      $capString = "What is this object?";
      $objectNum = rand(1,3);
      switch ($objectNum) {
        case 1:
          $objectWord = 'red';
          break;
        case 2:
          $objectWord = 'green';
          break;
        case 3:
          $objectWord = 'blue';
          break;
      }
      break;
  }
  $capString = str_replace('[objectWord]',$objectWord, $capString);

  // Set up the image
  $image = imagecreatetruecolor($X, $Y);
  $white = imagecolorallocate ($image, 255, 255, 255);
  $black = imagecolorallocate ($image, 0, 0, 0);

  // If black is the chosen background, lighten the current "black" to a medium grey
  $black = ($b) ? imagecolorallocate ($image, 128, 128, 128) : $black;
  $bkg = ($b) ? $black : $white;
  $fgc = ($b) ? $white : $black;

  // Fill the image with the desired background (the gradient will be handled later)
  imagefill ($image, 0, 0, $bkg);


  // Paints the gradient background, if set
  if ($g) {
    $lrl = 128;
    $lrh = 200;
    $drl = 64;
    $drh = 160;
    for ($c = 0; $c <= $minX; $c++) {
      $l = 255 - (($c / $minX) * 255);
      $color = imagecolorallocate($image, $l, $l, $l);
      imageline($image, $c, 0, $c, $minY, $color);
    }
  }

  // get the description image
  $fn = str_replace('[capKey]', $capKey, $fn);
  $fn = str_replace('[objectWord]', $objectWord, $fn);
  $overlay = imagecreatefrompng(CAPTCHA_PATH . "$fn.png");
  $trans = imagecolorallocatealpha($overlay,0,0,0,127);
  imagecopy($image,$overlay,$X-64,$Y-64,0,0,64,64);



  // Base value for the distance between each character
  imagestring($image,2,3,3,$capString,$fgc);
  $_SESSION["capKey"] = sha1(strtolower($capKey));
  header ("Content-type: image/jpeg");
  imagejpeg($image,null,100);
  imagedestroy($image);
  // Debug
  //file_put_contents(_LOG_PATH_ . 'capKey.txt', "$capKey\r\n",FILE_APPEND);
?>