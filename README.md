# NodeMCU Based: 3D Printed Indoor Gauge Thermometer

[![vO6adrETQIA](https://i.imgur.com/lqY467A.png)](https://youtu.be/vO6adrETQIA)


Had some time this weekend and a desire to create something new and interesting, so went ahead and created an Arduino/NodeMCU based indoor dial thermometer. This device displays the temperature in degree centigrade on a D-Shaped Gauge as well as on a 7-Segment display.

In addition to that, it also saves the temperature and humidity readings in a MySQL DataBase hosted on a home based Raspberry Pi Server. The data is then displayed using the "Our Smart Home" app.

Video: https://youtu.be/vO6adrETQIA


Components Required
-------------------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEheo5gQBx8Iows0_cWKiQ75jKITo-NBQYeOiVnp08FMznUk3_HPj-_MpO6lBUhMfnSeUN_LLnxMwDh_dzy0VKgGqUzmtTspcdRM0HDGdW59DqYlTNxIC9h6BKCCcxDTzc4zfNC7K2gOfAiPsFJFywgt6zKQmYNfD3LLlBCj0-pStqlFyK2zfJQVco2LEfiA/w640-h360/1.png)

For this project we need: 
2 x TM1637 Display Modules
1 x DHT22 or DHT11 Module
1 x NodeMCU Microcontroller
1 x 28BYJ-48 Stepper Motor with ULN2003 Driver Board
1 x 10K Resistor
A 3D Printer 
Copper Wire and Some Nuts & Bolts


Circuit Diagram 
---------------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgQaulXYkbfvtODDAavfnQwdgC4Gl5_8F0oyy0e4jZ4vy5X0kFZRKRxNPfLA9v1ZVZpgg3AHxBjMtJzxXlA5pXWOjyfxBgGLGxA7tLMqFhyphenhyphenxhmpreP9YAk6f5bYdSqWPI_RaXftGcpCRwDPvAPEP1n_ivWTI0fLluB2w-X4QGqeZCw9WPOF5ibu-8IYSlOS/w640-h360/2.png)

The circuit is very simple. Connect the ULN2003 driver board’s IN1, IN2, IN3 and IN4 to the NodeMCUs digital pins D0, D1, D2 and D3.
Then connect the OUT Pin of the DHT22 to the D5 Pin of NodeMCU.
After that connect the 2 x Display Modules to the microcontroller. We are going to use a Common Clock Pin D4 for both modules. Then connect the DIO of one of the modules to D6 (TEMP) and the other one to D7 (HUM) pins on the NodeMCU.
Important: Please avoid using the boot config pins D3, D4, D8 and the RTC pin D0 for the displays.

Now, on the D8 Pin we are going to connect the switch. This switch has a very important role in this circuit.
This switch acts as the 'home' or the 'starting point' of the stepper motor.
When the switch is open Pin D8 is connected to GND through the pull-down resistor and we read a LOW. When the switch is closed, Pin D8 connects to 3.3v pin of NodeMCU and we read a HIGH. 
When the 'temperature changes' or the 'device boots up', the pointer starts moving 'counterclockwise'. As soon as the pointer hits the home position, Pin D8 reads HIGH and the logic moves the pointer 'clockwise' to display the temperature on the gauge as read by the DHT22 module.


The Code
--------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEjl9dx05TS7mq2G8DC14pW19qUXQBVeOwoLdOUfKwfVDJiJtwd3zllxogG0OsDgsMj9xrdsdbXDYZkwr7tvbq0TjqA75VPQCspH_l9m7yqjaSjK05qjqHdIucss2XwioqQyNTH3fUIBCnBWSdtqcWsqsIHjcya3PnXCw7voDjMPLv_lT1VQIpUDL4Z_pVxk/w640-h360/3.png)

The code starts by including all the necessary libraries.
Then it defines all the variables needed for setting up the WiFi connection.
Next, it assigns a static IP address to the ESP8266 (if you want to use DHCP then go ahead and delete these three lines from the code).

