#include "mbed.h"
#include "IHM.h"
#include "NBoard.h"
#include "Robot.h"

BusOut Bus5Led(PB_3, PA_7, PA_6, PA_5, PA_3); 

DigitalIn JACK(PB_6);
DigitalIn BP(PB_7);

PwmOut MotG (PB_5);
PwmOut MotD (PB_4);

Timer chronometre;

bool  Run = false, Raccourci = false;
bool  TabCapt[5];
float Vpot, Vg = 0.0f, Vd = 0.0f, Vseuil = 0.5f, Vmoy = 0.0f, Correction = 0.0f;
float coef1 = sqrt((80.0f + 11.5f)/(80.0f - 11.5f)); 
float coef2 = sqrt((50.0f + 11.5f)/(50.0f - 11.5f)); 
float coef3 = sqrt((30.0f + 11.5f)/(30.0f - 11.5f)); 



void automateBP () {

  static T_automBP etat = init;

  switch (etat) {
    case init :
      Run = false;
      if (!JACK) {
        etat = run;
        Vmoy = Vpot;
        chronometre.start();
      }
    break;

    case run :
      Run = true;
      if (BP) {
        etat = stop;
        chronometre.stop();
      }
    break;

    case stop :
      Run = false;
      if (JACK) {
        etat = init;
        chronometre.reset();
      }
    break;
  }
}



void lireCapteur() {
  static int busCapteur[5]={0, 1, 2, 3, 4};
  float VCapteur;
  int leds = 0;

  Vpot = lireCanal(7);
  Led7 = JACK;
  Led6 = BP;
  Led5 = Run;

  for (int i=0 ; i<5 ; i++) {
    VCapteur = lireCanal (busCapteur[i]);
    TabCapt[i] = (VCapteur>Vseuil);
    leds += (TabCapt[i]<<i);
  }
  Bus5Led = leds;
}



void suiviLigne () {
  static T_automSuivi etat = td;

  float Vgcor = Vmoy + Correction;
  float Vdcor = Vmoy - Correction;
  
  if (Raccourci == false) {
    switch (etat) {
      case td :
        Vg = Vgcor;
        Vd = Vdcor;
        if (!Ced && !Cd &&  Cg && !Ceg) etat = corG;
        if (!Ced &&  Cd && !Cg && !Ceg) etat = corD;
        ihm.BAR_set (0x030);
      break;
      case corD :
        Vg = Vgcor*coef1;
        Vd = Vdcor/coef1;
        if (!Ced &&  Cd &&  Cg && !Ceg) etat = td;
        if (!Ced && !Cd &&  Cg && !Ceg) etat = corG;
        if ( Ced && !Cd && !Cg && !Ceg) etat = virD;
        ihm.BAR_set (0x00C);
      break;
      case corG :
        Vg = Vgcor/coef1;
        Vd = Vdcor*coef1;
        if (!Ced &&  Cd &&  Cg && !Ceg) etat = td;
        if (!Ced &&  Cd && !Cg && !Ceg) etat = corD;
        if (!Ced && !Cd && !Cg &&  Ceg) etat = virG;
        ihm.BAR_set (0x0C0);
      break;
      case virD :
        Vg = Vgcor*coef2;
        Vd = Vdcor/coef2;
        if (!Ced && !Cd && !Cg && !Ceg) etat = sorD;
        if (!Ced &&  Cd && !Cg && !Ceg) etat = corD; 
        ihm.BAR_set (0x006);
      break;
      case virG :
        Vg = Vgcor/coef2;
        Vd = Vdcor*coef2;
        if (!Ced && !Cd && !Cg && !Ceg) etat = sorG;
        if (!Ced && !Cd && !Cg &&  Ceg) etat = corG; 
        ihm.BAR_set (0x180);
      break;
      case sorD :
        Vg = Vgcor*coef3;
        Vd = Vdcor/coef3;
        if ( Ced && !Cd && !Cg && !Ceg) etat = virD;
        ihm.BAR_set (0x300);
      break;
      case sorG :
        Vg = Vgcor/coef3;
        Vd = Vdcor*coef3;
        if (!Ced && !Cd && !Cg &&  Ceg) etat = virG;
        ihm.BAR_set (0x003);
      break;
    } 
  } else etat = td;
}



