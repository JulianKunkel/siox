<?php

function hyperlink_ids($action_url, $item_array)
{
	$retval = array();

	foreach ($item_array as $item) 
		$retval[] = "<a href=\"$action_url$item->unique_id\">$item->name()</a>";

	return $retval;
}

?>
