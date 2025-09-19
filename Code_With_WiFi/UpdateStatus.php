<?PHP
	try {
		$Token	  = $_GET["Token"];
		$Location = $_GET["Location"];
		include "ConnectionStringArduino.php"; // Create connection
		$sql 	  = 'Update `Status` SET `DateTime`=\''.time().'\',`State`=\'1\' WHERE `Device`=\''.$Location.'\' AND `Token` = \''.$Token.'\';';
		$result   = $con->query( $sql ); if($result === FALSE) { die(mysqli_error());}
		mysqli_close($con);
	} catch (Exception $e) {}
?>