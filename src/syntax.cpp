#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "head.h"
#include "vector"
using namespace std;

FILE *ffp;
FILE *inter_file;
FILE *err;

char return_fun_name[100][100];
int return_fun_index;
char non_return_fun_name[100][100];
int non_return_fun_index;

vector<entry> sb;
vector<int> index_stack;
vector<inter_code> ic;
vector<entry> const_table;
vector<entry> string_table;
vector<entry> label_table;

int temp_var_index = 0;
int temp_str_index = 0;
int temp_label_index = 0;
int level = 1;
int is_non_return_fun = 0;
//entry enter = entry((char *)"\\n");

enum MyEnum factor_next[10] = {PLUS, MINU, MULT, DIV};
vector<MyEnum> rparent_next = {LBRACE, RBRACE, PLUS, MINU, MULT, DIV, RBRACK, RPARENT, SEMICN, EQL, NEQ, GEQ, GRE, LEQ, LSS, COMMA, IFTK, WHILETK, DOTK, FORTK, IDENFR, SCANFTK, PRINTFTK, RETURNTK};
vector<MyEnum> rbrack_next = {LBRACE, PLUS, MINU, MULT, DIV, RBRACK, RPARENT, SEMICN, EQL, NEQ, GEQ, GRE, LEQ, LSS, COMMA, ASSIGN};
vector<MyEnum> semicn_next = {CONSTTK, INTTK, CHARTK, VOIDTK, ELSETK, WHILETK, RBRACE, IFTK, DOTK, FORTK, LBRACE, IDENFR, SCANFTK, PRINTFTK, RETURNTK, SEMICN, PLUS, MINU, CHARTK, INTTK, LPARENT};
vector<MyEnum> state_next = {ELSETK, WHILETK, RBRACE};
vector<MyEnum> state_begin = {IFTK, WHILETK, DOTK, FORTK, LBRACE, IDENFR, SCANFTK, PRINTFTK, RETURNTK, SEMICN};

int return_count = 0;
char now_type[TYPE_LEN];

void add_return_name(char *fun_name);
void add_non_return_name(char *fun_name);
int is_return_fun(char *fun_name);
void idenfr(char *name);
void non_sign_integer(int *non_sign_integer);
void integer(int *addr);
void my_char(char *char_addr);
void my_string(char *string);
void const_define();
void const_clarify();
void type_idfr(char *type);
void var_define();
void var_clarify();
void para_table();
void mult_op(char *op_addr);
void add_op(char *op_addr);
int64_t term(char *term_type);
int64_t expression(char *expres_type);
void value_para_table();
void return_fun_call();
void non_return_fun_call();
int64_t factor(char *factor_type);
MyEnum relation_op();
int64_t condition();
void condition_state();
void stride(int *value);
void recur_state();
void assign();
void scanf_state();
void printf_state();
void return_state();
void state();
void state_list();
void composite_state();
void non_return_fun();
void define_head(char *name, char *type);
void return_fun();
void fun_define();
void main_program();
void programs();
void in_table(entry ety, int location);
void out_table(int location);
int search_table(char *name, char *type);
void print_table();
void printf_inter();
void get_tmp_name(char *tmp_name);
int64_t in_const_table(entry ety)
{
	for (int i = 0; i < const_table.size(); i++)
	{
		if (strcmp(ety.type, CHAR) == 0)
		{
			if (const_table[i].char_value == ety.char_value)
			{
				return (int64_t)&const_table[i];
			}
		}
		else
		{
			if (const_table[i].int_value == ety.int_value)
			{
				return (int64_t)&const_table[i];
			}
		}
	}
	const_table.push_back(ety);
	return (int64_t)&const_table.back();
}
int64_t in_string_table(entry ety)
{
	string_table.push_back(ety);
	return (int64_t)&string_table.back();
}
int64_t in_label_table(entry ety)
{
	label_table.push_back(ety);
	return (int64_t)&label_table.back();
}
void print_string_table()
{
	for (int i = 0; i < string_table.size(); i++)
	{
		printf("%s %s\n", string_table[i].name, string_table[i].string);
	}
}
void print_const_table()
{
	for (int i = 0; i < const_table.size(); i++)
	{
		if (strcmp(const_table[i].type, CHAR) == 0)
		{
			printf("'%c'\n", const_table[i].char_value);
		}
		else
		{
			printf("%d\n", const_table[i].int_value);
		}
	}
}
void syntax_and_inter()
{
	//printf("%x\n",ic.capacity());
	sb.reserve(VECTOR_INIT_SIZE);
	ic.reserve(VECTOR_INIT_SIZE);
	string_table.reserve(VECTOR_INIT_SIZE);
	const_table.reserve(VECTOR_INIT_SIZE);
	label_table.reserve(VECTOR_INIT_SIZE);
	//printf("%x\n",ic.capacity());

	int length = 0;
	ffp = fopen("output.txt", "w");
	err = fopen("error_fake.txt", "w");
	inter_file = fopen("intermediate.txt", "w");
	memset(program,0,MAXX);
	//printf("%s\n",n_program);
	//strcpy(program,n_program);
	read_file((char *)"testfile.txt", program, &length);
	
	in_string_table(entry((char *)"\\n"));

	get_and_print(ffp);
	programs();
	//printf("1");
	//printf_inter(); //PRINT intermeidate code to intermediate.txt

	//print_table(); // print symbol table to stdout
	printf("\n");
	//print_const_table(); // print immediate value to stdout
	printf("\n");
	//print_string_table(); // print string to stdout
}

