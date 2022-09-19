/*
 * Programme de gestion de la maison connectée
 *
 * Alexis Millette
 */

#include <Servo.h>              // Gestion des servos-moteurs
#include <Wire.h>               // Gestion de la connection I2C
#include <LiquidCrystal_I2C.h>  // Gestion de l'écran LCD avec I2C
#include <SoftwareSerial.h>     // Gestion du port série virtuel

// Adresse de communication avec l'écran I2C est 0x27, 16 caractères par ligne, 2 lignes au total
LiquidCrystal_I2C mylcd(0x27, 16, 2);

// Instances des servos-moteurs
Servo servoFenetre;
Servo servoPorte;

SoftwareSerial Mega(3, 2); // Instantiation d'un port série (Rx, Tx) (Vert, Jaune)

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
const int gazAlarmeTag = 1; // Tag pour l'alarme de gaz dans la pile d'alarme

const int lumiereMax = 600; // Valeur indiquant le jour
const int lumiereMin = 200; // Valeur indiquant la nuit
bool lumiereFlag = false;   // Indicateur qu'on doit vérifier le mouvement
bool mouvementFlag = false; // Indicateur de présence de mouvement
bool mouvementAlarme = false; // État de l'alarme de mouvement
const int mouvementAlarmeTag = 2; // Tag pour l'alarme de mouvement dans la pile d'alarme

const int pluieMax = 500; // Valeur indiquant qu'il y a de la pluie
const int pluieMin = 150; // Valeur indiquant que le toit est sec
bool pluieFlag = false;   // Indicateur de présence de pluie
bool pluieAlarme = false; // État de l'alarme de pluie
const int pluieAlarmeTag = 3; // Tag pour l'alarme de pluie dans la pile d'alarme

const int terreMax = 130; // Valeur indiquant que la terre est humide
const int terreMin = 20;  // Valeur indiquant que la terre est sèche
bool terreFlag = false;   // Indicateur que la terre a soif
bool terreAlarme = false; // État de l'alarme de terre
const int terreAlarmeTag = 4; // Tag pour l'alarme d'humidité de la terre dans la pile d'alarme

bool alarmesCourantes[] = {false, false, false, false};
bool alarmesHistorique[] = {false, false, false, false};

void setup() {
  Serial.begin(9600);       // Activer la console
  Mega.begin(9600);         // Activer le port série pour la communication avec la maison

  mylcd.init();             // Initialiser l'écran LCD
  mylcd.backlight();        // Allumer l'écran LCD
  mylcd.setCursor(0, 0);    // Placer le curseur à la première colonne, première rangée
  mylcd.print("Bienvenue!");

  servoFenetre.attach(servoFenetrePin); // Attache la pin du servo à l'instance
  servoPorte.attach(servoPortePin);
  servoFenetre.write(0);                // Met le servo-moteur à la position 0°
  servoPorte.write(0);

  //pinMode(mouvementPin, INPUT);     // Met les pins en mode lecture
  pinMode(boutonGauchePin, INPUT);
  pinMode(boutonDroitPin, INPUT);
  pinMode(gazPin, INPUT);
  pinMode(lumierePin, INPUT);
  pinMode(terrePin, INPUT);
  pinMode(pluiePin, INPUT);
  
  //pinMode(buzzerPin, OUTPUT);       // Met les pins en mode écriture
  pinMode(ledInterieurPin, OUTPUT);
  pinMode(ledExterieurPin, OUTPUT);
  pinMode(fanSensHoraire, OUTPUT);
  pinMode(fanSensAntiHoraire, OUTPUT);
  pinMode(relaiPin, OUTPUT);


  Serial.println(F("Maison Alexis Millette - Fin du Setup"));

}

void loop() {
  traiterDetecteurs();
  activerAlarmes();
  traiterAlarmes();
  traiterCommandes();
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
  int gaz = analogRead(gazPin); // Lit la valeur de gaz
  if (gaz > gazMax) {
    gazFlag = true;             // Indique que l'alarme de gaz doit s'allumer
  } else if (gaz < gazMin) {
    gazFlag = false;            // Indique que l'alarme de gaz doit s'éteindre
  } 
}

