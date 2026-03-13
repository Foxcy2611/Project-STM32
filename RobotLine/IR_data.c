#include "ir_data.h"
#include <math.h>

// Store calibration values for black and white
static uint16_t black_level[IR_COUNT];
static uint16_t white_level[IR_COUNT];


// Real positions of each sensor
// Right side is positive, left side is negative
static const float sensor_pos[IR_COUNT] = { 
     6.1f, 4.3f, 2.9f, 1.8f, -0.9f, -1.6f, -3.7f, -5.8f
};


// Save calibration values for black and white
void IRSensor_SetCalibration(uint16_t black[], uint16_t white[])
{
    for (int i = 0; i < IR_COUNT; i++)
    {
        black_level[i] = black[i];
        white_level[i] = white[i];
    }
}

//  uint16_t raw[IR_COUNT]; // gtri ADC
//	uint16_t norm[IR_COUNT];// ADC = 0 - 1

// Read ADC values, normalize them, and calculate line position
void IRSensor_Read(IRSensor_TypedefStruct *ir, uint16_t* ADC_data_)
{
    float weightedSum = 0.0f;          // sum of (normalized value * sensor position)
    float sumWeights  = 0.0f;          // sum of normalized values

    for (int i = 0; i < IR_COUNT; i++)
    {
			ir->raw[i] = ADC_data_[i];     // store raw ADC value

			// Normalize value between 0.0 ~ 1.0
			float norm = (float)(ir->raw[i] - white_level[i]) / (black_level[i] - white_level[i]);
			if (norm < 0.0f) norm = 0.0f;
			if (norm > 1.0f) norm = 1.0f;

			ir->norm[i] = (uint16_t)(norm * 1000);

			// Add to weighted sum
			weightedSum   += norm * sensor_pos[i];
			sumWeights    += norm;
    }

		// Calculate line position (weighted average)
    if (sumWeights != 0) {
        ir->position = weightedSum / sumWeights; 
    } else {
        ir->position = -999.0f;        // error: no line detected
    }
}
