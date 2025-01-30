<?php
include 'koneksi.php';

date_default_timezone_set('Asia/Jakarta');
$tgl = date("d-m-Y");
$jam = date("H:i:s");
$ketinggian = $_POST['ketinggian'];
$suhu = $_POST['suhu'];
$kelembaban = $_POST['kelembaban'];
$rain_value = $_POST['rain_value'];

$sql = "INSERT INTO bendungan (tgl, jam, ketinggian, suhu, kelembaban, rain_value) VALUES ('$tgl', '$jam', '$ketinggian', '$suhu', '$kelembaban', '$rain_value')";

if ($koneksi->query($sql) === FALSE) {
    echo "Error: " . $sql . "<br>" . $koneksi->error;
}