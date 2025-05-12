#include <WiFi.h>
#include <ThingerESP32.h>

#define usuario "KevinS-bot"
#define device_Id "LEDS"
#define device_credentials "12345678900"
ThingerESP32 thing(usuario, device_Id, device_credentials);

const char WiFi_ssid[] = "TU_CONEXION_EDWIN_SUQUI";
const char WiFi_password[] = "1900470053";

int LED_MONOCROMO = 2;
int LED_RGB_R = 13;
int LED_RGB_G = 12;
int LED_RGB_B = 14;

void setup() {
  thing.add_wifi(WiFi_ssid, WiFi_password);

  pinMode(LED_MONOCROMO, OUTPUT);
  pinMode(LED_RGB_R, OUTPUT);
  pinMode(LED_RGB_G, OUTPUT);
  pinMode(LED_RGB_B, OUTPUT);

  thing["LED MONOCROMO"] << [](pson& in){
    if(in.is_empty()){
      in = (bool) digitalRead(LED_MONOCROMO);
    }
    else{
      digitalWrite(LED_MONOCROMO, in ? HIGH : LOW);
    }
  };

  thing["LED RGB"] << [](pson& in){
    analogWrite(LED_RGB_R, in["Rojo"]);
    analogWrite(LED_RGB_G, in["Verde"]);
    analogWrite(LED_RGB_B, in["Azul"]);
  };
}

void loop() {
  thing.handle();
}
