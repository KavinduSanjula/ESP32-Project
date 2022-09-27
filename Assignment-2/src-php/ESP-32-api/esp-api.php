<?php

//config
$host = "localhost";
$user = "root";
$password = "";
$database = "esp-32-data";

$connection = mysqli_connect($host,$user,$password,$database);

if (isset($_POST['submit'])){

    $time = $_POST['timestamp'];    //"20211208111500";
    $temperature = $_POST['temp'];  //32.5;
    $lightlevel = $_POST['light'];  //50;

    $query = 'INSERT INTO sensor_data(_time,temperature,lightlevel) VALUES("'.$time . '",' .$temperature .',' . $lightlevel .')';


    if ($connection){

        $result = mysqli_query($connection,$query);
        if ($result){
            echo "succsessfull!";
        }

    }else{
        echo("Connection faild!");
    }
    

}else{
    echo "Method not allowd!";
}
