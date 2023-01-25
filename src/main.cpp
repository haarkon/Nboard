#include <mbed.h>
#include "IHM.h"
#include "NBoard.h"
#include "Robot.h"

int main() {

  int nbLeds;

  MotG.period_us(50);
  MotD.period_us(50);
  MotG = Vg;
  MotD = Vd;

  ihm.BAR_set (0x000);
  ihm.LCD_clear();
  ihm.LCD_gotoxy(0,0);
  ihm.LCD_printf("MOT  : BP0 + BP1");
  ihm.LCD_gotoxy(1,0);
  ihm.LCD_printf("CAPT : BP0 + BP3");

  chronometre.reset();
  chronometre.start();
  while(chronometre.read()<2.0f){
    if ((BP0==0) && (BP1==0)) testMoteur();
    if ((BP0==0) && (BP ==0)) testCapteur();
    nbLeds = 1+(int)(chronometre.read()/0.25f);
    Bus8Led = (1<<nbLeds)-1;
  }

  chronometre.stop();
  chronometre.reset();

  while(1) {

    lireCapteur();
    automateBP();
    suiviLigne();
    automateRaccourci();

    if (Run) {
      if (Raccourci) {
        MotG = 0.0f;
        MotD = Vmoy;
      } else {
        MotG = Vg;
        MotD = Vd;
      }
    } else {
      MotG = 0.0f;
      MotD = 0.0f;
      // Affichage ligne 1 = temps
      ihm.LCD_gotoxy(0,0);
      ihm.LCD_printf ("%6.2f",chronometre.read());
      // Affichage ligne 2 = Vpot
      ihm.LCD_gotoxy(1,0);
      ihm.LCD_printf ("%5.3f",Vpot);
    }
  }
}
