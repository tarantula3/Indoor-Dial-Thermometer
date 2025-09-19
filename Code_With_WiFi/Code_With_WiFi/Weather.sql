-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Mar 07, 2022 at 01:11 PM
-- Server version: 10.3.22-MariaDB-0+deb10u1
-- PHP Version: 7.3.14-1~deb10u1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `Arduino`
--

-- --------------------------------------------------------

--
-- Table structure for table `Weather`
--

CREATE TABLE `Weather` (
  `ID` int(11) NOT NULL,
  `DateTime` varchar(12) NOT NULL,
  `Temperature` varchar(4) NOT NULL DEFAULT '0',
  `Humidity` varchar(4) NOT NULL DEFAULT '0',
  `Location` varchar(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Dumping data for table `Weather`
--
INSERT INTO `Weather` (`ID`, `DateTime`, `Temperature`, `Humidity`, `Location`) VALUES
(1, '1633235759', '22', '61', 'pegBox');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `Weather`
--
ALTER TABLE `Weather`
  ADD PRIMARY KEY (`ID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `Weather`
--
ALTER TABLE `Weather`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3144;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
