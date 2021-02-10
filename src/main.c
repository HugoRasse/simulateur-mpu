#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"

#define OUTPUT_FILENAME "output.s"

/* Instance du MPU */
s_MPU* MPU;

/* Servira pour l'écriture du fichier .s */
int written_index = 0;
uint16_t written[RAM_SIZE];

/* Compteur de cycles */
int NB_CYCLES = 0;

/* Compteur de lignes */
s_Lines* lines;

void writeInstruction(char* instruction)
{
    int fd = open(OUTPUT_FILENAME, O_APPEND|O_CREAT|O_WRONLY);
    if(fd < 0)
    {
        perror("Couldn't open file");
        exit(0);
    }

    write(fd, instruction, strlen(instruction));

    close(fd);
}

/*
 * Fonction : decode
 * Objectif : Décoder le byte hexadécimal passé en paramètre
 * Paramètre : Le contenu du registre MPU->m_RI
*/
int decode(uint8_t byte)
{
    char* instruction = malloc(sizeof(char) * 100);
    uint16_t address = MPU->m_Adresses->m_PC - 1;

    if (byte == 0xFF)
    {
        sprintf(instruction, "%d: FF FF\n", address);
        writeInstruction(instruction);

        return EOF;
    }

    /* NOP */
    if (byte == 0x00)
    {
        sprintf(instruction, "%d: NOP\n", address);
    }
    /* JMP HHLL */
    else if (byte == 0x70)
    {
        uint16_t HHLL = (MPU->m_RAM->m_Memory[address + 2] << 8) | MPU->m_RAM->m_Memory[address + 1];
        sprintf(instruction, "%d: JMP %d\n", address, HHLL);
     
        JMP();
    }
    /* JZ HHLL */
    else if (byte == 0x71)
    {
        uint16_t HHLL = (MPU->m_RAM->m_Memory[address + 2] << 8) | MPU->m_RAM->m_Memory[address + 1];
        sprintf(instruction, "%d: JZ %d\n", address, HHLL);
        
        JZ();
    }
    /* JC */
    else if (byte == 0x72)
    {
        uint16_t HHLL = (MPU->m_RAM->m_Memory[address + 2] << 8) | MPU->m_RAM->m_Memory[address + 1];
        sprintf(instruction, "%d: JC %d\n", address, HHLL);
        
        JC();
    }
    /* JMP RXO */
    else if (byte == 0x73)
    {
        sprintf(instruction, "%d: JMP R%d\n", address, MPU->m_Registres->m_CS);
    }
    /* ST R0, RXN */
    else if (byte >= 0x78 && byte <= 0x7B)
    {
        unsigned int RXN = byte & 0x3;

        sprintf(instruction, "%d: ST R0, R%d\n", address, RXN);
        
        ST_REG(RXN);
    }
    /* LD R0, RXN */
    else if (byte >= 0x7C && byte <= 0x7F)
    {
        unsigned int RXN = byte & 0x7;

        sprintf(instruction, "%d: LD R0, R%d\n", address, RXN);

        LD_REG(RXN);
    }
    /* ST RN, HHLL */
    else if (byte >= 0x40 && byte <= 0x47)
    {
        unsigned int RN = byte & 0x7;

        uint16_t HHLL = (MPU->m_RAM->m_Memory[address + 2] << 8) | MPU->m_RAM->m_Memory[address + 1];
        sprintf(instruction, "%d: ST R%d, %d\n", address, RN, HHLL);

        ST_MEMORY(RN);
    }
    /* LD RN, HHLL */
    else if (byte >= 0x48 && byte <= 0x4F)
    {
        unsigned int RN = byte & 0x7;
 
        uint16_t HHLL = (MPU->m_RAM->m_Memory[address + 2] << 8) | MPU->m_RAM->m_Memory[address + 1];
        sprintf(instruction, "%d: LD R%d, %d\n", address, RN, HHLL);

        LD_MEMORY(RN);
    }
    /* MV RN, arg */
    else if (byte >= 0x50 && byte <= 0x57)
    {
        unsigned int RN = byte & 0x7;

        uint8_t arg = MPU->m_RAM->m_Memory[MPU->m_Adresses->m_PC];
        sprintf(instruction, "%d: MV R%d, %d\n", address, RN, arg);

        MV_ARG(RN);
    }
    /* DEC RN */
    else if (byte >= 0x58 && byte <= 0x5F)
    {
        unsigned int RN = byte & 0x7;

        sprintf(instruction, "%d: DEC R%d\n", address, RN);

        DEC(RN);
    }
    /* INC RN  */
    else if (byte >= 0x60 && byte <= 0x67)
    {
        unsigned int RN = byte & 0x7;

        sprintf(instruction, "%d: INC R%d\n", address, RN);

        INC(RN);
    }
    /* NOT RN */
    else if (byte >= 0x68 && byte <= 0x6F)
    {
        unsigned int RN = byte & 0x7;

        sprintf(instruction, "%d: NOT R%d", address, RN);

        NOT(RN);
    }
    /* ADD RN, RM */
    else if (byte >= 0x80 && byte <= 0x9F)
    {
        unsigned int RN = (byte & 0x18) >> 3;
        unsigned int RM = byte & 0x7;

        sprintf(instruction, "%d: ADD R%d, R%d\n", address, RN, RM);

        ADD(RN, RM);
    }
    /* SUB RN, RM */
    else if (byte >= 0xA0 && byte <= 0xBF)
    {
        unsigned int RN = (byte & 0x18) >> 3;
        unsigned int RM = byte & 0x7;
       
        sprintf(instruction, "%d: SUB R%d, R%d\n", address, RN, RM);

        SUB(RN, RM);
    }
    /* AND RN, RM */
    else if (byte >= 0xC0 && byte <= 0xDF)
    {
        unsigned int RN = (byte - 0xC0) & 0x18;
        unsigned int RM = (byte - 0xC0) & 0x7;
        
        sprintf(instruction, "%d: AND R%d, R%d\n", address, RN, RM);

        AND(RN, RM);
    }
    /* SWP RN, RM */
    else if (byte >= 0xE0 && byte < 0xFF)
    {
        unsigned int RN = (byte - 0xC0) & 0x18;
        unsigned int RM = (byte - 0xC0) & 0x7;
        
        sprintf(instruction, "%d: SWP R%d, R%d\n", address, RN, RM);

        SWP(RN, RM);
    }
    /* MV RN, RM */
    else if (byte <= 0x3F)
    {
        unsigned int RN = (byte - 0xC0) & 0x38;
        unsigned int RM = (byte - 0xC0) & 0x7;
        
        sprintf(instruction, "%d: MV R%d, R%d\n", address, RN, RM);

        MV_REG(RN, RM);
    }

    int found = 0;
    for (int i = 0; i < written_index; i++)
    {
        if (written[i] == address)
        {
            found = 1;
            break;
        }
    }

    if (found == 0)
    {
        writeInstruction(instruction);
        written[written_index] = address;
        written_index++;
    }
    
    free(instruction);
    return 0;
}

