<?php
include 'koneksi.php';
// $query = "SELECT * FROM bendungan ORDER BY id DESC";
$query = "SELECT id AS 'ID', tgl AS 'Tanggal', jam AS 'Jam', ketinggian AS 'Ketinggian', suhu AS 'Suhu', kelembaban AS 'Kelembaban', rain_value AS 'Rain_Value' FROM bendungan ORDER BY id DESC";
$result = mysqli_query($koneksi, $query);

$filename = 'History Monitoring Bendungan.csv';

header('Content-Type: text/csv');
header('Content-Disposition: attachment; filename="' . $filename . '"');

$output = fopen('php://output', 'w');

// Mengambil dan menuliskan nama kolom sebagai header
$first_row = mysqli_fetch_assoc($result);
if ($first_row) {
    fputcsv($output, array_keys($first_row));
    mysqli_data_seek($result, 0); // Kembali ke baris pertama setelah mengambil nama kolom
}

// Menuliskan data dari tabel
foreach ($result as $row) {
    fputcsv($output, $row);
}

fclose($output);