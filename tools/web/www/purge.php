<?php

require_once("include/DB.php"); 
require_once("include/Activity.php"); 

Activity::purge();

header("Location: index.php");
?>