void get_tmp_name(char *tmp_name)
{
	char name[NAME_LEN];
	char tmp[NAME_LEN];
	strcpy(name, "t");
	sprintf(tmp, "%d", temp_var_index);
	//itoa(temp_var_index, tmp, 10);
	temp_var_index++;
	strcat(name, tmp);
	strcpy(tmp_name, name);
}
void get_string_name(char *string_name)
{
	char name[NAME_LEN];
	char tmp[NAME_LEN];
	strcpy(name, "string_");
	sprintf(tmp, "%d", temp_str_index);
	//itoa(temp_str_index, tmp, 10);
	temp_str_index++;
	strcat(name, tmp);
	strcpy(string_name, name);
}
void get_label_name(char *label_name)
{
	char name[NAME_LEN];
	char tmp[NAME_LEN];
	strcpy(name, "label_");
	sprintf(tmp, "%d", temp_label_index);
	temp_label_index++;
	strcat(name, tmp);
	strcpy(label_name, name);
}
void printf_inter(vector<inter_code> ic, FILE *inter_file)
{

	for (int i = 0; i < ic.size(); i++)
	{
		if (true)
		{	fprintf(inter_file,"%d :",ic[i].inter_type);
			switch (ic[i].inter_type)
			{
			case CONST_DEFINE:
				if (strcmp(ic[i].x->type, CHAR) == 0)
				{
					fprintf(inter_file, "const char %s = '%c'\n", ic[i].x->name, ic[i].x->char_value);
				}
				else
				{
					fprintf(inter_file, "const int %s = %d\n", ic[i].x->name, ic[i].x->int_value);
				}
				break;
			case VAR_DEFINE:
				if (strcmp(ic[i].x->type, CHAR) == 0)
				{
					fprintf(inter_file, "var char %s\n", ic[i].x->name);
				}
				else
				{
					fprintf(inter_file, "var int %s\n", ic[i].x->name);
				}
				break;
			case FUN_DEFINE:
				if (strcmp(ic[i].x->type, CHAR) == 0)
				{
					fprintf(inter_file, "\nfun char %s()\n", ic[i].x->name);
				}
				else if (strcmp(ic[i].x->type, INT) == 0)
				{
					fprintf(inter_file, "\nfun int %s()\n", ic[i].x->name);
				}
				else
				{
					fprintf(inter_file, "\nfun void %s()\n", ic[i].x->name);
				}
				break;
			case MAIN_DEFINE:
				fprintf(inter_file, "\nfun void main()\n");
				break;
			case PARA_DEFINE:
				if (strcmp(ic[i].x->type, CHAR) == 0)
				{
					fprintf(inter_file, "para char %s\n", ic[i].x->name);
				}
				else
				{
					fprintf(inter_file, "para int %s\n", ic[i].x->name);
				}
				break;
			case PARA_PUSH:
				if (ic[i].x->is_immediate == true)
				{
					if (strcmp(ic[i].x->type, INT) == 0)
					{
						fprintf(inter_file, "push %d\n", ic[i].x->int_value);
					}
					else
					{
						fprintf(inter_file, "push '%c'\n", ic[i].x->char_value);
					}
				}
				else
				{
					fprintf(inter_file, "push %s\n", ic[i].x->name);
				}
				break;
			case FUN_CALL:
				fprintf(inter_file, "call %s\n", ic[i].x->name);
				break;
			case ASSIGN_WITHOUT_ARRAY:
				if (ic[i].z == NULL)
				{
					if (ic[i].y->is_immediate == true || strcmp(ic[i].y->category, CONST) == 0)
						if (strcmp(ic[i].y->type, INT) == 0)
						{
							fprintf(inter_file, "%s = %d\n", ic[i].x->name, ic[i].y->int_value);
						}
						else
						{
							fprintf(inter_file, "%s = '%c'\n", ic[i].x->name, ic[i].y->char_value);
						}
					else
						fprintf(inter_file, "%s = %s\n", ic[i].x->name, ic[i].y->name);
				}
				else
				{
					//printf("2\n");
					if (ic[i].y->is_immediate == true || strcmp(ic[i].y->category, CONST) == 0)
					{
						//	printf("4\n");
						if (strcmp(ic[i].y->type, INT) == 0)
						{
							if (ic[i].z->is_immediate == true || strcmp(ic[i].z->category, CONST) == 0)
								if (strcmp(ic[i].z->type, INT) == 0)
								{
									fprintf(inter_file, "%s = %d %c %d\n", ic[i].x->name, ic[i].y->int_value, ic[i].op, ic[i].z->int_value);
								}
								else
								{
									fprintf(inter_file, "%s = %d %c '%c'\n", ic[i].x->name, ic[i].y->int_value, ic[i].op, ic[i].z->char_value);
								}
							else
								fprintf(inter_file, "%s = %d %c %s\n", ic[i].x->name, ic[i].y->int_value, ic[i].op, ic[i].z->name);
						}
						else
						{
							if (ic[i].z->is_immediate == true || strcmp(ic[i].z->category, CONST) == 0)
								if (strcmp(ic[i].z->type, INT) == 0)
								{
									fprintf(inter_file, "%s = '%c' %c %d\n", ic[i].x->name, ic[i].y->char_value, ic[i].op, ic[i].z->int_value);
								}
								else
								{
									fprintf(inter_file, "%s = '%c' %c '%c'\n", ic[i].x->name, ic[i].y->char_value, ic[i].op, ic[i].z->char_value);
								}
							else
								fprintf(inter_file, "%s = '%c' %c %s\n", ic[i].x->name, ic[i].y->char_value, ic[i].op, ic[i].z->name);
						}
					}
					else
					{
						//	printf("3\n");
						if (ic[i].z->is_immediate == true || strcmp(ic[i].z->category, CONST) == 0)
							if (strcmp(ic[i].z->type, INT) == 0)
							{
								fprintf(inter_file, "%s = %s %c %d\n", ic[i].x->name, ic[i].y->name, ic[i].op, ic[i].z->int_value);
							}
							else
							{
								fprintf(inter_file, "%s = %s %c '%c'\n", ic[i].x->name, ic[i].y->name, ic[i].op, ic[i].z->char_value);
							}

						else
							fprintf(inter_file, "%s = %s %c %s\n", ic[i].x->name, ic[i].y->name, ic[i].op, ic[i].z->name);
					}
				}
				break;
			case ASSIGN_WITH_ARRAY:
				if (ic[i].z->is_immediate == true || strcmp(ic[i].z->category, CONST) == 0)
					fprintf(inter_file, "%s = %s[%d]\n", ic[i].x->name, ic[i].y->name, ic[i].z->int_value);
				else
					fprintf(inter_file, "%s = %s[%s]\n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
				break;
			case ASSIGN_AN_ARRAY:
				if (ic[i].y->is_immediate == true || strcmp(ic[i].y->category, CONST) == 0)
				{
					if (ic[i].z->is_immediate == true || strcmp(ic[i].z->category, CONST) == 0)
						if (strcmp(ic[i].z->type, INT) == 0)
						{
							fprintf(inter_file, "%s[%d] = %d\n", ic[i].x->name, ic[i].y->int_value, ic[i].z->int_value);
						}
						else
						{
							fprintf(inter_file, "%s[%d] = '%c'\n", ic[i].x->name, ic[i].y->int_value, ic[i].z->char_value);
						}
					else
						fprintf(inter_file, "%s[%d] = %s\n", ic[i].x->name, ic[i].y->int_value, ic[i].z->name);
				}
				else
				{
					if (ic[i].z->is_immediate == true || strcmp(ic[i].z->category, CONST) == 0)
						if (strcmp(ic[i].z->type, INT) == 0)
						{
							fprintf(inter_file, "%s[%s] = %d\n", ic[i].x->name, ic[i].y->name, ic[i].z->int_value);
						}
						else
						{
							fprintf(inter_file, "%s[%s] = '%c'\n", ic[i].x->name, ic[i].y->name, ic[i].z->char_value);
						}
					else
						fprintf(inter_file, "%s[%s] = %s\n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
				}
				break;
			case PRINTF:
				if (ic[i].x->is_string == true)
				{
					fprintf(inter_file, "printf %s\n", ic[i].x->name);
				}
				else if (ic[i].x->is_immediate == true)
				{
					if (strcmp(ic[i].x->type, CHAR) == 0)
						fprintf(inter_file, "printf '%c'\n", ic[i].x->char_value);
					else
						fprintf(inter_file, "printf %d\n", ic[i].x->int_value);
				}
				else
				{
					fprintf(inter_file, "printf %s\n", ic[i].x->name);
				}
				break;
			case SCANF:
				fprintf(inter_file, "scanf %s\n", ic[i].x->name);
				break;
			case SET:
				fprintf(inter_file, "%s:\n", ic[i].x->name);
				break;
			case JUMP:
				fprintf(inter_file, "jump %s\n", ic[i].x->name);
				break;
			case _BEQ:
				if (strcmp(ic[i].x->category, CONST) == 0 || ic[i].x->is_immediate)
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "beq %d %d %s \n", ic[i].x->int_value, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "beq %d %s %s \n", ic[i].x->int_value, ic[i].y->name, ic[i].z->name);
					}
				}
				else
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "beq %s %d %s \n", ic[i].x->name, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "beq %s %s %s \n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
					}
				}
				break;
			case _BNE:
				if (strcmp(ic[i].x->category, CONST) == 0 || ic[i].x->is_immediate)
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bne %d %d %s \n", ic[i].x->int_value, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bne %d %s %s \n", ic[i].x->int_value, ic[i].y->name, ic[i].z->name);
					}
				}
				else
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bne %s %d %s \n", ic[i].x->name, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bne %s %s %s \n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
					}
				}
				break;
			case _LSS:
				if (strcmp(ic[i].x->category, CONST) == 0 || ic[i].x->is_immediate)
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bltz %d %d %s \n", ic[i].x->int_value, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bltz %d %s %s \n", ic[i].x->int_value, ic[i].y->name, ic[i].z->name);
					}
				}
				else
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bltz %s %d %s \n", ic[i].x->name, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bltz %s %s %s \n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
					}
				}
				break;
			case _LEQ:
				if (strcmp(ic[i].x->category, CONST) == 0 || ic[i].x->is_immediate)
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "blez %d %d %s \n", ic[i].x->int_value, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "blez %d %s %s \n", ic[i].x->int_value, ic[i].y->name, ic[i].z->name);
					}
				}
				else
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "blez %s %d %s \n", ic[i].x->name, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "blez %s %s %s \n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
					}
				}
				break;
			case _GRE:
				if (strcmp(ic[i].x->category, CONST) == 0 || ic[i].x->is_immediate)
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bgtz %d %d %s \n", ic[i].x->int_value, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bgtz %d %s %s \n", ic[i].x->int_value, ic[i].y->name, ic[i].z->name);
					}
				}
				else
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bgtz %s %d %s \n", ic[i].x->name, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bgtz %s %s %s \n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
					}
				}
				break;
			case _GEQ:
				if (strcmp(ic[i].x->category, CONST) == 0 || ic[i].x->is_immediate)
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bgez %d %d %s \n", ic[i].x->int_value, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bgez %d %s %s \n", ic[i].x->int_value, ic[i].y->name, ic[i].z->name);
					}
				}
				else
				{
					if (strcmp(ic[i].y->category, CONST) == 0 || ic[i].y->is_immediate)
					{
						fprintf(inter_file, "bgez %s %d %s \n", ic[i].x->name, ic[i].y->int_value, ic[i].z->name);
					}
					else
					{
						fprintf(inter_file, "bgez %s %s %s \n", ic[i].x->name, ic[i].y->name, ic[i].z->name);
					}
				}
				break;
			case RET:
				if (ic[i].x != NULL)
				{
					if (ic[i].x->is_immediate || strcmp(ic[i].x->category, CONST) == 0)
					{
						if (strcmp(ic[i].x->type, INT) == 0)
						{
							fprintf(inter_file, "return %d\n", ic[i].x->int_value);
						}
						else
						{
							fprintf(inter_file, "return '%c'\n", ic[i].x->char_value);
						}
					}
					else
					{
						fprintf(inter_file, "return %s\n", ic[i].x->name);
					}
				}
				else
				{
					fprintf(inter_file, "return;\n");
				}
				break;
			case ASSIGN_RET:
				fprintf(inter_file, "%s = RET\n", ic[i].x->name);
				break;
			case END_OF_FUN:
				fprintf(inter_file, "END_OF_FUN\n");
				break;
			case START_CALL:
				fprintf(inter_file, "start call %s\n", ic[i].x->name);
				break;
			default:
				break;
			}
		}
	}
	fprintf(inter_file, "\n");
}
/*inter_code::inter_code(Inter inter_type, entry *x, char op = '_', entry *y = NULL, entry *z = NULL)
{
	this->inter_type = inter_type;
	this->x = x;
	this->y = y;
	this->z = z;
	this->op = op;
}*/

