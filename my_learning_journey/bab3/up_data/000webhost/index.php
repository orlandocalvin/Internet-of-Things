<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <!--<meta http-equiv="refresh" content="60"> autorefresh 60s -->
    <link rel="shortcut icon" href="icon.png" type="image/x-icon"/>
    <link href="https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.5.2/css/bootstrap.css" rel="stylesheet">
    <link href="https://cdn.datatables.net/1.13.4/css/dataTables.bootstrap4.min.css" rel="stylesheet">
    <script src="https://code.jquery.com/jquery-3.5.1.js"></script>
    <script src="https://cdn.datatables.net/1.13.4/js/jquery.dataTables.min.js"></script>
    <script src="https://cdn.datatables.net/1.13.4/js/dataTables.bootstrap4.min.js"></script>

    <!--datepicker-->
    <link rel="stylesheet" href="https://cdn.datatables.net/datetime/1.4.1/css/dataTables.dateTime.min.css">
    <script src="https://cdn.datatables.net/datetime/1.4.1/js/dataTables.dateTime.min.js"></script>

    <!--responsive-->
    <link rel="stylesheet" href="https://cdn.datatables.net/responsive/2.4.1/css/responsive.bootstrap4.min.css">
    <script src="https://cdn.datatables.net/responsive/2.4.1/js/dataTables.responsive.min.js"></script>
    <script src="https://cdn.datatables.net/responsive/2.4.1/js/responsive.bootstrap4.min.js"></script>
    <title>Sensor Ultrasonik HC-SR04</title>
</head>

<body class="mt-2 mb-5">
    <div class="container">
        <h2 class="text-center"> Tabel Monitoring Sensor Ultrasonik HC-SR04</h2>
        <table id="tabel" class="table table-striped table-bordered dt-responsive nowrap" style="width:100%">
            <thead class="text-white bg-success">
                <tr>
                    <th>No</th>
                    <th>Tanggal</th>
                    <th>Jam</th>
                    <th>Hasil Baca</th>
                </tr>
            </thead>
            <tbody>
                <?php
                include 'koneksi.php';
                $datasensor = mysqli_query($koneksi, "SELECT * FROM jarak order by id desc");
                $no = 0;
                while ($d = mysqli_fetch_array($datasensor)) {
                    $no++;
                ?>
                    <tr>
                        <td><?php echo $no; ?></td>
                        <td><?php echo $d['tgl']; ?></td>
                        <td><?php echo $d['jam']; ?></td>
                        <td><?php echo $d['jarak'] . " CM"; ?></td>
                    </tr>
                <?php } ?>
            </tbody>
        </table>
    </div>

    <script>
        $(document).ready(function() {
            var table = $('#tabel').DataTable({
                responsive: true
            });
        });
    </script>

</body>
</html>