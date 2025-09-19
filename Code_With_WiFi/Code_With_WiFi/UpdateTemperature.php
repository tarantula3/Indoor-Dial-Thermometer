<?PHP
	try {
		$Location = $_GET["Location"];
		$TEMP	  = $_GET["TEMP"];
		$HUM	  = $_GET["HUM"];
		
		include "ConnectionStringArduino.php"; // Create connection
		$sql 	= "INSERT INTO `Weather` (`DateTime`,`Temperature`,`Humidity`,`Location`) VALUES ('".time()."','".$TEMP."','".$HUM."','".$Location."');";
		$result = $con->query( $sql ); if($result === FALSE) { die(mysqli_error());}
		
		mysqli_close($con);
	} catch (Exception $e) {}
?>