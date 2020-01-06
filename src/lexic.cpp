#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "head.h"

const char *name[REMAIN] = {
	"CONSTTK", "INTTK", "CHARTK", "VOIDTK", "MAINTK", "IFTK", "ELSETK", "DOTK",
	"WHILETK", "FORTK", "SCANFTK", "PRINTFTK", "RETURNTK"};
const char *remain[REMAIN] = {
	"const", "int", "char", "void", "main", "if", "else",
	"do", "while", "for", "scanf", "printf", "return"};
//const char* output = "output.txt";

char program[MAXX];
char g_char;
char g_token[MAX];
char g_token_tem[MAX];
char pre_token[MAX];
enum MyEnum g_symbol;
enum MyEnum g_symbol_tem;
enum MyEnum pre_symbol;
int _index;
int _index_tem;
int line = 1;
int line_tem;

void error()
{
	fprintf(err, "%d a\n", line);
	printf("error took place!\n");
}
char get_char()
{
	return program[_index++];
}

char get_none_block_char()
{
	while (isspace(get_char()))
	{
		if (program[_index - 1] == '\n')
		{
			//printf("%d ",program[_index-2]);
			line++;
		}
	}
	return program[_index - 1];
}

void unget_char()
{
	_index--;
}

int is_reserve()
{
	int i = 0;
	for (i = 0; i < REMAIN; i++)
	{
		if (strcmp(remain[i], g_token) == 0)
		{
			return i;
		}
	}
	return -1;
}

void get_sym()
{
	memset(g_token, 0, MAX);
	g_char = get_none_block_char();
	//printf("%d\n", isalpha(g_char));
	if (isalpha(g_char) || g_char == '_')
	{
		do
		{
			strcat(g_token, &g_char);
		} while (isalnum(g_char = get_char()) || g_char == '_');
		unget_char();
		if (is_reserve() != -1)
			g_symbol = (MyEnum)is_reserve();
		else
			g_symbol = IDENFR;
	}
	else if (isdigit(g_char) && g_char != '0')
	{
		do
		{
			strcat(g_token, &g_char);
		} while (isdigit(g_char = get_char()));
		unget_char();
		g_symbol = INTCON;
	}
	else if (g_char == '0')
	{
		strcat(g_token, &g_char);
		g_symbol = INTCON;
	}
	else if (g_char == '\'')
	{
		g_char = get_char();
		if (g_char == '+' || g_char == '-' || g_char == '*' || g_char == '/' ||g_char == '_'|| isalnum(g_char) || isdigit(g_char))
		{
			strcat(g_token, &g_char);
			g_char = get_char();
			if (g_char == '\'')
				g_symbol = CHARCON;
		}
		else
		{
			g_char = get_char();
		}
	}
	else if (g_char == '"')
	{
		g_char = get_char();
		if (g_char == '"')
			g_symbol = STRCON;
		else
		{
			while (g_char == 32 || g_char == 33 || (g_char >= 35 && g_char <= 126))
			{
				if(g_char == '\\'){
					strcat(g_token,&g_char);
				}
				strcat(g_token, &g_char);
				g_char = get_char();
			}
			if (g_char == '"')
				g_symbol = STRCON;
			if (g_char == '\n')
				unget_char();
		}
	}
	else if (g_char == '+')
	{
		strcat(g_token, &g_char);
		g_symbol = PLUS;
	}
	else if (g_char == '-')
	{
		strcat(g_token, &g_char);
		g_symbol = MINU;
	}
	else if (g_char == '*')
	{
		strcat(g_token, &g_char);
		g_symbol = MULT;
	}
	else if (g_char == '/')
	{
		strcat(g_token, &g_char);
		g_symbol = DIV;
	}
	else if (g_char == '<')
	{
		strcat(g_token, &g_char);
		if ((g_char = get_char()) == '=')
		{
			strcat(g_token, &g_char);
			g_symbol = LEQ;
		}
		else
		{
			g_symbol = LSS;
			unget_char();
		}
	}
	else if (g_char == '>')
	{
		strcat(g_token, &g_char);
		if ((g_char = get_char()) == '=')
		{
			strcat(g_token, &g_char);
			g_symbol = GEQ;
		}
		else
		{
			g_symbol = GRE;
			unget_char();
		}
	}
	else if (g_char == '=')
	{
		strcat(g_token, &g_char);
		if ((g_char = get_char()) == '=')
		{
			strcat(g_token, &g_char);
			g_symbol = EQL;
		}
		else
		{
			g_symbol = ASSIGN;
			unget_char();
		}
	}
	else if (g_char == '!')
	{
		strcat(g_token, &g_char);
		g_char = get_char();
		if (g_char == '=')
		{
			strcat(g_token, &g_char);
			g_symbol = NEQ;
		}
	}
	else if (g_char == ';')
	{
		strcat(g_token, &g_char);
		g_symbol = SEMICN;
	}
	else if (g_char == ',')
	{
		strcat(g_token, &g_char);
		g_symbol = COMMA;
	}
	else if (g_char == '(')
	{
		strcat(g_token, &g_char);
		g_symbol = LPARENT;
	}
	else if (g_char == ')')
	{
		strcat(g_token, &g_char);
		g_symbol = RPARENT;
	}
	else if (g_char == '[')
	{
		strcat(g_token, &g_char);
		g_symbol = LBRACK;
	}
	else if (g_char == ']')
	{
		strcat(g_token, &g_char);
		g_symbol = RBRACK;
	}
	else if (g_char == '{')
	{
		strcat(g_token, &g_char);
		g_symbol = LBRACE;
	}
	else if (g_char == '}')
	{
		strcat(g_token, &g_char);
		g_symbol = RBRACE;
	}
	else if (g_char == '\0')
	{
		strcat(g_token, &g_char);
		g_symbol = FINISH;
	}
	else
	{
		g_symbol = ERROR;
	}
}

