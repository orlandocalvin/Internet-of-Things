<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/png" href="images/favicon.png">
    <link href='https://unpkg.com/boxicons@2.1.4/css/boxicons.min.css' rel='stylesheet'>
    <link rel="stylesheet" href="css/style.css">
    <title>SmartDam Web Monitoring</title>
</head>

<body>

    <!-- Sidebar -->
    <div class="sidebar">
        <a class="logo">
            <i class='bx bx-shield-quarter'></i>
            <div class="logo-name"><span>Duba</span>Tech</div>
        </a>
        <ul class="side-menu">
            <li class="active"><a onclick="loadPage('menu/remote.php')"><i class='bx bx-devices'></i>Remote</a></li>
            <li><a onclick="loadPage('menu/history.php')"><i class='bx bx-history'></i>History</a></li>
        </ul>
    </div>
    <!-- End of Sidebar -->

    <!-- Main Content -->
    <div class="content">
        <!-- Navbar -->
        <nav>
            <i class='bx bx-menu'></i>
            <form action="#">
                <!-- Search => form-input {display:none;} -->
                <div class="form-input">
                    <input type="search" placeholder="Search...">
                    <button class="search-btn" type="submit"><i class='bx bx-search'></i></button>
                </div>
            </form>
            <input type="checkbox" id="theme-toggle" hidden>
            <label for="theme-toggle" class="theme-toggle"></label>
        </nav>
        <!-- End of Navbar -->

        <div id="main-content">
            <!-- Konten dari file eksternal akan dimuat di sini -->
        </div>
    </div>

    <script>
        function loadPage(pageUrl) {
            var content = document.getElementById("main-content");
            content.classList.add('fadeOut');
            setTimeout(function () {
                fetch(pageUrl)
                    .then(response => response.text())
                    .then(data => {
                        content.innerHTML = data;
                        content.classList.remove('fadeOut');
                    })
                    .catch(error => {
                        console.error('Error:', error);
                    });
            }, 300);
        }
        loadPage('menu/remote.php');
    </script>
    <script src="js/script.js"></script>
</body>
</html>