void add_inter_code(inter_code code)
{

	ic.push_back(code);
	//fprintf(inter_file,"%x\n",&ic.back());
	//	printf_inter();
}
entry::entry(char *category, char *type, char *name, int level, int value = 0, int dimension = 0)
{

	strcpy(this->category, category);
	strcpy(this->type, type);
	strcpy(this->name, name);
	this->level = level;
	this->int_value = value;
	this->dimension = dimension;
}
entry::entry(char *category, char *type, char *name, int level, char value = ' ', int dimension = 0)
{
	strcpy(this->category, category);
	strcpy(this->type, type);
	strcpy(this->name, name);
	this->level = level;
	this->char_value = value;
	this->dimension = dimension;
}
entry::entry(int immediate)
{
	strcpy(this->category, "const_num");
	strcpy(this->name, "no_name");
	this->int_value = immediate;
	this->is_immediate = true;
	strcpy(this->type, INT);
	this->level = 2;
}
entry::entry(char immediate)
{
	strcpy(this->category, "const_ch");
	strcpy(this->name, "no_name");
	this->char_value = immediate;
	this->is_immediate = true;
	strcpy(this->type, CHAR);
	this->level = 2;
}
entry::entry(char *string)
{
	strcpy(this->string, string);
	this->is_string = true;
	char string_name[NAME_LEN];
	get_string_name(string_name);
	strcpy(this->name, string_name);
}
entry::entry()
{
	char label_name[NAME_LEN];
	get_label_name(label_name);
	strcpy(name, label_name);
	this->is_label = true;
}
void in_table(entry ety, int location)
{

	/*for (int i = sb.size() - 1; i >= location; i--)
	{
		if (strcmp(sb[i].name, ety.name) == 0 && sb[i].is_effective)
		{
			//printf("%s %s %d  ",sb[i].name,sb[i].type,sb[i].level);
			//printf("%s %s %d\n",ety.name,ety.type,ety.level);
			fprintf(err, "%d b\n", line);
			return;
		}
	}*/
	//printf("%x\n",&sb.front());

	sb.push_back(ety);
}
void out_table(int location)
{
	int tmp = 0;
	for (int i = sb.size() - 1; i >= location; i--)
	{
		if (strcmp(sb[i].category, PARA) == 0)
		{
			tmp++;
			if (strcmp(sb[i - 1].category, FUN) == 0)
				sb[i - 1].para_num = tmp;
		}
		else
		{
		}
	}
}
int search_table(char *name, char *category)
{

	if (strcmp(category, (char *)"fun") == 0)
	{
		for (int i = sb.size() - 1; i >= 0; i--)
		{
			if (sb[i].is_effective == true && strcmp(sb[i].name, name) == 0 && strcmp(sb[i].category, (char *)"fun") == 0 && sb[i].is_tmp == false)
			{
				return i;
			}
		}
	}
	else
	{
		int i;
		for (i = sb.size() - 1; i >= 0; i--)
		{
			if (sb[i].is_effective == true && strcmp(sb[i].name, name) == 0 && strcmp(sb[i].category, (char *)"fun") != 0 && sb[i].is_tmp == false)
			{
				return i;
			}
			if (strcmp(sb[i].category, FUN) == 0)
			{
				break;
			}
		}
		for (int j = i; j >= 0; j--)
		{

			if (sb[j].level == 1 && sb[j].is_effective == true && strcmp(sb[j].name, name) == 0 && strcmp(sb[j].category, (char *)"fun") != 0 && sb[i].is_tmp == false)
			{
				return j;
			}
		}
	}
	print_table();
	printf("%s\n", name);
	fprintf(err, "%d c\n", line);
	exit(0);
}
void print_table()
{
	for (int i = 0; i < sb.size(); i++)
	{
		printf("%s %s %s %d %d\n", sb[i].category, sb[i].type, sb[i].name, sb[i].level, sb[i].offset_to_gp);
	}
	printf("\n");
}
void add_return_name(char *fun_name)
{
	strcat(return_fun_name[return_fun_index], fun_name);
	return_fun_index++;
}
void add_non_return_name(char *fun_name)
{
	strcat(non_return_fun_name[non_return_fun_index++], fun_name);
}

int is_return_fun(char *fun_name)
{
	int i = 0;
	for (i = 0; i < return_fun_index; i++)
	{
		if (strcmp(return_fun_name[i], fun_name) == 0)
		{
			return 1;
		}
	}
	return 0;
}
void idenfr(char *name)
{
	if (g_symbol != IDENFR)
	{
		printf("error in idenfr\n");
		error();
	}
	strcpy(name, g_token);
}

void non_sign_integer(int *non_sign_value_addr)
{
	if (g_symbol != INTCON)
	{
		fprintf(err, "%d o\n", line);
	}
	else
	{
		*non_sign_value_addr = atoi(g_token);
	}
	fprintf(ffp, "<无符号整数>\n");
}

void integer(int *value_addr)
{
	int sign = 1;
	int non_sign_value;
	if (g_symbol == PLUS || g_symbol == MINU)
	{
		if (g_symbol == PLUS)
			sign = 1;
		else
			sign = -1;
		get_and_print(ffp);
	}
	non_sign_integer(&non_sign_value);
	*value_addr = sign == 1 ? non_sign_value : -non_sign_value;
	//printf("%d\n",sign);
	fprintf(ffp, "<整数>\n");
}
void my_char(char *char_addr)
{
	if (g_symbol != CHARCON)
	{
		fprintf(err, "%d o\n", line);
		//error();
	}
	strcpy(char_addr, g_token);
}

