#define _CRT_SECURE_NO_WARNINGS
#include "head.h"
#define A 2
#define T 1
#define S 0

#define WORD 0
#define BYTE 1
#define $0 (char *)"$0"
#define $FP (char *)"$fp"
#define $SP (char *)"$sp"
#define $GP (char *)"$gp"
#define $V0 (char *)"$v0"
#define $A0 (char *)"$a0"
#define $T0 (char *)"$t0"
#define $T1 (char *)"$t1"
#define $T2 (char *)"$t2"
#define $T3 (char *)"$t3"
#define $T4 (char *)"$t4"
#define $T5 (char *)"$t5"
#define $T6 (char *)"$t6"
#define $T7 (char *)"$t7"
#define $T8 (char *)"$t8"
#define $T9 (char *)"$t9"

#define $RA (char *)"$ra"
#define $S0 (char *)"$s0"
#define $S1 (char *)"$s1"
#define $S2 (char *)"$s2"
#define $S3 (char *)"$s3"
#define $S4 (char *)"$s4"
#define $S5 (char *)"$s5"
#define $S6 (char *)"$s6"
#define $S7 (char *)"$s7"

FILE *mips;
uint32_t sp = 0x00002ffc;
uint32_t fp = 0x00000000;
uint32_t gp = 0x00001800;
int global_var_space = 0;
int local_var_space;
int all_para_num;
bool is_main;
int current_para_num;
vector<int> current_push_num;
vector<int> all_push_num;
int last_push_location;
int started;
int ic_index;
vector<reg> t_reg_available;
vector<reg> t_reg_being_used;
vector<reg> s_reg_available;
vector<reg> s_reg_being_used;
vector<reg> a_reg_available;
vector<reg> a_reg_being_used;

reg a1 = reg(A, 1);
reg a2 = reg(A, 2);
reg a3 = reg(A, 3);
reg t0 = reg(T, 0);
reg t1 = reg(T, 1);
reg t2 = reg(T, 2);
reg t3 = reg(T, 3);
reg t4 = reg(T, 4);
reg t5 = reg(T, 5);
reg t6 = reg(T, 6);
reg t7 = reg(T, 7);
reg s0 = reg(S, 0);
reg s1 = reg(S, 1);
reg s2 = reg(S, 2);
reg s3 = reg(S, 3);
reg s4 = reg(S, 4);
reg s5 = reg(S, 5);
reg s6 = reg(S, 6);
reg s7 = reg(S, 7);
reg init = reg(T, -1);

void prepare_string();
void prepare_reg();
void gen_mips();
void gen_blank();
void gen_syscall();
void gen_move(char *r1, char *r2);
void gen_addu(char *r1, char *r2, int value);
void gen_addu(char *r1, char *r2, char *r3);
void gen_subu(char *r1, char *r2, int value);
void gen_subu(char *r1, char *r2, char *r3);
void gen_mul(char *r1, char *r2, int value);
void gen_mul(char *r1, char *r2, char *r3);
void gen_divu(char *r1, char *r2, int value);
void gen_divu(char *r1, char *r2, char *r3);
void gen_div(char *r1, char *r2, int value);
void gen_div(char *r1, char *r2);
void gen_load_immediate(char *r1, int value);
void gen_load_immediate(char *r1, char value);
void gen_load_word(char *r1, uint32_t addr);
void gen_load(char *r1, char *r2, int offset, int word_or_byte);
void gen_load_byte(char *r1, uint32_t addr);
void gen_load_byte(char *r1, char *r2, int offset);
void gen_store_word(char *r1, uint32_t addr);
void gen_store_word(char *r1, char *r2, int offset);
void gen_store_byte(char *r1, uint32_t addr);
void gen_store_byte(char *r1, char *r2, int offset);
void gen_load_address(char *r1, char *string_name);
void move_word_to_register(char *r1, entry *ety);
void move_byte_to_register(char *r1, entry *ety);
void move_register_to_word(entry *ety, char *r1);

void handle_var_define(entry *ety);
void handle_fun_define(entry *ety);
void handle_main_define();
void handle_para_define(entry *ety);
void handle_para_push(entry *ety);
void handle_assign_without_array(inter_code itcd);
void handle_assign_with_array(inter_code itcd);
void handle_assign_an_array(inter_code itcd);
void handle_scanf(inter_code itcd);
void handle_printf(inter_code itcd);
void handle_set(inter_code itcd);
void handle_jump(inter_code itcd);
void handle_condition(inter_code itcd);
void handle_return(inter_code itcd);
void handle_fun_call(inter_code itcd);
void handle_start_call(inter_code itcd);
void handle_assign_ret(inter_code itcd);
void pop_fp();
void switch_relation(inter_code itcd, int x_is_immediate, int y_is_immediate);
void move_sp_to_fp();
void move_fp_to_sp();
void get_s_register(char *name_to_fill);
void get_s_by_num(int num, char *name_to_fill);

void optimize_inter();
void delete_straight_assign();
void assign_reg(entry *ety);
void release_reg(entry *ety);
void release_reg_for();
void help_three_one(char *relation, inter_code itcd);

void help_three_two(char *relation, inter_code itcd);

void help_four_one(char *relation, inter_code itcd);
void help_four_two(char *relation, inter_code itcd);