void read_file(char *file_name, char *array_name, int *length)
{
	FILE *fp;
	int i = 0;
	fp = fopen(file_name, "r");
	if (fp == NULL)
	{
		return;
	}
	while (fscanf(fp, "%c", &array_name[i]) != EOF)
		i++;
	fclose(fp);
	*length = i;
	return;
}

void print_word(FILE *ffp)
{
	if (g_symbol >= CONSTTK && g_symbol <= RETURNTK)
	{
		fprintf(ffp, "%s %s\n", name[g_symbol], remain[g_symbol]);
	}
	else if (g_symbol == IDENFR)
	{
		fprintf(ffp, "IDENFR %s\n", g_token);
	}
	else if (g_symbol == INTCON)
	{
		fprintf(ffp, "INTCON %s\n", g_token);
	}
	else if (g_symbol == CHARCON)
	{
		fprintf(ffp, "CHARCON %s\n", g_token);
	}
	else if (g_symbol == STRCON)
	{
		fprintf(ffp, "STRCON %s\n", g_token);
	}
	else if (g_symbol == PLUS)
	{
		fprintf(ffp, "PLUS +\n");
	}
	else if (g_symbol == MINU)
	{
		fprintf(ffp, "MINU -\n");
	}
	else if (g_symbol == MULT)
	{
		fprintf(ffp, "MULT *\n");
	}
	else if (g_symbol == DIV)
	{
		fprintf(ffp, "DIV /\n");
	}
	else if (g_symbol == LSS)
	{
		fprintf(ffp, "LSS <\n");
	}
	else if (g_symbol == LEQ)
	{
		fprintf(ffp, "LEQ <=\n");
	}
	else if (g_symbol == GRE)
	{
		fprintf(ffp, "GRE >\n");
	}
	else if (g_symbol == GEQ)
	{
		fprintf(ffp, "GEQ >=\n");
	}
	else if (g_symbol == EQL)
	{
		fprintf(ffp, "EQL ==\n");
	}
	else if (g_symbol == NEQ)
	{
		fprintf(ffp, "NEQ !=\n");
	}
	else if (g_symbol == ASSIGN)
	{
		fprintf(ffp, "ASSIGN =\n");
	}
	else if (g_symbol == SEMICN)
	{
		fprintf(ffp, "SEMICN ;\n");
	}
	else if (g_symbol == COMMA)
	{
		fprintf(ffp, "COMMA ,\n");
	}
	else if (g_symbol == LPARENT)
	{
		fprintf(ffp, "LPARENT (\n");
	}
	else if (g_symbol == RPARENT)
	{
		fprintf(ffp, "RPARENT )\n");
	}
	else if (g_symbol == LBRACK)
	{
		fprintf(ffp, "LBRACK [\n");
	}
	else if (g_symbol == RBRACK)
	{
		fprintf(ffp, "RBRACK ]\n");
	}
	else if (g_symbol == LBRACE)
	{
		fprintf(ffp, "LBRACE {\n");
	}
	else if (g_symbol == RBRACE)
	{
		fprintf(ffp, "RBRACE }\n");
	}
	else if (g_symbol == FINISH)
	{
	}
	else
	{
		fprintf(err, "%d a\n", line);
		return;
	}
}

void get_and_print(FILE *ffp)
{
	get_sym();
	print_word(ffp);
}

