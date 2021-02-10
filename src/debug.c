#ifndef __DEBUG_C__
#define __DEBUG_C__

#include "debug.h"

s_Lines* init_lines()
{
    s_Lines* lst = malloc(sizeof(s_Lines));
    lst->m_First = NULL;

    return lst;
}

void print_lines(s_Lines* p_Lines)
{
    if (p_Lines->m_First != NULL)
    {
        s_Line* current = p_Lines->m_First;
        while (current != NULL)
        {
            printf("Ligne %d, adresse RAM : %d\n", current->m_Num, current->m_Address);
            current = current->m_Next;
        }
    }
}

void push_line(s_Lines* p_Lines, s_Line* line)
{
    if (p_Lines->m_First == NULL)
    {
        p_Lines->m_First = line;
        p_Lines->m_First->m_Next = NULL;
    }
    else
    {
        s_Line* last = p_Lines->m_First;
        while (last->m_Next != NULL)
        {
            last = last->m_Next;
        }

        last->m_Next = line;
        last->m_Next->m_Next = NULL;
    }
}

void pop_line(s_Lines* p_Lines)
{
    if (p_Lines->m_First != NULL)
    {
        s_Line *tmp = p_Lines->m_First;
        p_Lines->m_First = lines->m_First->m_Next;
        free(tmp);
    }
}

s_Line* find_line_by_num(s_Line* p_First, int num)
{
    if (p_First != NULL)
    {
        for (s_Line* current = p_First; current != NULL; current = current->m_Next)
        {
            if (current->m_Num == num)
            {
                return current;
            }
        }
    }

    return NULL;
}

s_Line* find_line_by_address(s_Line* p_First, uint16_t address)
{
    if (p_First != NULL)
    {
        for (s_Line* current = p_First; current != NULL; current = current->m_Next)
        {
            if (current->m_Address == address)
            {
                return current;
            }
        }
    }

    return NULL;
}

void empty_lines(s_Lines* p_Lines)
{
    while (p_Lines->m_First != NULL)
    {
        pop_line(p_Lines);
    }
}

s_Debugger* init_debugger(s_MPU* p_MPU)
{
    s_Debugger* debug = malloc(sizeof(s_Debugger));
    debug->m_MPU = p_MPU;
    debug->m_Breakpoints = init_lines();

    return debug;
}

void show_commands()
{
    printf("Liste des commandes : \n");
    printf("\n");
    printf("b -- Arrête le programme à la ligne donnée\n");
    printf("h -- Affiche ce message\n");
    printf("r -- Lance le programme\n");
    printf("q -- Quitter\n");
}

void show_bp_commands()
{
    printf("Liste des commandes lors d'un point d'arrêt :\n");
    printf("\n");
    printf("c -- Continuer le programme\n");
    printf("h -- Afficher ce message\n");
    printf("l -- Afficher le numéro de la ligne courante\n");
    printf("print (reg) -- Afficher la valeur hexadécimale du registe reg\n");
    printf("q -- Quitter le programme\n");
    printf("s -- Aller à la ligne suivante\n");
    printf("show data -- Afficher l'ensemble des données stockées\n");
}

uint8_t wait_running_state(s_Debugger* d)
{
    char input[10];
    for (;;)
    {
        printf("> ");
        fgets(input, 10, stdin);

        if (input[0] == 'b')
        {
            s_Line* line = find_line_by_num(lines->m_First, atoi(input+2));
            if(line == NULL)
            {
                printf("Ligne non trouvée !\n\n");
                continue;
            }

            s_Line* newLine = malloc(sizeof(s_Line));
            *newLine = *line;
            push_line(d->m_Breakpoints, newLine);
        }
        else if (input[0] == 'h')
        {
            show_commands();
        }
        else if (input[0] == 'r')
        {
            break;
        }
        else if (input[0] == 'q')
        {
            return DEBUG_QUIT;
        }
        else
        {
            printf("Commande inconnue ! Tapez 'h' pour voir les commandes possibles\n");
        }
    }

    return 0;
}

uint8_t handle_breakpoint(s_Debugger* d, int line)
{
    int input_size = 30;
    char input[input_size];
    //printf("Instruction : ");

    if (find_line_by_num(d->m_Breakpoints->m_First, line) != NULL)
    {
        printf("Point d'arrêt, ligne %d\n", line);
    }

    for (;;)
    {
        printf("> ");
        fgets(input, input_size, stdin);

        if (input[0] == 'c')
        {
            printf("Continuing\n");
            break;
        }
        else if(input[0] == 'h')
        {
            show_bp_commands();
        }
        else if(input[0] == 'l')
        {
            printf("Ligne courante : %d\n", line);
        }
        else if (strncmp(input, "print", strlen("print")) == 0)
        {
            char* reste = input + strlen("print") + 1;
            reste[strlen(reste) - 1] = '\0';
            if (strlen(reste) == 2)
            {
                if (reste[0] == 'R')
                {
                    if (reste[1] == 'I')
                    {
                        printf("RI : %X\n", MPU->m_RI);
                    }
                    else if (reste[1] >= '0' && reste[1] <= '7')
                    {
                        printf("%s : %X\n", reste, MPU->m_Registres->m_REGS[reste[1] - '0']);
                    }
                    else
                    {
                        goto unknown;
                    }
                }
            }
            else
            {
unknown:
                printf("Registre inconnu\n");
            }
        }
        else if (input[0] == 'q')
        {
            return DEBUG_QUIT;
        }
        else if (input[0] == 's' && strlen(input) == 2)
        {
            return DEBUG_STEP;
        }
        else if (strncmp(input, "show data", strlen("show data")) == 0)
        {
            show_registers(MPU);
        }
        printf("\n");
    }

    return 0;
}

void destroy_debugger(s_Debugger* d)
{
    free(d->m_Breakpoints);
    free(d);
}

#endif
