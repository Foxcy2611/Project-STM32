#include "control_matrix.h"

#define ACCEL_THRESHOLD 2500
#define DEBOUNCE_LIMIT 3

volatile int8_t Gravity = 1; 

extern uint8_t max7219_buffer[MAX7219_NUM_DEVICES * 8];


uint8_t Delay_Check(uint32_t ms, int8_t cur_gravity) {
	for(int i = 0 ; i < ms ; i++) {
		if(Gravity != cur_gravity) return 0;
		Delay_ms(1);
	}
	
	return 1;
}


void LC_ClearDisplay(uint8_t addr) {
  MAX7219_ClearDisplay(addr);
}

void LC_SetXY(uint8_t addr, uint8_t x, uint8_t y, bool state) {
  if(addr >= MAX7219_NUM_DEVICES || x > 7 || y > 7) {
    return;
  }
  uint8_t mapped_x = x;
  uint8_t mapped_y = y;
   
	MAX7219_SetLed(addr, mapped_y, mapped_x, state ? 1 : 0);
}

bool LC_GetXY(uint8_t addr, uint8_t x, uint8_t y) {
  if(addr >= MAX7219_NUM_DEVICES || x > 7 || y > 7) {
    return false;
  }   
  uint8_t mapped_x = x;
  uint8_t mapped_y = y;

	uint16_t offset = addr * 8;
	uint8_t row_data = max7219_buffer[offset + mapped_y];
	uint8_t bit_mask = 0x80 >> mapped_x;

	return (row_data & bit_mask) != 0;
}

void TIM2_NVIC(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef tim;

  tim.TIM_Prescaler = 7200 - 1; 
  tim.TIM_CounterMode = TIM_CounterMode_Up;
  tim.TIM_Period = 1000 - 1;
  tim.TIM_ClockDivision = 0;
  TIM_TimeBaseInit(TIM2, &tim);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM2_IRQn);

  TIM_Cmd(TIM2, ENABLE);
}

/*
76543210


1: Xuoi
-1: Nguoc
2: Ngieng sang phai: Cat -> 7
-2: Ngieng sang trai
*/



void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		static int8_t candidate_gravity = 0;
    static uint8_t stable_count = 0;
		
		int16_t Ax = MPU6050_ReturnAx();
		int16_t Ay = MPU6050_ReturnAy();
		
		int16_t abs_Ax = (Ax >= 0) ? Ax : -Ay;
		int16_t abs_Ay = (Ay >= 0) ? Ay : -Ay;	
		int8_t new_gra = Gravity;
		
		if(abs_Ay > abs_Ax) {
      if(abs_Ay > ACCEL_THRESHOLD) {
        if (Ay > 0) new_gra = 1;
        else new_gra = -1;
      }
			
    } else {
      
			if (abs_Ax > ACCEL_THRESHOLD) {
				if (Ax > 0) new_gra = 2;            
				else new_gra = -2;           
			}    
			
		}
        
		if (new_gra != Gravity){           
			if (new_gra == candidate_gravity) {                
				stable_count++;    
				if (stable_count >= DEBOUNCE_LIMIT) {
					Gravity = new_gra;
					stable_count = 0;            
				}          
			} else {
				candidate_gravity = new_gra;
        stable_count = 0;
			}

		} else { 
			stable_count = 0;
		}
	}
}

void Fill_Sand(uint8_t addr){
	int i, j;
	for(i = 0 ; i < 8 ; i++){
		for(j = 0 ; j < 8 ; j++){
			LC_SetXY(addr, i, j, true);
		}
	}
}

void Half_Sand(uint8_t addr){
	int i, j;
	for(i = 0 ; i < 8 ; i++){
		for(j = i ; j >= 0 ; j--){
			LC_SetXY(addr, i, j, true);
		}
	}
}

void Start_Sand_Vertical(void) {
	// sum = x + y => lay vi tri cao nhat
	for (int sum = 14; sum >= 0; sum--) {  
		if(Gravity == -1) return;

		for(int y = 7; y >= 0; y--) {
			int x = sum - y;
			if(x >= 0 && x <= 7 && y >= 0 && y <= 7) {
				if(LC_GetXY(0, x, y) == false) continue;

				LC_SetXY(0, x, y, false);

				if(Delay_Check(Speed_Sand, 1) == 0){
					LC_SetXY(0, x, y, true);
					return;
				}

				Stun_Fall_Vertical(7);
			}
		}
	}
	
}

