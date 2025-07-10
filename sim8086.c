#include <stdio.h>
#include <windows.h>
#include <stdint.h>

#define NULLPTR (void *)0
#define NULLCHAR '\0'
#define MAX_STRING_LEN 255
#define REGISTER_NAME_LEN 2

typedef uint8_t u8;
typedef uint16_t u16;

#include "dispatch.c"

#define Assert(Expression) if(!Expression) {NULLPTR = 1;}

struct istream
{
    LARGE_INTEGER StreamSize;
    u8 *DoNotCrossThisLine;
    u8 *Start;
};

struct inst
{
    u8 *Binary;
    int Size;
    u8 Opcode;
    char *OperandOne;
    int OperandOneLen;
    char *OperandTwo;
    int OperandTwoLen;
};

char *ByteRegLUT[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
char *WordRegLUT[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

u8 *Win32_ReadFile(HANDLE FileHandle);
HANDLE Win32_OpenFile(LPCSTR FilePath);
void Debug_OutputErrorMessage(char *ErrorMessage, char *CallingFunction, int Line);
char *SteenCopy(char *Dest, char *Source, SIZE_T NumBytes);
SIZE_T SteenLen(char *String);
void Win32_WriteFile(u8 *OutBuffer, SIZE_T OutBufferSize);
void Win32_LoadInstStream(HANDLE FileHandle, struct istream *IStream);
char *DisassembleInst(struct inst *Inst, struct istream *IStream, char *OutBufPtr);
int DecodeMnemonic(u8 FirstByte);

int 
main(int argc, char **argv)
{
    if(argc != 2)
    {
	Debug_OutputErrorMessage("Error: Usage", __func__, __LINE__);
	exit(1);
    }

    LPCSTR FilePath = argv[1];
    HANDLE FileHandle = Win32_OpenFile(FilePath);
    struct istream IStream = {};
    
    Win32_LoadInstStream(FileHandle, &IStream);
    u8 *IP = IStream.Start;

    char *Directive = "bits 16\n\n";
    HANDLE HeapHandle = GetProcessHeap();
    char *OutBuf = (char *)HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, 1000);
    char *OutBufPtr = OutBuf;
    char *OutBuf_DoNotCross = OutBuf + 1000;
    
    // write directive
    SIZE_T DirectiveLen = SteenLen(Directive);
    SteenCopy(OutBufPtr, Directive, DirectiveLen);
    OutBufPtr += DirectiveLen;

    while(IP < IStream.DoNotCrossThisLine) 
    {
	struct inst Inst = {0};
	Inst.Binary = IP;

	Inst.Opcode = OpcodeLUT[Inst.Binary[0]];
	if(Inst.Opcode == EXTENDED)
	{
	    Inst.Opcode = ReadExtendedOpcode(&Inst);
	}

	switch(Inst.Opcode)
	{
	    // Disassemble functions always return updated
	    //	  OutBufPtr

	    case MOV:
	    {
		OutBufPtr = DisassembleMOV(
	    }
	}

	


	
	struct inst Inst;
	Inst.Opcode = (u8)*InstPtr & OPCODE_FIELD;
	Inst.RegIsDest = (u8)*InstPtr & REG_IS_DEST_FLAG;
	Inst.IsWord = (u8)*InstPtr & WORD_FLAG;
	Inst.OpcodeMod = (u8)((*InstPtr >> 8) & MOD_FIELD);
	Inst.Reg = (u8)((*InstPtr >> 8) & REG_FIELD);
	Inst.RorM = (u8)((*InstPtr >> 8) & R_OR_M_FIELD);

	if( (OutBufPtr + SteenLen("mov xx, xx\n")) >= OutBuf_DoNotCross )
	{
	    Debug_OutputErrorMessage("Error: OutBuf overflow", __func__, __LINE__);
	    exit(1);
	}
	// DisassembleInst passes back OutBufPtr
	OutBufPtr = DisassembleInst(&Inst, &IStream, OutBufPtr);
	if(i != (NumStart - 1))
	{
	    *OutBufPtr = '\n';
	    OutBufPtr++;
	}
	InstPtr++;


    }
    SIZE_T OutFileSize = (OutBufPtr - OutBuf);

    Win32_WriteFile((u8 *)OutBuf, OutFileSize);

    return(0);
}

SIZE_T
SteenLen(char *String)
{
    char *p = String;
    SIZE_T Len = 0;
    
    while(*p != NULLCHAR)
    {
	Len++;
	p++;
    }

    return(Len);
}


char *
SteenCopy(char *Dest, char *Source, SIZE_T NumBytes)
{
    for(int i = 0; i < NumBytes; i++)
    {
	*Dest++ = *Source++;
    }

    return(Dest);
}

HANDLE 
Win32_OpenFile(LPCSTR FilePath)
{
    HANDLE FileHandle = CreateFileA(FilePath, GENERIC_READ, 0,
			     0, 3, FILE_ATTRIBUTE_READONLY, 0);
    if(FileHandle == INVALID_HANDLE_VALUE)
    {
	Debug_OutputErrorMessage("Error: Failed to open open input file", __func__, __LINE__);
	exit(1);
    }

    return(FileHandle);
}

void
Win32_LoadInstStream(HANDLE FileHandle, struct istream *IStream)
{
    if(FileHandle)
    {
	GetFileSizeEx(FileHandle, &IStream->StreamSize);
	if(IStream->StreamSize.QuadPart)
	{
	    HANDLE HeapHandle = GetProcessHeap();
	    if(HeapHandle)
	    {
		IStream->Start = (u8 *)HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, 
					     (SIZE_T)IStream->StreamSize.LowPart);
		if(IStream->Start)
		{
		    DWORD BytesRead = 0;
		    BOOL Result = ReadFile(FileHandle, IStream->Start, 
					    IStream->StreamSize.LowPart, &BytesRead, 0);
		    if((BytesRead == IStream->StreamSize.LowPart) && Result)
		    {
			IStream->DoNotCrossThisLine = ((u8 *)IStream->Start +
						    IStream->StreamSize.LowPart);
		    }
		    else
		    {
			Debug_OutputErrorMessage("Error: Failed to read instruction stream"
						    "into buffer", 
						    __func__, __LINE__);
			exit(1);
		    }
		}
		else
		{
		    Debug_OutputErrorMessage("Error: Failed to allocate memory"
					     "for instruction stream",
						__func__, __LINE__);
		    exit(1);
		}
	    }
	    else
	    {
		Debug_OutputErrorMessage("Error: Failed to get process heap handle", 
					    __func__, __LINE__);
		exit(1);
	    }
	}
	else
	{
	    Debug_OutputErrorMessage("Error: Failed to read input file size", 
					__func__, __LINE__);
	    exit(1);
	}
    }
    else
    {
	Debug_OutputErrorMessage("Error: Bad input file handle", 
				    __func__, __LINE__);
	exit(1);
    }

}

void
Win32_WriteFile(u8 *OutBuffer, SIZE_T OutBufferSize)
{
    char *OutFilePath = "out.asm";
    HANDLE OutFileHandle = CreateFileA((LPCSTR)OutFilePath, GENERIC_WRITE,
					0, 0, CREATE_NEW,
					FILE_ATTRIBUTE_NORMAL, 0);

    DWORD BytesWritten = 0;
    BOOL UltimateWriteSuccessPlease = WriteFile(OutFileHandle, (LPCVOID)OutBuffer,
				    (DWORD)OutBufferSize, &BytesWritten, 0);

    if( !(UltimateWriteSuccessPlease && (BytesWritten == OutBufferSize)) )
    {
	Debug_OutputErrorMessage("Error writing outfile", __func__, __LINE__);
	exit(1);
    }
}

void
Debug_OutputErrorMessage(char *ErrorMessage, char *CallingFunction, int Line)
{
    char ErrorBuffer[MAX_STRING_LEN];
    sprintf_s(ErrorBuffer, sizeof(ErrorBuffer), 
	    "\nERROR:\n  In function %s, on line %d,\n\n    ", CallingFunction, Line);
    OutputDebugStringA(ErrorBuffer);
    sprintf_s(ErrorBuffer, sizeof(ErrorBuffer), ErrorMessage);
    OutputDebugStringA(ErrorBuffer);
    OutputDebugStringA(".\n\n");
}

u8
ReadExtendedOpcode(struct inst *Inst)
{
    u8 FirstByte = Inst->Binary[0];
    u8 MeetTheDistinguisher = 0x38; //0011 1000
    u8 IHaveMetTheDistinguisher = Inst->Binary[1] & MeetTheDistinguisher;

    switch(FirstByte)
    {
	case 0x80:
	case 0x81:
	{
	    switch(IHaveMetTheDistinguisher)
	    {
		case 0x00: // 000
		{
		    return(ADD);
		} break;

		case 0x08: // 001
		{
		    return(OR);
		} break;

		case 0x10: // 010
		{
		    return(ADC);
		} break;

		case 0x18: // 011
		{
		    return(SBB);
		} break;

		case 0x20: // 100
		{
		    return(AND);
		} break;

		case 0x28: // 101
		{
		    return(SUB);
		} break;

		case 0x30: // 110
		{
		    return(XOR);
		} break;

		case 0x38: // 111
		{
		    return(CMP);
		} break;

		default:
		{
		    exit(1);
		} break;
	    }
	} break;

	case 0x82:
	{
	    switch(IHaveMetTheDistinguisher)
	    {
		case 0x00: // 000
		{
		    return(ADD);
		} break;

		case 0x10: // 010
		{
		    return(ADC);
		} break;

		case 0x18: // 011
		{
		    return(SBB);
		} break;

		case 0x28: // 101
		{
		    return(SUB);
		} break;

		case 0x38: // 111
		{
		    return(CMP);
		} break;

		default:
		{
		    exit(1);
		} break;
	    }
	} break;

	case 0x83:
	{
	    switch(IHaveMetTheDistinguisher)
	    {
		case 0x00: // 000
		{
		    return(ADD);
		} break;

		case 0x10: // 010
		{
		    return(ADC);
		} break;

		case 0x18: // 011
		{
		    return(SBB);
		} break;

		case 0x28: // 101
		{
		    return(SUB);
		} break;

		case 0x38: // 111
		{
		    return(CMP);
		} break;

		default:
		{
		    exit(1);
		} break;
	    }
	} break;

	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	{
	    switch(IHaveMetTheDistinguisher)
	    {
		case 0x00: // 000
		{
		    return(ROL);
		} break;

		case 0x08: // 001
		{
		    return(ROR);
		} break;

		case 0x10: // 010
		{
		    return(RCL);
		} break;

		case 0x18: // 011
		{
		    return(RCR);
		} break;

		case 0x20: // 100
		{
		    return(SALorSHL);
		} break;

		case 0x28: // 101
		{
		    return(SHR);
		} break;

		case 0x38: // 111
		{
		    return(SAR);
		} break;

		default:
		{
		    exit(1);
		} break;
	    }
	} break;

	case 0xF6:
	case 0xF7:
	{
	    switch(IHaveMetTheDistinguisher)
	    {
		case 0x00: // 000
		{
		    return(TEST);
		} break;

		case 0x10: // 010
		{
		    return(NOT);
		} break;

		case 0x18: // 011
		{
		    return(NEG);
		} break;

		case 0x20: // 100
		{
		    return(MUL);
		} break;

		case 0x28: // 101
		{
		    return(IMUL);
		} break;

		case 0x30: // 110
		{
		    return(DIV);
		} break;

		case 0x38: // 111
		{
		    return(IDIV);
		} break;

		default:
		{
		    exit(1);
		} break;
	    }
	} break;

	case 0xFE:
	{
	    switch(IHaveMetTheDistinguisher)
	    {
		case 0x00: // 000
		{
		    return(INC);
		} break;

		case 0x10: // 010
		{
		    return(DEC);
		} break;

		default:
		{
		    exit(1);
		} break;
	    }
	} break;

	case 0xFF:
	{
	    switch(IHaveMetTheDistinguisher)
	    {
		case 0x00: // 000
		{
		    return(INC);
		} break;

		case 0x08: // 001
		{
		    return(DEC);
		} break;

		case 0x10: // 010
		case 0x18: // 011
		{
		    return(CALL);
		} break;

		case 0x20: // 100
		case 0x28: // 101
		{
		    return(JMP);
		} break;

		case 0x30: // 110
		{
		    return(PUSH);
		} break;

		default:
		{
		    exit(1);
		} break;
	    }
	} break;

	default:
	{
	    exit(1);
	} break;
    }
}

/*char **/
/*DisassembleInst(struct inst *Inst, struct istream *IStream, char *OutBufPtr)*/
/*{*/
/*    if( (Inst->Opcode != 0x88) || (Inst->OpcodeMod != 0xC0) || */
/*		(Inst->RegIsDest == REG_IS_DEST_FLAG) )*/
/*    {*/
/*	Debug_OutputErrorMessage("Error: Unrecognized opcode", __func__, __LINE__);*/
/*	exit(1);*/
/*    }*/
/**/
/*    char *Opcode = "mov ";*/
/*    SIZE_T OpcodeLen = SteenLen(Opcode);*/
/*    SteenCopy(OutBufPtr, Opcode, OpcodeLen);*/
/*    OutBufPtr += OpcodeLen;*/
/**/
/*    // write operands*/
/*    u8 DestRegCode = Inst->RorM;*/
/*    char *DestReg = ByteRegLUT[DestRegCode];*/
/*    u8 SourceRegCode = (Inst->Reg >> 3);*/
/*    char *SourceReg = ByteRegLUT[SourceRegCode];*/
/*    if(Inst->IsWord & WORD_FLAG)*/
/*    {*/
/*	DestReg = WordRegLUT[DestRegCode];*/
/*	SourceReg = WordRegLUT[SourceRegCode];*/
/*    }*/
/*    SteenCopy(OutBufPtr, DestReg, REGISTER_NAME_LEN);*/
/*    OutBufPtr += REGISTER_NAME_LEN;*/
/**/
/*    *OutBufPtr++ = ',';*/
/*    *OutBufPtr++ = ' ';*/
/**/
/*    SteenCopy(OutBufPtr, SourceReg, REGISTER_NAME_LEN);*/
/*    OutBufPtr += REGISTER_NAME_LEN;*/
/**/
/*    return(OutBufPtr);*/
/*}*/

u8 *
DisassembleMov(struct inst *Inst, u8 *OutBufPtr)
{
    OpcodeLen = SteenLen("mov ");
    SteenCopy(OutBufPtr, "mov ", OpcodeLen);
    OutBufPtr += OpcodeLen;
    u8 ByteOne = Inst->Binary[0];
    u8 ByteTwo = Inst->Binary[1];

    enum
    {
	MEMorREG_TOorFROM_REG = 0x88,
	IMM_TO_REGorMEM = 0xC6,
	IMM_TO_REG = 0xB0,
	MEM_TO_ACCUM = 0xA0,
	ACCUM_TO_MEM = 0xA2,
	REGorMEM_TO_SEGREG = 0x8E,
	SEGREG_TO_REGorMEM = 0x8C
    };

    enum
    {
	MOD_FIELD = 0xC0,
	FLEX_FIELD = 0x38, // very flexible field
	RorM_FIELD = 0x03
    };

    switch(ByteOne)
    {
	case MEMorREG_TOorFROM_REG:
	case (MEMorREG_TOorFROM_REG + 1): // if Word flag is set
	{
	    struct
	    {
		u8 RegIsDest;
		u8 IsWord;
		u8 OpcodeMod;
		u8 Reg;
		u8 RorM;
	    } DecodedInst;

	    DecodedInst.RegIsDest = (ByteOne & 0x02);
	    DecodedInst.IsWord = (ByteOne & 0x01);
	    DecodedInst.OpcodeMod = (ByteTwo & MOD_FIELD);
	    DecodedInst.Reg = (ByteTwo & REG_FIELD);
	    DecodedInst.RorM = (ByteTwo & R_OR_M_FIELD);

	    if(DecodedInst.OpcodeMod == 0x00) // MOD == 00
	    {
		if(DecodedInst.RorM == 0x06) // Special case: Direct address
		{
		    if(IsWord)
		    {
			Inst->OperandOne = WordRegLUT[DecodedInst.Reg];
		    }
		    else
		    {
			Inst->OperandOne = ByteRegLUT[DecodedInst.Reg];
		    }
		    Inst->OperandOneLen = REGISTER_NAME_LEN;

		    char AddressBuffer[MAX_STRING_LEN];
		    int BytesPrinted = sprintf_s(AddressBuffer, sizeof(AddressBuffer), "0x%x", (u16)Inst->Binary[2]);
		    OperandTwo = AddressBuffer;
		    OperandTwoLen = BytesPrinted;
		}
		
		if(RegIsDest)
		{
		    //stop i am tired....
		    
		}
		
	    }
	    else if(CheckMODField == 0x04) // 01
	    {




	    }
	    else if(CheckMODField == 0x80) // 10
	    {

	    }
	    else if(CheckMODField == 0xC0) // 11
	    {
		u8 DestField = ByteTwo & RorM_FIELD;
		u8 SourceField = ByteTwo & AUXILARY_FIELD;

		u8 IsWord = (ByteOne & 0x01);
		if(IsWord)
		{
		    OperandOne = WordRegLUT[DestField];
		    OperandTwo = WordRegLUT[SourceField];
		}
		else
		{
		    OperandOne = ByteRegLUT[DestField];
		    OperandTwo = ByteRegLUT[SourceField];
		}
	    }
	    else
	    {
		// Error
		exit(1);
	    }


	    
	} break;

	case IMM_TO_REGorMEM:
	case IMM_TO_REGorMEM + 1: // if Word flag is set
	{

	}


	default:
	{
	    // ERROR
	    // stop: in the middle of writing this function
	} break;
    }
}
