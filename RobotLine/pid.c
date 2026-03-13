#include "pid.h"
#include "delay.h"   // ho?c systick/millis tu? b?n dang dùng

void PID_Init(PID_TypedefStruct *PID, float kp, float ki, float kd, float max_output)
{
    PID->kp = kp;
    PID->ki = ki;
    PID->kd = kd;
    PID->max_output = max_output;

    PID->error = 0;
    PID->P = 0;
    PID->I = 0;
    PID->D = 0;

    PID->output = 0;
    PID->pre_pv = 0;

    PID->cur_time = 0;
    PID->pre_time = 0;
    PID->dt = 0;
}

// Standard PID update
float PID_Update(PID_TypedefStruct *PID, float sp, float pv, float ubias)
{
	// Time calculation
	PID->cur_time = millis();
	if (PID->pre_time == 0) {
			PID->pre_time = PID->cur_time;
			PID->pre_pv   = pv;
			return 0.0f;                         // first run, no control yet
	}
	PID->dt = PID->cur_time - PID->pre_time;
	PID->pre_time = PID->cur_time;

	if (PID->dt < 1) PID->dt = 1;            // avoid divide by zero
	float dt_s = PID->dt / 1000.0f;          // ms -> seconds

	// Error
	PID->error = (sp - pv);

	// P
	PID->P = PID->kp * PID->error;

	// I
	PID->I += PID->ki * PID->error * dt_s;

	// D
	float dv  = (pv - PID->pre_pv) / dt_s;
	PID->pre_pv = pv;

	static float dv_filt = 0.0f;
	const float alpha = 0.2f;
	dv_filt += alpha * (dv - dv_filt);

	PID->D = -PID->kd * dv_filt;

	float out = ubias + PID->P + PID->I + PID->D;

	if (out > PID->max_output) {
			out = PID->max_output;
			if (PID->ki != 0.0f && PID->error > 0.0f) {
					PID->I -= PID->ki * PID->error * dt_s;
			}
	} else if (out < -PID->max_output) {
			out = -PID->max_output;
			if (PID->ki != 0.0f && PID->error < 0.0f) {
					PID->I -= PID->ki * PID->error * dt_s;
			}
	}

	PID->output = out;
	return PID->output;
}


