<?php

if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $data_packet = $_POST["data_packet"];

    $fileName = "HTTPSpeedTest.txt";
    $fileStatus = file_put_contents($fileName,$data_packet,FILE_APPEND);

}

?>