void Stun_Fall_Vertical(int x_start) {
  int x = x_start;
  int y = 7;

  while(1){
    LC_SetXY(1, x, y, true);

		if(Delay_Check(Speed_Sand, 1) == 0) return;	
    if (y == 0 || x == 0) break;

    bool down   = LC_GetXY(1, x - 1, y - 1);
    bool left   = LC_GetXY(1, x, y - 1);
    bool right  = LC_GetXY(1, x - 1, y);

    if(!down) {
      LC_SetXY(1, x, y, false);
      x--; y--; continue;
    }
    if(right == false) {
      LC_SetXY(1, x, y, false);
      x--; continue;
    }
     if(!left) {
       LC_SetXY(1, x, y, false);
       y--; continue;
     }
     break;
  }
  LC_SetXY(1, x, y, true);
}

void Start_Sand_Horizontal(void) {
  for (int sum = 0; sum <= 14; sum++) {  
		if(Gravity == 1) return;	
    for (int y = 0; y <= 7; y++) {  
      int x = sum - y;
      if (x >= 0 && x <= 7 && y >= 0 && y <= 7) {		
				if(LC_GetXY(1, x, y) == false) continue;
				LC_SetXY(1, x, y, false);  
				if(Delay_Check(Speed_Sand, -1) == 0){
					LC_SetXY(1, x, y, true);
					return;
				}    		
        Stun_Fall_Horizontal(0); 
      }
    }
  }
}

void Stun_Fall_Horizontal(int x_start) {
  int x = x_start;
  int y = 0;

  while(1){
    LC_SetXY(0, x, y, true);
		if(Delay_Check(Speed_Sand, -1) == 0) return;
    if (y == 7 || x == 7) break;
		
    bool down_left = LC_GetXY(0, x + 1, y + 1);
    bool down      = LC_GetXY(0, x, y + 1);
    bool left      = LC_GetXY(0, x + 1, y);
		
    if(!down_left) {
      LC_SetXY(0, x, y, false);
      x++; y++;
      continue;
    }
    if(left == false) {
      LC_SetXY(0, x, y, false);
      x++; continue;
    }
    if(!down) {
       LC_SetXY(0, x, y, false);
       y++; continue;
     }
     break;
  }
  LC_SetXY(0, x, y, true);
}

void Update_Sand_Vertical(uint8_t addr) {
	bool moved = true;
	static uint8_t parity = 0; 
	
	while (moved) {
		moved = false;
		parity++;

		for(int x = 0; x <= 7; x++) {
			for(int y = 0; y <= 7; y++) {
                               
				if (!LC_GetXY(addr, x, y)) continue;

				bool can_fall_center = (x > 0 && y > 0 && !LC_GetXY(addr, x - 1, y - 1));
				bool can_slide_x = (x > 0 && !LC_GetXY(addr, x - 1, y));
				bool can_slide_y = (y > 0 && !LC_GetXY(addr, x, y - 1));
                
				if (can_fall_center) {
					LC_SetXY(addr, x, y, false);
					LC_SetXY(addr, x - 1, y - 1, true);
					moved = true;
               
				} else {
					
					bool try_x_first = (parity % 2 == 0);
					if (try_x_first) {
						if (can_slide_x) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x - 1, y, true);
							moved = true;
                       
						} else if (can_slide_y) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x, y - 1, true);
							moved = true;
                       
						}
                    
					} else {
						if (can_slide_y) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x, y - 1, true);
							moved = true;
                        
						} else if (can_slide_x) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x - 1, y, true);
							moved = true;
                      
						}
					}
				}
			}
		}
    Delay_ms(20); 

	}
}


