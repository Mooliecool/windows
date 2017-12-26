<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: gui/plain/index.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: MAY 17TH 2014
  * DETAILS: repling to @mentions on twitter
  ***************************************/
  ?>
  <?php
  require_once("twitteroauth/OAuth.php");
  require_once("twitteroauth/twitteroauth.php");

  /*--------------------------------------
  Steps
  1.Create a twitter account for your chatbot
  2.Goto https://dev.twitter.com/ and sign in with your twitters chatbot account
  3.Once signed in goto https://dev.twitter.com/apps and click "Create new application"
  4.Fill in your details making sure
    - Application Type = Read and Write access
    - Create your access tokens

  5.Make a note of ...

   - Consumer key: *
   - Consumer secret: *
   - Access token: *
   - Access token secret: *

   6.Locate the path of conversation_start.php in the chatbot files.

   7.Once you have edited the variables below you will need to set up a cron task
   Every 5 mins this script will run, look for new @mentions on twitter and reply to them.

   !Important
   If you change the cron occurrence entry in the cron table you will have alter the cron_every_x_minutes value belo, so that the minutes are the same.

   The cron job instruction will look something like this php -q /home/yoursite/public_html/yourbot/gui/twitter/tweet.php
   For more information on setting up cron jobs please drop by to forum.program-o.com
   */

  //--------------------------------------------------------------------------------------------------------------------
  // EDIT PARAMS
  //--------------------------------------------------------------------------------------------------------------------

  $consumerkey="";
  $consumersecret="";
  $accesstoken="";
  $accesstokensecret="";
  $path_to_bot = "http://www.yoursite.com/chatbot/conversation_start.php";
  $cron_every_x_minutes = 5;

  //--------------------------------------------------------------------------------------------------------------------
  // CONNECT
  //--------------------------------------------------------------------------------------------------------------------

  $connection = new TwitterOAuth($consumerkey, $consumersecret, $accesstoken, $accesstokensecret);
  $content = $connection->get('account/verify_credentials');
  $connection->host = "https://api.twitter.com/1.1/";
  $twitterid = $content->id;

  //--------------------------------------------------------------------------------------------------------------------
  // RUN
  //--------------------------------------------------------------------------------------------------------------------

  //search for all mentions that occured since the last time we rand the script
  $myMentionsArr = get_my_mentions($connection);
  //formulate responses
  $myReplies = makeReplies($myMentionsArr);
  //tweet responses
  tweetthis($connection,$myReplies);


  //--------------------------------------------------------------------------------------------------------------------
  // FUNCTIONS
  //--------------------------------------------------------------------------------------------------------------------

  /* once all the data has been collected send out a tweet */
  function tweetthis($connection,$myReplies) {
    if(!isset($myReplies[0])){
      echo "<br/>No new tweets";
    } else {
      foreach($myReplies as $i => $replies){
        $tweetStatus = $connection->post('statuses/update', array('status' => $replies['message'], 'in_reply_to_status_id'=>$replies['in_reply_to_status_id'] ));
        if(isset($tweetStatus->errors[0]->code)){
          echo "<br/>Could not tweet ".$replies['message'].": ".$tweetStatus->errors[0]->message;
        }
      }
    }
  }

   /* based upon user input get a bot response */
   function getReply($convo_id,$usersay){
     global $path_to_bot;
     $botsay = '';
     $request_url = $path_to_bot."?say=".urlencode($usersay)."&convo_id=".$convo_id."&bot_id=6&format=xml";
     $conversation = @simplexml_load_file($request_url,"SimpleXmlElement",LIBXML_NOERROR+LIBXML_ERR_FATAL+LIBXML_ERR_NONE);

     if((@$conversation)&&(count(@$conversation)>0)){
       $botsay = (string)$conversation->chat->line[0]->response;
       $botsay = str_replace("undefined","...",$botsay);

    }

   return $botsay;
   }

   /* get all the mentions tweeted to me since the last time the cron ran */
     function get_my_mentions($connection){
        global $cron_every_x_minutes;
       $tArr = array();
       $tweets_to_me = $connection->get('statuses/mentions_timeline',array('count'=>1));
       foreach($tweets_to_me as $i => $tweet){
        if(strtotime($tweet->created_at) > strtotime("-$cron_every_x_minutes minutes")) {
          $tArr[$i]['created']=$tweet->created_at;
          $tArr[$i]['tweetidstr']=$tweet->id_str;
          $tArr[$i]['tweet']=cleanTweet($tweet->text);
          $tArr[$i]['userid']=$tweet->user->id;
          $tArr[$i]['useridstr']=$tweet->user->id_str;
          $tArr[$i]['userscreenname']=$tweet->user->screen_name;
         }
       }
       return $tArr;
     }

    /* clean the tweet text before looking for a match */
    function cleanTweet($tweet){
      $pattern = '/@([a-zA-Z0-9_]+)/';
      str_replace("#","",$tweet);
      $tweet = preg_replace($pattern,"",$tweet);
      return trim($tweet);
    }

    /* package together the details to send the actual response */
    function makeReplies($myMentionsArr){
      $rArr = array();
      foreach($myMentionsArr as $i=>$mention){
          $rArr[$i]['in_reply_to_status_id']=$mention['tweetidstr'];
          //build the reply to tweet
           $botresponse = getReply($mention['useridstr'],$mention['tweet']);

          if($botresponse!=''){
            $message = safeToTweet("@".$mention['userscreenname']." ".$botresponse);
            $rArr[$i]['message']=$message;
          }

      }
    return $rArr;
    }

    /* check its with the 140 char limit */
    function safeToTweet($text){
      if(subStr($text,0,140)!=$text){
        $text = subStr($text,0,137)."...";
      }
      return trim($text);
    }