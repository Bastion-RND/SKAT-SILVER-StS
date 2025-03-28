
#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

//---------------------------------------------------------------

void TEN_Control(void);
void TEN_Chice(uint8_t N);
void Pump_Control(void);
void Fan_Control(void);
uint8_t PowerLimit_JumpersRead(void);
void TEN_Raiting_Compute(void);
void Set_TEN(uint8_t T, uint8_t set);


#endif /* INC_CONTROL_H_ */