/*
 * Fonction : Initialize
 * Objectif : Charger les instructions dans la MPU->m_RAM->m_Memory 
 * Paramètre : Le programme héxadécimal à charger
 * Retourne : 0 si chargement réussi, -1 en cas d'erreur
 */

int Initialize(const char* filename)
{
    FILE* file = fopen(filename, "r");

    char* line;
    ssize_t read;
    size_t n = 0;

    int i,j, nLine;
    char address[4];
    uint16_t address_hex;

    /* Vide le fichier sortie */
    fclose(fopen(OUTPUT_FILENAME, "w"));

    nLine = 1;
    while ((read = getline(&line, &n, file)) != -1)
    {
        s_Line* currentLine = malloc(sizeof(s_Line));
        currentLine->m_Num = nLine;

        for (i = 0; i < 4; i++)
        {
            address[i] = line[i];
        }
        address[4] = '\0';

        address_hex = strtoul(address, NULL, 16);
        currentLine->m_Address = address_hex;

        if (nLine == 1)
        {
            /* Ecrit l'adresse de la première instruction dans le registre PC */
            MPU->m_Adresses->m_PC = address_hex;
        }

        i = 6, j = 0;
        char byte[2] = "";
        for (;;)
        {
            if (line[i] != ' ' && line[i] != '\n')
            {
                byte[j] = line[i];
                j++;
            }
            else
            {
                byte[2] = '\0';
                MPU->m_RAM->m_Memory[address_hex] = strtol(byte, NULL, 16);
                address_hex++;
                j = 0;

                if(line[i] == '\n')
                    break;
            }
            i++;
        }
        nLine++;

        push_line(lines, currentLine);
    }
    
    fclose(file);

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Spécifiez un fichier à décoder !\n");
        exit(EXIT_FAILURE);
    }

    /* Initialisation des variables */
    MPU = init_mpu();
    lines = init_lines();
    NB_CYCLES = 0;

    /* Debugger */
    s_Debugger *debugger = init_debugger(MPU);

    /* Chargement du programme */
    if (Initialize(argv[1]) != 0)
    {
        printf("Couldn't initialize file !");
        return -1;
    }

    /* Header du fichier sortie */
    writeInstruction(".text\n.globl _start\n\n_start:\n");

    int status;

    uint8_t state = wait_running_state(debugger);
    if (state == DEBUG_QUIT)
    {
        goto fin;
    }

    for (;;)
    {
        s_Line* currentLine = find_line_by_address(lines->m_First, MPU->m_Adresses->m_PC);

        if (find_line_by_num(debugger->m_Breakpoints->m_First, currentLine->m_Num) != NULL || state == DEBUG_STEP)
        {
            state = handle_breakpoint(debugger, currentLine->m_Num);
            if(state == DEBUG_QUIT)
            {
                goto fin;
            }
        }

        /* PC > InternalAddressBus */
        PCout();
        /* InternalAddressBus -> AddressLatch */
        ALin();
        /* MPU->m_RAM->m_Memory[AddressLatch] -> MPU->m_DLatch */
        Read();
        /* MPU->m_DLatch -> MPU->m_InternalDataBus */
        DLout();
        /* MPU->m_InternalDataBus -> MPU->m_RI */
        IRin();

        /* Remettre l'adresse dans PC */
        Increment();
        PCin();

        status = decode(MPU->m_RI);

        NB_CYCLES++;

        if (status == EOF)
        {
            break;
        }
    }

    /* Affichage */
    printf("Programme exécuté en %d cycles\n", NB_CYCLES);

    printf("\n");

    show_registers(MPU);
   
fin:
    /* Destruction des variables */
    destroy_debugger(debugger);
    destroy_mpu(MPU);

    return 0;
}
