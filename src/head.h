#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stdint.h"
#include "vector"

#define MAXX 1000000
#define MAX 100
#define REMAIN 13
#define TYPE_LEN 10
#define CATEGORY_LEN 10
#define NAME_LEN 100
#define STRING_LEN 100
#define PARA (char *)"para"
#define CONST (char *)"const"
#define VAR (char *)"var"
#define FUN (char *)"fun"
#define INT (char *)"int"
#define CHAR (char *)"char"
#define VOID (char *)"void"
#define EMPTY (char *)""
#define VECTOR_INIT_SIZE 1000000
using namespace std;

extern char n_program[MAXX];
enum MyEnum
{
	CONSTTK,
	INTTK,
	CHARTK,
	VOIDTK,
	MAINTK,
	IFTK,
	ELSETK,
	DOTK,
	WHILETK,
	FORTK,
	SCANFTK,
	PRINTFTK,
	RETURNTK,
	PLUS,
	MINU,
	MULT,
	DIV,
	LSS,
	LEQ,
	GRE,
	GEQ,
	EQL,
	NEQ,
	ASSIGN,
	SEMICN,
	COMMA,
	LPARENT,
	RPARENT,
	LBRACK,
	RBRACK,
	LBRACE,
	RBRACE,
	IDENFR,
	INTCON,
	CHARCON,
	STRCON,
	ERROR,
	FINISH,
	NOTHING
};
enum Inter
{
	BLANK,

	FUN_DEFINE,
	MAIN_DEFINE,
	PARA_DEFINE,
	START_CALL,
	PARA_PUSH,
	FUN_CALL,

	ASSIGN_WITHOUT_ARRAY,
	ASSIGN_WITH_ARRAY,
	ASSIGN_AN_ARRAY,
	ASSIGN_RET,

	VAR_DEFINE,
	CONST_DEFINE,

	PRINTF,
	SCANF,

	_BEQ,
	_BNE,
	_GRE,
	_GEQ,
	_LSS,
	_LEQ,

	SET,
	JUMP,

	RET,
	END_OF_FUN,

};


extern char program[MAXX];
extern char g_char;
extern char g_token[MAX];
extern char g_token_tem[MAX];
extern char pre_token[MAX];
extern enum MyEnum g_symbol;
extern enum MyEnum g_symbol_tem;
extern enum MyEnum pre_symbol;
extern int _index;
extern int _index_tem;
extern int line;
extern int line_tem;

void read_file(char *file_name, char *array_name, int *length);
void get_sym();
void error();
void get_and_print(FILE *ffp);
void only_get();
void only_print(FILE *ffp);
void unget_char();
void anchor();
void set();
void pre_read(int word_num);
void set_nothing();

class reg
{
public:
	int reg_index;
	int reg_type; // 0 for s_reg, 1 for t_reg,2 for a reg
	char reg_name[NAME_LEN];
	int offset_tp_fp[15];
	int index = 0;
	reg()
	{
		reg_index = -1;
		reg_type = -1;
	}
	reg(int type, int index)
	{
		reg_index = index;
		reg_type = type;
		if (reg_type == 0)
		{
			char name[NAME_LEN];
			char tmp[NAME_LEN];
			strcpy(name, "$s");
			sprintf(tmp, "%d", index);
			strcat(name, tmp);
			strcpy(reg_name, name);
		}
		else if (reg_type == 1)
		{
			char name[NAME_LEN];
			char tmp[NAME_LEN];
			strcpy(name, "$t");
			sprintf(tmp, "%d", index);
			strcat(name, tmp);
			strcpy(reg_name, name);
		}else if(reg_type == 2){
			char name[NAME_LEN];
			char tmp[NAME_LEN];
			strcpy(name, "$a");
			sprintf(tmp, "%d", index);
			strcat(name, tmp);
			strcpy(reg_name, name);
		}
	}

private:
};

class entry
{
private:
	/* data */
public:
	// compulsory
	char category[CATEGORY_LEN]; // const,var,para,fun
	char type[TYPE_LEN];		 // int,char,void
	char name[NAME_LEN];		 // name of the symbol
	int level;					 // level of the symbol, start from 1
	uint32_t offset_to_fp = 0;   // address of the variable in memory. Right now, all variables are stored in memory
	uint32_t offset_to_gp = -1;
	bool has_reg = false;

	reg my_reg;

	//optional
	int int_value;			  // if it's an integer, its value
	char char_value;		  // if it's a char, its value
	int dimension = 0;		  // if it's an array, its dimension
	int para_num = 0;		  // if it's a function, its number of parameters
	bool is_effective = true; // when search the symbol table, whether this symbol can be cited.(only for parameter)
	bool is_immediate = false;
	bool is_string = false;
	bool is_label = false;
	bool is_tmp = false;
	bool is_entrance = false;
	bool can_be_inlined = false;
	int block_size = 0;
	bool has_fun_call = false;
	bool has_var_const_define = false;
	int intermidiate_start;
	int intermidiate_finish;
	int last_appear_time = -1;
	char string[STRING_LEN];
	// constructor
	entry(char *category, char *type, char *name, int level, int value, int dimension);
	entry(char *category, char *type, char *name, int level, char value, int dimension);
	entry(int immediate);
	entry(char immediate);
	entry(char *string);
	entry();
};
class inter_code
{
private:
	/* data */
public:
	Inter inter_type;
	entry *x = NULL;
	char op;
	entry *y = NULL;
	entry *z = NULL;
	int block_loc = 0;
	inter_code(Inter inter_type, entry *x = NULL, char op = '_', entry *y = NULL, entry *z = NULL)
	{
		this->inter_type = inter_type;
		this->x = x;
		this->y = y;
		this->z = z;
		this->op = op;
	}
};




// syntax.cpp
extern int level;
extern FILE *err;
extern vector<inter_code> ic;
extern vector<entry> sb;
extern vector<entry> string_table;

void syntax_and_inter();
void print_table();
void printf_inter(vector<inter_code> ic, FILE *inter_file);
void get_tmp_name(char *tmp_name);

extern int temp_var_index;
// intermediate.cpp
extern FILE *inter_file;
void const_inter(char *type, char *name, char char_value);
void const_inter(char *type, char *name, int int_value);
void var_inter(char *type, char *name, int dimension);
void var_inter(char *type, char *name);
void fun_inter(char *type, char *name);
void para_inter(char *type, char *name);
void global_const_define();
void global_var_define();

void syntax();
void n_read_file(char *file, char *array, int *length);

void optimize_inter();
