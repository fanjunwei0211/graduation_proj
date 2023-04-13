#ifndef MOTOR_TASK_H
#define MOTOR_TASK_H

extern char isStraight;
extern int speedLeft,speedRight;

float AngleCorrect(float ang);
void DirectionSet(char mode);
void SpeedSet(int left, int right);
void ShutMotorDown(void);


#endif  