int main()
{
    syntax();
    int error_length = 0;
    char error_file[MAXX];
    n_read_file((char *)"error.txt", error_file, &error_length);

    if (error_length > 0)
    {
        exit(0);
    }

    syntax_and_inter();
    //printf_inter(ic, fopen("n.txt", "w"));

    mips = fopen("mips.txt", "w");

    gen_mips();
}
void gen_mips()
{
    prepare_string();
    optimize_inter();
    fprintf(mips, ".text\n");
    fprintf(mips, "  jal main\n");
    gen_load_immediate($V0, 10);
    gen_syscall();
    for (ic_index = 0; ic_index < ic.size(); ic_index++)
    {
        if (ic[ic_index].inter_type != BLANK)
        {
            switch (ic[ic_index].inter_type)
            {
            case CONST_DEFINE:
                break;
            case VAR_DEFINE:
                if (ic[ic_index].x->level == 1 || ic[ic_index].x->dimension != 0)
                    handle_var_define(ic[ic_index].x);
                break;
            case FUN_DEFINE:
                handle_fun_define(ic[ic_index].x);
                break;
            case MAIN_DEFINE:
                handle_main_define();
                break;
            case ASSIGN_WITHOUT_ARRAY:
                handle_assign_without_array(ic[ic_index]);
                break;
            case ASSIGN_WITH_ARRAY:
                handle_assign_with_array(ic[ic_index]);
                break;
            case ASSIGN_AN_ARRAY:
                handle_assign_an_array(ic[ic_index]);
                break;
            case ASSIGN_RET:
                handle_assign_ret(ic[ic_index]);
                break;
            case SCANF:
                handle_scanf(ic[ic_index]);
                break;
            case PRINTF:
                handle_printf(ic[ic_index]);
                break;
            case SET:
                handle_set(ic[ic_index]);
                break;
            case JUMP:
                handle_jump(ic[ic_index]);
                break;
            case _BEQ:
            case _BNE:
            case _LSS:
            case _LEQ:
            case _GRE:
            case _GEQ:
                handle_condition(ic[ic_index]);
                release_reg_for();
                break;
            case PARA_DEFINE:
                handle_para_define(ic[ic_index].x);
                break;
            case RET:
                handle_return(ic[ic_index]);
                break;
            case PARA_PUSH:
                handle_para_push(ic[ic_index].x);
                break;
            case FUN_CALL:
                handle_fun_call(ic[ic_index]);
                break;
            case END_OF_FUN:
                /*char sr[NAME_LEN];
                for (int i = 0; i < 7; i++)
                {
                    get_s_by_num(i, sr);
                    gen_load_word(sr, $FP, -4 * (i + 1));
                }*/
                move_fp_to_sp();
                pop_fp();
                fprintf(mips, "  jr $ra\n");
                gen_blank();
                break;
            case START_CALL:
                handle_start_call(ic[ic_index]);
                break;
            default:
                break;
            }
        }
    }
    //print_table();
}
void prepare_reg()
{
    while (t_reg_available.size() > 0)
    {
        t_reg_available.pop_back();
    }
    while (s_reg_available.size() > 0)
    {
        s_reg_available.pop_back();
    }
    while (t_reg_being_used.size() > 0)
    {
        t_reg_being_used.pop_back();
    }
    while (s_reg_being_used.size() > 0)
    {
        s_reg_being_used.pop_back();
    }
    while (a_reg_available.size() > 0)
    {
        a_reg_available.pop_back();
    }
    while (a_reg_being_used.size() > 0)
    {
        a_reg_being_used.pop_back();
    }
    t_reg_available.push_back(t0);
    t_reg_available.push_back(t1);
    t_reg_available.push_back(t2);
    t_reg_available.push_back(t3);
    t_reg_available.push_back(t4);
    t_reg_available.push_back(t5);
    t_reg_available.push_back(t6);
    t_reg_available.push_back(t7);
    s_reg_available.push_back(s0);
    s_reg_available.push_back(s1);
    s_reg_available.push_back(s2);
    s_reg_available.push_back(s3);
    s_reg_available.push_back(s4);
    s_reg_available.push_back(s5);
    s_reg_available.push_back(s6);
    s_reg_available.push_back(s7);
    a_reg_available.push_back(a1);
    a_reg_available.push_back(a2);
    a_reg_available.push_back(a3);
}

void assign_reg(entry *ety)
{
    if (ety->is_tmp) // assign t reg
    {
        ety->my_reg = t_reg_available[0];
        ety->has_reg = true;
        t_reg_available.erase(t_reg_available.begin());
        t_reg_being_used.push_back(ety->my_reg);
        printf("allocate %s %s at %d\n", ety->name, ety->my_reg.reg_name, ic_index);
    }
    else // assign s reg
    {
        ety->my_reg = s_reg_available[0];
        ety->has_reg = true;
        s_reg_available.erase(s_reg_available.begin());
        s_reg_being_used.push_back(ety->my_reg);
        printf("allocate %s %s at %d\n", ety->name, ety->my_reg.reg_name, ic_index);
    }
}
void release_reg(entry *ety)
{

    if (ety->is_tmp && ety->has_reg && ety->last_appear_time == ic_index) //release an t reg
    {
        //printf("%s\n",ety->name);
        printf("release %s %s at %d\n", ety->name, ety->my_reg.reg_name, ic_index);

        ety->has_reg = false;
        for (int i = 0; i < t_reg_being_used.size(); i++)
        {
            if (t_reg_being_used[i].reg_index == ety->my_reg.reg_index)
            {
                t_reg_being_used.erase(t_reg_being_used.begin() + i);
                break;
            }
        }
        t_reg_available.insert(t_reg_available.begin(), ety->my_reg);
        ety->my_reg = init;
    }
    else if (!ety->is_tmp && ety->has_reg && ety->last_appear_time == ic_index) // release an s reg
    {
        printf("release %s %s at %d\n", ety->name, ety->my_reg.reg_name, ic_index);

        ety->has_reg = false;
        for (int i = 0; i < s_reg_being_used.size(); i++)
        {
            if (s_reg_being_used[i].reg_index == ety->my_reg.reg_index)
            {
                s_reg_being_used.erase(s_reg_being_used.begin() + i);
                break;
            }
        }
        s_reg_available.insert(s_reg_available.begin(), ety->my_reg);
        ety->my_reg = init;
    }
}
void release_reg_for()
{

    int i;
    for (i = ic_index; i >= 0; i--)
    {
        if (ic[i].inter_type == SET && ic[i].x == ic[ic_index].z)
            break;
    }
    printf("%d\n", ic_index);
    printf("%d\n", i);
    if (i > 0)
    {
        for (int k = i; k <= ic_index; k++)
        {
            if (ic[k].x != NULL && !ic[k].x->is_tmp && strcmp(ic[k].x->category, VAR) == 0 && ic[k].x->last_appear_time == ic_index)
            {
                //printf("%s ",ic[i].x->name);
                release_reg(ic[k].x);
            }
            if (ic[k].y != NULL && !ic[k].y->is_tmp && strcmp(ic[k].y->category, VAR) == 0 && ic[k].y->last_appear_time == ic_index)
            {
                //printf("%s ",ic[i].y->name);
                release_reg(ic[k].y);
            }
            if (ic[k].z != NULL && !ic[k].z->is_tmp && strcmp(ic[k].z->category, VAR) == 0 && ic[k].z->last_appear_time <= ic_index)
            {
                //printf("%s ",ic[i].z->name);
                release_reg(ic[k].z);
            }
        }
    }
}

