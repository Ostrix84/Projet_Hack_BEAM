const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
int X0,Y0,x,y,Xmap,Ymap;

void setup() {
  // Initialise la broche 9 en tant que sortie PWM
  initTimer1_7800Hz();
  // Baudrate pour l'affichage
  Serial.begin(9600);
  initPin();
  initReadJoy();
  move_joystick(0,0);

}

void initTimer1_7800Hz(){ // Regler la PWM
  TCCR1A = TCCR1A & 0xe0 | 1;
  TCCR1B = TCCR1B & 0xe0 | 0x0a;
}

void initReadJoy(){ // Initialisation du joystick (joystick au centre)
  X0=analogRead(X_pin);
  Y0=analogRead(Y_pin);
  x=0;
  y=0;
}

void initPin(){ // Configuration des PINs
  pinMode(10, OUTPUT); // L wheel
  pinMode(9, OUTPUT); // R wheel
  pinMode(8, OUTPUT); // DIR Droite
  pinMode(7, OUTPUT); // DIR Gauche
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  digitalWrite(8, 1); //Dir at 1 (default)
}

void move_joystick(int8_t valueX, int8_t valueY){ // Value_max = 15.5% * 255/100 = 40, value entre -40 et 40
  if (valueX < 0 && valueX >= -40) { //En arriere
    Serial.print("Dir inverser\n");
    digitalWrite(8, 0); //Dir a 0 pour inverser le sens de rotation
    digitalWrite(7, 1); //Dir at 1 (default)
    valueX = -valueX;
  }
  else if (valueX > 0 && valueX <= 40) { //En avant
    digitalWrite(8, 1); //Dir a 1
    digitalWrite(7, 0); //Dir a 0
  }
  else if (valueX==0){ // Rotation (deux sens possibles)
    if (valueY < 0){
      valueX = -valueY/2;
    }
    else {
      valueX = valueY/2;
    }
  }
  if (valueY < 0 && valueY >= -40) { // Right wheel faster
    valueY = -valueY;
    if (valueX - valueY/2 < 0){
      analogWrite(10,0);
    }
    else {
      analogWrite(10, valueX - valueY/2);
    }
    analogWrite(9, valueX + valueY/2);
  }
  else if (valueY <= 40){ // Left wheel faster
    analogWrite(10, valueX + valueY/2);
    if (valueX - valueY/2 < 0){
      analogWrite(9, 0);
    }
    else {
      analogWrite(9, valueX - valueY/2);
    }
  }
}

void move_affiche(int x, int y){ // Retour des commandes sur l'écran
    Serial.print("\n");
    Serial.print("X-axis: ");
    Serial.print(x);
    Serial.print("\n");
    Serial.print("Y-axis: ");
    Serial.println(y);
    Serial.print("\n\n");
}

void readXY(){ // Lire valeur analogique du Joystick
  if(x!=analogRead(X_pin)-X0 || y!=analogRead(Y_pin)-Y0){ // Màj de la position seulement quand le joystick bouge
    x=analogRead(X_pin)-X0;
    y=analogRead(Y_pin)-Y0;
    
    Xmap=map(x,-512,512,-40,40); // Mise a l'echelle des valeurs dans nos plages
    Ymap=map(y,-512,512,-40,40);
    move_affiche(Xmap,Ymap); // Visualisation de la commande
    move_joystick(Xmap,Ymap); // Maj des rapports cycliques de la PWM
  }
}

// Boucle d'utilisation
void loop() {
  readXY();
  delay(1);
}