After that, it sets up the 2 x URLs that are needed for updating the heartbeat, temperature and humidity.
```
String URLUpdateStatus = "http://192.168.0.7/Arduino/Weather/UpdateStatus.php";
String URLUpdateTemp   = "http://192.168.0.7/Arduino/Weather/UpdateTemperature.php";
```
Before going ahead let's have a quick look at the 2 php files.
The "UpdateStutus.php" file uses an UPDATE query to update the timestamp of the device sending the request to the current epoch time and hence updating the heartbeat.

```
<?PHP
    try {
        $Token    = $_GET["Token"];
        $Location = $_GET["Location"];
        include "ConnectionStringArduino.php"; // Create connection
        $sql      = 'Update `Status` SET `DateTime`=\''.time().'\',`State`=\'1\' WHERE `Device`=\''.$Location.'\' AND `Token` = \''.$Token.'\';';
        $result   = $con->query( $sql ); if($result === FALSE) { die(mysqli_error());}
        mysqli_close($con);
    } catch (Exception $e) {}
?>
```

The "UpdateTemperature.php" uses an INSERT query to add a new row to the database with the current values of Temperature and Humidity. 

```
<?PHP
    try {
        $Location = $_GET["Location"];
        $TEMP     = $_GET["TEMP"];
        $HUM      = $_GET["HUM"];
        
        include "ConnectionStringArduino.php"; // Create connection
        $sql    = "INSERT INTO `Weather` (`DateTime`,`Temperature`,`Humidity`,`Location`) VALUES ('".time()."','".$TEMP."','".$HUM."','".$Location."');";
        $result = $con->query( $sql ); if($result === FALSE) { die(mysqli_error());}
        
        mysqli_close($con);
    } catch (Exception $e) {}
?>
```

This is what is written to the database and can be displayed using Google Charts, in my case, I am using the "Our Smart Home" app to display the data using php and JavaScript. Currently I am only displaying the data from the Study room and the Peg Box. To know more about my award winning "Peg Box" project please have a look at my electronics tutorial no. 34 "Peg Box with Temperature and Humidity Monitor using NodeMCU"  (https://youtu.be/elH331NXPsU).

After that, I am defining all the variables required for reading and storing the value of temperature and humidity.

Next, I am defining all the variables and setting up any additional symbols required for displaying temperature and humidity on the TM1637 Display Module.

After that, I am defining the D8 pin of the NodeMCU as the reset switch pin. We will talk about this in detail in the following sections.

Next, I am setting up the Steps Per Revolution of the stepper motor as 2038 and then initializing the stepper library through pins D0 to D3.

const int stepsPerRevolution = 2038;      // Change this to fit the number of steps per revolution of your motor
Stepper myStepper(stepsPerRevolution, D0, D2, D1, D3);// Initialize the stepper library on pins D0 through D3

One thing to note: since I need both clockwise and counterclockwise movements, I have to initialize the pins in the order shown on screen.

Then in the setup() section, first I am setting up the WiFi connection and then sending a heartbeat to the server. Then I am setting up the brightness of the 7-Segments to their max values followed by starting the dht module and finally setting the pin-mode of the switch to INPUT.

```
void setup() {  
  Serial.begin(115200);                   // Initialize the serial port
  
  /*********** Setup a WiFi connection ***********/
  if (WiFi.config(local_IP, gateway, subnet)) { Serial.println("Static IP Configured"); }
  else                                        { Serial.println("Static IP Configuration Failed"); };
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };
  Serial.println("\nWiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  SendIamAlive();                         // Send the initial wakeup message to the server
  /**********************************************/
  
  display_TMP.setBrightness(7);           // Set the display brightness (0-7)
  display_HUM.setBrightness(7);           // Set the display brightness (0-7)
  
  dht.begin();                            // Setup the DHT sensor

  pinMode(SWITCH, INPUT);                 // Declare the switch pin as input
};
```

Now, in the loop() section I am reading the temperature using the Read_Temp() function and then sending the Temperature and Humidity values every 30 minutes and heartbeat every minute to the home server.

```
void loop() {   
  /** Read the temp and humidity info from ther sensor and display it on the 7-segmnet and Gauge **/
  Read_Temp();                            
  
  /** Sending Humidity and temperature every 30 minutes **/
  if((millis() - lastTime) > timerDelay){ Serial.println("Sending Temp and Humidity");SendTemperatureAndHumidity(); lastTime = millis(); };

  /** Sending I am alive message every minute **/
  if((millis() - StatusCounter) > 60000){ Serial.println("Sending Heartbeat"); SendIamAlive(); StatusCounter = millis(); };
};
```

Next, you see the definition of the SendIamAlive() and SendTemperatureAndHumidity() functions which utilizes the WiFiConnect() function to send the values using the previously discussed URLs.

The Read_Temp() function reads the temperature and humidity and updates the 7-Segment displays and moves the pointer only if there is a change in the values.

The Move_Needle() function first sends the pointer to the home position using the Return_Home() function and then looks through and moves the pointer to the correct position until the stepCout is = STEPS.
The value of STEPS is calculated based on the "stepsPerRevolution" which we previously set it up as 2038.
So, 2038 / 2 (for half circle) = 1019
Now by dividing 1019 by 180 degrees we get the steps required to display each degree centigrade.
Now to display each degree centigrade we need 180/60 = 3 divisions.
Since our gauge starts from -10 and not 0 we also have to add the first 10 blocks which is (5.661 * 10 * 3) to our calculation.

```
int STEPS = (5.661 * 3 * TEMP) + 169.833; // 5.661 (step resolution) * 3 (steps to display each °C) * TEMP + 169.833 (5.661 * 10 * 3) = since it starts from -10 and not 0)
```
That's it as easy as that.


3D Designing
------------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEi4vGvrRQEvPNSQmlXRxMDsSziZNzfMx3eZknSuqbbvvqqkjYRduLV5HFocOcQCOQHcH1fINwjAJDvIVFIH4lHgl1pjnA1nSJeSlbWUOqCXvApe8q3fMslLDh8CZveoKUm7VHEf7511JtGH0yUL3dpWvMquw8xiLZZy2m2UZBaIRrmEGAVXhXZB28mru1Nz/w640-h360/4.png)

