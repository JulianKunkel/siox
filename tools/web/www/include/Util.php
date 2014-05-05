<?php

function hyperlink_ids($action_url, $item_array)
{
	$retval = array();

	foreach ($item_array as $item) 
		$retval[] = "<a href=\"$action_url$item->unique_id\">$item->name()</a>";

	return $retval;
}

function format_seconds($seconds)
{
	$days     = floor($seconds / (24*60*60));
	$seconds %= 24*60*60;
	$hours    = floor($seconds / (60*60));
	$seconds %= 60*60;
	$minutes  = floor($seconds / 60);
	$seconds %= 60;

	$retval = "";

	if ($days > 0)
		$retval .= $days."d ";

	$retval .= str_pad($hours, 2, '0', STR_PAD_LEFT).":";
	$retval .= str_pad($minutes, 2, '0', STR_PAD_LEFT).":";
	$retval .= str_pad($seconds, 2, '0', STR_PAD_LEFT);

	return $retval;
}

?>
