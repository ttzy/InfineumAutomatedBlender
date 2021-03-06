#include <stdlib.h>     /* abs */
#include "Stepper.h"
#include "TypeDefines.h"
#include "TimerMgrHeader.h"
#include "TimerAPI.h"
Stepper* Stepper::stepper_motor = NULL;
Stepper::Stepper(){
	exportPin(STEPPER_MS1_PIN);
	exportPin(STEPPER_MS2_PIN);
	exportPin(STEPPER_STEP_PIN);
	exportPin(STEPPER_SLP_PIN);
	exportPin(STEPPER_DIR_PIN);
	setPinDir(STEPPER_MS1_PIN,OUT);
	setPinDir(STEPPER_MS2_PIN,OUT);
	setPinDir(STEPPER_STEP_PIN,OUT);
	setPinDir(STEPPER_SLP_PIN,OUT);
	setPinDir(STEPPER_DIR_PIN,OUT);
	setPin(STEPPER_SLP_PIN,1);
	inUse=0;
	Proximity temp;
	disSensor=temp;
	stepper_motor = this;
	setMode(0);//FULLSTEP
}
Stepper::~Stepper(){
	unexportPin(STEPPER_MS1_PIN);
	unexportPin(STEPPER_MS2_PIN);
	unexportPin(STEPPER_STEP_PIN);
	unexportPin(STEPPER_SLP_PIN);
	unexportPin(STEPPER_DIR_PIN);

	stepper_motor = NULL;

}
void Stepper::step(void* direction){
	int* dir = (int*)direction;
	//if(&dir == FORWARD){
	//	setPin(STEPPER_DIR_PIN,1);
		setPin(STEPPER_STEP_PIN,0);
		setPin(STEPPER_STEP_PIN,1);
//	}	
//	else{
//
//		setPin(STEPPER_DIR_PIN,0);
//		setPin(STEPPER_STEP_PIN,0);
//		setPin(STEPPER_STEP_PIN,1);
//
//
//	}

}
int Stepper::getPosition(double* ret_val){
	int err_code = disSensor.measureDistance(ret_val);
	return err_code;
}
int Stepper::controlPosition(double distance, double rpm){
	inUse=1;
	int waitdelay = (int)((60/rpm)/STEPSPERREV)*1000*1000;
	INT8U err_val;
	INT8 *timer_name[1] = {"Timer1"};
	int direction = FORWARD;
	RTOS_TMR *timer_obj1 = RTOSTmrCreate(0,waitdelay,RTOS_TMR_PERIODIC,
										step,&direction,timer_name[0],&err_val);
	RTOSTmrStart(timer_obj1, &err_val);
	double eps = MIN_STEP_DIS;
	double measDis;
	if(getPosition(&measDis)==-1)
		return -1;
	if(setMode(FULLSTEP)==-1)
		return -1;
	while(abs(distance-measDis)>eps){
		if(measDis<5*MIN_STEP_DIS)
			if(setMode(EIGHTSTEP)==-1)
				return -1;
		if(measDis<distance){
			if(direction==BACKWARD)
				break;//means that we crossed over
			direction=FORWARD;
		}
		else{
			direction=BACKWARD;
		}

		if(getPosition(&measDis)==-1)
			return -1;

	}
	RTOSTmrDel(timer_obj1,&err_val);
	if(err_val != 0)
		return -1;
	inUse=0;
	return 0;

}
int Stepper::returnToStartPos(){
	return controlPosition(START_POS,BASE_SPEED);

}
int Stepper::setMode(int mode){
	if(mode == 0){//FULLSTEP
		if(setPin(STEPPER_MS1_PIN,0)==-1)
			return -1;
		if(setPin(STEPPER_MS2_PIN,0)==-1)
			return -1;
		factor=1;
	}
	else if(mode == 1){//HALFSTEP
		if(setPin(STEPPER_MS1_PIN,1)==-1)
			return -1;
		if(setPin(STEPPER_MS2_PIN,0)==-1)
			return -1;
		factor=2;

	}
	else if(mode == 2){//QUARTERSTEP
		if(setPin(STEPPER_MS1_PIN,0)==-1)
			return -1;
		if(setPin(STEPPER_MS2_PIN,1)==-1)
			return -1;
		factor=4;

	}
	else {//EIGHTSTEP
		if(setPin(STEPPER_MS1_PIN,1)==-1)
			return -1;
		if(setPin(STEPPER_MS2_PIN,1)==-1)
			return -1;
		factor=8;

	}
	return 0;

}
