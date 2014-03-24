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

static function store_data($x, $y, $tmp_dir = '/tmp', $start, $stop)
{
	global $dbcon;

	if ($start > 0 && $stop > 0)
		$where = "AND timestamp BETWEEN $start AND $stop";
	else
		$where = "";

	$sql = "SELECT timestamp, value FROM statistics.stats_full WHERE childobjectid = :id AND value IS NOT NULL $where ORDER BY timestamp ASC";
	
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':id', $y);

	if ($start > 0 && $stop > 0) {
//		$stmt->bindParam(':start', $stop, PDO::PARAM_INT);
//		$stmt->bindParam(':stop',  $start, PDO::PARAM_INT);
	}

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting statistic. Id=$id");
	}

	$data = array();
	$first_tick = -1;

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		if ($first_tick == -1)
			$first_tick = $row->$x;
		$data[$row->$x - $first_tick] = $row->value;
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
