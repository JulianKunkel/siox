<?php

class GNUPlot {
	protected $image_dir;
	protected $image_file;
	protected $plot_commands;

	public function __construct($tmp_dir) 
	{
		$this->image_dir = $tmp_dir;
		$this->image_file = tempnam($this->image_dir, "gnuplot-");
		$this->plot_commands = array();
		$this->plot_commands[] = "set term png";
		$this->plot_commands[] = "set output \"$this->image_file\"";
	}

	public function __destruct()
	{
		unlink($this->image_file);
	}

	public function set($cmd) 
	{
		$this->plot_commands[] = $cmd;
	}

	public function render()
	{
		if (!($pgp = popen("/usr/bin/gnuplot", "w"))) {
			die("Error opening gnuplot.");
		}

		fputs($pgp, implode("\n", $this->plot_commands));
		pclose($pgp);
		if (filesize($this->image_file) > 0) {
			header("Content-Type: image/png");
			passthru("cat $this->image_file");
		} else {
			header("Content-Type: image/jpg");
			passthru("cat $this->image_dir/unavail.jpg");
		}

	}
}

?>