Lets have a quick look at the 3D model of the project.
At the front, we have The Pointer, D-Shaped Dial, and the Temperature Scale on the dial.

Down at the bottom we have the Enclosure that will house the microcontroller and all other electronics components in it. The enclosure has a Lid to keep the electronic components safe and sound.

![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEie3C1m0t7dzvVFnklFQnJTf-UyNElO9Wq1dETA59-mtix2AjMteVkVlHGYWNCGoNA6J09U-8E6tA_3qOcoRSeVZdtCPvxzwGMX0QYPsR2HCGryd48WNI0G9FnzPU8hh63kB6Y3n1nhyphenhyphenuDq2iSN9h9nz7TzlBE09xHMjPFasqy1hulgCaPtA6XGvOeZzegw/w640-h360/5.png)

At the back, we have a pocket to hold the DHT22 Module, 3 x holes for the stepper motor, 2 x groves for the TM1637 Display Module and 2 x L-Shaped Brackets to hold the top Dial to the bottom Enclosure.


3D Printing
-----------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgGpiCX_bgN0GK_Hr4zTvqckULjbUORlU2pEMKH9v8aSn_v5ijmIj0iAX8xmOd1VhRUOCaYyTuokilDK0bsQ4kzTS4fcem783FBMdgifZHweqLXT8ZWhvHHPwQ0PbHeAcFyf9oDvX6Im28iXFEoshpM66hXiXFo7fEmhBE1C-xaVn1trtAdyVV43DHK6jwC/w640-h360/8.png)

Once the 3D models were sorted, it was time for me to fire up my 3D printing oven and start printing these 3D models.

I used:
- 1.75mm Cold White PLA Filament, and printed the models with
- 0.2mm
- with 0% infill
- and with support.

As we all know, 3D printing is the process that uses computer-aided design or CAD, to create objects layer by layer.
3D printing is not a new technology, it's been there since the 1980's, when Charles W. Hull invented the process and created the first 3D-printed part. Since then, the field of 3D printing has grown exponentially and holds countless possibilities. The 3D printing process fascinates me a lot and I sometimes love to sit near my printer and watch these layers getting printed.

