//  Copyright (c) 2019 Antoine Tran Tan
//

#include "mbed.h"
#include "NBoard.h"

IHM ihm;

DigitalOut Led0(PB_3);
DigitalOut Led1(PA_7);
DigitalOut Led2(PA_6);
DigitalOut Led3(PA_5);
DigitalOut Led4(PA_3);
DigitalOut Led5(PA_1);
DigitalOut Led6(PA_0);
DigitalOut Led7(PA_2);

BusOut Bus8Led(PB_3, PA_7, PA_6, PA_5, PA_3, PA_1, PA_0, PA_2);

DigitalIn BP0(PA_9, PullUp);
DigitalIn BP1(PA_10, PullUp);
DigitalIn BP2(PB_0, PullUp);
//DigitalIn BP3(PB_7, PullUp);

BusOut BusSelectMux(PA_8, PF_1, PF_0);

AnalogIn AnaIn(PB_1);