void checkLumiere() {
  int lumiere = analogRead(lumierePin); // Lit la valeur de lumière
  if (lumiere < lumiereMin) {
    lumiereFlag = true;                 // Indique qu'on doit vérifier le détecteur de mouvement
  } else if (lumiere > lumiereMax) {
    lumiereFlag = false;
    mouvementFlag = false;              // Si c'est le jour, on ne vérifie plus le détecteur de mouvement
  }
}

void checkMouvement() {
  bool mouvement = digitalRead(mouvementPin); // Lit la valeur du détecteur de mouvement
  if (mouvement) {
    mouvementFlag = true;                     // Indique que l'alarme de mouvement doit s'allumer
  } else {
    mouvementFlag = false;                    // Indique que l'alarme de mouvement doit s'allumer
  }
}

void checkPluie() {
  int pluie = analogRead(pluiePin);           // Lit la valeur de pluie
  if (pluie > pluieMax) {
    pluieFlag = true;                         // Indique que l'alarme de pluie doit s'allumer
  } else if (pluie < pluieMin) {
    pluieFlag = false;                        // Indique que l'alarme de pluie doit s'éteindre
  }
}

void checkTerre() {
  int terre = analogRead(terrePin);           // Lit la valeur d'humidité de la terre
  if (terre > terreMin) {
    terreFlag = true;                         // Indique que l'alarme d'humidité doit s'allumer
  } else if (terre < terreMax) {
    terreFlag = false;                        // Indique que l'alarme d'humidité doit s'éteindre
  } 
}

void activerAlarmes() {
  if (gazFlag && !gazAlarme) {                    
    gazAlarme = true;                            // Allume l'alarme si elle ne l'est pas déjà
    digitalWrite(fanSensAntiHoraire, HIGH);      // Allume la fan dans le sens anti-horaire
    digitalWrite(fanSensHoraire, LOW);
    servoFenetre.write(0);                       // Ouvre la fenêtre
    ajouterAlarmeCourante(gazAlarmeTag);
    ajouterAlarmeHistorique(gazAlarmeTag);
  } else if (!gazFlag && gazAlarme) {
    gazAlarme = false;                           // Ferme l'alarme si elle ne l'est pas déjà
    digitalWrite(fanSensAntiHoraire, LOW);       // Ferme la fan
    digitalWrite(fanSensHoraire, LOW);
    enleverAlarmeCourante(gazAlarmeTag);
  }

  if (mouvementFlag && !mouvementAlarme) {
    mouvementAlarme = true;
    ajouterAlarmeCourante(mouvementAlarmeTag);
    ajouterAlarmeHistorique(mouvementAlarmeTag);
  } else if (!mouvementFlag && mouvementAlarme) {
    mouvementAlarme = false;
    enleverAlarmeCourante(mouvementAlarmeTag);
  }

  if (lumiereFlag) {
    digitalWrite(ledExterieurPin, HIGH);       // Allume/Éteint la lumière extérieur s'il fait noir dehors
  } else {
    digitalWrite(ledExterieurPin, LOW);
  }

  if (pluieFlag && !pluieAlarme) {
    pluieAlarme = true;
    Serial.println("Y MOUILLE");
    servoFenetre.write(180); // Ferme la fenêtre
    ajouterAlarmeCourante(pluieAlarmeTag);
    enleverAlarmeHistorique(pluieAlarmeTag);
  } else if (!pluieFlag && pluieAlarme) {
    pluieAlarme = false;
    enleverAlarmeCourante(pluieAlarmeTag);
  }

  if (terreFlag && !terreAlarme) {
    terreAlarme = true;
    ajouterAlarmeCourante(terreAlarmeTag);
    enleverAlarmeHistorique(terreAlarmeTag);
  } else if (!terreFlag && terreAlarme) {
    terreAlarme = false;
    enleverAlarmeCourante(terreAlarmeTag);
  }
}


void traiterAlarmes() {
  if (gazAlarme) {
    beep(1);
  }
  if (mouvementAlarme) {
    beep(2);
  }
  if (pluieAlarme) {
    beep(3);
  }
  if (terreAlarme) {
    beep(4);
  }
}