void my_string(char *string)
{
	if (g_symbol != STRCON)
	{
		error();
	}
	strcpy(string, g_token);
	fprintf(ffp, "<字符串>\n");
}
void const_define()
{

	char type[TYPE_LEN];
	char category[CATEGORY_LEN];
	char name[NAME_LEN];

	if (g_symbol == INTTK)
	{
		int int_value;
		strcpy(type, (char *)"int");

		get_and_print(ffp);

		idenfr(name);

		get_and_print(ffp);
		if (g_symbol != ASSIGN)
		{
			printf("error in const define(1)!\n  %s", g_token);
			error();
		}

		get_and_print(ffp);
		integer(&int_value);

		in_table(entry(CONST, type, name, level, int_value), index_stack.back());
		add_inter_code(inter_code(CONST_DEFINE, &sb.back()));

		pre_read(1);
		while (pre_symbol == COMMA)
		{
			get_and_print(ffp); //g_token is ","

			get_and_print(ffp);

			idenfr(name);

			get_and_print(ffp);
			if (g_symbol != ASSIGN)
			{
				printf("error in const define(2)!\n");
				error();
			}

			get_and_print(ffp);
			integer(&int_value);

			in_table(entry(CONST, type, name, level, int_value), index_stack.back());
			add_inter_code(inter_code(CONST_DEFINE, &sb.back()));

			pre_read(1);
		}
	}
	else if (g_symbol == CHARTK)
	{
		char char_value;
		strcpy(type, (char *)"char");

		get_and_print(ffp);

		idenfr(name);

		get_and_print(ffp);
		if (g_symbol != ASSIGN)
		{
			printf("error in const define!(3)\n");
			error();
		}

		get_and_print(ffp);
		my_char(&char_value);

		pre_read(1);

		in_table(entry(CONST, type, name, level, char_value), index_stack.back());
		add_inter_code(inter_code(CONST_DEFINE, &sb.back()));

		while (pre_symbol == COMMA)
		{
			get_and_print(ffp); // g_token is ","

			get_and_print(ffp);

			idenfr(name);

			get_and_print(ffp);
			if (g_symbol != ASSIGN)
			{
				printf("error in const define(4)!\n");
				error();
			}

			get_and_print(ffp);
			my_char(&char_value);

			in_table(entry(CONST, type, name, level, char_value), index_stack.back());
			add_inter_code(inter_code(CONST_DEFINE, &sb.back()));

			pre_read(1);
		}
	}
	else
	{
		printf("error in const define(5)!\n");
		error();
	}
	fprintf(ffp, "<常量定义>\n");
}
void const_clarify()
{
	if (g_symbol != CONSTTK)
	{
		printf("error in const_clarify(1)/n");
		error();
	}
	get_and_print(ffp);

	const_define();
	get_and_print(ffp);

	if (g_symbol != SEMICN)
	{
		printf("error in const_clarify(2)/n");
		error();
	}
	pre_read(1);
	while (pre_symbol == CONSTTK)
	{

		get_and_print(ffp); // g_token is "const"
		get_and_print(ffp);

		const_define();
		get_and_print(ffp);

		if (g_symbol != SEMICN)
		{
			printf("error in const_clarify(3)/n");
			error();
		}
		pre_read(1);
	}
	fprintf(ffp, "<常量说明>\n");
}
void type_idfr(char *type)
{
	if (g_symbol != INTTK && g_symbol != CHARTK)
	{
		printf("error in type_idfr\n");
		error();
	}
	if (g_symbol == INTTK)
		strcpy(type, INT);
	else
		strcpy(type, CHAR);
}
void var_define()
{
	char type[TYPE_LEN];
	char category[CATEGORY_LEN];
	char name[NAME_LEN];
	int dimension = 0;

	type_idfr(type);

	get_and_print(ffp);
	idenfr(name);

	pre_read(1);

	if (pre_symbol == LBRACK)
	{
		get_and_print(ffp); //g_token is "["
		get_and_print(ffp);
		non_sign_integer(&dimension);
		if (dimension == 0)
			fprintf(err, "%d i\n", line);

		get_and_print(ffp);
		if (g_symbol != RBRACK)
		{
			printf("error in var_define(1)\n");
			error();
		}
		in_table(entry(VAR, type, name, level, 0, dimension), index_stack.back());
		add_inter_code(inter_code(VAR_DEFINE, &sb.back()));
	}
	else
	{

		in_table(entry(VAR, type, name, level, 0, 0), index_stack.back());

		add_inter_code(inter_code(VAR_DEFINE, &sb.back()));
	}

	pre_read(1);

	while (pre_symbol == COMMA)
	{
		get_and_print(ffp); //g_token is ","
		get_and_print(ffp);
		idenfr(name);

		pre_read(1);

		if (pre_symbol == LBRACK)
		{
			get_and_print(ffp); //g_token is "["
			get_and_print(ffp);
			dimension = 0;
			non_sign_integer(&dimension);
			if (dimension == 0)
				fprintf(err, "%d i\n", line);

			get_and_print(ffp);
			if (g_symbol != RBRACK)
			{
				printf("error in var_define(2)\n");
				error();
			}
			in_table(entry(VAR, type, name, level, 0, dimension), index_stack.back());
			add_inter_code(inter_code(VAR_DEFINE, &sb.back()));
		}
		else
		{
			in_table(entry(VAR, type, name, level, 0, 0), index_stack.back());
			add_inter_code(inter_code(VAR_DEFINE, &sb.back()));
		}
		pre_read(1);
	}
	fprintf(ffp, "<变量定义>\n");
}
void var_clarify()
{

	var_define();
	get_and_print(ffp);
	if (g_symbol != SEMICN)
	{
		printf("error in var_clarify(1)\n %s", g_token);
		error();
	}

	pre_read(1);
	while (pre_symbol == INTTK || pre_symbol == CHARTK)
	{
		pre_read(3);
		if (pre_symbol != LPARENT)
		{

			get_and_print(ffp);
			var_define();

			get_and_print(ffp);
			if (g_symbol != SEMICN)
			{
				printf("error in var_clarify(2)\n");
				error();
			}
		}
		else
		{
			break;
		}
		pre_read(1);
	}
	fprintf(ffp, "<变量说明>\n");
}

