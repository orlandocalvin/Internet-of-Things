<?php
require __DIR__ . '/config.php'; // DB credentials (gitignored; see config.example.php)

$koneksi = mysqli_connect($servername, $username, $password, $dbname);
if (!$koneksi){
    die("Koneksi Gagal :".mysqli_connect_error());
}