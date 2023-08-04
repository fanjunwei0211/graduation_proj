#ifndef MOTOR_TASK_H
#define MOTOR_TASK_H

#define MotorBufLen 15

extern char isStraight;
extern int speedLeft,speedRight,speedW,speedF;
extern unsigned char	MotorRxBuffer[];

float AngleCorrect(float ang);
void DirectionSet(char mode);
void SpeedSet(int left, int right);
void ShutMotorDown(void);
void CarInit(void);
void StopMotor(void);


#endif  
