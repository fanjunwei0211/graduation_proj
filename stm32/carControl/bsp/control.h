#ifndef __CONTROL_H
#define __CONTROL_H

#include "string.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "JY901.h"
#include "nimotion.h"
#include "laser.h"
#include "pid.h"

extern float yaw;
extern char isStraight;
extern pid_type_def angControl_pid;


void HWT_init();
void SpeedSet(int left, int right);
float AngleCorrect(float ang);
 
#endif
