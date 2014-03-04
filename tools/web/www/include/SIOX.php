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

static function get_program_list()
{
	global $dbcon;

	$sql = "SELECT * FROM get_program_list()";
	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute)) {
		print_r($dbcon->errorInfo());
		die("Error getting program list.");
	}

	$progs = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$progs[] = $row;

	return $progs;
}


}

}

?>
