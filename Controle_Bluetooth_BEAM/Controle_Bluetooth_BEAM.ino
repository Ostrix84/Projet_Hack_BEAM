#include "BluetoothSerial.h"

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "Jerome";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define Speed 50 // On choisit un duty cicle des PWMs qui vont controler la vitesse de rotation des roues : Valeur entre 0 et 254
#define SpeedTurn 30 // On choisit un duty cicle à ajouter à une des deux roues pour initier un virage

int pwmChannelG = 0; //Choisit le canal 0 pour la roue gauche
int pwmChannelD = 1; //Choisit le canal 1 pour la roue droite
int frequence = 10000; //Fréquence PWM de 10 KHz
int resolution = 8; // Résolution de 8 bits, 256 valeurs possibles
int pwmPinG = 23;
int pwmPinD = 22;
BluetoothSerial SerialBT;
int valG=0;
int valD=0;
char LC = '9'; //LastChar, dernier valeur connu de la commande bluetooth, initialisé à une valeur absurde pour ne pas perturber le code
char LCC = '9'; // Avant derniere valeur connu de la commande bluetooth, initialisé à une valeur absurde pour ne pas perturber le code
int dirG = 19; //Pin DIR de la roue gauche
int dirD = 21; //Pin DIR de la roue droite


void setup() {
  Serial.begin(9600);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
  // Configuration du canal 0 avec la fréquence et la résolution choisie
    ledcSetup(pwmChannelG, frequence, resolution);
    ledcSetup(pwmChannelD, frequence, resolution);

    // Assigne le canal PWM au pin 23 et 22
    ledcAttachPin(pwmPinG, pwmChannelG);
    ledcAttachPin(pwmPinD, pwmChannelD);

    //Initialisation des pins
    pinMode(dirG,OUTPUT);
    pinMode(dirD,OUTPUT);
    pinMode(stopPin,OUTPUT);
    digitalWrite(dirG,0);
    digitalWrite(dirD,1);
    digitalWrite(stopPin,1);

}

//Boucle principal
void loop() {
  if (SerialBT.available()) {
    move(SerialBT.read());
  }
  delay(1);
}

//Valeur retourné par le controleur Bluetooth de l'application Arduino Bluetooth Control
//F : flèche avant : avancer
//R  L : Right et Left : demi tour sur place à droite et à gauche
//B : flèche arrière : reculer

//T (triangle) : avancer
//C (circle) : Tourner à droite à droite en roulant
//S (square) : Tourner à gauche gauche en roulant 
//X : Reculer

//Le controleur renvoi '0' lorsqu'on arrete d'appuyer sur une touche

void move(char order){
  if(order == 'F' || order == 'T'){
    valG=Speed;
    valD=Speed;
    ledcWrite(pwmChannelG, valG);
    ledcWrite(pwmChannelD, valD);
    LC=order;
    LCC=order;
  }
  if(order == 'R'){
    digitalWrite(dirD,0);
    valG=Speed;
    valD=Speed;
    ledcWrite(pwmChannelG, valG);
    ledcWrite(pwmChannelD, valD);
    LC=order;
    LCC=order;
  }
   if(order == 'L'){
    digitalWrite(dirG,1);
    valG=Speed;
    valD=Speed;
    ledcWrite(pwmChannelG, valG);
    ledcWrite(pwmChannelD, valD);
    LC=order;
    LCC=order;
  }
   if(order == 'C'){
    valG+=SpeedTurn;
    ledcWrite(pwmChannelG, valG);
    ledcWrite(pwmChannelD, valD);
    LCC=LC;
    LC=order;
    
  }
   if(order == 'S'){
     valD+=SpeedTurn;
    ledcWrite(pwmChannelG, valG);
    ledcWrite(pwmChannelD, valD);
    LCC=LC;
    LC=order;
  }

  if(order == 'B' or order == 'X'){
    digitalWrite(dirG,1);
    digitalWrite(dirD,0);
    valG=Speed;
    valD=Speed;
    ledcWrite(pwmChannelG, valG);
    ledcWrite(pwmChannelD, valD);
    LC=order;
    LCC=order;
  }

  if(order == '0'){
    resetLast();
    LC=order;
    
  }

  Serial.write(order);

}
// Fonction qui reset les valeurs en fonction de la valeur recu avant la valeur '0'
void resetLast(){
  switch(LC){
    case 'F':
      valG=0;
      valD=0;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;

    case 'T':
      valG=0;
      valD=0;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;
      
    case 'C':
      valG-=SpeedTurn;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;

    case 'S':
      valD-=SpeedTurn;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;

    case '0':
      digitalWrite(dirG,0);
      digitalWrite(dirD,1);
      valG=0;
      valD=0;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;

    case 'R':
      digitalWrite(dirG,0);
      digitalWrite(dirD,1);
      valG=0;
      valD=0;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;

    case 'L':
      digitalWrite(dirG,0);
      digitalWrite(dirD,1);
      valG=0;
      valD=0;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;
    
    case 'B':
      digitalWrite(dirG,0);
      digitalWrite(dirD,1);
      valG=0;
      valD=0;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;

    case 'X':
      digitalWrite(dirG,0);
      digitalWrite(dirD,1);
      valG=0;
      valD=0;
      ledcWrite(pwmChannelG, valG);
      ledcWrite(pwmChannelD, valD);
      break;

    default : 
      break;
  }


}