![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgbp1uc4Pnag6NpLhCKYeSklrTg8whi0bevZkewFXbBEZeYIuCdu14l8a42KAKYzwXRU5EK3LMRHLl-X0MFaLl43y7iCsBaTXUrR6WPqlV3BrW7ayyZ7WNai7jZ1egZOD_7FwFalUWhmH1QgExVEDvMSxvuY8QdXdSfVu1zXZv8wq1vgAYxJ_l-_5gH1GxF/w640-h360/9.png)

The entire printing process took a little over 5 hours to complete and this is the final result.
Alright now, let's start gluing all the printed parts. I first superglued the L-Shaped Brackets to the dial followed by the pocket that will hold the DHT22 module. Then, I went ahead and screwed the bottom enclosure to the top section via the L-Shaped Brackets. So, this is how it looks like.



Breadboard Demo
---------------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgrP-bjuuSFsIEBKM5w8OcP01awicroWXovei6W8QdU0F79EHAkQDNRVe5F3KBDuhk80oO_pESC2GvgrXDKC18mNH6WcTxOLn_MPBb9IZcmA6adKp_LIu1MLONr6x4Rigobb55JjMhtXFquVXYxsr17YU5PjlbMETkrBe3Ao6cukRAzoongUFrTG4Zb_1Mx/w640-h360/13_a.png)

Before adding the electronic bits to the 3D printed bits, let's do a quick test to make sure everything works as expected.
So, this 7-Segment display is displaying the temperature and the other one is displaying the humidity. 
The needle is currently going round and round in circles as it has no idea where to stop. To stop the needle, and to send it the correct position on the gauge, I need to connect this red jumper cable connected to 3.3v to the D8 Pin of the NodeMCU. 
Once I short the cable, the needle changes its direction and moves clockwise to display the temperature value read from the DHT22 module.

The temperature and humidity values are also sent to the 'Home Server' which are then displayed using the "Our Smart Home" app.


Coloring
--------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgu-UzoIBO0w7dLtDb6-y1P7H7cUtQNlzKlwCgCYQL7olT_Y9sFiL8tU47qJEbYHSyDohTRldaan5qowVKT6SpfATKBUnHo9lV5prXQeEN07_zmkyM5qmOXajDK9nwQCYXs5H4Hh_KS0Xu6r_GnboaGuBie_TICz2QjkSTHeHR7ZXaVW-WwBueqmjm6KWWD/w640-h360/15.png)

Using Acrylic colors, I painted all the 3D printed parts of the project.


Assembling
----------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEjqmcUkfawR2boFZcRY8trX54zsn_P2CdNWabcPGWkcqzuICO1HK5GoF4jP2-595Fv8uy2g7HQb9Zm5j-NBP02aFZl026UrJM4Rt6yjf-1V8tAyq-TOHW-qnXCDw16KnlDUukBVQU1RGvaA9lMReE63nWKn5ABOHxF-1tAKg48g0Qofo_8AsYGfOjfXY9J5/w640-h360/17.png)

Once the coloring is done, its now time for me to put all the electronic components together. 
First I screwed the stepper motor to the back of the dial. Then, I gently pushed the DHT22 Module into its pocket at the back of the dial.

![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEhEDL0F9jj6Z_6gjFkWbWbwZ0UD-PvxjjmLiVBdgjHpSCP3c9UrVMxmQRp4B9g2wV5eUI6m0bZ1RVsG9k2fSZUuKIople-XEvvn3DTQoxakitaDIlT2ssi8-s2HTr0jf8VqpbUBVPCtrz71LurD3EH-hgbTNeHf-GqW8VK7p22pEIE8EFeZYo55nnDURG79/w640-h360/18.png)

Now the interesting bit. As per our previous discussion, we are going to use a copper wire as a switch that will move the pointer to its correct position. The copper wire will be fed through these two holes from the back and will loop through this small pipe like structure in the front. A small cut will be made on the top exposed side of the copper wire. 

![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEhasl7rq0oHH-HSWvz1CTHayf8nx_5Rp8FsQY-vWxRJcmZD8bRgAKNr2WuZVb3CclJg3RKzMlblgOre_oZ5Mlmm2oDWOUbaUuemuXedwUHnbExSrnvi6WMa_GBFezOPGFYCMnhCO65R1CVR1tPyu7YGHJAWizW-5uRw37s-D1rT0kXj9zeLfhVYsoRrDwqn/w640-h360/21.png)

