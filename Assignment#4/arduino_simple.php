

<html>
<head>
<style>
table, th, td {
    border: 1px solid black;
	border-collapse: collapse;
}
</style>
</head>
<body>


<?php

if (!is_null($_GET['sample']) and !is_null($_GET['IR']) and !is_null($_GET['RED'])) {

	$handle = fopen('data.csv', 'a') or die("Unable to open file to write!");

	//$input_data = $_GET['sample'] . "," . $_GET['IR'] . "," . $_GET['RED'] . "\n";
	$input_data = $_POST;
	fwrite($handle, $input_data);

	fclose($handle);
} else {
	echo "one or more data is empty!<br />\n";
	echo "sample: " . !is_null($_GET['sample']) . ", IR: " . !is_null($_GET['IR']) . ", RED: " . !is_null($_GET['RED']) . "<br />\n";

	$handle = fopen("data.csv", "r") or die("Unable to open file to read data!");
	if ($handle !== FALSE){
		echo "<table><tr><th>Sample</th><th>IR1</th><th>RED1</th><th>IR2</th><th>RED2</th><th>IR3</th><th>RED3</th><th>IR4</th><th>RED4</th></tr>";
	    while (($data = fgetcsv($handle, 1000, ",")) !== FALSE) {
	        $num = count($data);
			echo "<tr>";
	        for ($c=0; $c < $num; $c++) {
	            //echo $data[$c] . "<br />\n";
	            echo "<td>" . $data[$c] . "</td>";
	        }
			echo "</tr>";
	    }
		echo "</table>";
	    fclose($handle);
	} else {
		echo ("Hello world!");
	}
}

?>

</body>
</html>

