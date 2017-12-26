<?php

  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: main.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: FEB 01 2016
    * DETAILS: Displays the "Home"  section of the admin page
    ***************************************/

    $noRightNav    = $template->getSection('NoRightNav');
    $logo          = $template->getSection('Logo');
    $topNav        = $template->getSection('TopNav');
    $leftNav       = $template->getSection('LeftNav');
    $main          = $template->getSection('Main');
    $rightNav      = '';
    $footer        = trim($template->getSection('Footer'));
    #$lowerScripts  = '';
    #$pageTitleInfo = '';
    $divDecoration = $template->getSection('DivDecoration');
    
    $navHeader     = $template->getSection('NavHeader');
    
    $mainTitle     = 'Home';
    $rightNavLinks = '';
    $FooterInfo    = getFooter();
    $titleSpan     = $template->getSection('TitleSpan');
    $errMsgStyle   = (!empty($msg)) ? "ShowError" : "HideError";
    $errMsgStyle   = $template->getSection($errMsgStyle);
    $mediaType     = ' media="screen"';
    $upperScripts  = '';
    $noLeftNav     = '';
    $noTopNav      = '';
    $pageTitle     = 'My-Program O - Main Page';
    $headerTitle   = 'Actions:';
    $mainContent   = <<<endMain
        <p>
          Welcome to 'My Program-O', the Program-O chatbot admin area. Please
          use the links above or to the left to perform administrative tasks,
          as needed.
        </p>

endMain;

?>