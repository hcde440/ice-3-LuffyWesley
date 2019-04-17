// This code takes reading from the barometric sensor and dht22 sensor that provides
// temp and humidity data. The data is then sent to the OLED display and adafruit.io.

#include "config.h" // edit the config.h tab and enter your credentials

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <Adafruit_MPL115A2.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1  // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 
Adafruit_MPL115A2 mpl115a2; 

#define DATA_PIN 12 // pin connected to DH22 data line

DHT_Unified dht(DATA_PIN, DHT22); // create DHT22 instance

// set up the 'temperature', 'humidity' and 'pressure' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *pressure = io.feed("pressure");

void setup() {

  Serial.begin(115200); // start the serial connection
  
  // Prints the results to the serial monitor
  Serial.print("This board is running: ");  //Prints that the board is running
  Serial.println(F(__FILE__));
  Serial.print("Compiled: "); //Prints that the program was compiled on this date and time
  Serial.println(F(__DATE__ " " __TIME__));

  while(! Serial); // wait for serial monitor to open

  dht.begin(); // initialize dht22

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  mpl115a2.begin(); // initialize barometric sensor

  //setup for the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initailize with the i2c addre 0x3C
  display.clearDisplay();                    //Clears any existing images or text
  display.setTextSize(1);                    //Set text size
  display.setTextColor(WHITE);               //Set text color to white
  display.setCursor(0,0);                    //Puts cursor on top left corner
  display.println("Starting up...");         //Test and write up
  display.display();                         //Displaying the display
}

void loop() {

  // io.run(); is required for all sketches. it should always be present at the top of your loop
  // function. it keeps the client connected to io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;
  dht.temperature().getEvent(&event); // get temperature data from dht22

  float celsius = event.temperature; // celcius variable set to what we get from dht22
  float fahrenheit = (celsius * 1.8) + 32; // fahrenheit variable conversion
  float pressureKPA = mpl115a2.getPressure();; // pressure variable set to what we get from pressure sensor

  // prints celcius message on serial
  Serial.print("Celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  // prints fahrenheit message on serial
  Serial.print("Fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  temperature->save(fahrenheit); // save fahrenheit to Adafruit IO

  dht.humidity().getEvent(&event); // get humidity data from dht22

  // prints humidity message on serial
  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");

  humidity->save(event.relative_humidity); // save humidity to Adafruit IO

  // prints pressure message on serial
  Serial.print("Pressure (kPa): "); 
  Serial.print(pressureKPA, 4); 
  Serial.println(" kPa");

  pressure->save(pressureKPA); // save pressure to Adafruit IO

  delay(5000); // wait 5 seconds (5000 milliseconds == 5 seconds)
  
  display.clearDisplay(); //Clears any existing images or text
  display.setCursor(0,0); //Puts cursor on top left corner

  // prints fahrenheit message on display
  display.print("fahrenheit: ");
  display.print(fahrenheit);
  display.println(F("F"));

  // prints celcius message on display
  display.print("Celsius: ");
  display.print(celsius);
  display.println(F("C"));

  // prints pressure message on display
  display.print("Pressure: "); 
  display.print(pressureKPA, 3); 
  display.println(F(" kPa"));

  // prints humidity message on display
  display.print("humidity: ");
  display.print(event.relative_humidity);
  display.println(F("%"));
  
  display.display(); //Displaying the display
  delay(5000); // wait 5 seconds (5000 milliseconds == 5 seconds)
}
