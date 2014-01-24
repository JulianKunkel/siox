<?php

require_once("config.php");

$db_string = "pgsql:port=$db_port;dbname=$db_name;user=$db_user;password=$db_pass";

$dbcon = new PDO($db_string);

?>