void para_table()
{
	if (g_symbol != NOTHING)
	{
		char type[TYPE_LEN];
		char name[NAME_LEN];
		//printf("%s\n", g_token);
		type_idfr(type);

		get_and_print(ffp);

		idenfr(name);

		in_table(entry(PARA, type, name, level, 0, 0), index_stack.back());
		add_inter_code(inter_code(PARA_DEFINE, &sb.back()));

		pre_read(1);
		while (pre_symbol == COMMA)
		{
			get_and_print(ffp);
			get_and_print(ffp);
			type_idfr(type);
			get_and_print(ffp);
			idenfr(name);

			in_table(entry(PARA, type, name, level, 0, 0), index_stack.back());
			add_inter_code(inter_code(PARA_DEFINE, &sb.back()));

			pre_read(1);
		}
	}
	fprintf(ffp, "<参数表>\n");
}
void mult_op(char *op_addr)
{
	if (g_symbol == MULT || g_symbol == DIV)
	{
		if (g_symbol == MULT)
			*op_addr = '*';
		else
			*op_addr = '/';
	}
	else
	{
		error();
	}
}
void add_op(char *op)
{
	if (g_symbol == PLUS || g_symbol == MINU)
	{
		if (g_symbol == PLUS)
			*op = '+';
		else
			*op = '-';
	}
	else
	{
		error();
	}
}
int64_t term(char *term_type)
{
	char factor_type[TYPE_LEN];

	int64_t factor1_addr = factor(factor_type);

	strcpy(term_type, factor_type);

	pre_read(1);
	while (pre_symbol == MULT || pre_symbol == DIV)
	{
		char tmp_name[NAME_LEN];
		get_tmp_name(tmp_name);
		in_table(entry(VAR, INT, tmp_name, level, 0, 0), index_stack.back());

		int64_t tmp_addr = (int64_t)&sb.back();
		sb.back().is_tmp = true;

		strcpy(term_type, (char *)"int");
		get_and_print(ffp);
		char op;
		mult_op(&op);
		get_and_print(ffp);
		int64_t factor2_addr = factor(factor_type);

		add_inter_code(inter_code(ASSIGN_WITHOUT_ARRAY, (entry *)tmp_addr, op, (entry *)factor1_addr, (entry *)factor2_addr));
		factor1_addr = tmp_addr;
		pre_read(1);
	}
	fprintf(ffp, "<项>\n");

	return factor1_addr;
}
int64_t expression(char *expres_type)
{
	int sign = 1;
	if (g_symbol == PLUS || g_symbol == MINU)
	{
		if (g_symbol == MINU)
			sign = -1;
		get_and_print(ffp);
		strcpy(expres_type, (char *)"int");
	}

	char term_type[TYPE_LEN];
	int64_t term1_addr = term(term_type);
	if (sign == -1)
	{
		int64_t minus_addr = in_const_table(-1);
		char tmp_name[NAME_LEN];
		get_tmp_name(tmp_name);

		in_table(entry(VAR, INT, tmp_name, level, 0, 0), index_stack.back());

		int64_t tmp_addr = (int64_t)&sb.back();
		sb.back().is_tmp = true;
		add_inter_code(inter_code(ASSIGN_WITHOUT_ARRAY, (entry *)tmp_addr, '*', (entry *)term1_addr, (entry *)minus_addr));
		term1_addr = tmp_addr;
	}

	if (strcmp(expres_type, "int") != 0)
		strcpy(expres_type, term_type);

	pre_read(1);

	while (pre_symbol == PLUS || pre_symbol == MINU)
	{
		char tmp_name[NAME_LEN];
		get_tmp_name(tmp_name);

		in_table(entry(VAR, INT, tmp_name, level, 0, 0), index_stack.back());

		int64_t tmp_addr = (int64_t)&sb.back();
		sb.back().is_tmp = true;

		strcpy(expres_type, (char *)"int");
		get_and_print(ffp);
		char op;
		add_op(&op);
		get_and_print(ffp);
		int64_t term2_addr = term(term_type);

		add_inter_code(inter_code(ASSIGN_WITHOUT_ARRAY, (entry *)tmp_addr, op, (entry *)term1_addr, (entry *)term2_addr));

		term1_addr = tmp_addr;
		pre_read(1);
	}

	fprintf(ffp, "<表达式>\n");
	return term1_addr;
}

void value_para_table()
{
	if (g_symbol != NOTHING)
	{
		char ex_type[TYPE_LEN];
		int64_t ety_addr = expression(ex_type);
		add_inter_code(inter_code(PARA_PUSH, (entry *)ety_addr));
		pre_read(1);
		while (pre_symbol == COMMA)
		{
			get_and_print(ffp); //g_token is ,
			get_and_print(ffp);

			char ex_type[TYPE_LEN];
			int64_t ety_addr = expression(ex_type);
			add_inter_code(inter_code(PARA_PUSH, (entry *)ety_addr));

			pre_read(1);
		}
	}
	fprintf(ffp, "<值参数表>\n");
}
void return_fun_call()
{

	char name[NAME_LEN];
	int n;
	idenfr(name);
	n = search_table(name, FUN);

	entry &ety = sb[n];
	add_inter_code(inter_code(START_CALL, &ety));
	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		error();
	}
	pre_read(1);
	if (pre_symbol == RPARENT)
	{
		set_nothing();
	}
	else
	{
		get_and_print(ffp);
	}
	value_para_table();

	get_and_print(ffp);

	if (g_symbol != RPARENT)
	{
		error();
	}
	fprintf(ffp, "<有返回值函数调用语句>\n");
	add_inter_code(inter_code(FUN_CALL, &ety));
}
void non_return_fun_call()
{
	char name[NAME_LEN];
	int n;
	idenfr(name);
	n = search_table(name, FUN);
	entry &ety = sb[n];
	add_inter_code(inter_code(START_CALL, &ety));
	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		error();
	}
	pre_read(1);
	if (pre_symbol == RPARENT)
	{
		set_nothing();
	}
	else
	{
		get_and_print(ffp);
	}
	value_para_table();

	get_and_print(ffp);

	if (g_symbol != RPARENT)
	{
		error();
	}

	fprintf(ffp, "<无返回值函数调用语句>\n");
	add_inter_code(inter_code(FUN_CALL, &ety));
}

int64_t factor(char *factor_type)
{

	if (g_symbol == IDENFR)
	{
		char name[NAME_LEN];
		int n;
		idenfr(name);

		pre_read(1);
		if (pre_symbol == LBRACK)
		{

			n = search_table(name, EMPTY);
			if (n >= 0)
				strcpy(factor_type, sb[n].type);
			entry &ety = sb[n];

			char tmp_name[NAME_LEN];
			get_tmp_name(tmp_name);
			in_table(entry(VAR, sb[n].type, tmp_name, level, 0, 0), index_stack.back());
			int64_t tmp_addr = (int64_t)&sb.back();
			sb.back().is_tmp = true;

			get_and_print(ffp); //g_token is [
			get_and_print(ffp);

			char ex_type[TYPE_LEN];
			int64_t offset_addr = expression(ex_type);

			add_inter_code(inter_code(ASSIGN_WITH_ARRAY, (entry *)tmp_addr, '_', &ety, (entry *)offset_addr));

			get_and_print(ffp);
			if (g_symbol != RBRACK)
			{
				error();
			}
			return tmp_addr;
		}
		else if (pre_symbol == LPARENT)
		{
			n = search_table(name, FUN);
			if (n >= 0)
				strcpy(factor_type, sb[n].type);
			return_fun_call();

			char tmp_name[NAME_LEN];
			get_tmp_name(tmp_name);
			in_table(entry(VAR, sb[n].type, tmp_name, level, 0, 0), index_stack.back());
			int64_t tmp_addr = (int64_t)&sb.back();
			sb.back().is_tmp = true;

			add_inter_code(inter_code(ASSIGN_RET, (entry *)tmp_addr));
			return tmp_addr;
		}
		else
		{
			//printf("%s\n", name);
			n = search_table(name, EMPTY);

			if (n >= 0)
				strcpy(factor_type, sb[n].type);
			entry &ety = sb[n];
			return (int64_t)&ety;
		}
	}
	else if (g_symbol == LPARENT)
	{
		strcpy(factor_type, INT);
		get_and_print(ffp);

		char ex_type[TYPE_LEN];
		int64_t factor_addr = expression(ex_type);

		char tmp_name[NAME_LEN];
		get_tmp_name(tmp_name);
		in_table(entry(VAR, INT, tmp_name, level, 0, 0), index_stack.back());
		int64_t tmp_addr = (int64_t)&sb.back();
		sb.back().is_tmp = true;

		add_inter_code(inter_code(ASSIGN_WITHOUT_ARRAY, (entry *)tmp_addr, '=', (entry *)factor_addr));

		get_and_print(ffp);
		if (g_symbol != RPARENT)
		{
			error();
		}
		return tmp_addr;
	}

	else if (g_symbol == CHARCON)
	{
		strcpy(factor_type, CHAR);
		char char_value;
		my_char(&char_value);

		int64_t addr = in_const_table(entry(char_value));
		//in_table(entry(char_value), index_stack.back());
		return addr;
	}
	else if (g_symbol == INTCON || g_symbol == PLUS || g_symbol == MINU)
	{
		strcpy(factor_type, INT);
		int int_value;
		integer(&int_value);
		int64_t addr = in_const_table(entry(int_value));
		//in_table(entry(int_value), index_stack.back());
		return addr;
	}
	else
	{
		error();
	}
	fprintf(ffp, "<因子>\n");
	//else if(g_symbol == NOTHING|| g_symbol ==ERROR/*|| g_symbol == STRCON|| g_symbol == RBRACE|| g_symbol == LBRACE|| g_symbol == RBRACK|| g_symbol == LBRACK|| g_symbol == LPARENT|| g_symbol == RPARENT|| g_symbol == COMMA*/|| g_symbol == SEMICN) { }
}

