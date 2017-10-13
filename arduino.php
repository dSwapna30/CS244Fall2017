

<html>
<head>
<body>


<?php

if (!empty($_GET['time']) and !empty($_GET['data'])) {
	$file = fopen("data.txt", "a+") or die("Unable to open file!");
	$input_data = "time:" . $_GET['time'] . "data:" . $_GET['data'] . "\n";
	fwrite($file, $input_data);
	fclose($file);
}

$file = fopen("data.txt", "r") or die("Unable to open file!");
if (filesize("data.txt") != 0){
	echo nl2br(fread($file, filesize("data.txt")));
} else {
	echo ("Hello world!");
}

//echo "Study " . $x . " at " . $y;
?>


</body>
</head>
</html>

