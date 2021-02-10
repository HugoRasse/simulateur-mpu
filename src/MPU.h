#ifndef __MPU_H__
#define __MPU_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define RAM_SIZE 1024

#define ZF 0x1
#define CF 0x2

/* Bloc registres */
typedef struct
{
    int m_CS;
    uint8_t m_REGS[8];
} s_Registres;

/* Bloc ALU */
typedef struct
{
    uint8_t m_Reg;
    uint8_t m_RFlags;
    uint8_t m_X;
    uint8_t m_Y;
} s_ALU;

/* Bloc gestion des adresses */
typedef struct
{
    uint16_t m_InternalAddressBus;
    uint16_t m_AddressLatch;
    uint16_t m_PC;
} s_AddressHandler;

typedef struct
{
    uint8_t m_Memory[RAM_SIZE];
    int m_Size;
} s_RAM;

typedef struct
{
    uint8_t m_DLatch;
    uint8_t m_RI;
    uint8_t m_InternalDataBus;
    s_Registres* m_Registres;
    s_AddressHandler* m_Adresses;
    s_ALU* m_ALU;
    s_RAM* m_RAM;
} s_MPU;

extern s_MPU* MPU;

s_MPU* init_mpu();

void show_registers(s_MPU* mpu);
uint8_t get_instruction(int address);

void destroy_mpu(s_MPU* mpu);

/* Signaux */

void mSetZF();
void mAddition();
void mSubstraction();
void mIncrementation();
void mDecrementation();
void mBitwiseConjunction();
void mNegation();


void SR(int RN);
void Rin();
void Rout();

void Xin();
void Yin();
void ALUout();

void ALin();
void ALLin();
void ALHin();

void PCin();
void PCLin();
void PCHin();
void PCout();

void AAout();
void Increment();

void IRin();
void DLin();
void DLout();

void Read();
void Write();


/* ASM */

void JMP();
void JZ();
void JC();

void ST_REG(int RXN);
void ST_MEMORY(int RN);

void LD_REG(int RXN);
void LD_MEMORY(int RN);

void MV_REG(int RN, int RM);
void MV_ARG(int RN);

void DEC(int RN);
void INC(int RN);
void NOT(int RN);
void ADD(int RN, int RM);
void SUB(int RN, int RM);
void AND(int RN, int RM);
void SWP(int RN, int RM);

#endif
