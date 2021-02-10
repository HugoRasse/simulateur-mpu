#ifndef __MPU_C__
#define __MPU_C__

#include "MPU.h"

s_MPU* init_mpu()
{
    s_MPU* t_MPU = malloc(sizeof(s_MPU));

    t_MPU->m_Registres = malloc(sizeof(s_Registres));
    t_MPU->m_ALU = malloc(sizeof(s_ALU));
    t_MPU->m_Adresses = malloc(sizeof(s_AddressHandler));
    t_MPU->m_RAM = malloc(sizeof(s_RAM));
    t_MPU->m_RAM->m_Size = RAM_SIZE;

    return t_MPU;
}

void show_registers(s_MPU* mpu)
{
    for (int i = 0; i < 8; i++)
    {
        printf("R%d : %08X\n", i, mpu->m_Registres->m_REGS[i]);
    }

    printf("\n");

    printf("ALU : %08X\n", mpu->m_ALU->m_Reg);
    printf("RFlags : %08X\n", mpu->m_ALU->m_RFlags);
    printf("X : %08X, Y : %08X\n", mpu->m_ALU->m_X, mpu->m_ALU->m_Y);

    printf("\n");

    printf("Bus de données intenre : %08X\n", mpu->m_InternalDataBus);
    printf("Bus d'adresses interne : %08X\n", mpu->m_Adresses->m_InternalAddressBus);
    printf("Adress Latch : %016X\n", mpu->m_Adresses->m_AddressLatch);
    printf("PC : %016X\n", mpu->m_Adresses->m_PC);

    printf("\n");

    printf("DLatch : %08X\n", mpu->m_DLatch);

    printf("\n");

    printf("RI : %08X\n", MPU->m_RI);
}

/*
        __  ____                    _            __                  __  _                 
       /  |/  (_)_____________     (_)___  _____/ /________  _______/ /_(_)___  ____  _____
      / /|_/ / / ___/ ___/ __ \   / / __ \/ ___/ __/ ___/ / / / ___/ __/ / __ \/ __ \/ ___/
     / /  / / / /__/ /  / /_/ /  / / / / (__  ) /_/ /  / /_/ / /__/ /_/ / /_/ / / / (__  ) 
    /_/  /_/_/\___/_/   \____/  /_/_/ /_/____/\__/_/   \__,_/\___/\__/_/\____/_/ /_/____/  
                                                                                       
*/

void mSetZF()
{
    if (MPU->m_ALU->m_Reg == 0)
    {
        MPU->m_ALU->m_RFlags |= ZF;
    }
    else
    {
        if (MPU->m_ALU->m_RFlags & ZF)
        {
            MPU->m_ALU->m_RFlags &= ~ZF;
        }
    }
}

void mAddition()
{
    uint8_t resultat = MPU->m_ALU->m_X + MPU->m_ALU->m_Y;

    if (resultat >= 0xFF)
    {
        MPU->m_ALU->m_RFlags |= CF;
    }
    else
    {
        if (MPU->m_ALU->m_RFlags & CF)
        {
            MPU->m_ALU->m_RFlags &= ~CF;
        }
    }

    MPU->m_ALU->m_Reg = resultat;
    MPU->m_ALU->m_X = 0;
    MPU->m_ALU->m_Y = 0;

    mSetZF();
}

void mSubstraction()
{
    uint8_t resultat = MPU->m_ALU->m_X - MPU->m_ALU->m_Y;

    MPU->m_ALU->m_Reg = resultat;
    MPU->m_ALU->m_X = 0;
    MPU->m_ALU->m_Y = 0;

    mSetZF();
}

void mIncrementation()
{
    uint8_t resultat = MPU->m_ALU->m_X + 1;

    if (resultat >= 0xFF)
    {
        MPU->m_ALU->m_RFlags |= CF;
    }
    else
    {
        if (MPU->m_ALU->m_RFlags & CF)
        {
            MPU->m_ALU->m_RFlags &= ~CF;
        }
    }

    MPU->m_ALU->m_Reg = resultat;
    MPU->m_ALU->m_X = 0;

    mSetZF();
}

void mDecrementation()
{
    uint8_t resultat = MPU->m_ALU->m_X - 1;

    MPU->m_ALU->m_Reg = resultat;
    MPU->m_ALU->m_X = 0;

    mSetZF();
}

void mBitwiseConjunction()
{
    uint8_t resultat = MPU->m_ALU->m_X & MPU->m_ALU->m_Y;
    MPU->m_ALU->m_Reg = resultat;
    MPU->m_ALU->m_X = 0;
    MPU->m_ALU->m_Y = 0;

    mSetZF();
}

