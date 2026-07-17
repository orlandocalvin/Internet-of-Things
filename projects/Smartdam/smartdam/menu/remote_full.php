<?php
    if (isset($_GET['servo'])) {
        $servo = $_GET['servo'];
    } else {
        $servo = "off";
    }

    if ($servo == "on") {
        $file = fopen("../json/servo.json", "w") or die("can't open file");
        fwrite($file, '{"servo": "on"}');
        fclose($file);
    } else if ($servo == "off") {
        $file = fopen("../json/servo.json", "w") or die("can't open file");
        fwrite($file, '{"servo": "off"}');
        fclose($file);
    }

    // $json = file_get_contents("servo.json") or die("can't open file");
    // $json_servo = json_decode($json, true);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/png" href="../images/favicon.png">
    <link href='https://unpkg.com/boxicons@2.1.4/css/boxicons.min.css' rel='stylesheet'>
    <title>Remote Control</title>
    <style>
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
    
        .remote {
            width: 250px;
            height: 350px;
            background-color: #333;
            border: 2px solid #000;
            border-radius: 10px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            position: relative;
            overflow: hidden;
        }
    
        .power {
            width: 90px;
            height: 90px;
            border: 2px solid #000;
            border-radius: 50%;
            display: flex;
            justify-content: center;
            align-items: center;
            color: #fff;
            font-weight: bold;
            cursor: pointer;
            text-decoration: none;
        }

        #power-on {
            margin-top: 35px;
            background-color: green;
        }

        #power-off {
            margin-top: 20px;
            background-color: red;
        }

        .bx-power-off {
            display: flex;
            justify-content: center;
            font-size: 3rem;
        }
    
        .lcd {
            width: 70%;
            background-color: #444;
            color: red;
            padding: 15px;
            margin-top: -5px;
            border: 2px solid red;
            border-radius: 5px;
            text-align: center;
            font-size: 24px;
            box-shadow: 0 0 5px rgba(255, 0, 0, 0.5);
            overflow: hidden;
        }

        .lcd-on {
            color: green;
            border: 2px solid green;
            box-shadow: 0 0 5px rgba(0, 255, 0, 0.5);
        }

        .warning {
            color: yellow;
            border: 2px solid yellow;
            box-shadow: 0 0 5px rgba(255, 255, 0, 0.5);
        }

        .rText {
            width: 285%;
            white-space: nowrap;
            animation: runningText 4s linear infinite;
        }
    
        @keyframes runningText {
            from {
                transform: translateX(100%);
            }
            to {
                transform: translateX(-100%);
            }
        }
    </style>
    
</head>
<body>
    <div class="remote">
        <?php
            $mati = "<div id='lcd' class='lcd'><div id='state' class='state'>OFF</div></div>";
            $idup = "<div id='lcd' class='lcd lcd-on'><div id='state' class='state'>ON</div></div>";

            if($servo == "on") {
                echo $idup;
            } else {
                echo $mati;
            }
        ?>
        <div class="tombol">
            <a href="?servo=on" id="power-on" class="power"><i class='bx bx-power-off'></i></a>
            <a href="?servo=off" id="power-off" class="power"><i class='bx bx-power-off'></i></a>
        </div>
    </div>
</body>
</html>