#include "dht11.h"


// === GPIO PA1 cho DHT11 ===
void DHT11_SetOutput(void) {
  GPIO_InitTypeDef gpio; 
  gpio.GPIO_Pin = GPIO_Pin_1;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_OD; // khi 0 -> OKE, KHI 1 THI NO THA NOI(TRANH CHAY CHIP)
  GPIO_Init(GPIOA, &gpio);
}

void DHT11_SetInput(void) {
  GPIO_InitTypeDef gpio;
  gpio.GPIO_Pin = GPIO_Pin_1;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &gpio);
}

uint8_t DHT11_ReadPin(void) {
  return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}

// === Giao tiep DHT11 ===
uint8_t DHT11_ReadData(uint8_t *buf) {
  // su dung con tro *buf de luu dia chi cua buf
	uint8_t i, j;

	// B1: Gui tín hieu start
  DHT11_SetOutput();  // mcu muon gui lenh
  GPIO_ResetBits(GPIOA, GPIO_Pin_1); // kéo xuong
  delay_ms(20); // min = 18ms
  GPIO_SetBits(GPIOA, GPIO_Pin_1);   // kéo lên
  delay_us(30); // time = (20-40 us)
  DHT11_SetInput(); // mcu muon nhan lenh

    // Ðoi phan hoi
  delay_us(40);
// B2: HIGH -< LOW (80US) --> HIGH(80MS)
	if (DHT11_ReadPin() == 1) return 1; // Không phan hoi
		// cho dht keo tu HIGH -> LOW
	// NEU = HIGH --> KO PHAN HOI
  // NEU = LOW --> OKE KO RETURN  
	delay_us(80); // WAIT
  if (DHT11_ReadPin() == 0) return 2; // Sai dinh dang
// KEO DHT LEN HIGH
	// sau khi xuong, keo len HIGH
  delay_us(80); // DHT kéo lên truoc khi goi data
// --> Chi la kiem tra DHT co Phan hoi ko ???
  for (j = 0; j < 5; j++) {
    uint8_t byte = 0;
      for (i = 0; i < 8; i++) {
        while (DHT11_ReadPin() == 0); // doi lên
        delay_us(30);
        if (DHT11_ReadPin() == 1){
					byte |= (1 << (7 - i)); // d?c bit
				}
					while (DHT11_ReadPin()); // doi xuong
      }
      buf[j] = byte;
   }
/* neu DHT_PinPA1 van HIGH (= 1) -> doc
			|= XOR , chi lay bit 1, vdu 001 |= 100 == 101
			<< dich bit sang tri, 1 << n: so nhi phan co 1 o vi tri n tu phai -> trai
			vd: 1 << 3:  0b00001000
											..43210		
			==> bit dau tien 1 << 7 = 0b10000000 = 128
					bit cuoi     1 << 0 = 0b00000001 = 1
			*/

    // Ki?m tra checksum
   uint8_t sum = buf[0] + buf[1] + buf[2] + buf[3];
   if (sum != buf[4]) return 3;

   return 0; // OK
}