void mNegation()
{
    uint8_t resultat = ~MPU->m_ALU->m_X;
    MPU->m_ALU->m_Reg = resultat;
    MPU->m_ALU->m_X = 0;

    mSetZF();
}

/*

       _____ _                             
      / ___/(_)___ _____  ____ ___  ___  __
      \__ \/ / __ `/ __ \/ __ `/ / / / |/_/
     ___/ / / /_/ / / / / /_/ / /_/ />  <  
    /____/_/\__, /_/ /_/\__,_/\__,_/_/|_|  
           /____/                          

*/ 

void SR(int RN)
{
    if (RN >= 0 && RN <= 7)
        MPU->m_Registres->m_CS = RN;
    else
        printf("Invalid register ! (%d)\n", RN);
}

void Rin()
{
    MPU->m_Registres->m_REGS[MPU->m_Registres->m_CS] = MPU->m_InternalDataBus;
    MPU->m_InternalDataBus = 0;
}

void Rout()
{
    MPU->m_InternalDataBus = MPU->m_Registres->m_REGS[MPU->m_Registres->m_CS];
    MPU->m_Registres->m_REGS[MPU->m_Registres->m_CS] = 0;
}

void Xin()
{
    MPU->m_ALU->m_X = MPU->m_InternalDataBus;
    MPU->m_InternalDataBus = 0;
}

void Yin()
{
    MPU->m_ALU->m_Y = MPU->m_InternalDataBus;
    MPU->m_InternalDataBus = 0;
}

void ALUout()
{
    MPU->m_InternalDataBus = MPU->m_ALU->m_Reg;
    MPU->m_ALU->m_Reg = 0;
}

/* Signaux relatifs au bloc de gestion des adresses */
void ALin()
{
    MPU->m_Adresses->m_AddressLatch = MPU->m_Adresses->m_InternalAddressBus;
    MPU->m_Adresses->m_InternalAddressBus = 0;
}

void ALLin()
{
    MPU->m_Adresses->m_AddressLatch = (MPU->m_Adresses->m_AddressLatch << 8) + MPU->m_InternalDataBus;
    MPU->m_InternalDataBus = 0;
}

void ALHin()
{
    MPU->m_Adresses->m_AddressLatch = ((MPU->m_Adresses->m_AddressLatch & 0x4) << 8) + MPU->m_InternalDataBus;
    MPU->m_InternalDataBus = 0;
}

void PCin()
{
    MPU->m_Adresses->m_PC = MPU->m_Adresses->m_InternalAddressBus;
    MPU->m_Adresses->m_InternalAddressBus = 0;
}

void PCLin()
{
    MPU->m_Adresses->m_PC = (MPU->m_Adresses->m_PC & ~0x00FF) | (MPU->m_InternalDataBus);
    MPU->m_InternalDataBus = 0;
}

void PCHin()
{
    MPU->m_Adresses->m_PC = (MPU->m_Adresses->m_PC & ~0xFF00) | (MPU->m_InternalDataBus << 8);
    MPU->m_InternalDataBus = 0;
}

void PCout()
{
    MPU->m_Adresses->m_InternalAddressBus = MPU->m_Adresses->m_PC;
    MPU->m_Adresses->m_PC = 0;
}

void AAout()
{
    MPU->m_Adresses->m_InternalAddressBus = MPU->m_Adresses->m_AddressLatch;
    MPU->m_Adresses->m_AddressLatch = 0;
}

/*
 * Incrémenteur 
 * Entrée : Contenu du registre AddressLatch
 * Sortie : L'adresse suivante (incrémentée de 1)
 */
void Increment()
{
    MPU->m_Adresses->m_AddressLatch++;

    AAout();
}

/* Signaux relatifs au bloc de contrôle */
void IRin()
{
    MPU->m_RI = MPU->m_InternalDataBus;
    MPU->m_InternalDataBus = 0;
}

/* Signaux relatifs au bloc gestion de la mémoire */
void DLin()
{
    MPU->m_DLatch = MPU->m_InternalDataBus;
    MPU->m_InternalDataBus = 0;
}

void DLout()
{
    MPU->m_InternalDataBus = MPU->m_DLatch;
    MPU->m_DLatch = 0;
}

void Read()
{
    MPU->m_DLatch = MPU->m_RAM->m_Memory[MPU->m_Adresses->m_AddressLatch];
}

void Write()
{
    MPU->m_RAM->m_Memory[MPU->m_Adresses->m_AddressLatch] = MPU->m_DLatch;
}

