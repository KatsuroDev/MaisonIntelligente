/*
 * Programme de gestion de la maison connectée
 *
 * Alexis Millette
 */

#include <Servo.h>              // Gestion des servos-moteurs
#include <Wire.h>               // Gestion de la connection I2C
#include <LiquidCrystal_I2C.h>  // Gestion de l'écran LCD avec I2C

// Adresse de communication avec l'écran I2C est 0x27, 16 caractères par ligne, 2 lignes au total
LiquidCrystal_I2C mylcd(0x27, 16, 2);

// Instances des servos-moteurs
Servo servoFenetre;
Servo servoPorte;

const int mouvementPin = 2; // Pin du détecteur de mouvement
const int buzzerPin = 3;
const int boutonGauchePin = 4;
const int ledInterieurPin = 5;
const int fanSensHoraire = 6;
const int fanSensAntiHoraire = 7;
const int boutonDroitPin = 8;
const int servoFenetrePin = 9;
const int servoPortePin = 10;
const int relaiPin = 12;
const int ledExterieurPin = 13;
const int gazPin = A0;      // Pin du détecteur analogue de gaz
const int lumierePin = A1;  // Pin du détecteur analogue de lumière
const int terrePin = A2;    // Pin du détecteur analogue d'humidité dans la terre
const int pluiePin = A3;    // Pin du détecteur analogue de pluie


const int gazMax = 20;      // Valeur nécessaire pour déclancher l'alarme de gaz
const int gazMin = 15;      // Valeur nécessaire pour éteindre l'alarme de gaz
bool gazFlag = false;       // Indicateur de présence de gaz
bool gazAlarme = false;     // État de l'alarme de gaz

const int lumiereMax = 600; // Valeur indiquant le jour
const int lumiereMin = 200; // Valeur indiquant la nuit
bool lumiereFlag = false;   // Indicateur qu'on doit vérifier le mouvement
bool mouvementFlag = false; // Indicateur de présence de mouvement
bool mouvementAlarme = false; // État de l'alarme de mouvement

const int pluieMax = 500; // Valeur indiquant qu'il y a de la pluie
const int pluieMin = 150; // Valeur indiquant que le toit est sec
bool pluieFlag = false;   // Indicateur de présence de pluie
bool pluieAlarme = false; // État de l'alarme de pluie

const int terreMax = 130; // Valeur indiquant que la terre est humide
const int terreMin = 20;  // Valeur indiquant que la terre est sèche
bool terreFlag = false;   // Indicateur que la terre a soif
bool terreAlarme = false; // État de l'alarme de terre

void setup() {
  Serial.begin(9600);       // Activer la console

  mylcd.init();             // Initialiser l'écran LCD
  mylcd.backlight();        // Allumer l'écran LCD
  mylcd.setCursor(0, 0);    // Placer le curseur à la première colonne, première rangée
  mylcd.print("Bienvenue!");

  servoFenetre.attach(servoFenetrePin); // Attache la pin du servo à l'instance
  servoPorte.attach(servoPortePin);
  servoFenetre.write(0);                // Met le servo-moteur à la position 0°
  servoPorte.write(0);

  pinMode(mouvementPin, INPUT);     // Met les pins en mode lecture
  pinMode(boutonGauchePin, INPUT);
  pinMode(boutonDroitPin, INPUT);
  pinMode(gazPin, INPUT);
  pinMode(lumierePin, INPUT);
  pinMode(terrePin, INPUT);
  pinMode(pluiePin, INPUT);
  
  pinMode(buzzerPin, OUTPUT);       // Met les pins en mode écriture
  pinMode(ledInterieurPin, OUTPUT);
  pinMode(ledExterieurPin, OUTPUT);
  pinMode(fanSensHoraire, OUTPUT);
  pinMode(fanSensAntiHoraire, OUTPUT);
  pinMode(relaiPin, OUTPUT);


  Serial.println(F("Maison Alexis Millette - Fin du Setup"));

}

void loop() {
  traiterDetecteurs();
}

void traiterDetecteurs() {
  checkGas();
  checkPluie();
  checkTerre();
  checkLumiere();
  if (lumiereFlag) 
    checkMouvement();
}

void checkGas() {
  int gaz = analogRead(gazPin);
  if (gaz > gazMax) {
    gazFlag = true;
  } else if (gaz < gazMin) {
    gazFlag = false;
  }
}

void checkLumiere() {
  int lumiere = analogRead(lumierePin);
  if (lumiere < lumiereMin) {
    lumiereFlag = true;
  } else if (lumiere > lumiereMax) {
    lumiereFlag = false;
    mouvementFlag = false;
  }
}

void checkMouvement() {
  bool mouvement = digitalRead(mouvementPin);
  if (mouvement) {
    mouvementFlag = true;
  } else {
    mouvementFlag = false;
  }
}

void checkPluie() {
  int pluie = analogRead(pluiePin);
  if (pluie > pluieMax) {
    pluieFlag = true;
    Serial.println("Y MOUILLE"); // move
    servoFenetre.write(180); // move
  } else if (pluie < pluieMin) {
    pluieFlag = false;
  }
}

void checkTerre() {
  int terre = analogRead(terrePin);
  if (terre > terreMin) {
    terreFlag = true;
  } else if (terre < terreMax) {
    terreFlag = false;
  }
}

void activerAlarmes() {
  if (gazFlag && !gazAlarme) {
    gazAlarme = true;
  } else if (!gazFlag && gazAlarme) {
    gazAlarme = false;
  }

  if (mouvementFlag && !mouvementAlarme) {
    mouvementAlarme = true;
  } else if (!mouvementFlag && mouvementAlarme) {
    mouvementAlarme = false;
  }

  if (pluieFlag && !pluieAlarme) {
    pluieAlarme = true;
  } else if (!pluieFlag && pluieAlarme) {
    pluieAlarme = false;
  }

  if (terreFlag && !terreAlarme) {
    terreAlarme = true;
  } else if (!terreFlag && terreAlarme) {
    terreAlarme = false;
  }
}


void traiterAlarmes() {
  
}






















