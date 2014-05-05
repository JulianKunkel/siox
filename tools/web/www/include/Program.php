<?php

require_once("SIOX.php");

class Program {

static function get_count($cmd = NULL, $node = NULL, $user = NULL)
{
	global $dbcon;

	if ($cmd && $user) {
		$sql = "
SELECT count(*) 
FROM (
	SELECT childobjectid, attribute, value 
	FROM program_with_attrs 
	WHERE childobjectid IN (
		SELECT childobjectid 
		FROM program_with_attrs 
		WHERE value = :user
	)
) AS p 
WHERE attribute = 'description/commandLine' AND value LIKE :cmd";
	} else if (!$cmd && $user) {
		$sql = "
SELECT count(*)
FROM (
	SELECT * 
	FROM program_with_attrs 
	WHERE childobjectid IN (
		SELECT childobjectid 
		FROM program_with_attrs 
		WHERE value = :user
	)
) AS p 
WHERE attribute = 'description/commandLine'";

	} else if ($cmd && !$user) {
		$sql = "
SELECT count(*) 
FROM program_with_attrs
WHERE attribute = 'description/commandLine' AND value LIKE :cmd";
	} else {
		$sql = "SELECT count(*) FROM programs";
	}

	$stmt = $dbcon->prepare($sql);
	if ($cmd)
		$stmt->bindValue(':cmd', "%$cmd%");
	if ($user)
		$stmt->bindParam(':user', $user);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting program count. Command=$cmd, Node=$node, User=$user.");
	}

	$row = $stmt->fetch(PDO::FETCH_OBJ);

	return $row->count;

}

static function get_attribute_list($key)
{
	global $dbcon;


}


static function get_attributes($procid)
{
	global $dbcon;

	$sql = "SELECT * FROM topology.get_attributes_by_proc_number(:pid)";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':pid', $procid);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting program attributes.");
	}

	$attrs = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$attrs[$row->key] = $row->val;
	}

	return $attrs;
}


static function get_list($page = 1, $page_size = 200, $cmd = NULL, $node = NULL, $user = NULL)
{
	global $dbcon;
	$page_offset = $page_size*($page-1);

	if ($cmd && $user) {
		$sql = "
SELECT * 
FROM (
	SELECT childobjectid, childname, attribute, value 
	FROM program_with_attrs 
	WHERE childobjectid IN (
		SELECT childobjectid 
		FROM program_with_attrs 
		WHERE value = :user
	)
) AS p 
WHERE attribute = 'description/commandLine' AND value LIKE :cmd";
	} else if (!$cmd && $user) {
		$sql = "
SELECT *
FROM (
	SELECT childobjectid, childname, attribute, value 
	FROM program_with_attrs 
	WHERE childobjectid IN (
		SELECT childobjectid 
		FROM program_with_attrs 
		WHERE value = :user
	)
) AS p 
WHERE attribute = 'description/commandLine'";

	} else if ($cmd && !$user) {
		$sql = "
SELECT childobjectid, childname, attribute, value 
FROM program_with_attrs
WHERE attribute = 'description/commandLine' AND value LIKE :cmd";
	} else {
		$sql = "SELECT * FROM programs";
	}

	$sql .= " ORDER BY childobjectid ASC LIMIT :page_size OFFSET :page_offset";	

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':page_size', $page_size);
	$stmt->bindValue(':page_offset', $page_size*($page-1));
	if ($cmd)
		$stmt->bindValue(':cmd', "%$cmd%");
	if ($user)
		$stmt->bindParam(':user', $user, PDO::PARAM_STR);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting program list.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		self::load_program($row);
		$list[] = $row;
	}

	return $list;

}


static function get($pid)
{
	global $dbcon;
	$sql = "SELECT * FROM get_program(:pid)";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':pid', $pid);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting program. PID=$pid.");
	}

	$row = $stmt->fetch(PDO::FETCH_OBJ);

	self::load_program($row);

	return $row;
}


static function load_program(&$row)
{
	$row->attributes = self::get_attributes($row->childobjectid);
	$trimmed_id      = trim($row->childname, "()");
	$exploded_id     = explode(',', $trimmed_id);
	$row->nid        = $exploded_id[0];
	$row->pid        = $exploded_id[1];
	$row->time       = $exploded_id[2];
	$row->node       = SIOX::get_node_name($row->nid);
	$row->times      = self::start_stop_times($row->nid, $row->pid, $row->time);
}

static function start_stop_times($nid, $pid, $time)
{
	global $dbcon;

	$result = array();

	$sql = "SELECT MIN(time_start) AS time_start, MAX(time_stop) AS time_stop 
		FROM activity.activity 
		WHERE time_start IS NOT NULL AND time_stop IS NOT NULL AND cid_pid_nid = :nid AND cid_pid_pid = :pid AND cid_pid_time = :time";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':nid', $nid);
	$stmt->bindParam(':pid', $pid);
	$stmt->bindParam(':time', $time);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error getting start time. NID=$nid, PID=$pid, TIME=$time.");
	}

	$row = $stmt->fetch(PDO::FETCH_OBJ);

	$result['start'] = $row->time_start;	
	$result['stop'] = $row->time_stop;	
	
	return $result;	

}

}

?>