MyEnum relation_op()
{
	if (g_symbol == LSS ||
		g_symbol == LEQ ||
		g_symbol == GRE ||
		g_symbol == GEQ ||
		g_symbol == EQL ||
		g_symbol == NEQ)
	{
		return g_symbol;
	}
	else
	{
		printf("error in relation_op\n");
		//error();
	}
}
int64_t condition()
{
	char ex_type[TYPE_LEN];

	int64_t expre1_addr = expression(ex_type);
	int64_t label_addr = in_label_table(entry());
	pre_read(1);
	if (pre_symbol == LSS ||
		pre_symbol == LEQ ||
		pre_symbol == GRE ||
		pre_symbol == GEQ ||
		pre_symbol == EQL ||
		pre_symbol == NEQ)
	{
		get_and_print(ffp);
		//printf("%s\n", g_token);
		MyEnum op_symbol = relation_op();
		get_and_print(ffp);
		char ex_type[TYPE_LEN];
		int64_t expre2_addr = expression(ex_type);
		switch (op_symbol)
		{
		case LSS:
			add_inter_code(inter_code(_LSS, (entry *)expre1_addr, '_', (entry *)expre2_addr, (entry *)label_addr));
			break;
		case LEQ:
			add_inter_code(inter_code(_LEQ, (entry *)expre1_addr, '_', (entry *)expre2_addr, (entry *)label_addr));
			break;
		case GRE:
			add_inter_code(inter_code(_GRE, (entry *)expre1_addr, '_', (entry *)expre2_addr, (entry *)label_addr));
			break;
		case GEQ:
			add_inter_code(inter_code(_GEQ, (entry *)expre1_addr, '_', (entry *)expre2_addr, (entry *)label_addr));
			break;
		case EQL:
			add_inter_code(inter_code(_BEQ, (entry *)expre1_addr, '_', (entry *)expre2_addr, (entry *)label_addr));
			break;
		case NEQ:
			add_inter_code(inter_code(_BNE, (entry *)expre1_addr, '_', (entry *)expre2_addr, (entry *)label_addr));
			break;
		default:
			break;
		}
	}
	else
	{
		int64_t zero_addr = in_const_table(entry(0));
		add_inter_code(inter_code(_BNE, (entry *)expre1_addr, '_', (entry *)zero_addr, (entry *)label_addr));
	}
	fprintf(ffp, "<条件>\n");
	return label_addr;
}
void condition_state()
{
	if (g_symbol != IFTK)
	{
		printf("error in condition_state\n");
		error();
	}
	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		printf("error in condition_state\n");
		error();
	}

	get_and_print(ffp);
	int64_t satisfy_label_addr = condition();
	switch (ic[ic.size() - 1].inter_type)
	{
	case _BEQ:
		ic[ic.size() - 1].inter_type = _BNE;
		break;
	case _BNE:
		ic[ic.size() - 1].inter_type = _BEQ;
		break;
	case _LSS:
		ic[ic.size() - 1].inter_type = _GEQ;
		break;
	case _LEQ:
		ic[ic.size() - 1].inter_type = _GRE;
		break;
	case _GRE:
		ic[ic.size() - 1].inter_type = _LEQ;
		break;
	case _GEQ:
		ic[ic.size() - 1].inter_type = _LSS;
		break;
	default:
		break;
	}
	int64_t unsatisfy_label_addr = in_label_table(entry());
	ic[ic.size() - 1].z  =(entry *)unsatisfy_label_addr;
	//add_inter_code(inter_code(JUMP, (entry *)unsatisfy_label_addr));

	get_and_print(ffp);
	if (g_symbol != RPARENT)
	{
		printf("error in condition_state\n");
		error();
	}

	get_and_print(ffp);
	//add_inter_code(inter_code(SET, (entry *)satisfy_label_addr));
	state();
	pre_read(1);
	if (pre_symbol == ELSETK)
	{
		int64_t out_label_addr = in_label_table(entry());
		add_inter_code(inter_code(JUMP, (entry *)out_label_addr));
		add_inter_code(inter_code(SET, (entry *)unsatisfy_label_addr));
		get_and_print(ffp); //g_token is else
		//printf("%s\n", g_token);
		get_and_print(ffp);
		state();
		add_inter_code(inter_code(SET, (entry *)out_label_addr));
	}
	else
	{
		add_inter_code(inter_code(SET, (entry *)unsatisfy_label_addr));
	}
	fprintf(ffp, "<条件语句>\n");
}
void stride(int *value)
{
	int stride;
	non_sign_integer(&stride);
	*value = stride;
	fprintf(ffp, "<步长>\n");
}
void recur_state()
{

	if (g_symbol == WHILETK)
	{
		int64_t judge_label_addr = in_label_table(entry());
		add_inter_code(inter_code(JUMP, (entry *)judge_label_addr));
		add_inter_code(inter_code(SET, (entry *)judge_label_addr));
		get_and_print(ffp);
		if (g_symbol != LPARENT)
		{
			error();
		}
		get_and_print(ffp);
		int64_t satisfy_label_addr = condition();
		int64_t unsatisfy_label_addr = in_label_table(entry());
		//add_inter_code(inter_code(JUMP, (entry *)unsatisfy_label_addr));
		get_and_print(ffp);
		if (g_symbol != RPARENT)
		{
			error();
		}
		get_and_print(ffp);
		add_inter_code(inter_code(SET, (entry *)satisfy_label_addr));
		state();
		int i;
		for (i = ic.size() - 1; ic[i].x != (entry *)satisfy_label_addr || ic[i].inter_type != SET; i--)
			;
		int j;
		for (j = i - 1; ic[j].x != (entry *)judge_label_addr || ic[j].inter_type != SET; j--)
			;

		for (int k = 0; k < ic.size() - i; k++)
		{
			inter_code tmp = ic[ic.size() - 1];
			ic.erase(ic.begin() + ic.size() - 1);
			ic.insert(ic.begin() + j, tmp);
		}
		//add_inter_code(inter_code(JUMP, (entry *)judge_label_addr));
		//add_inter_code(inter_code(SET, (entry *)unsatisfy_label_addr));
	}
	else if (g_symbol == DOTK)
	{
		int64_t start_label_addr = in_label_table(entry());
		add_inter_code(inter_code(SET, (entry *)start_label_addr));
		get_and_print(ffp);
		state();
		get_and_print(ffp);
		if (g_symbol != WHILETK)
		{
			error();
		}
		get_and_print(ffp);
		if (g_symbol != LPARENT)
		{
			error();
		}
		get_and_print(ffp);
		int64_t satisfy_label_addr = condition();
		ic[ic.size() - 1].z = (entry *)start_label_addr;
		int64_t unsatisfy_label_addr = in_label_table(entry());
		/*add_inter_code(inter_code(JUMP, (entry *)unsatisfy_label_addr));
		add_inter_code(inter_code(SET, (entry *)satisfy_label_addr));
		add_inter_code(inter_code(JUMP, (entry *)start_label_addr));
		add_inter_code(inter_code(SET, (entry *)unsatisfy_label_addr));*/
		get_and_print(ffp);
		if (g_symbol != RPARENT)
		{
			error();
		}
	}
	else if (g_symbol == FORTK)
	{
		get_and_print(ffp);
		if (g_symbol != LPARENT)
		{
			error();
		}
		get_and_print(ffp);

		char name[NAME_LEN];
		int k;
		idenfr(name);
		k = search_table(name, EMPTY);

		get_and_print(ffp);
		if (g_symbol != ASSIGN)
		{
			error();
		}
		get_and_print(ffp);
		char ex_type[TYPE_LEN];
		int64_t expres_addr = expression(ex_type);

		add_inter_code(inter_code(ASSIGN_WITHOUT_ARRAY, &sb[k], '=', (entry *)expres_addr));

		get_and_print(ffp);
		if (g_symbol != SEMICN)
		{
			error();
		}
		get_and_print(ffp);
		int64_t judge_label_addr = in_label_table(entry());
		add_inter_code(inter_code(JUMP, (entry *)judge_label_addr));
		add_inter_code(inter_code(SET, (entry *)judge_label_addr));
		int64_t satisfy_label_addr = condition();
		int64_t unsatisfy_label_addr = in_label_table(entry());
		//add_inter_code(inter_code(JUMP, (entry *)unsatisfy_label_addr));

		get_and_print(ffp);
		if (g_symbol != SEMICN)
		{
			error();
		}
		get_and_print(ffp);
		int n;
		idenfr(name);
		n = search_table(name, EMPTY);
		get_and_print(ffp);
		if (g_symbol != ASSIGN)
		{
			error();
		}
		get_and_print(ffp);
		int m;
		idenfr(name);
		m = search_table(name, EMPTY);

		get_and_print(ffp);
		char op;
		add_op(&op);
		get_and_print(ffp);
		int int_value;
		stride(&int_value);
		int64_t int_addr = in_const_table(entry(int_value));
		get_and_print(ffp);
		if (g_symbol != RPARENT)
		{
			error();
		}
		get_and_print(ffp);

		add_inter_code(inter_code(SET, (entry *)satisfy_label_addr));
		state();
		add_inter_code(inter_code(ASSIGN_WITHOUT_ARRAY, &sb[n], op, &sb[m], (entry *)int_addr));
		//add_inter_code(inter_code(JUMP, (entry *)judge_label_addr));
		//add_inter_code(inter_code(SET, (entry *)unsatisfy_label_addr));
		int i;
		for (i = ic.size() - 1; ic[i].x != (entry *)satisfy_label_addr || ic[i].inter_type != SET; i--)
			;
		int j;
		for (j = i - 1; ic[j].x != (entry *)judge_label_addr || ic[j].inter_type != SET; j--)
			;

		for (int k = 0; k < ic.size() - i; k++)
		{
			inter_code tmp = ic[ic.size() - 1];
			ic.erase(ic.begin() + ic.size() - 1);
			ic.insert(ic.begin() + j, tmp);
		}
	}
	else
	{
		printf("error in recur_state");
		//error();
	}

	fprintf(ffp, "<循环语句>\n");
}