void move_sp_to_fp()
{
    fprintf(mips, "  move $fp $sp\n");
    fp = sp;
}
void move_fp_to_sp()
{
    fprintf(mips, "  move $sp $fp\n");
    //sp = fp;
}
void prepare_string()
{
    if (string_table.size() > 0)
    {
        fprintf(mips, ".data\n");
    }
    for (int i = 0; i < string_table.size(); i++)
    {
        fprintf(mips, "  %s:.asciiz \"%s\"\n", string_table[i].name, string_table[i].string);
    }
}
void handle_main_define()
{
    prepare_reg();
    local_var_space = 0;
    is_main = true;
    fprintf(mips, "main:\n");
    move_sp_to_fp();
    gen_blank();
}
void handle_scanf(inter_code itcd)
{
    if (itcd.x->offset_to_fp == 0 && itcd.x->offset_to_gp == -1 && !itcd.x->has_reg)
    {
        handle_var_define(itcd.x);
    }
    if (itcd.x->has_reg)
    {
        if (strcmp(itcd.x->type, INT) == 0)
        {
            gen_load_immediate($V0, 5);
            gen_syscall();
            gen_move(itcd.x->my_reg.reg_name, $V0);
        }
        else
        {
            gen_load_immediate($V0, 12);
            gen_syscall();
            gen_move(itcd.x->my_reg.reg_name, $V0);
        }
    }
    else
    {
        if (strcmp(itcd.x->type, INT) == 0)
        {
            gen_load_immediate($V0, 5);
            gen_syscall();
            gen_store_word($V0, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
        }
        else
        {
            gen_load_immediate($V0, 12);
            gen_syscall();
            gen_store_byte($V0, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
        }
    }
    gen_blank();
}
void handle_printf(inter_code itcd)
{
    if (itcd.x->is_immediate || strcmp(itcd.x->category, CONST) == 0)
    {

        if (strcmp(itcd.x->type, INT) == 0)
        {
            gen_load_immediate($V0, 1);
            gen_load_immediate($A0, itcd.x->int_value);
            gen_syscall();
        }
        else
        {
            gen_load_immediate($V0, 11);
            gen_load_immediate($A0, itcd.x->char_value);
            gen_syscall();
        }
    }
    else
    {
        if (itcd.x->is_string)
        {
            gen_load_immediate($V0, 4);
            gen_load_address($A0, itcd.x->name);
            gen_syscall();
        }
        else if (strcmp(itcd.x->type, INT) == 0)
        {
            gen_load_immediate($V0, 1);
            move_word_to_register($A0, itcd.x);
            //gen_load_word($A0, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
            gen_syscall();
        }
        else if (strcmp(itcd.x->type, CHAR) == 0)
        {
            gen_load_immediate($V0, 11);
            move_byte_to_register($A0, itcd.x);
            //gen_load_byte($A0, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
            gen_syscall();
        }
    }
    gen_blank();
}
void handle_return(inter_code itcd)
{
    if (itcd.x != NULL)
    {
        if (itcd.x->is_immediate || strcmp(itcd.x->category, CONST) == 0)
        {
            if (strcmp(itcd.x->type, INT) == 0)
            {
                gen_load_immediate($V0, itcd.x->int_value);
            }
            else
            {
                gen_load_immediate($V0, itcd.x->char_value);
            }
        }
        else
        {
            move_word_to_register($V0, itcd.x);
            //gen_load_word($V0, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
        }
    }
    /*char sr[NAME_LEN];
    for (int i = 0; i < 7; i++)
    {
        get_s_by_num(i, sr);
        gen_load_word(sr, $FP, -4 * (i + 1));
    }*/
    move_fp_to_sp();
    pop_fp();

    fprintf(mips, "  jr $ra\n");
    gen_blank();
}
void handle_assign_ret(inter_code itcd)
{
    if (itcd.x->offset_to_fp == 0 && itcd.x->offset_to_gp == -1 && !itcd.x->has_reg)
    {
        handle_var_define(itcd.x);
    }
    move_register_to_word(itcd.x, $V0);
    //gen_store_word($V0, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
}
void pop_fp()
{
    gen_load($FP, $SP, 0, WORD);
    gen_addu($SP, $SP, 4);
}
void handle_start_call(inter_code itcd)
{

    for (int i = 0; i < t_reg_being_used.size(); i++)
    {
        sp -= 4;
        gen_store_word(t_reg_being_used[i].reg_name, $FP, sp - fp);
        t_reg_being_used[i].offset_tp_fp[t_reg_being_used[i].index] = sp - fp;
        t_reg_being_used[i].index++;
    }
    for (int i = 0; i < s_reg_being_used.size(); i++)
    {
        sp -= 4;
        gen_store_word(s_reg_being_used[i].reg_name, $FP, sp - fp);
        s_reg_being_used[i].offset_tp_fp[s_reg_being_used[i].index] = sp - fp;
        s_reg_being_used[i].index++;
    }
    for (int i = 0; i < a_reg_being_used.size(); i++)
    {
        sp -= 4;
        gen_store_word(a_reg_being_used[i].reg_name, $FP, sp - fp);
        a_reg_being_used[i].offset_tp_fp[a_reg_being_used[i].index] = sp - fp;
        a_reg_being_used[i].index++;
    }
    last_push_location = sp;
    started++;
}
void handle_fun_call(inter_code itcd)
{
    gen_addu($SP, $SP, sp - fp - 4);
    /* all_para_num = itcd.x->para_num;
    char sr[NAME_LEN];
    for (int i = 0; i < current_push_num; i++)
    {
        get_s_by_num(i, sr);
        gen_addu($SP, $SP, -4);
        gen_store_word(sr, $SP, 0);
    }
    current_push_num -= all_para_num;*/
    //gen_addu($SP, $SP, -4);
    gen_store_word($RA, $SP, 0);
    fprintf(mips, "  jal %s\n", itcd.x->name);
    gen_load($RA, $SP, 0, WORD);
    //gen_addu($SP, $SP, +4);
    //gen_addu($SP, $SP, 4 * (itcd.x->para_num + 1));
    gen_addu($SP, $SP, int(fp - sp) + 4);
    sp = sp + ((itcd.x->para_num) * 4 > 0 ? (itcd.x->para_num) * 4 : 0);
    for (int i = a_reg_being_used.size() - 1; i >= 0; i--)
    {
        gen_load(a_reg_being_used[i].reg_name, $FP, a_reg_being_used[i].offset_tp_fp[a_reg_being_used[i].index - 1], WORD);
        a_reg_being_used[i].index--;
        sp += 4;
    }
    for (int i = s_reg_being_used.size() - 1; i >= 0; i--)
    {
        gen_load(s_reg_being_used[i].reg_name, $FP, s_reg_being_used[i].offset_tp_fp[s_reg_being_used[i].index - 1], WORD);
        s_reg_being_used[i].index--;
        sp += 4;
    }
    for (int i = t_reg_being_used.size() - 1; i >= 0; i--)
    {
        gen_load(t_reg_being_used[i].reg_name, $FP, t_reg_being_used[i].offset_tp_fp[t_reg_being_used[i].index - 1], WORD);
        t_reg_being_used[i].index--;
        sp += 4;
    }

    started--;

    last_push_location = sp;
}
void handle_var_define(entry *ety)
{
    if (ety->level != 1)
    {
        if (t_reg_available.size() > 0 && ety->is_tmp)
        {
            assign_reg(ety);
        }
        else if (s_reg_available.size() > 0 && !ety->is_tmp && ety->dimension == 0)
        {
            assign_reg(ety);
        }
        else
        {
            if (ety->dimension == 0)
            {
                //gen_addu($SP, $SP, -4);
                local_var_space += 4;
                sp -= 4;
                ety->offset_to_fp = sp - fp;
            }
            else
            {
                if (strcmp(ety->type, INT) == 0)
                {

                    //gen_addu($SP, $SP, -4 * (ety->dimension));
                    local_var_space += 4 * (ety->dimension);
                    sp -= 4 * (ety->dimension);

                    ety->offset_to_fp = sp - fp;
                }
                else
                {

                    //basgen_addu($SP, $SP, -4 * ((ety->dimension) / 4 + 1));
                    local_var_space += 4 * ((ety->dimension) / 4 + 1);
                    sp -= 4 * ((ety->dimension) / 4 + 1);

                    ety->offset_to_fp = sp - fp;
                }
            }
        }
    }
    else
    {

        if (ety->dimension == 0)
        {
            ety->offset_to_gp = global_var_space - 0;
            global_var_space += 8;
        }
        else
        {
            if (strcmp(ety->type, INT) == 0)
            {
                ety->offset_to_gp = global_var_space;
                global_var_space += 4 * (ety->dimension);
            }
            else
            {
                ety->offset_to_gp = global_var_space;
                global_var_space += 4 * ((ety->dimension) / 4 + 1);
            }
        }
    }
}
void handle_para_define(entry *ety)
{
    if (a_reg_available.size() > 0)
    {
        reg a_reg = a_reg_available[0];
        a_reg_available.erase(a_reg_available.begin());
        a_reg_being_used.push_back(a_reg);
        ety->my_reg = a_reg;
        ety->has_reg = true;
        gen_load(ety->my_reg.reg_name, $FP, (all_para_num - current_para_num + 1) * 4, WORD);
    }
    else
    {
        if (s_reg_available.size() > 0)
        {
            assign_reg(ety);
            gen_load(ety->my_reg.reg_name, $FP, (all_para_num - current_para_num + 1) * 4, WORD);
        }
        else
        {
            ety->offset_to_fp = (all_para_num - current_para_num + 1) * 4;
        }
    }
    current_para_num++;
}
void handle_para_push(entry *ety)
{
    /*char available_register[NAME_LEN];
    get_s_register(available_register);
    if (ety->is_immediate || strcmp(ety->category, CONST) == 0)
    {
        if (strcmp(ety->type, INT) == 0)
        {
            gen_load_immediate(available_register, ety->int_value);
        }
        else
        {
            gen_load_immediate(available_register, ety->char_value);
        }
    }
    else
    {
        gen_load_word(available_register, ety->level == 1 ? $GP : $FP, ety->level == 1 ? ety->offset_to_gp : ety->offset_to_fp);
    }*/
    /*if (a_reg_available.back().size() > 0)
    {
        reg a_reg = a_reg_available.back()[0];
        a_reg_available.back().erase(a_reg_available.back().begin());
        a_reg_being_used.back().push_back(a_reg);
        if (ety->is_immediate || strcmp(ety->category, CONST) == 0)
        {
            if (strcmp(ety->type, INT) == 0)
            {
                gen_load_immediate(a_reg.reg_name, ety->int_value);
            }
            else
            {
                gen_load_immediate(a_reg.reg_name, ety->char_value);
            }
        }
        else
        {
            if (ety->has_reg)
            {
                gen_move(a_reg.reg_name, ety->my_reg.reg_name);
                //gen_store_word(ety->my_reg.reg_name, $FP, sp - fp);
                release_reg(ety);
            }
            else
            {
                gen_load(a_reg.reg_name, ety->level == 1 ? $GP : $FP, ety->level == 1 ? ety->offset_to_gp : ety->offset_to_fp, strcmp(ety->type, INT) == 0 ? WORD : BYTE);
                //gen_store_word($T8, $FP, sp - fp);
            }
        }
    }
    else*/
    {
        sp = last_push_location;
        sp -= 4;
        last_push_location = sp;

        if (ety->is_immediate || strcmp(ety->category, CONST) == 0)
        {
            if (strcmp(ety->type, INT) == 0)
            {
                gen_load_immediate($T8, ety->int_value);
            }
            else
            {
                gen_load_immediate($T8, ety->char_value);
            }
            gen_store_word($T8, $FP, sp - fp);
        }
        else
        {
            if (ety->has_reg)
            {
                gen_store_word(ety->my_reg.reg_name, $FP, sp - fp);
                release_reg(ety);
            }
            else
            {
                gen_load($T8, ety->level == 1 ? $GP : $FP, ety->level == 1 ? ety->offset_to_gp : ety->offset_to_fp, strcmp(ety->type, INT) == 0 ? WORD : BYTE);
                gen_store_word($T8, $FP, sp - fp);
            }
        }
    }
}
void handle_fun_define(entry *ety)
{
    prepare_reg();
    local_var_space = 0;
    is_main = false;
    all_para_num = ety->para_num;
    current_para_num = 0;
    fprintf(mips, "%s:\n", ety->name);

    gen_addu($SP, $SP, -4);
    gen_store_word($FP, $SP, 0);
    move_sp_to_fp();
    /*char sr[NAME_LEN];

    for (int i = 0; i < 7; i++)
    {
        get_s_by_num(i, sr);
        gen_addu($SP, $SP, -4);
        gen_store_word(sr, $SP, 0);
        sp -= 4;
    }*/

    gen_blank();
}
void handle_assign_without_array(inter_code itcd)
{
    if (itcd.x->offset_to_fp == 0 && itcd.x->offset_to_gp == -1 && !itcd.x->has_reg)
    {
        handle_var_define(itcd.x);
    }
    if (itcd.z == NULL)
    {
        if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
        {
            if (itcd.x->has_reg)
            {
                if (strcmp(itcd.y->type, INT) == 0)
                {
                    gen_load_immediate(itcd.x->my_reg.reg_name, itcd.y->int_value);
                }
                else
                {
                    gen_load_immediate(itcd.x->my_reg.reg_name, itcd.y->char_value);
                }
            }
            else
            {
                if (strcmp(itcd.y->type, INT) == 0)
                {
                    gen_load_immediate($T8, itcd.y->int_value);
                }
                else
                {
                    gen_load_immediate($T8, itcd.y->char_value);
                }
                if (strcmp(itcd.x->type, INT) == 0)
                {
                    gen_store_word($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
                else
                {
                    gen_store_byte($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
            }
        }
        else
        {
            //printf("1\n");
            if (itcd.x->has_reg && itcd.y->has_reg)
            {
                gen_move(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name);
                release_reg(itcd.y);
            }
            else if (itcd.x->has_reg && !itcd.y->has_reg)
            {

                gen_load(itcd.x->my_reg.reg_name, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
            }
            else if (!itcd.x->has_reg && itcd.y->has_reg)
            {
                gen_store_word(itcd.y->my_reg.reg_name, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                release_reg(itcd.y);
            }
            else
            {
                if (strcmp(itcd.x->type, INT) == 0)
                {
                    gen_load($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                    gen_store_word($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
                else
                {
                    gen_load_byte($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp);
                    gen_store_byte($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
            }
        }
    }
    else
    {
        if (itcd.x->has_reg)
        {
            if (itcd.y->has_reg && itcd.z->has_reg)
            {
                switch (itcd.op)
                {
                case '+':
                    gen_addu(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    break;
                case '-':
                    gen_subu(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    break;
                case '*':
                    gen_mul(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    break;
                case '/':
                    gen_div(itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                    break;
                default:
                    break;
                }
                release_reg(itcd.y);
                release_reg(itcd.z);
            }
            else if (itcd.y->has_reg && !itcd.z->has_reg)
            {
                if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_addu(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        break;
                    case '-':
                        gen_subu(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        break;
                    case '*':
                        gen_mul(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        break;
                    case '/':
                        //gen_load_immediate($T8, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        gen_div(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        //fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_addu(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, $T9);
                        break;
                    case '-':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_subu(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, $T9);
                        break;
                    case '*':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_mul(itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, $T9);
                        break;
                    case '/':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_div(itcd.y->my_reg.reg_name, $T9);
                        fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                        break;
                    default:
                        break;
                    }
                }
                release_reg(itcd.y);
            }
            else if (!itcd.y->has_reg && itcd.z->has_reg)
            {
                if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_addu(itcd.x->my_reg.reg_name, itcd.z->my_reg.reg_name, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        break;
                    case '-':
                        gen_subu(itcd.x->my_reg.reg_name, itcd.z->my_reg.reg_name, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        gen_subu(itcd.x->my_reg.reg_name, $0, itcd.x->my_reg.reg_name);
                        break;
                    case '*':
                        gen_mul(itcd.x->my_reg.reg_name, itcd.z->my_reg.reg_name, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        break;
                    case '/':
                        gen_load_immediate($T8, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        gen_div($T8, itcd.z->my_reg.reg_name);
                        fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_addu(itcd.x->my_reg.reg_name, $T9, itcd.z->my_reg.reg_name);
                        break;
                    case '-':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_subu(itcd.x->my_reg.reg_name, $T9, itcd.z->my_reg.reg_name);
                        break;
                    case '*':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_mul(itcd.x->my_reg.reg_name, $T9, itcd.z->my_reg.reg_name);
                        break;
                    case '/':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_div($T9, itcd.z->my_reg.reg_name);
                        fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                        break;
                    default:
                        break;
                    }
                }
                release_reg(itcd.z);
            }
            else
            {
                if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
                {

                    if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
                    {
                        switch (itcd.op)
                        {
                        case '+':
                            gen_load_immediate(itcd.x->my_reg.reg_name, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) + (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        case '-':
                            gen_load_immediate(itcd.x->my_reg.reg_name, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) - (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        case '*':
                            gen_load_immediate(itcd.x->my_reg.reg_name, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) * (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        case '/':
                            gen_load_immediate(itcd.x->my_reg.reg_name, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) / (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {

                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);

                        switch (itcd.op)
                        {
                        case '+':
                            gen_addu(itcd.x->my_reg.reg_name, $T9, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            break;
                        case '-':
                            gen_subu(itcd.x->my_reg.reg_name, $T9, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            gen_subu(itcd.x->my_reg.reg_name, $0, itcd.x->my_reg.reg_name);
                            break;
                        case '*':
                            gen_mul(itcd.x->my_reg.reg_name, $T9, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            break;
                        case '/':
                            gen_load_immediate($T8, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            gen_div($T8, $T9);
                            fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                            break;
                        default:
                            break;
                        }
                    }
                }
                else
                {
                    if (strcmp(itcd.y->type, INT) == 0)
                    {
                        gen_load($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                    }
                    else
                    {
                        gen_load_byte($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp);
                    }
                    if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
                    {
                        switch (itcd.op)
                        {
                        case '+':
                            gen_addu(itcd.x->my_reg.reg_name, $T8, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            break;
                        case '-':
                            gen_subu(itcd.x->my_reg.reg_name, $T8, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            break;
                        case '*':
                            gen_mul(itcd.x->my_reg.reg_name, $T8, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            break;
                        case '/':
                            //gen_load_immediate($T9, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            gen_div(itcd.x->my_reg.reg_name, $T8, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            //fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {

                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);

                        switch (itcd.op)
                        {
                        case '+':
                            gen_addu(itcd.x->my_reg.reg_name, $T8, $T9);
                            break;
                        case '-':
                            gen_subu(itcd.x->my_reg.reg_name, $T8, $T9);
                            break;
                        case '*':
                            gen_mul(itcd.x->my_reg.reg_name, $T8, $T9);
                            break;
                        case '/':
                            gen_div($T8, $T9);
                            fprintf(mips, "  mflo %s\n", itcd.x->my_reg.reg_name);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if (itcd.y->has_reg && itcd.z->has_reg)
            {
                switch (itcd.op)
                {
                case '+':
                    gen_addu($T8, itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    break;
                case '-':
                    gen_subu($T8, itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    break;
                case '*':
                    gen_mul($T8, itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    break;
                case '/':
                    gen_div(itcd.y->my_reg.reg_name, itcd.z->my_reg.reg_name);
                    fprintf(mips, "  mflo %s\n", $T8);
                    break;
                default:
                    break;
                }
                release_reg(itcd.y);
                release_reg(itcd.z);
            }
            else if (itcd.y->has_reg && !itcd.z->has_reg)
            {
                if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_addu($T8, itcd.y->my_reg.reg_name, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        break;
                    case '-':
                        gen_subu($T8, itcd.y->my_reg.reg_name, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        break;
                    case '*':
                        gen_mul($T8, itcd.y->my_reg.reg_name, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        break;
                    case '/':
                        gen_load_immediate($T8, strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value);
                        gen_div(itcd.y->my_reg.reg_name, $T8);
                        fprintf(mips, "  mflo %s\n", $T8);
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_addu($T8, itcd.y->my_reg.reg_name, $T9);
                        break;
                    case '-':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_subu($T8, itcd.y->my_reg.reg_name, $T9);
                        break;
                    case '*':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_mul($T8, itcd.y->my_reg.reg_name, $T9);
                        break;
                    case '/':
                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                        gen_div(itcd.y->my_reg.reg_name, $T9);
                        fprintf(mips, "  mflo %s\n", $T8);
                        break;
                    default:
                        break;
                    }
                }
                //printf("%s %d %d \n",itcd.y->name,itcd.y->last_appear_time,ic_index);
                release_reg(itcd.y);
            }
            else if (!itcd.y->has_reg && itcd.z->has_reg)
            {
                if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_addu($T8, itcd.z->my_reg.reg_name, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        break;
                    case '-':
                        gen_subu($T8, itcd.z->my_reg.reg_name, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        gen_subu($T8, $0, $T8);
                        break;
                    case '*':
                        gen_mul($T8, itcd.z->my_reg.reg_name, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        break;
                    case '/':
                        gen_load_immediate($T8, strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value);
                        gen_div($T8, itcd.z->my_reg.reg_name);
                        fprintf(mips, "  mflo %s\n", $T8);
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    switch (itcd.op)
                    {
                    case '+':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_addu($T8, $T9, itcd.z->my_reg.reg_name);
                        break;
                    case '-':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_subu($T8, $T9, itcd.z->my_reg.reg_name);
                        break;
                    case '*':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_mul($T8, $T9, itcd.z->my_reg.reg_name);
                        break;
                    case '/':
                        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                        gen_div($T9, itcd.z->my_reg.reg_name);
                        fprintf(mips, "  mflo %s\n", $T8);
                        break;
                    default:
                        break;
                    }
                }
                release_reg(itcd.z);
            }
            else
            {
                if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
                {

                    if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
                    {
                        switch (itcd.op)
                        {
                        case '+':
                            gen_load_immediate($T8, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) + (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        case '-':
                            gen_load_immediate($T8, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) - (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        case '*':
                            gen_load_immediate($T8, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) * (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        case '/':
                            gen_load_immediate($T8, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value) / (strcmp(itcd.z->type, INT) == 0 ? itcd.z->int_value : itcd.z->char_value));
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {

                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);

                        switch (itcd.op)
                        {
                        case '+':
                            gen_addu($T8, $T9, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            break;
                        case '-':
                            gen_subu($T8, $T9, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            gen_subu($T8, $0, $T8);
                            break;
                        case '*':
                            gen_mul($T8, $T9, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            break;
                        case '/':
                            gen_load_immediate($T8, (strcmp(itcd.y->type, INT) == 0 ? itcd.y->int_value : itcd.y->char_value));
                            gen_div($T8, $T9);
                            fprintf(mips, "  mflo %s\n", $T8);
                            break;
                        default:
                            break;
                        }
                    }
                }
                else
                {

                    gen_load($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);

                    if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
                    {
                        switch (itcd.op)
                        {
                        case '+':
                            gen_addu($T8, $T8, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            break;
                        case '-':
                            gen_subu($T8, $T8, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            break;
                        case '*':
                            gen_mul($T8, $T8, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            break;
                        case '/':
                            gen_load_immediate($T9, (strcmp(itcd.z->type, INT)) == 0 ? itcd.z->int_value : itcd.z->char_value);
                            gen_div($T8, $T9);
                            fprintf(mips, "  mflo %s\n", $T8);
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {

                        gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);

                        switch (itcd.op)
                        {
                        case '+':
                            gen_addu($T8, $T8, $T9);
                            break;
                        case '-':
                            gen_subu($T8, $T8, $T9);
                            break;
                        case '*':
                            gen_mul($T8, $T8, $T9);
                            break;
                        case '/':
                            gen_div($T8, $T9);
                            fprintf(mips, "  mflo %s\n", $T8);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            gen_store_word($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
        }
    }

    gen_blank();
}
void handle_assign_with_array(inter_code itcd)
{
    if (itcd.x->offset_to_fp == 0 && itcd.x->offset_to_gp == -1 && !itcd.x->has_reg)
    {
        handle_var_define(itcd.x);
    }
    if (strcmp(itcd.x->type, INT) == 0)
    {
        gen_addu($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp);
        //gen_load_immediate($T8, itcd.y->level == 1 ? (int)(gp + itcd.y->offset_to_gp) : (int)(fp + itcd.y->offset_to_fp));
        if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
        {

            if (itcd.x->has_reg)
            {
                gen_load(itcd.x->my_reg.reg_name, $T8, (itcd.z->int_value) * 4, WORD);
            }
            else
            {
                gen_load($T9, $T8, (itcd.z->int_value) * 4, WORD);
                gen_store_word($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
            }
        }
        else
        {
            if (itcd.z->has_reg)
            {
                gen_mul($T9, itcd.z->my_reg.reg_name, 4);
                gen_addu($T9, $T8, $T9);
                if (itcd.x->has_reg)
                {
                    gen_load(itcd.x->my_reg.reg_name, $T9, 0, WORD);
                }
                else
                {
                    gen_load($T9, $T9, 0, WORD);
                    gen_store_word($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
                release_reg(itcd.z);
            }
            else
            {
                gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                gen_mul($T9, $T9, 4);
                gen_addu($T9, $T8, $T9);
                if (itcd.x->has_reg)
                {
                    gen_load(itcd.x->my_reg.reg_name, $T9, 0, WORD);
                }
                else
                {
                    gen_load($T9, $T9, 0, WORD);
                    gen_store_word($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
            }
        }
    }
    else
    {
        gen_addu($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp);
        //gen_load_immediate($T8, itcd.y->level == 1 ? (int)(gp + itcd.y->offset_to_gp) : (int)(fp + itcd.y->offset_to_fp));
        if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
        {
            if (itcd.x->has_reg)
            {
                gen_load_byte(itcd.x->my_reg.reg_name, $T8, (itcd.z->int_value));
            }
            else
            {
                gen_load_byte($T9, $T8, (itcd.z->int_value));
                gen_store_byte($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
            }
        }
        else
        {
            if (itcd.z->has_reg)
            {
                gen_addu($T9, $T8, itcd.z->my_reg.reg_name);
                if (itcd.x->has_reg)
                {
                    gen_load_byte(itcd.x->my_reg.reg_name, $T9, 0);
                }
                else
                {
                    gen_load_byte($T9, $T9, 0);
                    gen_store_byte($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
                release_reg(itcd.z);
            }
            else
            {
                gen_load($T9, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
                gen_addu($T9, $T8, $T9);
                if (itcd.x->has_reg)
                {
                    gen_load_byte(itcd.x->my_reg.reg_name, $T9, 0);
                }
                else
                {
                    gen_load_byte($T9, $T9, 0);
                    gen_store_byte($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
                }
            }
        }
    }
    gen_blank();
}
void handle_assign_an_array(inter_code itcd)
{
    if (strcmp(itcd.x->type, INT) == 0)
    {
        gen_addu($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
        if (itcd.y->has_reg && itcd.z->has_reg)
        {
            gen_mul($T8, itcd.y->my_reg.reg_name, 4);
            gen_addu($T9, $T9, $T8);
            gen_store_word(itcd.z->my_reg.reg_name, $T9, 0);
            release_reg(itcd.y);
            release_reg(itcd.z);
        }
        else if (itcd.y->has_reg && !itcd.z->has_reg)
        {
            gen_mul($T8, itcd.y->my_reg.reg_name, 4);
            gen_addu($T9, $T9, $T8);
            if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
            {
                if (strcmp(itcd.z->type, CHAR) == 0)
                {
                    gen_load_immediate($T8, itcd.z->char_value);
                }
                else
                {
                    gen_load_immediate($T8, itcd.z->int_value);
                }
            }
            else
            {
                gen_load($T8, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
            }
            gen_store_word($T8, $T9, 0);
            release_reg(itcd.y);
        }
        else if (!itcd.y->has_reg && itcd.z->has_reg)
        {
            if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
            {
                gen_addu($T9, $T9, itcd.y->int_value * 4);
            }
            else
            {
                gen_load($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                gen_mul($T8, $T8, 4);
                gen_addu($T9, $T9, $T8);
            }
            gen_store_word(itcd.z->my_reg.reg_name, $T9, 0);
            release_reg(itcd.z);
        }
        else
        {
            if (!itcd.y->is_immediate && strcmp(itcd.y->category, CONST) != 0)
            {
                gen_load($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                gen_mul($T8, $T8, 4);
                gen_addu($T9, $T9, $T8);
            }
            if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
            {
                if (strcmp(itcd.z->type, CHAR) == 0)
                {
                    gen_load_immediate($T8, itcd.z->char_value);
                }
                else
                {
                    gen_load_immediate($T8, itcd.z->int_value);
                }
            }
            else
            {

                gen_load($T8, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
            }
            if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
            {
                gen_store_word($T8, $T9, (itcd.y->int_value) * 4);
            }
            else
            {
                gen_store_word($T8, $T9, 0);
            }
        }
    }
    else
    {
        gen_addu($T9, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp);
        if (itcd.y->has_reg && itcd.z->has_reg)
        {
            gen_addu($T9, $T9, itcd.y->my_reg.reg_name);
            gen_store_byte(itcd.z->my_reg.reg_name, $T9, 0);
            release_reg(itcd.y);
            release_reg(itcd.z);
        }
        else if (itcd.y->has_reg && !itcd.z->has_reg)
        {
            gen_addu($T9, $T9, itcd.y->my_reg.reg_name);
            if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
            {
                if (strcmp(itcd.z->type, CHAR) == 0)
                {
                    gen_load_immediate($T8, itcd.z->char_value);
                }
                else
                {
                    gen_load_immediate($T8, itcd.z->int_value);
                }
            }
            else
            {

                gen_load($T8, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
            }
            gen_store_byte($T8, $T9, 0);
            release_reg(itcd.y);
        }
        else if (!itcd.y->has_reg && itcd.z->has_reg)
        {
            if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
            {
                gen_addu($T9, $T9, itcd.y->int_value);
            }
            else
            {
                gen_load($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                gen_addu($T9, $T9, $T8);
            }
            gen_store_byte(itcd.z->my_reg.reg_name, $T9, 0);
            release_reg(itcd.z);
        }
        else
        {
            if (!itcd.y->is_immediate && strcmp(itcd.y->category, CONST) != 0)
            {
                gen_load($T8, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
                gen_addu($T9, $T9, $T8);
            }
            if (itcd.z->is_immediate || strcmp(itcd.z->category, CONST) == 0)
            {
                if (strcmp(itcd.z->type, CHAR) == 0)
                {
                    gen_load_immediate($T8, itcd.z->char_value);
                }
                else
                {
                    gen_load_immediate($T8, itcd.z->int_value);
                }
            }
            else
            {
                gen_load($T8, itcd.z->level == 1 ? $GP : $FP, itcd.z->level == 1 ? itcd.z->offset_to_gp : itcd.z->offset_to_fp, strcmp(itcd.z->type, INT) == 0 ? WORD : BYTE);
            }
            if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
            {
                gen_store_byte($T8, $T9, (itcd.y->int_value));
            }
            else
            {
                gen_store_byte($T8, $T9, 0);
            }
        }
    }
    gen_blank();
}
void handle_set(inter_code itcd)
{
    fprintf(mips, "%s:\n", itcd.x->name);
}
void handle_jump(inter_code itcd)
{
    fprintf(mips, "  j %s\n", itcd.x->name);
}
void handle_condition(inter_code itcd)
{
    if (itcd.x->is_immediate || strcmp(itcd.x->category, CONST) == 0)
    {
        if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
        {
            switch_relation(itcd, 1, 1);
        }
        else
        {
            switch_relation(itcd, 1, 0);
        }
    }
    else
    {
        if (itcd.y->is_immediate || strcmp(itcd.y->category, CONST) == 0)
        {
            switch_relation(itcd, 0, 1);
        }
        else
        {
            switch_relation(itcd, 0, 0);
        }
    }
}
void switch_relation(inter_code itcd, int x_is_immediate, int y_is_immediate)
{
    if (x_is_immediate && y_is_immediate)
    {
        switch (itcd.inter_type)
        {
        case _BEQ:
            gen_load_immediate($T8, itcd.x->int_value);
            fprintf(mips, "  beq %s %d %s\n", $T8, itcd.y->int_value, itcd.z->name);
            break;
        case _BNE:
            gen_load_immediate($T8, itcd.x->int_value);
            fprintf(mips, "  bne %s %d %s\n", $T8, itcd.y->int_value, itcd.z->name);
            break;
        case _LSS:
            gen_load_immediate($T8, itcd.x->int_value);
            gen_subu($T8, $T8, itcd.y->int_value);
            fprintf(mips, "  bltz %s %s\n", $T8, itcd.z->name);
            break;
        case _LEQ:
            gen_load_immediate($T8, itcd.x->int_value);
            gen_subu($T8, $T8, itcd.y->int_value);
            fprintf(mips, "  blez %s %s\n", $T8, itcd.z->name);
            break;
        case _GRE:
            gen_load_immediate($T8, itcd.x->int_value);
            gen_subu($T8, $T8, itcd.y->int_value);
            fprintf(mips, "  bgtz %s %s\n", $T8, itcd.z->name);
            break;
        case _GEQ:
            gen_load_immediate($T8, itcd.x->int_value);
            gen_subu($T8, $T8, itcd.y->int_value);
            fprintf(mips, "  bgez %s %s\n", $T8, itcd.z->name);
            break;
        default:
            break;
        }
    }
    else if (x_is_immediate && !y_is_immediate)
    {
        entry *tmp = itcd.x;
        itcd.x = itcd.y;
        itcd.y = tmp;
        tmp = NULL;
        switch (itcd.inter_type)
        {
        case _BEQ:
            help_three_one((char *)"beq", itcd);
            break;
        case _BNE:
            help_three_one((char *)"bne", itcd);
            break;
        case _LSS:
            itcd.inter_type = _GRE;
            help_three_two((char *)"bgtz", itcd);
            break;
        case _LEQ:
            itcd.inter_type = _GEQ;
            help_three_two((char *)"bgez", itcd);
            break;
        case _GRE:
            itcd.inter_type = _LSS;
            help_three_two((char *)"bltz", itcd);
            break;
        case _GEQ:
            itcd.inter_type = _LEQ;
            help_three_two((char *)"blez", itcd);
            break;
        default:
            break;
        }
    }
    else if (!x_is_immediate && y_is_immediate)
    {
        switch (itcd.inter_type)
        {
        case _BEQ:
            help_three_one((char *)"beq", itcd);
            break;
        case _BNE:
            help_three_one((char *)"bne", itcd);
            break;
        case _LSS:
            help_three_two((char *)"bltz", itcd);
            break;
        case _LEQ:
            help_three_two((char *)"blez", itcd);
            break;
        case _GRE:
            help_three_two((char *)"bgtz", itcd);
            break;
        case _GEQ:
            help_three_two((char *)"bgez", itcd);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (itcd.inter_type)
        {
        case _BEQ:
            help_four_one((char *)"beq", itcd);
            break;
        case _BNE:
            help_four_one((char *)"bne", itcd);
            break;
        case _LSS:
            help_four_two((char *)"bltz", itcd);
            break;
        case _LEQ:
            help_four_two((char *)"blez", itcd);
            break;
        case _GRE:
            help_four_two((char *)"bgtz", itcd);
            break;
        case _GEQ:
            help_four_two((char *)"bgez", itcd);
            break;
        default:
            break;
        }
    }
}

void help_three_one(char *relation, inter_code itcd)
{
    if (itcd.x->has_reg)
    {
        fprintf(mips, "  %s %s %d %s\n", relation, itcd.x->my_reg.reg_name, itcd.y->int_value, itcd.z->name);
        release_reg(itcd.x);
    }
    else
    {
        gen_load($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp, strcmp(itcd.x->type, INT) == 0 ? WORD : BYTE);
        fprintf(mips, "  %s %s %d %s\n", relation, $T8, itcd.y->int_value, itcd.z->name);
    }
}
void help_three_two(char *relation, inter_code itcd)
{
    if (itcd.x->has_reg)
    {
        gen_subu($T8, itcd.x->my_reg.reg_name, itcd.y->int_value);
        fprintf(mips, "  %s %s %s\n", relation, $T8, itcd.z->name);
        release_reg(itcd.x);
    }
    else
    {
        gen_load($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp, strcmp(itcd.x->type, INT) == 0 ? WORD : BYTE);
        gen_subu($T8, $T8, itcd.y->int_value);
        fprintf(mips, "  %s %s %s\n", relation, $T8, itcd.z->name);
    }
}
void help_four_one(char *relation, inter_code itcd)
{
    if (itcd.x->has_reg && itcd.y->has_reg)
    {
        fprintf(mips, "  %s %s %s %s\n", relation, itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name, itcd.z->name);
        release_reg(itcd.x);
        release_reg(itcd.y);
    }
    else if (itcd.x->has_reg && !itcd.y->has_reg)
    {
        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
        fprintf(mips, "  %s %s %s %s\n", relation, itcd.x->my_reg.reg_name, $T9, itcd.z->name);
        release_reg(itcd.x);
    }
    else if (!itcd.x->has_reg && itcd.y->has_reg)
    {
        gen_load($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp, strcmp(itcd.x->type, INT) == 0 ? WORD : BYTE);
        fprintf(mips, "  %s %s %s %s\n", relation, $T8, itcd.y->my_reg.reg_name, itcd.z->name);
        release_reg(itcd.y);
    }
    else
    {
        gen_load($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp, strcmp(itcd.x->type, INT) == 0 ? WORD : BYTE);
        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
        fprintf(mips, "  %s %s %s %s\n", relation, $T8, $T9, itcd.z->name);
    }
}
void help_four_two(char *relation, inter_code itcd)
{
    if (itcd.x->has_reg && itcd.y->has_reg)
    {
        gen_subu($T8, itcd.x->my_reg.reg_name, itcd.y->my_reg.reg_name);
        fprintf(mips, "  %s %s %s\n", relation, $T8, itcd.z->name);
        release_reg(itcd.x);
        release_reg(itcd.y);
    }
    else if (itcd.x->has_reg && !itcd.y->has_reg)
    {
        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
        gen_subu($T8, itcd.x->my_reg.reg_name, $T9);
        fprintf(mips, "  %s %s %s\n", relation, $T8, itcd.z->name);
        release_reg(itcd.x);
    }
    else if (!itcd.x->has_reg && itcd.y->has_reg)
    {
        gen_load($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp, strcmp(itcd.x->type, INT) == 0 ? WORD : BYTE);
        gen_subu($T8, $T8, itcd.y->my_reg.reg_name);
        fprintf(mips, "  %s %s %s\n", relation, $T8, itcd.z->name);
        release_reg(itcd.y);
    }
    else
    {
        gen_load($T8, itcd.x->level == 1 ? $GP : $FP, itcd.x->level == 1 ? itcd.x->offset_to_gp : itcd.x->offset_to_fp, strcmp(itcd.x->type, INT) == 0 ? WORD : BYTE);
        gen_load($T9, itcd.y->level == 1 ? $GP : $FP, itcd.y->level == 1 ? itcd.y->offset_to_gp : itcd.y->offset_to_fp, strcmp(itcd.y->type, INT) == 0 ? WORD : BYTE);
        gen_subu($T8, $T8, $T9);
        fprintf(mips, "  %s %s %s\n", relation, $T8, itcd.z->name);
    }
}
void gen_blank()
{
    fprintf(mips, "\n");
}
void gen_syscall()
{
    fprintf(mips, "  syscall\n");
}
void gen_move(char *r1, char *r2)
{
    fprintf(mips, "  move %s %s\n", r1, r2);
}
void gen_addu(char *r1, char *r2, int value)
{
    fprintf(mips, "  add %s %s %d\n", r1, r2, value);
}
void gen_addu(char *r1, char *r2, char *r3)
{
    fprintf(mips, "  add %s %s %s\n", r1, r2, r3);
}
void gen_subu(char *r1, char *r2, int value)
{
    fprintf(mips, "  sub %s %s %d\n", r1, r2, value);
}
void gen_subu(char *r1, char *r2, char *r3)
{
    fprintf(mips, "  sub %s %s %s\n", r1, r2, r3);
}
void gen_mul(char *r1, char *r2, int value)
{
    fprintf(mips, "  mul %s %s %d\n", r1, r2, value);
}
void gen_mul(char *r1, char *r2, char *r3)
{
    fprintf(mips, "  mul %s %s %s\n", r1, r2, r3);
}
void gen_divu(char *r1, char *r2, int value)
{
    fprintf(mips, "  divu %s %s %d\n", r1, r2, value);
}
void gen_divu(char *r1, char *r2, char *r3)
{
    fprintf(mips, "  divu %s %s %s\n", r1, r2, r3);
}
void gen_div(char *r1, char *r2, int value)
{
    fprintf(mips, "  div %s %s %d\n", r1, r2, value);
}
void gen_div(char *r1, char *r2)
{
    fprintf(mips, "  div %s %s \n", r1, r2);
}
void gen_load_immediate(char *r1, int value)
{
    fprintf(mips, "  li %s 0x%x\n", r1, value);
}
void gen_load_immediate(char *r1, char value)
{
    fprintf(mips, "  li %s %d\n", r1, value);
}
void gen_load_word(char *r1, uint32_t addr)
{
    fprintf(mips, "  lw %s 0x%x\n", r1, addr);
}
void gen_load(char *r1, char *r2, int offset, int word_or_byte)
{
    if (word_or_byte == WORD)
        fprintf(mips, "  lw %s %d(%s)\n", r1, offset, r2);
    else
        fprintf(mips, "  lbu %s %d(%s)\n", r1, offset, r2);
}
void gen_load_byte(char *r1, uint32_t addr)
{
    fprintf(mips, "  lbu %s 0x%x\n", r1, addr);
}
void gen_load_byte(char *r1, char *r2, int offset)
{
    fprintf(mips, "  lbu %s %d(%s)\n", r1, offset, r2);
}
void gen_store_word(char *r1, uint32_t addr)
{
    fprintf(mips, "  sw %s 0x%x\n", r1, addr);
}
void gen_store_word(char *r1, char *r2, int offset)
{
    fprintf(mips, "  sw %s %d(%s)\n", r1, offset, r2);
}
void gen_store_byte(char *r1, uint32_t addr)
{
    fprintf(mips, "  sb %s 0x%x\n", r1, addr);
}
void gen_store_byte(char *r1, char *r2, int offset)
{
    fprintf(mips, "  sb %s %d(%s)\n", r1, offset, r2);
}
void gen_load_address(char *r1, char *string_name)
{
    fprintf(mips, "  la %s %s\n", r1, string_name);
}

void move_word_to_register(char *r1, entry *ety)
{
    if (ety->has_reg)
    {
        gen_move(r1, ety->my_reg.reg_name);
        release_reg(ety);
    }
    else
    {
        gen_load(r1, ety->level == 1 ? $GP : $FP, ety->level == 1 ? ety->offset_to_gp : ety->offset_to_fp, strcmp(ety->type, INT) == 0 ? WORD : BYTE);
    }
}
void move_byte_to_register(char *r1, entry *ety)
{
    if (ety->has_reg)
    {
        gen_move(r1, ety->my_reg.reg_name);
        release_reg(ety);
    }
    else
    {
        gen_load_byte(r1, ety->level == 1 ? $GP : $FP, ety->level == 1 ? ety->offset_to_gp : ety->offset_to_fp);
    }
}
void move_register_to_word(entry *ety, char *r1)
{
    if (ety->has_reg)
    {
        gen_move(ety->my_reg.reg_name, r1);
    }
    else
    {
        gen_store_word(r1, ety->level == 1 ? $GP : $FP, ety->level == 1 ? ety->offset_to_gp : ety->offset_to_fp);
    }
}
