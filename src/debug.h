#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MPU.h"

#define DEBUG_QUIT 0x1
#define DEBUG_STEP 0x2

typedef struct s_Line s_Line;
struct s_Line
{
    int m_Num;
    uint16_t m_Address;
    s_Line* m_Next;
};

typedef struct
{
    s_Line* m_First;
} s_Lines;

extern s_Lines* lines;

s_Lines* init_lines();
void print_lines(s_Lines* lines);
void push_line(s_Lines* p_Lines, s_Line* line);
void pop_line(s_Lines* p_Lines);
s_Line* find_line_by_num(s_Line* p_First, int num);
s_Line* find_line_by_address(s_Line* p_First, uint16_t address);
void empty_lines(s_Lines* lines);

typedef struct
{
    s_Lines* m_Breakpoints;
    s_MPU* m_MPU;
} s_Debugger;

s_Debugger* init_debugger(s_MPU *MPU);

void show_commands();
void show_bp_commands();
uint8_t wait_running_state(s_Debugger* d);
uint8_t handle_breakpoint(s_Debugger* d, int line);

void destroy_debugger(s_Debugger* d);

#endif
