

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

if (!is_null($_POST['data'])) {
	$handle = fopen('data.csv', 'a') or die("Unable to open file to write!");

	$input_data = $_POST['data'];
	fwrite($handle, $input_data);

	fclose($handle);
} else {
	echo("no data!");
}

$handle = fopen("data.csv", "r") or die("Unable to open file to read data!");
if ($handle !== FALSE){
	echo "<table><tr><th>Sample</th><th>IR1</th><th>RED1</th></tr>";
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

?>

</body>
</html>