void automateRaccourci (void) {
  static T_raccourci etat = idle;
  float temps = 0.0f;

  switch (etat) {
    case idle :
      if (Cr)   etat = mark;
      if (Ced)  etat = timeout;
      temps = chronometre.read();
      Raccourci = false;
    break;
    case mark :
      if (!Cr)  etat = attente;
      if (Ced)  etat = timeout;
      if (chronometre.read() > (temps + 0.5f)) etat = idle;
    break;
    case attente :
      if (Cr)   etat = raccourci;
      if (Ced)  etat = timeout;
      if (chronometre.read() > (temps + 1.0f)) etat = idle;
    break;
    case raccourci :
      if (Ced)  etat = virage;
      Raccourci = true;
    break;
    case virage :
      if (Cg)   etat = idle;
    break;
    case timeout :
      if (chronometre.read() > (temps + 1.0f)) etat = idle;
    break;
  }
}



void testMoteur (){
  static bool fin = false;
  static char message[]="Test  Moteur  / Vpot = Correction / BP2 : Vpot = Vmoy / BP1 + BP3 : Sortir / BP0 + BP1 + BP3 : Test capteurs";
  float vitG = 0, vitD = 0;
  
  ihm.LCD_clear();
  afficherMessage(message, true);
  ThisThread::sleep_for(200ms);

  while (!fin) {
    Vpot = lireCanal(7);

    automateBP();
    afficherMessage(message, false);

    if (BP2) {
      Correction = (2*Vpot-1)/10.0f;
      ihm.LCD_gotoxy(1,0);
      ihm.LCD_printf("CORR = %5.3f",Correction);
    } else {
      Vmoy = Vpot;
      ihm.LCD_gotoxy(1,0);
      ihm.LCD_printf("VMOY = %5.3f",Vmoy);
    }

    vitG = Vmoy + Correction;
    if (vitG > 1.0f) vitG = 1.0f;
    if (vitG < 0.0f) vitG = 0.0f;
    vitD = Vmoy - Correction;
    if (vitD > 1.0f) vitD = 1.0f;
    if (vitD < 0.0f) vitD = 0.0f;

    if (Run) {
      MotG = vitG;
      MotD = vitD;
    } else {
      MotG = 0.0f;
      MotD = 0.0f;
    }

    ThisThread::sleep_for(200ms);

    if ((BP1==0) && (BP==0)) {
      fin = true;
    }
  }
}



void testCapteur(){
  static bool fin = false;
  static char message[]="Test Capteur  / Vpot = Seuil / BP2 = Sortir / BP0 = Vcapteur";
  static int busCapteur[5]={0, 1, 2, 3, 4};

  int leds = 0;
  float vcapteur;
  float capteur[5];

  ihm.LCD_clear();
  afficherMessage (message, true);
  ThisThread::sleep_for(200ms);
  
  while (!fin) {
    Vseuil = lireCanal(7);

    Led7 = JACK;
    Led6 = BP;
    Led5 = Run;

    for (int i=0 ; i<5 ; i++) {
        vcapteur = lireCanal (busCapteur[i]);
        TabCapt[i] = (vcapteur>Vseuil);
        leds += (TabCapt[i]<<i);
        capteur[i] = vcapteur;
    }
    Bus5Led = leds;

    if (BP0) {
      afficherMessage (message, false);
      ihm.LCD_gotoxy(1,0);
      ihm.LCD_printf("Seuil = %5.3f   ",Vseuil);
    } else {
      ihm.LCD_clear();
      for (int i=0 ; i<5 ; i++) {
        ihm.LCD_gotoxy(i/3,(i%3)*5);
        ihm.LCD_printf("%4.2f",capteur[i]);
      }
    }
    ThisThread::sleep_for(200ms);
    
    if (BP2==0) {
      fin = true;
    }
  }
}



float lireCanal(int canal){
  BusSelectMux = canal;
  wait_us(1);
  return (AnaIn.read());
}



void afficherMessage (char *msg, bool start){
  static int index = 0;

  char chaine[17];
  int i, taille;

  strcat (msg," - ");
  taille = strlen(msg);

  if (start) index = 0;

  for (i=0; i<16; i++) chaine[i] = msg[(i+index)%(taille-1)];
  chaine[16]='\0';
  ihm.LCD_gotoxy(0,0);
  ihm.LCD_printf("%s",chaine);
  index += 1;
}
