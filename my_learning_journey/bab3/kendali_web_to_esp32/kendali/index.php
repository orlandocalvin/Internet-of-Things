<?php
    if (isset($_GET['light'])) {
        $light = $_GET['light'];
    } else {
        $light = "OFF";
    }

    if ($light == "on") {
        $file = fopen("light.json", "w") or die("can't open file");
        fwrite($file, '{"light": "on"}');
        fclose($file);
    } else if ($light == "off") {
        $file = fopen("light.json", "w") or die("can't open file");
        fwrite($file, '{"light": "off"}');
        fclose($file);
    }

    $json = file_get_contents("light.json") or die("can't open file");
    $json_light = json_decode($json, true);
    
?>

<html>
<head>
    <title>Modul Web-IoT</title>
    <script src="https://code.jquery.com/jquery-2.1.4.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js"></script>
    <link href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="//maxcdn.bootstrapcdn.com/font-awesome/4.3.0/css/font-awesome.min.css">
    <script src="light.js" type="text/javascript"></script>
</head>
<body>
    <div class="content">
        <div style="center">
            <header>
                <h1 align="center">KENDALI IOT BERBASIS WEB</h1>
            </header>
        </div>

        <div class="badan">
            <div class="row" style="margin-top: 20px;">
                <div class="col-md-8 col-md-offset-2">
                    <a id="light_on" href="?light=on" class="btn btn-success btn-block btn-lg">Turn On</a><br>
                    <a id="light_off" href="?light=off" class="led btn btn-danger btn-block btn-lg">Turn Off</a><br>

                    <div class="light-status well" style="margin-top: 5px; text-align:center">
                        <?php
                            if($light == "on") {
                                echo("Lampu Nyala");
                            } else if ($light == "off") {
                                echo("Lampu Mati");
                            } else {
                                echo ("Tidak ditemukan");
                            }
                        ?>
                    </div>

                    <div class="light-status well" style="margin-top: 5px; text-align:center;">
                        <img id="light_img_on" src="images/on.jpg" style="height:200px;">
                        <img id="light_img_off" src="images/off.jpg" style="height:200px;">
                    </div>
                </div>
            </div>
        </div>
    </div>
</body>
</html>