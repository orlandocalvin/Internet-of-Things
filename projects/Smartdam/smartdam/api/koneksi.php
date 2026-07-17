<?php
$servername = "localhost";
$username = "id20397065_orlando";
$password = "Data_mahasiswa123";
$dbname = "id20397065_data_mhs";

$koneksi = mysqli_connect($servername, $username, $password, $dbname);
if (!$koneksi){
    die("Koneksi Gagal :".mysqli_connect_error());
}