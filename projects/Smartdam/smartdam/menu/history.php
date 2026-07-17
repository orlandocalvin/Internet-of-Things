<?php
    $no = 0;
    $koneksi = mysqli_connect("localhost", "id20397065_orlando", "Data_mahasiswa123", "id20397065_data_mhs");
    $query = mysqli_query($koneksi, "SELECT * FROM bendungan ORDER BY id DESC LIMIT 6");
    $result = mysqli_fetch_array($query);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href='https://unpkg.com/boxicons@2.1.4/css/boxicons.min.css' rel='stylesheet'>
    <link rel="stylesheet" href="../css/style.css">
    <title>X</title>
    <style>
        .report {
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

    <div class="header">
        <div class="left">
            <h1>Status Bendungan</h1>
        </div>
    </div>

    <!-- Insights -->
    <ul class="insights">
        <li>
            <i class='bx bx-water'></i>
            <span class="info">
                <h3><?= $result['ketinggian'] . " CM"; ?></h3>
                <p>Water Level</p>
            </span>
        </li>
        <li><i class='bx bx-cloud-light-rain'></i>
            <span class="info">
                <h3><?= $result['rain_value']; ?></h3>
                <p>Rain Index</p>
            </span>
        </li>
        <li><i class='bx bxs-hot'></i>
            <span class="info">
                <h3><?= $result['suhu'] . " C"; ?></h3>
                <p>Temperature</p>
            </span>
        </li>
        <li><i class='bx bxs-droplet-half'></i>
            <span class="info">
                <h3><?= $result['kelembaban'] . "%"; ?></h3>
                <p>Humidity</p>
            </span>
        </li>
    </ul>

    <!-- History -->
    <div class="bottom-data">
        <div class="orders">
            <div class="header">
                <i class='bx bx-receipt'></i>
                <h3>History</h3>
                <a href="api/generate_csv.php" class="report">
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
                        while ($result = mysqli_fetch_array($query)) {
                        $no++;
                        ?>
                            <tr>
                                <td><?= $no; ?></td>
                                <td><?= $result['tgl']; ?></td>
                                <td><?= $result['jam']; ?></td>
                                <td><?= $result['ketinggian'] . " CM"; ?></td>
                                <td><?= $result['suhu'] . " C"; ?></td>
                                <td><?= $result['kelembaban'] . "%"; ?></td>
                                <td><?= $result['rain_value']; ?></td>
                            </tr>
                    <?php } ?>
                </tbody>
            </table>
            
            <ul class='pagination'>
                <li>
                    <a class='page-link' href='menu/history_full.php' target='_blank'>
                        See Full Data
                    </a>
                </li>
            </ul>
            
        </div>
    </div>

</body>
</html>