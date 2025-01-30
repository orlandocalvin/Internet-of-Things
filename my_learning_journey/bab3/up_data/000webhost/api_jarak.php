<?php
include 'koneksi.php';

date_default_timezone_set('Asia/Jakarta');
$date = date("d-m-Y");
$time = date("H:i:s");
$jarak = $_POST['jarak'];

$sql = "INSERT INTO jarak (tgl, jam, jarak) VALUES ('$date', '$time', '$jarak')";

if ($koneksi->query($sql) === FALSE) {
    echo "Error: " . $sql . "<br>" . $koneksi->error;
}