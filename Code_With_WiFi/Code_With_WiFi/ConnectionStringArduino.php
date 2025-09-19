<?PHP
  // VPS hosting
  $con = new mysqli("127.0.0.1", "user", "password", "Arduino");
  if (mysqli_connect_errno()) die('Could not connect: ' . mysqli_error());
?>