/*
 
        ______                 __  _                     ___   _____ __  ___
       / ____/___  ____  _____/ /_(_)___  ____  _____   /   | / ___//  |/  /
      / /_  / __ \/ __ \/ ___/ __/ / __ \/ __ \/ ___/  / /| | \__ \/ /|_/ / 
     / __/ / /_/ / / / / /__/ /_/ / /_/ / / / (__  )  / ___ |___/ / /  / /  
    /_/    \____/_/ /_/\___/\__/_/\____/_/ /_/____/  /_/  |_/____/_/  /_/   
                                                                            

 */

void JMP()
{
    PCout();
    ALin();
    /* MPU->m_DLatch = LLLL */
    Read();

    Increment();
    ALin();

    /* MPU->m_InternalDataBus = LLLL */
    DLout();
    PCLin();

    /* MPU->m_DLatch = HHHH */
    Read();
    DLout();
    PCHin();
}

void JZ()
{
    if (MPU->m_ALU->m_RFlags & ZF)
    {
        JMP();
    }
    else
    {
        PCout();
        ALin();
        Increment();
        ALin();
        Increment();
        PCin();
    }
}

void JC()
{
    if (MPU->m_ALU->m_RFlags & CF)
    {
        JMP();
    }
    else
    {
        PCout();
        ALin();
        Increment();
        ALin();
        Increment();
        PCin();
    }
}

void ST_REG(int RXN)
{
    SR(RXN);
    Rout();
    SR(0);
    Rin();
}

void ST_MEMORY(int RN)
{
    SR(RN);

    /* LLLL */
    PCout();
    ALin();
    /* MPU->m_DLatch = LLLL */
    Read();

    /* HHHH */
    Increment();
    ALin();

    /* MPU->m_InternalDataBus = LLLL */
    DLout();
    /* MPU->m_DLatch = HHHH */
    Read();
    
    /* Remettre l'adresse dans PC */
    Increment();
    PCin();

    ALLin();
    DLout();
    ALHin();

    Rout();
    DLin();

    Write();
}

void LD_REG(int RXN)
{
    SR(0);
    Rout();
    SR(RXN);
    Rin();
}

void LD_MEMORY(int RN)
{
    SR(RN);

    /* LLLL */
    PCout();
    ALin();
    /* MPU->m_DLatch = LLLL */
    Read();

    /* HHHH */
    Increment();
    ALin();

    /* MPU->m_InternalDataBus = LLLL */
    DLout();
    /* MPU->m_DLatch = HHHH */
    Read();

    /* Remettre l'adresse dans PC */
    Increment();
    PCin();
   
    ALLin();
    DLout();
    ALHin();

    Read();
    DLout();
    Rin();
}

void MV_REG(int RN, int RM)
{
    SR(RM);
    Rout();
    SR(RN);
    Rin();
    
    PCout();
    ALin();
    Increment();
    PCin();
}

void MV_ARG(int RN)
{
    SR(RN);

    PCout();
    ALin();
    Read();

    Increment();
    PCin();

    DLout();
    Rin();
}

void DEC(int reg)
{
    SR(reg);
    Rout();
    Xin();
    mDecrementation();
    ALUout();
    Rin();
}

void INC(int reg)
{
    SR(reg);
    Rout();
    Xin();
    mIncrementation();
    ALUout();
    Rin();
}

void NOT(int reg)
{
    SR(reg);
    Rout();
    Xin();
    mNegation();
    ALUout();
    Rin();
}

void ADD(int RN, int RM)
{
    SR(RN);
    Rout();
    Xin();
    SR(RM);
    Rout();
    Yin();
    mAddition();
    ALUout();
    SR(RN);
    Rin();
}

void SUB(int RN, int RM)
{
    SR(RN);
    Rout();
    Xin();
    SR(RM);
    Rout();
    Yin();
    mSubstraction();
    ALUout();
    SR(RN);
    Rin();
}

void AND(int RN, int RM)
{
    SR(RN);
    Rout();
    Xin();
    SR(RM);
    Yin();
    mBitwiseConjunction();
    ALUout();
    SR(RN);
    Rin();
}

void SWP(int RN, int RM)
{
    SR(RN);
    Rout();
    Xin();
    SR(RM);
    Yin();
    // ?
    ALUout();
    SR(RN);
    Rin();
}

void destroy_mpu(s_MPU* p_MPU)
{
    free(p_MPU->m_RAM);
    free(p_MPU->m_Adresses);
    free(p_MPU->m_ALU);
    free(p_MPU->m_Registres);
}

#endif
