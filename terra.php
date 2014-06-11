<?php

require("dbinfo.php");


function parseToXML($htmlStr) 
{ 
$xmlStr=str_replace('<','&lt;',$htmlStr); 
$xmlStr=str_replace('>','&gt;',$xmlStr); 
$xmlStr=str_replace('"','&quot;',$xmlStr); 
$xmlStr=str_replace("'",'&#39;',$xmlStr); 
$xmlStr=str_replace("&",'&amp;',$xmlStr); 
return $xmlStr; 
} 

//$time=$_GET[time];
$data=$_GET[data];
list($lat,$lng,$speed,$type)=explode(";", $data);

//Escribe datos a la DB 

// Abre conexion a MySQL server
$connection=mysql_connect ('localhost', $username, $password);
if (!$connection) {
  die('Not connected : ' . mysql_error());
}

// Inicia la base de datos MySQL
$db_selected = mysql_select_db($database, $connection);
if (!$db_selected) {
  die ('Can\'t use db : ' . mysql_error());
}

// Selecciona las columnas de la tabla malpha
$query = "INSERT INTO malpha ".
       "(lat,lng,speed,type) ".
       "VALUES('$lat','$lng','$speed','$type')";

$intro = mysql_query($query);
if (!$intro) {
  die('Invalid query: ' . mysql_error());
}

$time=date(DATE_RFC2822);

$fh=fopen("filename.txt",'a');
fwrite($fh, "$time $lat $lon"+ "\n");
//echo "done";  //Envia confirmacion a Arduino
fclose($fh);

//Printing the XML

// abre conexion a MySQL Server y se repite la consulta
$connection=mysql_connect ('localhost', $username, $password);
if (!$connection) {
  die('Not connected : ' . mysql_error());
}


$db_selected = mysql_select_db($database, $connection);
if (!$db_selected) {
  die ('Can\'t use db : ' . mysql_error());
}

// Se seleccionan TODOS los markers
$query = "SELECT * FROM malpha WHERE 1";
$result = mysql_query($query);
if (!$result) {
  die('Invalid query: ' . mysql_error());
}

header("Content-type: text/xml");

// Se inicia archivo XML
echo '<markers>';

//Imprime nodos de XML por cada fila
while ($row = @mysql_fetch_assoc($result)){
  // Agrega nodo XML
  echo '<marker ';
  echo 'name="' . parseToXML($row['name']) . '" ';
  echo 'address="' . parseToXML($row['address']) . '" ';
  echo 'lat="' . $row['lat'] . '" ';
  echo 'lng="' . $row['lng'] . '" ';
  echo 'type="' . $row['type'] . '" ';
  echo '/>';
}


// Final de archivo XML
echo '</markers>';





?>