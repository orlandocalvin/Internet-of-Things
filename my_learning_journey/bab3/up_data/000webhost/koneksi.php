<?php
$servername = "REDACTED";
$username = "REDACTED";
$password = "REDACTED";
$dbname = "REDACTED";

$koneksi = mysqli_connect($servername, $username, $password, $dbname);
if (!$koneksi){
    die("Koneksi Gagal :".mysqli_connect_error());
}
?>