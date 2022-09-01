#include "Control.h"

void Stop(void) {
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare4(TIM1, 0);
}

void Go_Ahead(void) {
    Motor1_SetDirct(1, 0);
    Motor2_SetDirct(1, 0);

    TIM_SetCompare1(TIM1, 200);
    TIM_SetCompare4(TIM1, 200);
}

void Go_Back(void) {
    Motor1_SetDirct(0, 1);
    Motor2_SetDirct(0, 1);

    TIM_SetCompare1(TIM1, 200);
    TIM_SetCompare4(TIM1, 200);
}

void Turn_Left(void) {

}

void Turn_Right(void) {

}
