#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "mbed.h"
#include "ihm.h"
#include "NBoard.h"
#include <string.h>

#define Cr          TabCapt[0]
#define Ceg         TabCapt[0]
#define Cg          TabCapt[0]
#define Cd          TabCapt[0]
#define Ced         TabCapt[0]

extern DigitalIn    JACK;
extern DigitalIn    BP;

extern BusOut       Bus5Led;

extern PwmOut       MotG;
extern PwmOut       MotD;

extern Timer        chronometre;

extern bool         TabCapt[5];
extern float        Vpot, Vg, Vd, Vseuil, Vmoy, Correction;
extern bool         Run, Raccourci;
extern float        coef1, coef2, coef3;

typedef enum {init, run, stop} T_automBP;
typedef enum {td, corG, corD, virG, virD, sorG, sorD} T_automSuivi;
typedef enum {idle, mark, attente, raccourci, virage, timeout} T_raccourci;

void automateBP (void);
void lireCapteur (void);
void suiviLigne (void);
void automateRaccourci (void);

void testCapteur (void);
void testMoteur (void);

float lireCanal (int canal);
void afficherMessage (char *msg, bool start);

#endif
