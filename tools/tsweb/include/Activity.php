<?php

class Activity {

static function get_list($full = false)
{
	global $dbcon;

	$sql_join = "LEFT JOIN activity.activity_ids AS b ON a.unique_id = b.unique_id";

	if ($full)
		$sql_join .= " LEFT JOIN sysinfo.activities AS c ON a.ucaid = c.ucaid";

	$sql = "SELECT a.*, b.*, c.activity_name FROM activity.activities AS a $sql_join ORDER BY a.time_start ASC";
	$stmt = $dbcon->prepare($sql);
	if (!$stmt->execute())
		die("Error querying activity list.");

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$list[] = $row;
	}

	return $list;
}


static function get_activity($unique_id, $full = false)
{
	global $dbcon;

	$sql_join = "LEFT JOIN activity.activity_ids AS b ON a.unique_id = b.unique_id";

	if ($full)
		$sql_join .= " LEFT JOIN sysinfo.activities AS c ON a.ucaid = c.ucaid";

	$sql = "SELECT * FROM activity.activities AS a $sql_join WHERE a.unique_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity.");

	$act = $stmt->fetch(PDO::FETCH_OBJ);

	$act->parents = self::get_parent_ids($unique_id);
	$act->children = self::get_child_ids($unique_id);
	$act->remote_calls = self::get_remote_call_ids($unique_id);

	return $act;
}


static function get_remote_call_ids($unique_id)
{
	global $dbcon;

	$sql = "SELECT unique_id FROM activity.remote_call_ids WHERE activity_uid = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity.");

	$remote_calls = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) 
		$remote_calls[] = $row->unique_id;

	return $remote_calls;
}


static function get_activity_id($unique_id, $recursive = 0)
{
	global $dbcon;

	$sql = "SELECT * FROM activity.activity_ids WHERE unique_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity.");

	$act = $stmt->fetch(PDO::FETCH_OBJ);

	$act->parents = array();
	$act->children = array();

	if ($recursive == 1 || $recursive == 3) 
		foreach (self::get_parent_ids($unique_id) as $p) 
			$act->parents[] = &self::get_activity_id($p, 1);
		
	if ($recursive == 2 || $recursive == 3) 
		foreach (self::get_child_ids($unique_id) as $c) 
			$act->children[] = self::get_activity_id($c, 2);

	return $act;
}


static function get_activity_name($unique_id)
{
	global $dbcon;

	$sql = "SELECT activity_name FROM sysinfo.activities AS a, activity.activities AS b WHERE a.ucaid = b.ucaid AND b.unique_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity name.");

	$name = $stmt->fetch(PDO::FETCH_OBJ)->activity_name;

	return $name;


}

static function get_parent_ids($unique_id)
{
	global $dbcon;

	$sql = "SELECT parent_id FROM activity.parents WHERE child_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	$parents = array();

	if (!$stmt->execute())
		die("Error querying parent list.");

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$parents[] = $row->parent_id;

	return $parents;

}


static function get_child_ids($unique_id)
{
	global $dbcon;

	$sql = "SELECT * FROM activity.parents WHERE parent_id = :unique_id";
	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	$children = array();

	if (!$stmt->execute())
		die("Error querying children list.");

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$children[] = $row->child_id;

	return $children;

}


static function add_child_edges(&$gv, $children, $parent)
{
	foreach ($children as $child) {
		$cname = self::get_activity_name($child->unique_id);
		$gv->addNode($child->unique_id, array('style' => 'filled','URL' => "activity.php?unique_id=$child->unique_id", 'label' => "#$child->unique_id: $cname()"));
		$gv->addEdge(array($parent->unique_id => $child->unique_id));
		if (!empty($child->children))
			self::add_child_edges($gv, $child->children, $child);
	}
}


static function add_parent_edges(&$gv, $parents, $child)
{
	foreach ($parents as $parent) {
		$pname = self::get_activity_name($parent->unique_id);
		$gv->addNode($parent->unique_id, array('style' => 'filled','URL' => "activity.php?unique_id=$parent->unique_id", 'label' => "#$parent->unique_id: $pname()"));
		$gv->addEdge(array($parent->unique_id => $child->unique_id));
		if (!empty($parent->parents))
			self::add_parent_edges($gv, $parent->parents, $parent);
	}
}


static function print_dot($unique_id)
{
	require_once "Image/GraphViz.php";

	$act = self::get_activity($unique_id, true);
	$aid = self::get_activity_id($unique_id, 3);
	
	$gv = new Image_GraphViz();

	$gv->addNode($unique_id, array('style' => 'filled', 'fillcolor' => 'greenyellow', 'label' => "#$aid->unique_id: $act->activity_name()"));

	self::add_parent_edges($gv, $aid->parents, $aid);
	self::add_child_edges($gv, $aid->children, $aid);
	
	$gv->image();
}


}

?>