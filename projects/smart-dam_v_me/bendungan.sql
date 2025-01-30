-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Mar 12, 2024 at 02:48 PM
-- Server version: 10.5.20-MariaDB
-- PHP Version: 7.3.33

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `id20397065_data_mhs`
--

-- --------------------------------------------------------

--
-- Table structure for table `bendungan`
--

CREATE TABLE `bendungan` (
  `id` int(11) NOT NULL,
  `tgl` varchar(12) DEFAULT NULL,
  `jam` varchar(12) DEFAULT NULL,
  `ketinggian` int(2) DEFAULT NULL,
  `suhu` int(2) DEFAULT NULL,
  `kelembaban` int(2) DEFAULT NULL,
  `rain_value` varchar(15) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `bendungan`
--

INSERT INTO `bendungan` (`id`, `tgl`, `jam`, `ketinggian`, `suhu`, `kelembaban`, `rain_value`) VALUES
(1015, '13-11-2023', '18:03:20', 0, 26, 76, 'Tidak Hujan'),
(1016, '13-11-2023', '18:03:25', 0, 26, 76, 'Tidak Hujan'),
(1017, '13-11-2023', '18:03:30', 0, 26, 76, 'Tidak Hujan'),
(1018, '13-11-2023', '18:03:36', 0, 26, 76, 'Tidak Hujan'),
(1019, '13-11-2023', '18:03:42', 0, 26, 76, 'Tidak Hujan'),
(1020, '13-11-2023', '18:03:47', 0, 26, 76, 'Tidak Hujan'),
(1021, '13-11-2023', '18:03:52', 0, 26, 76, 'Tidak Hujan'),
(1022, '13-11-2023', '18:03:58', 0, 26, 76, 'Tidak Hujan'),
(1023, '13-11-2023', '18:04:03', 0, 26, 76, 'Tidak Hujan'),
(1024, '13-11-2023', '18:04:08', 0, 26, 76, 'Tidak Hujan'),
(1025, '13-11-2023', '18:04:14', 0, 26, 76, 'Tidak Hujan'),
(1026, '13-11-2023', '18:04:19', 0, 26, 76, 'Tidak Hujan'),
(1027, '13-11-2023', '18:04:25', 0, 26, 76, 'Tidak Hujan'),
(1028, '13-11-2023', '18:04:30', 0, 26, 76, 'Tidak Hujan'),
(1029, '13-11-2023', '18:04:36', 0, 26, 76, 'Tidak Hujan'),
(1030, '13-11-2023', '18:04:42', 0, 26, 76, 'Tidak Hujan'),
(1031, '13-11-2023', '18:04:47', 0, 26, 76, 'Tidak Hujan'),
(1032, '13-11-2023', '18:04:52', 0, 26, 76, 'Tidak Hujan'),
(1033, '13-11-2023', '18:04:58', 0, 26, 76, 'Tidak Hujan'),
(1034, '13-11-2023', '18:05:03', 0, 26, 76, 'Tidak Hujan'),
(1035, '13-11-2023', '18:05:08', 0, 26, 76, 'Tidak Hujan'),
(1036, '13-11-2023', '18:05:14', 0, 26, 76, 'Tidak Hujan'),
(1037, '13-11-2023', '18:05:19', 0, 26, 76, 'Tidak Hujan'),
(1038, '13-11-2023', '18:05:24', 0, 26, 76, 'Tidak Hujan'),
(1039, '13-11-2023', '18:05:30', 0, 26, 76, 'Tidak Hujan'),
(1040, '13-11-2023', '18:05:35', 0, 26, 76, 'Tidak Hujan'),
(1041, '13-11-2023', '18:05:42', 0, 26, 76, 'Tidak Hujan'),
(1042, '13-11-2023', '18:05:48', 0, 26, 76, 'Tidak Hujan'),
(1043, '13-11-2023', '18:05:53', 0, 26, 76, 'Tidak Hujan'),
(1044, '13-11-2023', '18:05:58', 0, 26, 76, 'Tidak Hujan'),
(1045, '13-11-2023', '18:06:04', 0, 26, 76, 'Tidak Hujan'),
(1046, '13-11-2023', '18:06:09', 0, 26, 76, 'Tidak Hujan'),
(1047, '13-11-2023', '18:06:15', 0, 26, 76, 'Tidak Hujan'),
(1048, '13-11-2023', '18:06:20', 0, 26, 76, 'Tidak Hujan'),
(1049, '13-11-2023', '18:06:26', 0, 26, 76, 'Tidak Hujan'),
(1050, '13-11-2023', '18:06:31', 0, 26, 76, 'Tidak Hujan'),
(1051, '13-11-2023', '18:06:36', 0, 26, 76, 'Tidak Hujan'),
(1052, '13-11-2023', '18:06:42', 0, 26, 76, 'Tidak Hujan'),
(1053, '13-11-2023', '18:06:48', 0, 26, 76, 'Tidak Hujan'),
(1054, '13-11-2023', '18:06:53', 0, 26, 76, 'Tidak Hujan'),
(1055, '13-11-2023', '18:06:58', 0, 26, 76, 'Tidak Hujan'),
(1056, '13-11-2023', '18:07:04', 0, 26, 76, 'Tidak Hujan'),
(1057, '13-11-2023', '18:07:09', 0, 26, 76, 'Tidak Hujan'),
(1058, '13-11-2023', '18:07:14', 0, 26, 76, 'Tidak Hujan'),
(1059, '13-11-2023', '18:07:20', 0, 26, 76, 'Tidak Hujan'),
(1060, '13-11-2023', '18:07:25', 0, 26, 76, 'Tidak Hujan'),
(1061, '13-11-2023', '18:07:30', 0, 26, 76, 'Tidak Hujan'),
(1062, '13-11-2023', '18:07:36', 0, 26, 76, 'Tidak Hujan'),
(1063, '13-11-2023', '18:07:41', 0, 26, 76, 'Tidak Hujan'),
(1064, '13-11-2023', '18:07:47', 0, 26, 76, 'Tidak Hujan'),
(1065, '13-11-2023', '18:07:52', 0, 26, 76, 'Tidak Hujan'),
(1066, '13-11-2023', '18:08:09', 0, 26, 76, 'Tidak Hujan'),
(1067, '13-11-2023', '18:08:25', 0, 26, 76, 'Tidak Hujan'),
(1068, '13-11-2023', '18:08:31', 0, 26, 76, 'Tidak Hujan'),
(1069, '13-11-2023', '18:08:36', 0, 26, 76, 'Tidak Hujan'),
(1070, '13-11-2023', '18:08:53', 0, 26, 76, 'Tidak Hujan'),
(1071, '13-11-2023', '18:08:58', 0, 26, 76, 'Tidak Hujan'),
(1072, '13-11-2023', '18:09:04', 0, 26, 76, 'Tidak Hujan'),
(1073, '13-11-2023', '18:09:09', 0, 26, 76, 'Tidak Hujan'),
(1074, '13-11-2023', '18:09:14', 0, 26, 76, 'Tidak Hujan'),
(1075, '13-11-2023', '18:09:20', 0, 26, 76, 'Tidak Hujan'),
(1076, '13-11-2023', '18:09:25', 0, 26, 76, 'Tidak Hujan'),
(1077, '13-11-2023', '18:09:30', 0, 26, 76, 'Tidak Hujan'),
(1078, '13-11-2023', '18:09:36', 0, 26, 76, 'Tidak Hujan'),
(1079, '13-11-2023', '18:09:41', 0, 26, 76, 'Tidak Hujan'),
(1080, '13-11-2023', '18:09:46', 0, 26, 76, 'Tidak Hujan'),
(1081, '13-11-2023', '18:10:04', 0, 26, 76, 'Tidak Hujan'),
(1082, '13-11-2023', '18:10:09', 0, 26, 76, 'Tidak Hujan'),
(1083, '13-11-2023', '18:10:14', 0, 26, 76, 'Tidak Hujan'),
(1084, '13-11-2023', '18:10:20', 0, 26, 76, 'Tidak Hujan'),
(1085, '13-11-2023', '18:10:25', 0, 26, 76, 'Tidak Hujan'),
(1086, '13-11-2023', '18:10:30', 0, 26, 76, 'Tidak Hujan'),
(1087, '13-11-2023', '18:10:36', 0, 26, 76, 'Tidak Hujan'),
(1088, '13-11-2023', '18:10:41', 0, 26, 76, 'Tidak Hujan'),
(1089, '13-11-2023', '18:10:46', 0, 26, 76, 'Tidak Hujan'),
(1090, '13-11-2023', '18:10:52', 0, 26, 76, 'Tidak Hujan'),
(1091, '13-11-2023', '18:10:57', 0, 26, 76, 'Tidak Hujan'),
(1092, '13-11-2023', '18:11:03', 0, 26, 76, 'Tidak Hujan'),
(1093, '13-11-2023', '18:11:08', 0, 26, 76, 'Tidak Hujan'),
(1094, '13-11-2023', '18:11:13', 0, 26, 76, 'Tidak Hujan'),
(1095, '13-11-2023', '18:11:19', 0, 26, 76, 'Tidak Hujan'),
(1096, '13-11-2023', '18:11:24', 0, 26, 76, 'Tidak Hujan'),
(1097, '13-11-2023', '18:11:29', 0, 26, 76, 'Tidak Hujan'),
(1098, '13-11-2023', '18:11:35', 0, 26, 76, 'Tidak Hujan'),
(1099, '13-11-2023', '18:11:40', 0, 26, 76, 'Tidak Hujan'),
(1100, '13-11-2023', '18:11:45', 0, 26, 76, 'Tidak Hujan'),
(1101, '13-11-2023', '18:11:51', 0, 26, 76, 'Tidak Hujan'),
(1102, '13-11-2023', '18:11:56', 0, 26, 76, 'Tidak Hujan'),
(1103, '13-11-2023', '18:12:01', 0, 26, 76, 'Tidak Hujan'),
(1104, '13-11-2023', '18:12:07', 0, 26, 76, 'Tidak Hujan'),
(1105, '13-11-2023', '18:12:12', 0, 26, 76, 'Tidak Hujan'),
(1106, '13-11-2023', '18:12:18', 0, 26, 76, 'Tidak Hujan'),
(1107, '13-11-2023', '18:12:23', 0, 26, 76, 'Tidak Hujan'),
(1108, '13-11-2023', '18:12:28', 0, 26, 76, 'Tidak Hujan'),
(1109, '13-11-2023', '18:12:34', 0, 26, 76, 'Tidak Hujan'),
(1110, '13-11-2023', '18:12:39', 0, 26, 76, 'Tidak Hujan'),
(1111, '13-11-2023', '18:12:44', 0, 26, 76, 'Tidak Hujan'),
(1112, '13-11-2023', '18:12:50', 0, 26, 76, 'Tidak Hujan'),
(1113, '13-11-2023', '18:12:55', 0, 26, 76, 'Tidak Hujan'),
(1114, '13-11-2023', '18:13:00', 0, 26, 76, 'Tidak Hujan');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `bendungan`
--
ALTER TABLE `bendungan`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `bendungan`
--
ALTER TABLE `bendungan`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1115;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