void assign()
{
	char name[NAME_LEN];
	int n;
	idenfr(name);
	get_and_print(ffp);
	n = search_table(name, EMPTY);
	entry &ety = sb[n];
	if (g_symbol == ASSIGN)
	{

		get_and_print(ffp);
		if (g_symbol == SEMICN)
		{
			//exit(0);
		}
		char ex_type[TYPE_LEN];

		int64_t express_addr = expression(ex_type);
		add_inter_code(inter_code(ASSIGN_WITHOUT_ARRAY, &ety, '=', (entry *)express_addr));
	}
	else if (g_symbol == LBRACK)
	{

		get_and_print(ffp);

		char ex_type[TYPE_LEN];
		int64_t offset_addr = expression(ex_type);

		get_and_print(ffp);
		if (g_symbol != RBRACK)
		{
			error();
		}
		get_and_print(ffp);
		if (g_symbol != ASSIGN)
		{
			error();
		}
		get_and_print(ffp);

		char new_ex_type[TYPE_LEN];
		int64_t value_addr = expression(new_ex_type);
		add_inter_code(inter_code(ASSIGN_AN_ARRAY, &ety, '_', (entry *)offset_addr, (entry *)value_addr));
	}
	else
	{
		error();
	}
	fprintf(ffp, "<赋值语句>\n");
}

