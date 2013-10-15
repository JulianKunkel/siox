<?php

function hyperlink_ids($action_url, $id_array)
{
	$retval = array();

	foreach ($id_array as $id) 
		$retval[] = "<a href=\"$action_url$id\">$id</a>";

	return $retval;
}

?>