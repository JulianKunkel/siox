<?php

require_once("SIOX.php");

class Stats {

static function get_list()
{
	global $dbcon;

	$sql = "SELECT * FROM topology.get_statistics()";

	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting statistics.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$list[] = $row;

	return $list;
}

static function store_data($x, $y, $tmp_dir = '/tmp')
{
	global $dbcon;


	$sql = "SELECT timestamp, value FROM statistics.stats_full WHERE childobjectid = :id AND value IS NOT NULL";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':id', $y);
//	$stmt->bindParam(':start', $stop);
//	$stmt->bindParam(':stop', $start);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting statistic. Id=$id");
	}

	$data = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$data[$row->$x] = $row->value;
	}

	$data_string = self::tabulate_kv_array($data);
	$data_file = tempnam($tmp_dir, "data-");
	file_put_contents($data_file, $data_string);

	return $data_file;
}

static function destroy_data($data_file)
{
	unlink($data_file);
}

static function tabulate_kv_array(&$kv_array)
{
	array_walk($kv_array, create_function('&$value, $key', '$value = "$key\t$value";'));
	return implode("\n", $kv_array);
}

}
?>