void scanf_state()
{
	char name[NAME_LEN];
	int n;
	if (g_symbol != SCANFTK)
	{
		error();
	}
	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		error();
	}
	get_and_print(ffp);
	idenfr(name);
	n = search_table(name, EMPTY);
	add_inter_code(inter_code(SCANF, &sb[n]));

	pre_read(1);
	while (pre_symbol == COMMA)
	{
		get_and_print(ffp); //g_token is ,
		get_and_print(ffp);

		idenfr(name);
		n = search_table(name, EMPTY);
		add_inter_code(inter_code(SCANF, &sb[n]));

		pre_read(1);
	}
	get_and_print(ffp);
	if (g_symbol != RPARENT)
	{
		error();
	}
	fprintf(ffp, "<读语句>\n");
}
void printf_state()
{
	if (g_symbol != PRINTFTK)
	{
		error();
	}
	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		error();
	}
	get_and_print(ffp);
	if (g_symbol == STRCON)
	{
		char string[STRING_LEN];

		my_string(string);
		int64_t addr = in_string_table(entry(string));
		/*in_table(entry(string), index_stack.back());
		entry &ety = sb.back();
		strings.push_back(&ety);*/
		add_inter_code(inter_code(PRINTF, (entry *)addr));

		get_and_print(ffp);
		if (g_symbol == COMMA)
		{
			get_and_print(ffp);

			char ex_type[TYPE_LEN];
			int64_t express_addr = expression(ex_type);

			get_and_print(ffp);
			if (g_symbol != RPARENT)
			{
				error();
			}
			add_inter_code(inter_code(PRINTF, (entry *)express_addr));
		}
		else
		{

			if (g_symbol != RPARENT)
			{
				error();
			}
		}
	}
	else
	{
		char ex_type[TYPE_LEN];
		int64_t express_addr = expression(ex_type);
		get_and_print(ffp);
		if (g_symbol != RPARENT)
		{
			error();
		}
		add_inter_code(inter_code(PRINTF, (entry *)express_addr));
	}
	//printf("%s\n",string_table[0].name);
	add_inter_code(inter_code(PRINTF, (entry *)&string_table[0]));

	fprintf(ffp, "<写语句>\n");
}
void return_state()
{
	if (g_symbol != RETURNTK)
	{
		error();
	}
	pre_read(1);
	if (pre_symbol == LPARENT)
	{
		get_and_print(ffp); //g_token is (
		get_and_print(ffp);

		char ex_type[TYPE_LEN];
		int64_t expres_addr = expression(ex_type);
		add_inter_code(inter_code(RET, (entry *)expres_addr));

		get_and_print(ffp);
		if (g_symbol != RPARENT)
		{
			error();
		}
	}
	else
	{
		add_inter_code(inter_code(RET, NULL));
	}
	fprintf(ffp, "<返回语句>\n");
}
void state()
{
	if (g_symbol == IFTK)
	{
		condition_state();
	}
	else if (g_symbol == WHILETK || g_symbol == FORTK || g_symbol == DOTK)
	{

		recur_state();
	}
	else if (g_symbol == LBRACE)
	{

		pre_read(1);
		if (pre_symbol == RBRACE)
		{
			set_nothing();
		}
		else
		{
			get_and_print(ffp);
		}
		state_list();

		get_and_print(ffp);
		if (g_symbol != RBRACE)
		{
			error();
		}
	}
	else if (g_symbol == SEMICN)
	{
	}
	else if (g_symbol == SCANFTK)
	{

		scanf_state();
		get_and_print(ffp);
		if (g_symbol != SEMICN)
		{
			error();
		}
	}
	else if (g_symbol == PRINTFTK)
	{

		printf_state();
		get_and_print(ffp);
		if (g_symbol != SEMICN)
		{
			error();
		}
	}
	else if (g_symbol == RETURNTK)
	{
		return_state();
		get_and_print(ffp);
		if (g_symbol != SEMICN)
		{
			error();
		}
	}
	else if (g_symbol == IDENFR)
	{
		pre_read(1);
		if (pre_symbol == LPARENT)
		{
			//printf("%s\n %d", g_token,is_return_fun(g_token));
			if (is_return_fun(g_token))
			{
				return_fun_call();
				get_and_print(ffp);
				if (g_symbol != SEMICN)
				{
					error();
				}
			}
			else
			{
				non_return_fun_call();
				get_and_print(ffp);
				if (g_symbol != SEMICN)
				{
					error();
				}
			}
		}
		else
		{
			assign();
			get_and_print(ffp);
			if (g_symbol != SEMICN)
			{
				error();
			}
		}
	}

	fprintf(ffp, "<语句>\n");
}
void state_list()
{
	//FIRST(<语句>) = {if,while,do,for,{,标识符,scanf,printf,return,;}
	if (g_symbol != NOTHING)
	{
		if (g_symbol == IFTK || g_symbol == WHILETK || g_symbol == DOTK || g_symbol == FORTK || g_symbol == LBRACE || g_symbol == IDENFR || g_symbol == SCANFTK || g_symbol == PRINTFTK || g_symbol == RETURNTK || g_symbol == SEMICN)
		{
			state();
			pre_read(1);
			while (pre_symbol == IFTK || pre_symbol == WHILETK || pre_symbol == DOTK || pre_symbol == FORTK || pre_symbol == LBRACE || pre_symbol == IDENFR || pre_symbol == SCANFTK || pre_symbol == PRINTFTK || pre_symbol == RETURNTK || pre_symbol == SEMICN)
			{
				get_and_print(ffp);
				state();
				pre_read(1);
			}
		}
	}
	fprintf(ffp, "<语句列>\n");
}
void composite_state()
{
	if (g_symbol == CONSTTK)
	{
		const_clarify();
		pre_read(1);
		if (pre_symbol == RBRACE)
		{
			set_nothing();
		}
		else
		{
			get_and_print(ffp);
		}
	}
	if (g_symbol == INTTK || g_symbol == CHARTK)
	{
		var_clarify();
		pre_read(1);
		if (pre_symbol == RBRACE)
		{
			set_nothing();
		}
		else
		{
			get_and_print(ffp);
		}
	}

	state_list();
	fprintf(ffp, "<复合语句>\n");
}
void non_return_fun()
{

	if (g_symbol != VOIDTK)
	{
		printf("error in non_return_fun(1)");
		error();
	}
	get_and_print(ffp);
	char name[NAME_LEN];
	idenfr(name);

	in_table(entry(FUN, VOID, name, level, 0, 0), index_stack.back());
	add_inter_code(inter_code(FUN_DEFINE, &sb.back()));

	add_non_return_name(g_token);

	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		printf("error in non_return_fun(2)");
		error();
	}
	pre_read(1);
	if (pre_symbol == RPARENT)
	{
		set_nothing();
	}
	else
	{
		get_and_print(ffp);
	}
	index_stack.push_back(sb.size());
	level++;
	para_table();

	get_and_print(ffp);
	if (g_symbol != RPARENT)
	{
		printf("error in non_return_fun(3)");
		error();
	}
	get_and_print(ffp);
	if (g_symbol != LBRACE)
	{
		printf("error in non_return_fun(4)");
		error();
	}

	pre_read(1);
	if (pre_symbol == RBRACE)
	{
		set_nothing();
	}
	else
	{
		get_and_print(ffp);
	}
	composite_state();

	get_and_print(ffp);

	if (g_symbol != RBRACE)
	{
		printf("error in non_return_fun(5)");
		error();
	}

	out_table(index_stack.back());
	index_stack.pop_back();
	level--;

	fprintf(ffp, "<无返回值函数定义>\n");
	add_inter_code(inter_code(END_OF_FUN, NULL));
}
void define_head(char *name, char *type)
{
	//printf("%s\n", g_token);
	if (g_symbol == INTTK || g_symbol == CHARTK)
	{
		if (g_symbol == INTTK)
			strcpy(type, (char *)"int");
		else
			strcpy(type, (char *)"char");
		get_and_print(ffp);

		idenfr(name);
		add_return_name(g_token);
	}
	else
	{
		printf("error in define_head\n");
		error();
	}
	fprintf(ffp, "<声明头部>\n");
}
void return_fun()
{
	char name[NAME_LEN];
	char return_type[TYPE_LEN];
	define_head(name, return_type);

	in_table(entry(FUN, return_type, name, level, 0, 0), index_stack.back());
	add_inter_code(inter_code(FUN_DEFINE, &sb.back()));

	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		printf("error in non_return_fun(2)");
		error();
	}

	pre_read(1);
	if (pre_symbol == RPARENT)
	{
		set_nothing();
	}
	else
	{
		get_and_print(ffp);
	}
	index_stack.push_back(sb.size());
	level++;
	para_table();

	get_and_print(ffp);

	if (g_symbol != RPARENT)
	{
		printf("error in non_return_fun(3)");
		error();
	}
	get_and_print(ffp);
	if (g_symbol != LBRACE)
	{
		printf("error in non_return_fun(4)");
		error();
	}

	pre_read(1);
	if (pre_symbol == RBRACE)
	{
		set_nothing();
	}
	else
	{
		get_and_print(ffp);
	}
	composite_state();

	get_and_print(ffp);

	if (g_symbol != RBRACE)
	{
		printf("error in non_return_fun(5)");
		error();
	}
	//print_table();
	out_table(index_stack.back());
	index_stack.pop_back();
	level--;

	fprintf(ffp, "<有返回值函数定义>\n");
	add_inter_code(inter_code(END_OF_FUN, NULL));
}

void fun_define()
{
	if (g_symbol == VOIDTK)
	{
		non_return_fun();
	}
	else
	{
		is_non_return_fun = 2;
		return_fun();
	}
}
void main_program()
{
	is_non_return_fun = 0;
	if (g_symbol != VOIDTK)
	{
		error();
	}
	get_and_print(ffp);
	if (g_symbol != MAINTK)
	{
		error();
	}
	get_and_print(ffp);
	if (g_symbol != LPARENT)
	{
		error();
	}
	in_table(entry(FUN, VOID, (char *)"main", level, 0), index_stack.back());
	add_inter_code(inter_code(MAIN_DEFINE, &sb.back()));

	get_and_print(ffp);
	if (g_symbol != RPARENT)
	{
		error();
	}
	get_and_print(ffp);
	if (g_symbol != LBRACE)
	{
		error();
	}

	level++;
	index_stack.push_back(sb.size());

	pre_read(1);
	if (pre_symbol == RBRACE)
	{
		set_nothing();
	}
	else
	{
		get_and_print(ffp);
	}

	composite_state();

	get_and_print(ffp);

	if (g_symbol != RBRACE)
	{
		error();
	}

	out_table(index_stack.back());
	index_stack.pop_back();
	level--;

	fprintf(ffp, "<主函数>\n");
}
void programs()
{
	index_stack.push_back(sb.size());

	/*in_table(enter, index_stack.back());
	entry &ety = sb.back();
	strings.push_back(&ety);*/
	//print_table();
	if (g_symbol == CONSTTK)
	{
		//	global_const_define();
		const_clarify();
		get_and_print(ffp);
	}

	//print_table();
	pre_read(2);
	if (pre_symbol != LPARENT)
	{
		//	global_var_define();
		var_clarify();
		get_and_print(ffp);
	}
	//print_table();
	pre_read(1);

	while (pre_symbol != MAINTK)
	{
		//printf("%s\n", g_token);
		fun_define();
		get_and_print(ffp);
		pre_read(1);
		//	print_table();
	}
	main_program();
	//print_table();
	fprintf(ffp, "<程序>\n");
}
