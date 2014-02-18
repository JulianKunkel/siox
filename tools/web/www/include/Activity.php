<?php

class Activity {

static function get_list($page = 1, $page_size = 200)
{
	global $dbcon;

	$sql_join  = "LEFT JOIN activity.activity_ids AS b ON a.unique_id = b.unique_id ";
	$sql_join .= "LEFT JOIN sysinfo.activities AS c ON a.ucaid = c.ucaid";
	$sql       = "SELECT a.*, b.*, c.activity_name FROM activity.activities AS a $sql_join ORDER BY a.time_start ASC LIMIT :page_size OFFSET :offset";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':page_size', $page_size);
	$stmt->bindValue(':offset', $page_size*($page-1));

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error querying activity list.");
	}

	$list = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$list[] = $row;

	return $list;
}

static function get_count()
{
	global $dbcon;

	$sql_join  = "LEFT JOIN activity.activity_ids AS b ON a.unique_id = b.unique_id ";
	$sql_join .= "LEFT JOIN sysinfo.activities AS c ON a.ucaid = c.ucaid";
	$sql       = "SELECT count(*) AS count FROM activity.activities AS a $sql_join";

	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute())
		die("Error querying activity list.");

	$row = $stmt->fetch(PDO::FETCH_OBJ);

	return $row->count;
}


static function get_activity($unique_id)
{
	global $dbcon;

	$sql_join  = "LEFT JOIN activity.activity_ids AS b ON a.unique_id = b.unique_id ";
	$sql_join .= "LEFT JOIN sysinfo.activities AS c ON a.ucaid = c.ucaid";
	$sql       = "SELECT * FROM activity.activities AS a $sql_join WHERE a.unique_id = :unique_id";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying activity.");

	$act = $stmt->fetch(PDO::FETCH_OBJ);

	$act->parents = self::get_parents($unique_id);
	$act->children = self::get_children($unique_id);
	$act->remote_calls = self::get_remote_calls($unique_id);

	return $act;
}


static function get_remote_calls($unique_id)
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

// this is to be removed
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
			$act->parents[] = self::get_activity_id($p, 1);
		
	if ($recursive == 2 || $recursive == 3) 
		foreach (self::get_child_ids($unique_id) as $c) 
			$act->children[] = self::get_activity_id($c, 2);

	return $act;
}


static function get_parents($unique_id)
{
	global $dbcon;

	$sql = "SELECT p.parent_id AS unique_id, b.activity_name AS name 
			FROM activity.parents AS p 
			LEFT JOIN activity.activities AS a ON p.parent_id = a.unique_id
			LEFT JOIN sysinfo.activities  AS b ON a.ucaid = b.ucaid
			WHERE p.child_id = :unique_id";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying parent list.");

	$parents = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$parents[] = $row;

	return $parents;
}

// this is to be removed
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


static function get_children($unique_id)
{
	global $dbcon;

	$sql = "SELECT p.child_id AS unique_id, b.activity_name AS name
			FROM activity.parents AS p 
			LEFT JOIN activity.activities AS a ON p.child_id = a.unique_id
			LEFT JOIN sysinfo.activities  AS b ON a.ucaid = b.ucaid
			WHERE p.parent_id = :unique_id";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying parent list.");

	$children = array();

	while ($row = $stmt->fetch(PDO::FETCH_OBJ))
		$children[] = $row;

	return $children;

}

// this is to be removed
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


static function add_child_edges(&$gv, $children, $parent, $min_max_d = 0)
{
	foreach ($children as $child) {

		$c = self::get_activity($child->unique_id, true);

		$duration = $c->time_stop - $c->time_start;
		$scale = $duration / ($min_max_d["max"] - $min_max_d["min"]);

		$width = 1 + $scale;
		$height = 0.5 + $scale;

		$gv->addNode($child->unique_id, array('style' => 'filled', 'width' => $width, 'height' => $height, 'URL' => "activity.php?unique_id=$child->unique_id", 'label' => "#$child->unique_id: $c->activity_name()"));
		$gv->addEdge(array($parent->unique_id => $child->unique_id));
		if (!empty($child->children))
			self::add_child_edges($gv, $child->children, $child, $min_max_d);
	}
}


static function add_parent_edges(&$gv, $parents, $child, $min_max_d = 0)
{
	foreach ($parents as $parent) {
		$p = self::get_activity($parent->unique_id, true);

		$duration = $p->time_stop - $p->time_start;
		$scale = $duration / ($min_max_d["max"] - $min_max_d["min"]);

		$width = 1 + $scale;
		$height = 0.5 + $scale;

		$gv->addNode($parent->unique_id, array('style' => 'filled', 'width' => $width, 'height' => $height, 'URL' => "activity.php?unique_id=$parent->unique_id", 'label' => "#$parent->unique_id: $p->activity_name()"));

		$gv->addEdge(array($parent->unique_id => $child->unique_id));
		if (!empty($parent->parents))
			self::add_parent_edges($gv, $parent->parents, $parent, $min_max_d);
	}
}


static function print_dot($unique_id)
{
	require_once "Image/GraphViz.php";

	$act = self::get_activity($unique_id);
	$aid = self::get_activity_id($unique_id, 3);
	
	$gv = new Image_GraphViz();

	$min_max_d = self::get_min_max_duration($unique_id);

	$duration = $act->time_stop - $act->time_start;
	$scale = $duration / ($min_max_d["max"] - $min_max_d["min"]);

	$width = 1 + $scale;
	$height = 0.5 + $scale;

	$gv->addNode($unique_id, array('style' => 'filled', 'fillcolor' => 'greenyellow', 'width' => $width, 'height' => $height, 'label' => "#$aid->unique_id: $act->activity_name()"));

	self::add_parent_edges($gv, $aid->parents, $aid, $min_max_d);
	self::add_child_edges($gv, $aid->children, $aid, $min_max_d);
	
	$gv->image();
}


static function get_min_max_duration($unique_id)
{
	global $dbcon;

	$sql  = "SELECT min(min), max(max) FROM (";
	$sql .= "SELECT min(time_stop - time_start) AS min, max(time_stop - time_start) AS max ";
	$sql .= "	FROM activity.activities ";
	$sql .= "	WHERE unique_id = :unique_id ";
	$sql .= "UNION ";
	$sql .= "SELECT min(time_stop - time_start) AS min, max(time_stop - time_start) AS max ";
	$sql .= "	FROM activity.activities AS a, activity.parents AS b ";
	$sql .= "	WHERE a.unique_id = b.child_id AND b.parent_id = :unique_id ";
	$sql .= "UNION ";
	$sql .= "SELECT min(time_stop - time_start) AS min, max(time_stop - time_start) AS max ";
	$sql .= "	FROM activity.activities AS a, activity.parents AS b ";
	$sql .= "	WHERE a.unique_id = b.parent_id AND b.child_id = :unique_id ";
	$sql .= ") AS MIN_MAX";

	$stmt = $dbcon->prepare($sql);
	$stmt->bindParam(':unique_id', $unique_id);

	if (!$stmt->execute())
		die("Error querying children list.");

	$retval = array();
	while ($row = $stmt->fetch(PDO::FETCH_OBJ)) {
		$retval['min'] = $row->min;
		$retval['max'] = $row->max;
	}

	return $retval;
}


static function purge()
{
	global $dbcon;

	$sql  = "SELECT activity.reset_all()";
	$stmt = $dbcon->prepare($sql);

	if (!$stmt->execute()) {
		print_r($dbcon->errorInfo());
		die("Error purging activities.");
	}
		
}


}

?>
