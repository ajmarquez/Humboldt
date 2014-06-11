#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <floatToString.h> 
#include <SD.h>

SoftwareSerial mySerial(7,8); //Declarar los puertos Virtuales para Modulo GSM

TinyGPS gps;
const int buttonPin = 2;     // Pin del boton para AlerTag
const int ledPin =  13;      // Led de feedback
const int chipSelect = 10;   // Pin de tarjeta MicroSD



void setup()
{
  
  Serial.begin(9600);
  Serial.println("Bienvenido a la terminal del Modulo Caelus");
  Serial.println("Inicializano Puerto Serial a 9600bps...");
  Serial.println("Inicializano Puerto Virtual a 9600bps...");
  mySerial.begin(9600);
  // initiar el LED de alerta como salida:
  pinMode(ledPin, OUTPUT);      
  // Iniciar el pin del boton como entrada:
  pinMode(buttonPin, INPUT); 
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
   
    return;
  }
  Serial.println("Iniciando");
  
  delay(1000);
  Serial.println("GO!");
}

void loop()
{
  while (Serial.available())
  {
    int c = Serial.read();
    if (gps.encode(c)) //When (gps.encode(c))==TRUE the GPS has a fixed location / but not entirely accurate until a few seconds.
    {
      powerupGsm();
      Serial.println("Setting variables");
      Serial.println("Fetching Data from Satellites...");
      fetching(gps);
      powerupGsm();
    }
  }
}


void fetching (TinyGPS &gps)
{
  //Declara variables para obtener datos del GPS
  float flat,flon, speedData=0;
  String s_Kmph,lat,lon,s_year, s_month, s_day, s_hour, s_minute, s_second, alertag="";
  String data="";
  int gps_Time[6];
  char buffer[25];
  
  alertag=alerTagCheck();

  //Obtener Latitud y Longitud
  gps.f_get_position(&flat,&flon);
  lat=floatToString(buffer, flat , 5);
  lon=floatToString(buffer, flon , 5);

  //Obtener Velocidad en KMPH
  speedData= gps.f_speed_kmph();
  s_Kmph= floatToString(buffer, speedData, 5);

  //Obtener tiempo
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
  gps_Time[0]=year;
  gps_Time[1]=(int)month;
  gps_Time[2]=(int)day;
  gps_Time[3]=(int)hour;
  gps_Time[4]=(int)minute;
  gps_Time[5]=(int)second;

  Serial.print("La hora GTM en este momento es: ");
  Serial.print(gps_Time[3]);
  Serial.print(":");
  Serial.print(gps_Time[4]);
  Serial.print(":");
  Serial.print(gps_Time[5]);

  String type="alert";
  //Crear la trama de datos
  bitacora(lat,lon,s_Kmph,alertag);
  data=(lat+";"+lon+";"+s_Kmph+";"+alertag);

  goCaelus(data);


  Serial.println("Trama enviada");
}


//Power Up el GSM module
void powerupGsm()
{
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(3000);
  Serial.println("Falcon rockets On!");
}

//Modulo que revisa si es una alerta
String alerTagCheck()
{
  int buttonState = 0; 
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    // Prender  LED:    
    digitalWrite(ledPin, HIGH); 
   return ("alert"); 
  } 
  else {
    // apagar LED :
    digitalWrite(ledPin, LOW); 
    return ("quiet");
  }
}

//Modulo de bitacora
void bitacora( String &lat, String &lon, String &speedD, String &alert)
{
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if(dataFile){

  
  dataFile.print("Lat/Long:"); 
  dataFile.print(lat); 
  dataFile.print(";"); 
  dataFile.println(lon);
  dataFile.print(";"); 
  dataFile.println(speedD);
  dataFile.print(";"); 
  dataFile.println(alert);
  dataFile.close();
  Serial.println("It worked!");
  }
}

//Initialization and execution of the Millenium Falcon (the GSM/GPRS mode)
void goCaelus(String data) 
{
	mySerial.print("AT+CPIN= \"0000\" \r");
	delay(5000);
	mySerial.print("AT+CREG?");
	delay(1000);
	mySerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" \r"); //Inicializar tipo de conexion
	delay(1000);
        //Agregar direccion APN, en este caso para Digitel: gprsweb.digitel.ve
	mySerial.println("AT+SAPBR=3,1,\"APN\",\"gprsweb.digitel.ve\" \r"); 
	delay(1000);
	mySerial.println("AT+SAPBR=1,1"); 
	delay(1000);
	//Iniciar servicio HTTP
        mySerial.println("AT+HTTPINIT");
	delay(1000);
	//Introducir parametro CID
        mySerial.println("AT+HTTPPARA=\"CID\",1 \r"); 
	delay(1000);
        //colocar la URL de envio de datos
	mySerial.print("AT+HTTPPARA=\"URL\",\"hackadabra.com/humboldt/terra.php?data=");
        mySerial.print(data);
        mySerial.println("\"");
	delay(1000);
        //Ejecutar el envio de datos
	mySerial.println("AT+HTTPACTION=0");
	delay(5000);
	mySerial.println((char)26);
	delay(1000);
	mySerial.println();
	Serial.println("Caelus hizo un vuelo exitoso!");
        Serial.println(data);    
}

