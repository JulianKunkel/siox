<?php

class SIOX {

static function purge()
{
	global $dbcon;

	$sql = "SELECT reset_all_tables()";
	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error purging SIOX database,");
	}

}

}

?>