void set_nothing()
{
	memset(g_token, 0, MAX);
	g_symbol = NOTHING;
}

void anchor_token()
{
	memset(g_token_tem, 0, MAX);
	strcat(g_token_tem, g_token);
}

void set_token()
{
	memset(g_token, 0, MAX);
	strcat(g_token, g_token_tem);
}
void anchor_symbol()
{
	g_symbol_tem = g_symbol;
}

void set_symbol()
{
	g_symbol = g_symbol_tem;
}
void anchor_index()
{
	_index_tem = _index;
}

void anchor_line()
{
	line_tem = line;
}

void set_line()
{
	line = line_tem;
}
void set_index()
{
	_index = _index_tem;
}

void anchor()
{
	anchor_index();
	anchor_token();
	anchor_symbol();
	anchor_line();
}

void set()
{
	set_index();
	set_token();
	set_symbol();
	set_line();
}
void only_get()
{
	get_sym();
}
void only_print(FILE *ffp)
{
	print_word(ffp);
}

void pre_read(int word_num)
{
	anchor();
	int i;
	for (i = 0; i < word_num; i++)
	{
		only_get();
	}
	memset(pre_token, 0, MAX);
	strcat(pre_token, g_token);
	pre_symbol = g_symbol;
	set();
}
/*int main() {
	

	//FILE* fp;
	FILE *ffp;
	int length = 0;
	//fp = fopen("testfile.txt", "r");
	ffp = fopen("output.txt", "w");
	//if (fp == NULL) return -1;
	//while (fscanf(fp, "%c", &program[length]) != EOF) length++;
	//fclose(fp);
	read_file("testfile.txt", program, &length);
	for (int j = 0; j < length; j++) {
		printf("%c", program[j]);
	}
	while (1) {
		get_sym();
		//printf("%s %d\n", g_token, g_symbol);
		//printf("%d\n", CONSTTK);
		if (g_symbol >= CONSTTK && g_symbol <= RETURNTK) {
			fprintf(ffp, "%s %s\n", name[g_symbol], remain[g_symbol]);
		}
		else if (g_symbol == IDENFR) {
			fprintf(ffp, "IDENFR %s\n", g_token);
		}
		else if (g_symbol == INTCON) {
			fprintf(ffp, "INTCON %d\n", atoi(g_token));
		}
		else if (g_symbol == CHARCON) {
			fprintf(ffp, "CHARCON %s\n", g_token);
		}
		else if (g_symbol == STRCON) {
			fprintf(ffp, "STRCON %s\n", g_token);
		}
		else if (g_symbol == PLUS) {
			fprintf(ffp, "PLUS +\n");
		}
		else if (g_symbol == MINU) {
			fprintf(ffp, "MINU -\n");
		}
		else if (g_symbol == MULT) {
			fprintf(ffp, "MULT *\n");
		}
		else if (g_symbol == DIV) {
			fprintf(ffp, "DIV /\n");
		}
		else if (g_symbol == LSS) {
			fprintf(ffp, "LSS <\n");
		}
		else if (g_symbol == LEQ) {
			fprintf(ffp, "LEQ <=\n");
		}
		else if (g_symbol == GRE) {
			fprintf(ffp, "GRE >\n");
		}
		else if (g_symbol == GEQ) {
			fprintf(ffp, "GEQ >=\n");
		}
		else if (g_symbol == EQL) {
			fprintf(ffp, "EQL ==\n");
		}
		else if (g_symbol == NEQ) {
			fprintf(ffp, "NEQ !=\n");
		}
		else if (g_symbol == ASSIGN) {
			fprintf(ffp, "ASSIGN =\n");
		}
		else if (g_symbol == SEMICN) {
			fprintf(ffp, "SEMICN ;\n");
		}
		else if (g_symbol == COMMA) {
			fprintf(ffp, "COMMA ,\n");
		}
		else if (g_symbol == LPARENT) {
			fprintf(ffp, "LPARENT (\n");
		}
		else if (g_symbol == RPARENT) {
			fprintf(ffp, "RPARENT )\n");
		}
		else if (g_symbol == LBRACK) {
			fprintf(ffp, "LBRACK [\n");
		}
		else if (g_symbol == RBRACK) {
			fprintf(ffp, "RBRACK ]\n");
		}
		else if (g_symbol == LBRACE) {
			fprintf(ffp, "LBRACE {\n");
		}
		else if (g_symbol == RBRACE) {
			fprintf(ffp, "RBRACE }\n");
		}
		else if (g_symbol == FINISH) {
			break;
		}
		else {
			printf("error!\n");
			return 0;
		}
	}


	return 0;
}*/
