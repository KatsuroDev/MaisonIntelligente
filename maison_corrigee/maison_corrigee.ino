/*
 * Programme de gestion de la maison connectée
 *
 * Alexis Millette
 */

#include <Servo.h>              // Gestion des servos-moteurs
#include <Wire.h>               // Gestion de la connection I2C
#include <LiquidCrystal_I2C.h>  // Gestion de l'écran LCD avec I2C

// Adresse de communication avec l'écran I2C est 0x27, 16 caractères par ligne, 2 lignes au total
LiquidCrystal_I2C mylcd(0x27, 16, 2)

// Instances des servos-moteurs
Servo servoPorte;
Servo servoFenetre;