Now on the pointer, we need to add a small piece of copper wire. When this copper bit touches the two copper wires on the pipe, it will complete the circuit and will send a HIGH to the system.

Next, I am hot gluing the two TM1637 7-Segment Display Modules to the back of the dial.
Once done, it's pretty much just a matter of soldering all the sensors to the NodeMCU as per our circuit diagram.


Final Demo
----------
![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgs1ZUtmYd1CwXO9RM5MFyMq1wI5kDnXimIF1FDi7afD9h-fRbnCPoUjGNRtU7VxXivORsx7OUKFTGo-wrYV9ThXprh5UNyhkRHw4cH3RjRTbPdJVdnHEWwA5oUzIRCKs6t_dDyMchtmqqTZqUYqCUMkUScwDwY5FWiGcgWszofwJ314jGmO2Xp3gROqjIe/w640-h360/25.png)

So, this is how my final setup looks like. 
Once the device is turned on, the pointer moves counterclockwise until it touches the copper wires that acts like a switch. Upon touching the wires the pointer moves clockwise to display the temperature value read from the DHT22 module on the D-Shaped Gauge.
The temperature and humidity values are also displayed using 7-Segment Displays.
The values are also sent over WiFi to a RaspberryPi Home Server and stored in a MySQL database. Using google charts, you can display the data using various different graph options. In my case, I am using the "Our Smart Home" app to display the data using php and JavaScript.

![image](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgih30piS4jyZHaODRi37HKCUNllJRqEdEXDgp41ZfeVbuxh7_A5w2iMJklNEkFeaK3wrGA6I32p5wylOGtacPHN4y5hbZUmoTgJA6HwWhz7ermPT5AupXJbQYYijxE6Mf8mEVT-BmPBy-8B88rV0Qvw3UgGnSxGY-WzGNYvd1HLMTpl8ivtfQqKg_e1dxc/w640-h360/26.png)

Thanks for watching, please comment and let me know if there are any scopes of improvement.


Thanks
------
[![vO6adrETQIA](https://i.imgur.com/lqY467A.png)](https://youtu.be/vO6adrETQIA)


Thanks again for checking my post. I hope it helps you.
If you want to support me subscribe to my YouTube Channel: https://www.youtube.com/user/tarantula3


Video: [https://youtu.be/3Rp0QrV16nQ ](https://youtu.be/vO6adrETQIA)

Full Blog Post: [https://diy-projects4u.blogspot.com/2024/02/Rechargeable-Gothic-Lantern.html ](https://diy-projects4u.blogspot.com/2024/03/Arduino-Based-Indoor-Dial-Thermometer.html)

Thermometer STLs: [https://youtu.be/SPpgMl_TeIc ](https://github.com/tarantula3/Indoor-Dial-Thermometer)



**Support My Work:**

BTC:   1Hrr83W2zu2hmDcmYqZMhgPQ71oLj5b7v5

LTC:   LPh69qxUqaHKYuFPJVJsNQjpBHWK7hZ9TZ

DOGE:  DEU2Wz3TK95119HMNZv2kpU7PkWbGNs9K3

ETH:   0xD64fb51C74E0206cB6702aB922C765c68B97dCD4

BAT:   0x9D9E77cA360b53cD89cc01dC37A5314C0113FFc3

LBC:   bZ8ANEJFsd2MNFfpoxBhtFNPboh7PmD7M2

COS:   bnb136ns6lfw4zs5hg4n85vdthaad7hq5m4gtkgf23 Memo: 572187879

BNB:   0xD64fb51C74E0206cB6702aB922C765c68B97dCD4

MATIC: 0xD64fb51C74E0206cB6702aB922C765c68B97dCD4


Thanks, ca again in my next tutorial.



Blog: https://diyfactory007.blogspot.com/2024/03/NodeMCU-Based-Indoor-Dial-Thermometer.html

Rumble: https://rumble.com/v4jlhs2-indoor-dial-thermometer.html

Cos:    https://cos.tv/videos/play/51395935808295936

odysee: https://odysee.com/@Arduino:7/NodeMCU-Based-Indoor-Dial-Thermometer:f