void Update_Matrix_Horizontal(uint8_t addr) {
	bool moved = true;
	static uint8_t parity = 0; 
   
	while (moved) {
		moved = false;
		parity++;

		for (int x = 7; x >= 0; x--) {
			for (int y = 7; y >= 0; y--) {
                
				if (!LC_GetXY(addr, x, y)) continue;
				
				bool can_fall_center = (x < 7 && y < 7 && !LC_GetXY(addr, x + 1, y + 1));
				bool can_slide_x = (x < 7 && !LC_GetXY(addr, x + 1, y));
				bool can_slide_y = (y < 7 && !LC_GetXY(addr, x, y + 1));

				if (can_fall_center) {
					LC_SetXY(addr, x, y, false);
					LC_SetXY(addr, x + 1, y + 1, true);
					moved = true;
               
				} else {
                   
					bool try_x_first = (parity % 2 == 0);
					if (try_x_first) {
						if (can_slide_x) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x + 1, y, true);
							moved = true;
                      
						} else if (can_slide_y) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x, y + 1, true);
							moved = true;
                     
						}
						
					} else {
						if (can_slide_y) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x, y + 1, true);
							moved = true;
                    
						} else if (can_slide_x) {
							LC_SetXY(addr, x, y, false);
							LC_SetXY(addr, x + 1, y, true);
							moved = true;
                       
						}
					}
				}
			}
		}
		Delay_ms(20);
    
	}
}

void Update_Matrix_Left(uint8_t addr) {
	bool moved = true;
	static uint8_t parity = 0; 

	while (moved) {
		moved = false;
		parity++;
			
		for (int x = 7; x >= 0; x--) {
			for (int y = 0; y <= 7; y++) {
				
				if (!LC_GetXY(addr, x, y)) continue;

				bool can_fall_center = (x < 7 && y > 0 && !LC_GetXY(addr, x + 1, y - 1));
				bool can_slide_x = (x < 7 && !LC_GetXY(addr, x + 1, y));
				bool can_slide_y = (y > 0 && !LC_GetXY(addr, x, y - 1));

				if (can_fall_center) {
					LC_SetXY(addr, x, y, false);
					LC_SetXY(addr, x + 1, y - 1, true);
					moved = true;
							
				} else {
							 
					bool try_x_first = (parity % 2 == 0);
					if (try_x_first) {
						if (can_slide_x) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x + 1, y, true); moved = true;
						
						} else if (can_slide_y) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x, y - 1, true); moved = true;
						
						}
								 
					} else {
						
						if (can_slide_y) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x, y - 1, true); moved = true;
										 
						} else if (can_slide_x) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x + 1, y, true); moved = true;
						
						}
					}
				}
			}
		}
		Delay_ms(20);
	
	}
}

void Update_Matrix_Right(uint8_t addr) {
	bool moved = true;
  static uint8_t parity = 0;

	while (moved) {
		moved = false;
		parity++;

		for (int x = 0; x <= 7; x++) {
			for (int y = 7; y >= 0; y--) {
                
				if (!LC_GetXY(addr, x, y)) continue;

				bool can_fall_center = (x > 0 && y < 7 && !LC_GetXY(addr, x - 1, y + 1)); 
				bool can_slide_x = (x > 0 && !LC_GetXY(addr, x - 1, y));
				bool can_slide_y = (y < 7 && !LC_GetXY(addr, x, y + 1));

				if (can_fall_center) {
					LC_SetXY(addr, x, y, false);
					LC_SetXY(addr, x - 1, y + 1, true);
					moved = true;
               
				} else {
               
					bool try_x_first = (parity % 2 == 0);
					if (try_x_first) {
						
						if (can_slide_x) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x - 1, y, true); moved = true;
                     
						} else if (can_slide_y) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x, y + 1, true); moved = true;
                      
						}
                    
					} else {
                     
						if (can_slide_y) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x, y + 1, true); moved = true;
                    
						} else if (can_slide_x) {
							LC_SetXY(addr, x, y, false); LC_SetXY(addr, x - 1, y, true); moved = true;
                     
						}
					}					
				}
			}
		}
		Delay_ms(20);
	}
}

/*
ngang trai AX = 16628
ay = -1364

ngang phai AX = -15868
ay = -848


Thuan AX = 204
ay = 9956

Nguoc AX = 1496
ay = -15848

*/
