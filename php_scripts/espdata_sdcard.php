<?php

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $data_packet = $_POST["data_packet"];

    $fileName = "sdcard_datafile.txt";
    $fileStatus = file_put_contents($fileName,$data_packet,FILE_APPEND);

}

?>