void beep(int x) {
  for (int i = 0; i < x; i++) {
    tone(buzzerPin, 440, 100);  // Fait jouer la fréquence 440 pendant 100 millisecondes
    noTone(buzzerPin);          // Éteint le buzzer
    delay(50);
  }
}

void ajouterAlarmeCourante(int alarme)
{
  alarmesCourantes[alarme - 1] = true; // Active l'alarme dans la pile
}

void enleverAlarmeCourante(int alarme)
{
  alarmesCourantes[alarme - 1] = false; // Désactive l'alarme dans la pile
}

void ajouterAlarmeHistorique(int alarme)
{
  alarmesHistorique[alarme - 1] = true;
}

void enleverAlarmeHistorique(int alarme)
{
  alarmesHistorique[alarme - 1] = false;
}






void traiterCommandes() {
  int commande = 0; // Valeur de la commande reçu sur le port série, par la console ou le bluetooth

  if (Mega.available())
  {
    commande = Mega.read();
    Serial.print("Du Mega: ");
    Serial.println(commande);
  }

  if (Serial.available() > 0)
  {
    commande = Serial.read();
    Mega.write(commande);
    String valeurString;
    int angle;
    int intensite;
    switch (commande) {
      case 't':
        valeurString = Serial.readStringUntil('#'); // Lit la valeur avant le #
        angle = String(valeurString).toInt();          // Transforme la valeur en integer
        servoFenetre.write(angle);
        delay(300);
        break;
      case 'u':
        valeurString = Serial.readStringUntil('#');
        angle = String(valeurString).toInt();
        servoPorte.write(angle);
        delay(300);
        break;
      case 'v':
        valeurString = Serial.readStringUntil('#');
        intensite = String(valeurString).toInt();
        analogWrite(ledInterieurPin, intensite);
        break;
      case 'w':
        valeurString = Serial.readStringUntil('#');
        intensite = String(valeurString).toInt();
        digitalWrite(fanSensAntiHoraire, LOW);
        analogWrite(fanSensHoraire, intensite);
        break;

    }
  } // Pourquoi les 2 switchs sont séparés?? Je vais les laisser comme ça pour l'instant



  switch (commande) {
    case 'a':
      digitalWrite(ledExterieurPin, HIGH);
      break;
    case 'b':
      digitalWrite(ledExterieurPin, LOW);
      break;
    case 'c':
      digitalWrite(relaiPin, HIGH);
      break;
    case 'd':
      digitalWrite(relaiPin, LOW);
      break;
    case 'e':
      // Musique
      break;
    case 'f':
      // Musique 2
      break;
    case 'g':
      noTone(buzzerPin);
      break;
    case 'h':
      Serial.println(analogRead(lumierePin)); // Imprime la valeur du détecteur de lumière
      delay(100);
      break;
    case 'i':
      Serial.println(analogRead(gazPin)); // Imprime la valeur du détecteur de gaz
      delay(100);
      break;
    case 'j':
      Serial.println(analogRead(terrePin)); // Imprime la valeur du détecteur d'humidité de la terre
      delay(100);
      break;
    case 'k':
      Serial.println(analogRead(pluiePin)); // Imprime la valeur du détecteur de pluie
      delay(100);
      break;
    case 'l':
      servoFenetre.write(180); // Fermer la fenêtre
      delay(500);
      break;
    case 'm':
      servoFenetre.write(0); // Ouvrir la fenêtre
      delay(500);
      break;
    case 'n':
      servoPorte.write(180); // Fermer la porte
      delay(500);
      break;
    case 'o':
      servoPorte.write(0); // Ouvrir la porte
      delay(500);
      break;
    case 'p':
      digitalWrite(ledInterieurPin, HIGH);
      break;
    case 'q':
      digitalWrite(ledInterieurPin, LOW);
      break;
    case 'r':
      digitalWrite(fanSensAntiHoraire, LOW); // Allume la fan dans le sens anti-horaire
      digitalWrite(fanSensHoraire, HIGH);
      break;
    case 's':
      digitalWrite(fanSensAntiHoraire, LOW); // Ferme la fan
      digitalWrite(fanSensHoraire, LOW);
      break;
  }
}










