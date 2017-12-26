
DROP TABLE IF EXISTS `aiml`;
CREATE TABLE IF NOT EXISTS `aiml` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `bot_id` int(11) NOT NULL DEFAULT '1',
  `aiml` text NOT NULL,
  `pattern` varchar(255) NOT NULL,
  `thatpattern` varchar(255) NOT NULL,
  `template` text NOT NULL,
  `topic` varchar(255) NOT NULL,
  `filename` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `topic` (`topic`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `aiml_userdefined`
--

DROP TABLE IF EXISTS `aiml_userdefined`;
CREATE TABLE IF NOT EXISTS `aiml_userdefined` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `aiml` text NOT NULL,
  `pattern` text NOT NULL,
  `thatpattern` text NOT NULL,
  `template` text NOT NULL,
  `user_id` int(11) NOT NULL,
  `bot_id` int(11) NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `botpersonality`
--

DROP TABLE IF EXISTS `botpersonality`;
CREATE TABLE IF NOT EXISTS `botpersonality` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `bot_id` tinyint(4) NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT '',
  `value` text NOT NULL,
  PRIMARY KEY (`id`),
  KEY `botname` (`bot_id`,`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `bots`
--

DROP TABLE IF EXISTS `bots`;
CREATE TABLE IF NOT EXISTS `bots` (
  `bot_id` int(11) NOT NULL AUTO_INCREMENT,
  `bot_name` varchar(255) NOT NULL,
  `bot_desc` varchar(255) NOT NULL,
  `bot_active` int(11) NOT NULL DEFAULT '1',
  `bot_parent_id` int(11) NOT NULL DEFAULT '0',
  `format` varchar(10) NOT NULL DEFAULT 'html',
  `save_state` enum('session','database') NOT NULL DEFAULT 'session',
  `conversation_lines` int(11) NOT NULL DEFAULT '7',
  `remember_up_to` int(11) NOT NULL DEFAULT '10',
  `debugemail` text NOT NULL,
  `debugshow` int(11) NOT NULL DEFAULT '1',
  `debugmode` int(11) NOT NULL DEFAULT '1',
  `error_response` text NOT NULL,
  `default_aiml_pattern` varchar(255) NOT NULL DEFAULT 'RANDOM PICKUP LINE',
  `unknown_user` varchar(255) NOT NULL DEFAULT 'Seeker',
  PRIMARY KEY (`bot_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `client_properties`
--

DROP TABLE IF EXISTS `client_properties`;
CREATE TABLE IF NOT EXISTS `client_properties` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(11) NOT NULL,
  `bot_id` int(11) NOT NULL,
  `name` text NOT NULL,
  `value` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `conversation_log`
--

DROP TABLE IF EXISTS `conversation_log`;
CREATE TABLE IF NOT EXISTS `conversation_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `input` text NOT NULL,
  `response` longtext NOT NULL,
  `user_id` int(11) NOT NULL,
  `convo_id` text NOT NULL,
  `bot_id` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `myprogramo`
--

DROP TABLE IF EXISTS `myprogramo`;
CREATE TABLE IF NOT EXISTS `myprogramo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_name` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `last_ip` varchar(25) NOT NULL,
  `last_login` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `user_name` (`user_name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `spellcheck`
--

DROP TABLE IF EXISTS `spellcheck`;
CREATE TABLE IF NOT EXISTS `spellcheck` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `missspelling` varchar(100) NOT NULL,
  `correction` varchar(100) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

INSERT IGNORE INTO spellcheck (id, missspelling, correction) VALUES
(1, 'shakespear', 'shakespeare'),
(2, 'shakesper', 'shakespeare'),
(3, 'ws', 'william shakespeare'),
(4, 'shakespaer', 'shakespeare'),
(5, 'shakespere', 'shakespeare'),
(6, 'shakepeare', 'shakespeare'),
(7, 'shakeper', 'shakespeare'),
(8, 'willam', 'william'),
(9, 'willaim', 'william'),
(10, 'romoe', 'romeo'),
(11, 'julet', 'juliet'),
(12, 'juleit', 'juliet'),
(13, 'thats', 'that is'),
(89, 'Youa aare', 'you are'),
(88, 'that s', 'that is'),
(87, 'wot s', 'what is'),
(17, 'whats', 'what is'),
(18, 'wot', 'what'),
(19, 'wots', 'what is'),
(86, 'what s', 'what is'),
(21, 'lool', 'lol'),
(27, 'pogram', 'program'),
(23, 'progam', 'program'),
(26, 'progam', 'program'),
(28, 'r', 'are'),
(29, 'u', 'you'),
(30, 'ur', 'your'),
(31, 'v', 'very'),
(32, 'k', 'ok'),
(33, 'np', 'no problem'),
(34, 'ta', 'thank you'),
(35, 'ty', 'thank you'),
(36, 'omg', 'oh my god'),
(37, 'letts', 'lets'),
(38, 'yeah', 'yes'),
(39, 'yeh', 'yes'),
(40, 'portugues', 'portuguese'),
(41, 'hehe', 'lol'),
(42, 'ha', 'lol'),
(43, 'intersting', 'interesting'),
(44, 'qestion', 'question'),
(45, 'elrond hubbard', 'l.ron hubbard'),
(46, 'programm', 'program'),
(47, 'c''mon', 'come on'),
(48, 'ye', 'yes'),
(49, 'im', 'i am'),
(50, 'fuckahh', 'fucker'),
(51, 'shakespeare bot', 'shakespearebot'),
(52, 'goodf', 'good'),
(53, 'dont', 'do not'),
(54, 'cos', 'because'),
(55, 'cus', 'because'),
(56, 'coz', 'because'),
(57, 'cuz', 'because'),
(58, 'isnt', 'is not'),
(59, 'isn''t', 'is not'),
(60, 'i''m', 'i am'),
(61, 'ima', 'i am a'),
(62, 'chheese', 'cheese'),
(63, 'watsup', 'what is up'),
(64, 'let s', 'let us'),
(65, 'he s', 'he is'),
(66, 'she''s', 'she is'),
(67, 'i ll', 'i will'),
(68, 'they ll', 'they will'),
(69, 'you re', 'you are'),
(70, 'you ve', 'you have'),
(71, 'hy', 'hey'),
(72, 'msician', 'musician'),
(74, 'don t', 'do not'),
(75, 'can t', 'cannot'),
(76, 'favourite', 'favorite'),
(77, 'colour', 'color'),
(78, 'won t', 'will not'),
(79, 'a/s/l', 'asl'),
(80, 'haven t', 'have not'),
(81, 'doesn t', 'does not'),
(82, 'a/s/l/', 'asl'),
(83, 'wht', 'what'),
(84, 'It s been', 'It has been'),
(85, 'its been', 'it has been'),
(90, 'you re', 'you are'),
(91, 'theres', 'there is'),
(92, 'youa re', 'you are'),
(93, 'youa aare', 'you are'),
(94, 'wath', 'what'),
(95, 'waths', 'what is'),
(96, 'hy', 'hey'),
(97, 'oke', 'ok'),
(98, 'okay', 'ok'),
(99, 'errm', 'erm'),
(100, 'aare', 'are'),
(101, 'shakespeare bot', 'william shakespeare'),
(102, 'shakespearebot', 'william shakespeare'),
(103, 'werwerwer', 'war'),
(109, 'program o', 'programo'),
(106, 'ddddddddd', 'ddddddddd'),
(107, 'ddddddddd', 'ddddddddd'),
(108, 'fgfgfgfg', 'fgfgfgfg'),
(110, 'program-o', 'programo'),
(111, 'fav', 'favorite');

-- --------------------------------------------------------

--
-- Table structure for table `undefined_defaults`
--

DROP TABLE IF EXISTS `undefined_defaults`;
CREATE TABLE IF NOT EXISTS `undefined_defaults` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `bot_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL DEFAULT '0',
  `pattern` text NOT NULL,
  `template` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

INSERT IGNORE INTO undefined_defaults (id, bot_id, pattern, template) VALUES
(1, 1, 'your name', 'my friend'),
(2, 1, 'your it', 'it'),
(3, 1, 'your location', 'your town'),
(4, 1, 'your does', 'it'),
(5, 1, 'your genus', 'human'),
(6, 1, 'your he', 'him'),
(7, 1, 'your she', 'her'),
(8, 1, 'your them', 'those'),
(9, 1, 'your memory', 'that'),
(10, 1, 'your they', 'those'),
(11, 1, 'your gender', 'woman'),
(12, 1, 'your has', 'that'),
(13, 1, 'your we', 'you and me'),
(14, 1, 'your x', 'x'),
(15, 1, 'your personality', 'chatty'),
(16, 1, 'etype', 'great and witty'),
(17, 1, 'your top', 'om'),
(18, 1, 'your second', 'om'),
(19, 1, 'your third', 'om'),
(20, 1, 'your fourth', 'om'),
(21, 1, 'your fifth', 'om'),
(22, 1, 'your sixth', 'om'),
(23, 1, 'your seventh', 'om'),
(24, 1, 'your last', 'om'),
(25, 1, 'your want', 'it'),
(26, 1, 'your is', 'it'),
(27, 1, 'you dealerhand', 'The dealers hand'),
(28, 1, 'your playerhand', 'Your hand'),
(29, 1, 'your dealerace', 'dealer total'),
(30, 1, 'your playerace', 'your total');

-- --------------------------------------------------------

--
-- Table structure for table `unknown_inputs`
--

DROP TABLE IF EXISTS `unknown_inputs`;
CREATE TABLE IF NOT EXISTS `unknown_inputs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `bot_id` int(11) NOT NULL,
  `input` text NOT NULL,
  `user_id` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_name` text NOT NULL,
  `session_id` varchar(255) NOT NULL,
  `bot_id` int(11) NOT NULL,
  `chatlines` int(11) NOT NULL,
  `ip` varchar(100) NOT NULL,
  `referer` text NOT NULL,
  `browser` text NOT NULL,
  `date_logged_on` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_update` timestamp NULL DEFAULT NULL,
  `state` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;
-- --------------------------------------------------------

--
-- Table structure for table `wordcensor`
--

DROP TABLE IF EXISTS `wordcensor`;
CREATE TABLE IF NOT EXISTS `wordcensor` (
  `censor_id` int(11) NOT NULL AUTO_INCREMENT,
  `word_to_censor` varchar(50) NOT NULL,
  `replace_with` varchar(50) NOT NULL DEFAULT '****',
  `bot_exclude` varchar(255) NOT NULL,
  PRIMARY KEY (`censor_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

INSERT IGNORE INTO wordcensor (censor_id, word_to_censor, replace_with, bot_exclude) VALUES
(1, 'shit', 's***', ''),
(2, 'fuck', 'f***', '');

DROP TABLE IF EXISTS `srai_lookup`;
CREATE TABLE IF NOT EXISTS `srai_lookup` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `bot_id` int(11) NOT NULL,
  `pattern` text NOT NULL,
  `template_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `pattern` (`pattern`(64))
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Contains previously stored SRAI calls' AUTO_INCREMENT=1 ;
