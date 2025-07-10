#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

enum
{
    // MOV
    MOV_REGorMEM_TOorFROM_REG = 0x88,//1000 10    
    MOV_IMM_TO_REGorMEM = 0xC6,//1100 011    
    MOV_IMM_TO_REG = 0xB0,//1011       
    MOV_MEM_TO_ACCUM = 0xA0,//1010 000   
    MOV_ACCUM_TO_MEM = 0xA2,//1010 001   
    MOV_REGorMEM_TO_SEGREG = 0x8E,//1000 1110  
    MOV_SEGREG_TO_REGorMEM = 0x8C,//1000 1100  
    
    // PUSH
    PUSH_REGorMEM = 0xFF,//1111 1111
    PUSH_REG = 0x50,//0101 0
    PUSH_SEGREG = 0x06,//0000 0110

    // POP
    POP_REGorMEM = 0x8F,//1000 1111
    POP_REG = 0x58,//0101 1
    POP_SEGREG = 0x07,//0000 0111
    
    // XCHG
    XCHG_REGorMEM_WITH_REG = 0x86,//1000 011
    XCHG_REG_WITH_ACCUM = 0x90,//1001 0

    // IN
    IN_FIXED_PORT = 0xE4,//1110 010
    IN_VARIABLE_PORT = 0xEC,//1110 110

    // OUT
    OUT_FIXED_PORT = 0xE6,//1110 011
    OUT_VARIABLE_PORT = 0xEE,//1110 111
    
    // LEA
    LEA_LOAD_EA_TO_REG = 0x8D,//1000 1101
  
    // LDS 
    LDS_LOAD_PTR_TO_DS = 0xC5,//1100 0101
			      
    // LES
    LES_LOAD_PTR_TO_ES = 0xC4,//1100 0100

    // Arithmetic ops that cannot be differentiated from the first byte
    //	  of the instruction:
    //	      
    //	      0x80 (0b1000 00) (check bytes 4-6 of low byte to differentiate)
    //	          ADD_IMM_TO_REGorMEM	      
    //	          ADC_IMM_TO_REGorMEM	      
    //	          SUB_IMM_FROM_REGorMEM	      
    //	          SBB_IMM_FROM_REGorMEM	      
    //
    //	      0xFE (0b1111 111) (check bytes 4-6 of low byte to differentiate)
    //		  INC_REGorMEM
    //		  DEC_REGorMEM
    
    // ADD
    ADD_REGorMEM_WITH_REG_TO_EITHER = 0x00,//0000 00,
    ADD_IMM_TO_REGorMEM = 0x80,//1000 00
    ADD_IMM_TO_ACCUM = 0x04,//0000 010,

    // ADC
    ADC_REGorMEM_WITH_REG_TO_EITHER = 0x10,//0001 00
    ADC_IMM_TO_REGorMEM = 0x80,//1000 00
    ADC_IMM_TO_ACCUM = 0x14,//0001 010

    // INC
    INC_REGorMEM = 0xFE,//1111 111
    INC_REG = 0x40,//0100 0

    // SUB
    SUB_REGorMEM_AND_REG_TO_EITHER = 0x28,//0010 10
    SUB_IMM_FROM_REGorMEM = 0x80,//1000 00
    SUB_IMM_FROM_ACCUM = 0x2C,//0010 110

    // SBB
    SBB_REGorMEM_AND_REG_TO_EITHER = 0x18,//0001 10
    SBB_IMM_FROM_REGorMEM = 0x80,//1000 00
    SBB_IMM_FROM_ACCUM = 0x1C,//0001 110
    
    // DEC
    DEC_REGorMEM = 0xFE,//1111 111
    DEC_REG = 0x48,//0100 1
    DEC_NEG_CHANGE_SIGN = 0xF6,//1111 011

} dispatch;

u8 Mnemonics[] =
{
    // MOV
	MOV_REGorMEM_TOorFROM_REG, // d bit unset; w bit unset 
	(MOV_REGorMEM_TOorFROM_REG + 1), // d bit unset; w bit set 
	(MOV_REGorMEM_TOorFROM_REG + 2), // d bit set; w bit unset 
	(MOV_REGorMEM_TOorFROM_REG + 3), // d bit set; w bit set 
	MOV_IMM_TO_REGorMEM,
	MOV_IMM_TO_REG,
	MOV_MEM_TO_ACCUM,
	MOV_ACCUM_TO_MEM,
	MOV_REGorMEM_TO_SEGREG,
	MOV_SEGREG_TO_REGorMEM,
    
    // PUSH
	PUSH_REGorMEM,
	PUSH_REG,
	PUSH_SEGREG,

    // POP
	POP_REGorMEM,
	POP_REG,
	POP_SEGREG,
    
    // XCHG
	XCHG_REGorMEM_WITH_REG,
	XCHG_REG_WITH_ACCUM,

    // IN
	IN_FIXED_PORT,
	IN_VARIABLE_PORT,

    // OUT
	OUT_FIXED_PORT,
	OUT_VARIABLE_PORT,
    
    // LEA
	LEA_LOAD_EA_TO_REG,
  
    // LDS 
	LDS_LOAD_PTR_TO_DS,
			      
    // LES
	LES_LOAD_PTR_TO_ES,

    // ADD
	ADD_REGorMEM_WITH_REG_TO_EITHER,
	ADD_IMM_TO_REGorMEM,
	ADD_IMM_TO_ACCUM,

    // ADC
	ADC_REGorMEM_WITH_REG_TO_EITHER,
	ADC_IMM_TO_REGorMEM,
	ADC_IMM_TO_ACCUM,

    // INC
	INC_REGorMEM,
	INC_REG,

    // SUB
	SUB_REGorMEM_AND_REG_TO_EITHER,
	SUB_IMM_FROM_REGorMEM,
	SUB_IMM_FROM_ACCUM,

    // SBB
	SBB_REGorMEM_AND_REG_TO_EITHER,
	SBB_IMM_FROM_REGorMEM,
	SBB_IMM_FROM_ACCUM,
    
    // DEC
	DEC_REGorMEM,
	DEC_REG,
	DEC_NEG_CHANGE_SIGN
};
