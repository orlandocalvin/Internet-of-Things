<?php
    $koneksi = mysqli_connect("localhost", "id20397065_orlando", "Data_mahasiswa123", "id20397065_data_mhs");
    if (!$koneksi) {
        die("Koneksi database gagal: " . mysqli_connect_error());
    }

    $limit = 10;
    $page = isset($_GET['page']) ? max(1, intval($_GET['page'])) : 1;
    $start = ($page - 1) * $limit;

    $query = "SELECT * FROM bendungan ORDER BY id DESC LIMIT $start, $limit";
    $result = mysqli_query($koneksi, $query);

    if (!$result) {
        die("Error dalam menjalankan query: " . mysqli_error($koneksi));
    }
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/png" href="../images/favicon.png">
    <link href='https://unpkg.com/boxicons@2.1.4/css/boxicons.min.css' rel='stylesheet'>
    <link rel="stylesheet" href="../css/style.css">
    <title>History Full Data</title>
    <style>
        .bottom-data {
            margin:15px;
        } .report {
            height: 36px;
            padding: 0 16px;
            border-radius: 36px;
            background: var(--primary);
            color: var(--light);
            display: flex;
            align-items: center;
            justify-content: center;
            grid-gap: 10px;
            font-weight: 500;
        }
    </style>
</head>
<body>
    <div class="bottom-data">
        <div class="orders">
            <div class="header">
                <i class='bx bx-receipt'></i>
                <h3>History</h3>
                <a href="../api/generate_csv.php" class="report">
                    <i class='bx bx-cloud-download'></i>
                    <span>Download CSV</span>
                </a>
            </div>
            <table>
                <thead>
                    <tr>
                        <th>No</th>
                        <th>Tanggal</th>
                        <th>Jam</th>
                        <th>Water Level</th>
                        <th>Suhu</th>
                        <th>Kelembaban</th>
                        <th>Rain Value</th>
                    </tr>
                </thead>
                <tbody>
                    <?php 
                    $no = ($page - 1) * $limit;
                    while ($data = mysqli_fetch_assoc($result)) { ?>
                        <tr>
                            <td><?= ++$no; ?></td>
                            <td><?= $data['tgl']; ?></td>
                            <td><?= $data['jam']; ?></td>
                            <td><?= $data['ketinggian'] . " CM"; ?></td>
                            <td><?= $data['suhu'] . " C"; ?></td>
                            <td><?= $data['kelembaban'] . "%"; ?></td>
                            <td><?= $data['rain_value']; ?></td>
                        </tr>
                    <?php } ?>
                </tbody>
            </table>

            <!-- Pagination links -->
            <ul class='pagination'>
                <?php if ($page > 1): ?>
                    <li><a class='page-link' href='?page=<?= $page - 1 ?>'>Prev</a></li>
                <?php endif; ?>
                <?php if (mysqli_num_rows($result) == $limit): ?>
                    <li><a class='page-link' href='?page=<?= $page + 1 ?>'>Next</a></li>
                <?php endif; ?>
            </ul>
        </div>
    </div>
</body>
</html>
