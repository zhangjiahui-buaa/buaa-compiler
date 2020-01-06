#include "head.h"
using namespace std;

vector<vector<inter_code>> ic_by_fun;
int fun_count;
void delete_straight_assign_before_inline();
void delete_straight_assign_after_inline();

void delete_public_expression();
void function_inline();
void calculate_last_appear_time();
bool is_in_vector(entry *tmp, vector<entry *> ve);
int has_start_called;

void optimize_inter()
{

    printf_inter(ic, fopen("nn.txt", "w"));
    delete_straight_assign_before_inline();

    function_inline();

    delete_straight_assign_after_inline();
    //printf_inter(ic, fopen("n.txt", "w"));
    delete_public_expression();
    for (int i = 0; i < ic.size(); i++)
    {
        if (ic[i].op == '=' && ic[i].x != NULL && ic[i].x->is_tmp && ic[i].y != NULL && ic[i].z == NULL)
        {
            //sb.push_back(*(ic[i].y));
            //strcpy(sb[sb.size()-1].type,ic[i].x->type);
            int flag = 0;
            for (int j = i + 1; j < ic.size(); j++)
            {
                if (ic[i].block_loc == ic[j].block_loc)
                {
                    if (ic[j].x != NULL && ic[j].x == ic[i].x && strcmp(ic[j].x->type, ic[i].y->type) == 0)
                    {
                        ic[j].x = ic[i].y;
                        flag = 1;
                    }
                    if (ic[j].y != NULL && ic[j].y == ic[i].x && strcmp(ic[j].y->type, ic[i].y->type) == 0)
                    {
                        ic[j].y = ic[i].y;
                        flag = 1;
                    }
                    if (ic[j].z != NULL && ic[j].z == ic[i].x && strcmp(ic[j].z->type, ic[i].y->type) == 0)
                    {
                        ic[j].z = ic[i].y;
                        flag = 1;
                    }
                }
            }
            if (flag == 1)
            {
                ic.erase(ic.begin() + i);
                i--;
            }
        }
    }
    
    calculate_last_appear_time();
    printf_inter(ic, inter_file);
    //exit(0);
}
void delete_straight_assign_before_inline()
{
    vector<inter_code> tmp;
    int count = 0;
    for (int i = ic.size() - 1; i >= 0; i--)
    {
        if (ic[i].op == '=' && ic[i].y != NULL && ic[i].y->is_tmp)
        {
            for (int j = i - 1; j >= 0; j--)
            {
                if (ic[j].x != NULL && ic[j].x == ic[i].y && strcmp(ic[i].x->type, ic[j].x->type) == 0)
                {
                    //strcpy(ic[i].x->type, ic[j].x->type);
                    ic[j].x = ic[i].x;
                    ic.erase(ic.begin() + i);
                    break;
                }
            }
        }
    }
    for (int i = 0; i < ic.size(); i++)
    {
        if (ic[i].op == '=' && ic[i].x != NULL && ic[i].x->is_tmp && ic[i].y != NULL && ic[i].z == NULL)
        {
            //sb.push_back(*(ic[i].y));
            //strcpy(sb[sb.size()-1].type,ic[i].x->type);
            int flag = 0;
            for (int j = i + 1; j < ic.size(); j++)
            {
                if (ic[i].block_loc == ic[j].block_loc)
                {
                    if (ic[j].x != NULL && ic[j].x == ic[i].x && strcmp(ic[j].x->type, ic[i].y->type) == 0 /*&& strcmp(ic[j].x->category, ic[i].y->category) == 0*/ && ic[j].inter_type != ASSIGN_AN_ARRAY && ic[j].inter_type != ASSIGN_RET && ic[j].inter_type != ASSIGN_WITHOUT_ARRAY && ic[j].inter_type != ASSIGN_WITH_ARRAY)
                    {
                        ic[j].x = ic[i].y;
                        flag = 1;
                    }
                    if (ic[j].y != NULL && ic[j].y == ic[i].x && strcmp(ic[j].y->type, ic[i].y->type) == 0 /*&& strcmp(ic[j].y->category, ic[i].y->category) == 0*/)
                    {
                        ic[j].y = ic[i].y;
                        flag = 1;
                    }
                    if (ic[j].z != NULL && ic[j].z == ic[i].x && strcmp(ic[j].z->type, ic[i].y->type) == 0 /*&& strcmp(ic[j].z->category, ic[i].y->category) == 0*/)
                    {
                        ic[j].z = ic[i].y;
                        flag = 1;
                    }
                }
            }
            if (flag == 1)
            {
                ic.erase(ic.begin() + i);
                i--;
            }
        }
    }
}
void delete_straight_assign_after_inline()
{
    vector<inter_code> tmp;
    int count = 0;
    /*for (int i = ic.size() - 1; i >= 0; i--)
    {
        if (ic[i].op == '=' && ic[i].y != NULL && ic[i].y->is_tmp)
        {
            for (int j = i - 1; j >= 0; j--)
            {
                if (ic[j].x != NULL && ic[j].x == ic[i].y && strcmp(ic[i].x->type, ic[j].x->type) == 0 && (ic[j].inter_type ==ASSIGN_WITHOUT_ARRAY || ic[j].inter_type == ASSIGN_RET || ic[j].inter_type == ASSIGN_WITH_ARRAY ))
                {
                    //strcpy(ic[i].x->type, ic[j].x->type);
                    ic[j].x = ic[i].x;
                    ic.erase(ic.begin() + i);
                    break;
                }
            }
        }
    }*/
    for (int i = ic.size() - 1; i >= 0; i--)
    {
        if (ic[i].op == '=' && ic[i].x != NULL && ic[i].x->is_tmp && ic[i].y != NULL && ic[i].z == NULL)
        {
            //sb.push_back(*(ic[i].y));
            //strcpy(sb[sb.size()-1].type,ic[i].x->type);
            int flag = 0;
            for (int j = i + 1; j < ic.size(); j++)
            {
                if (ic[i].block_loc == ic[j].block_loc)
                {
                    if (ic[j].x != NULL && ic[j].x == ic[i].x && strcmp(ic[j].x->type, ic[i].y->type) == 0 && ic[j].inter_type != ASSIGN_AN_ARRAY && ic[j].inter_type != ASSIGN_RET && ic[j].inter_type != ASSIGN_WITHOUT_ARRAY && ic[j].inter_type != ASSIGN_WITH_ARRAY)
                    {
                        ic[j].x = ic[i].y;
                        flag = 1;
                    }
                    if (ic[j].y != NULL && ic[j].y == ic[i].x && strcmp(ic[j].y->type, ic[i].y->type) == 0)
                    {
                        ic[j].y = ic[i].y;
                        flag = 1;
                    }
                    if (ic[j].z != NULL && ic[j].z == ic[i].x && strcmp(ic[j].z->type, ic[i].y->type) == 0)
                    {
                        ic[j].z = ic[i].y;
                        flag = 1;
                    }
                }
            }
            if (flag == 1)
            {
                ic.erase(ic.begin() + i);
            }
        }
    }
    for (int i = 0; i < ic.size(); i++)
    {
        if (ic[i].inter_type == ASSIGN_WITHOUT_ARRAY || ic[i].inter_type == ASSIGN_WITH_ARRAY || ic[i].inter_type == ASSIGN_RET || ic[i].inter_type == ASSIGN_AN_ARRAY)
        {
            if (ic[i].x->is_tmp)
            {
                int found = 0;
                for (int j = i + 1; j < ic.size(); j++)
                {
                    if ((ic[j].x != NULL && ic[j].x == ic[i].x) || (ic[j].y != NULL && ic[j].y == ic[i].x) || (ic[j].z != NULL && ic[j].z == ic[i].x))
                    {
                        found = 1;
                    }
                }
                if (found == 0)
                {
                    ic.erase(ic.begin() + i);
                    i--;
                }
            }
        }
    }
}

void delete_public_expression()
{
    int current_loc = 0;
    for (int i = 0; i < ic.size(); i++)
    {
        if (ic[i].inter_type != ASSIGN_WITHOUT_ARRAY && ic[i].inter_type != ASSIGN_WITH_ARRAY && ic[i].inter_type != ASSIGN_AN_ARRAY && ic[i].inter_type != ASSIGN_RET && ic[i].inter_type != PARA_PUSH && ic[i].inter_type != PRINTF)
        {
            current_loc++;
            ic[i].block_loc = current_loc;
        }
        else
        {
            ic[i].block_loc = current_loc;
        }
    }
    for (int i = 0; i < ic.size(); i++)
    {
        if (ic[i].inter_type == ASSIGN_WITHOUT_ARRAY)
        {
            for (int j = i + 1; j < ic.size(); j++)
            {
                if (ic[j].x != NULL)
                {
                    if ((ic[i].y != NULL && ic[j].x == ic[i].y) || (ic[i].z != NULL && ic[j].x == ic[i].z) || ic[j].x == ic[i].x)
                    {
                        break;
                    }
                }
                if (ic[j].inter_type == ASSIGN_WITHOUT_ARRAY)
                {
                    if (ic[j].z == NULL)
                    {
                        if (ic[j].y == ic[i].y && ic[i].z == NULL && ic[i].block_loc == ic[j].block_loc)
                        {
                            ic[j].op = '=';
                            ic[j].y = ic[i].x;
                            ic[j].z = NULL;
                        }
                    }
                    else
                    {
                        if (((ic[j].y == ic[i].y && ic[j].z == ic[i].z) || (ic[j].y == ic[i].z && ic[j].z == ic[i].y)) && ic[i].block_loc == ic[j].block_loc)
                        {
                            ic[j].op = '=';
                            ic[j].y = ic[i].x;
                            ic[j].z = NULL;
                        }
                    }
                }
            }
        }
    }
}
void function_inline()
{

    entry *current_fun = NULL;

    for (int i = 0; i < ic.size(); i++)
    {
        if (ic[i].inter_type == FUN_DEFINE || ic[i].inter_type == MAIN_DEFINE)
        {
            current_fun = ic[i].x;
            current_fun->block_size += 1;
        }
        if (current_fun != NULL)
        {
            if (ic[i].inter_type != FUN_DEFINE && ic[i].inter_type != PARA_DEFINE && ic[i].inter_type != ASSIGN_WITHOUT_ARRAY && ic[i].inter_type != ASSIGN_WITH_ARRAY && ic[i].inter_type != ASSIGN_AN_ARRAY && ic[i].inter_type != PRINTF && ic[i].inter_type != SCANF && ic[i].inter_type != RET && ic[i].inter_type != END_OF_FUN)
                current_fun->block_size += 1;
            /*if (ic[i].inter_type == SET)
                current_fun->block_size += 1;
            if (ic[i].inter_type == _BEQ || ic[i].inter_type == _BNE || ic[i].inter_type == _GEQ || ic[i].inter_type == _GRE || ic[i].inter_type == _LEQ || ic[i].inter_type == _LSS || ic[i].inter_type == JUMP)
                current_fun->block_size += 1;*/
            if (ic[i].inter_type == RET && ic[i + 1].inter_type != END_OF_FUN)
                current_fun->block_size += 1;
            if (ic[i].inter_type == FUN_DEFINE && ic[i + 1].inter_type == END_OF_FUN)
                current_fun->block_size += 1;
            if (ic[i].inter_type == FUN_DEFINE && ic[i + 2].inter_type == END_OF_FUN)
                current_fun->block_size += 1;
            /*if (ic[i].inter_type == FUN_CALL)
                current_fun->has_fun_call = true;
            if (ic[i].inter_type == VAR_DEFINE || ic[i].inter_type == CONST_DEFINE)
                current_fun->has_var_const_define = true;*/
        }
    }

    current_fun = NULL;
    vector<entry *> inline_fun;
    for (int i = 0; i < ic.size(); i++)
    {
        int j;
        if (ic[i].inter_type == FUN_DEFINE && ic[i].x->block_size == 1 && ic[i].x->has_fun_call == false && ic[i].x->has_var_const_define == false)
        {
            ic[i].x->can_be_inlined = true;
            ic[i].x->intermidiate_start = i;
            current_fun = ic[i].x;
            for (j = i + 1; j < ic.size(); j++)
            {
                if (ic[j].inter_type == END_OF_FUN)
                {
                    ic[i].x->intermidiate_finish = j;
                    inline_fun.push_back(ic[i].x);
                    break;
                }
            }
        }
    }
    for (int i = 0; i < inline_fun.size(); i++)
    {
        printf("%s\n", inline_fun[i]->name);
    }
    bool flag = false;
    while (true)
    {
        flag = false;

        for (int i = 0; i < ic.size(); i++)
        {

            if (ic[i].inter_type == START_CALL && is_in_vector(ic[i].x, inline_fun))
            {
                int j;
                for (j = i + 1; j < ic.size(); j++)
                {
                    if (ic[j].inter_type == START_CALL)
                    {
                        break;
                    }
                    if (strcmp(ic[i].x->type, VOID) == 0)
                    {
                        if (ic[j].inter_type == FUN_CALL)
                            break;
                    }
                    else
                    {
                        if (ic[j].inter_type == ASSIGN_RET)
                            break;
                    }
                }
                if (ic[j].inter_type == START_CALL)
                    continue;
                flag = true;
                vector<inter_code> tmp;
                vector<inter_code> tmp_fun;
                /*for (int i = 0; i < inline_fun.size(); i++)
                {
                    printf("%s\n", inline_fun[i]->name);
                    printf("%d %d\n", ic[inline_fun[i]->intermidiate_start].inter_type, ic[inline_fun[i]->intermidiate_finish].inter_type);
                }
                printf("\n");*/
                for (int k = ic[i].x->intermidiate_start + 1; k <= ic[i].x->intermidiate_finish - 1; k++)
                {
                    tmp_fun.push_back(ic[k]);
                }

                vector<entry *> tmp_var_to_change;

                for (int k = 0; k < tmp_fun.size(); k++)
                {
                    if (tmp_fun[k].x != NULL && tmp_fun[k].x->is_tmp && !is_in_vector(tmp_fun[k].x, tmp_var_to_change))
                    {
                        tmp_var_to_change.push_back(tmp_fun[k].x);
                    }
                    if (tmp_fun[k].y != NULL && tmp_fun[k].y->is_tmp && !is_in_vector(tmp_fun[k].y, tmp_var_to_change))
                    {
                        tmp_var_to_change.push_back(tmp_fun[k].y);
                    }
                    if (tmp_fun[k].z != NULL && tmp_fun[k].z->is_tmp && !is_in_vector(tmp_fun[k].z, tmp_var_to_change))
                    {
                        tmp_var_to_change.push_back(tmp_fun[k].z);
                    }
                }

                for (int t = 0; t < tmp_var_to_change.size(); t++)
                {
                    char new_tmp_name[NAME_LEN];
                    get_tmp_name(new_tmp_name);
                    sb.push_back(entry(VAR, tmp_var_to_change[t]->type, new_tmp_name, level = 2, 0, 0));
                    sb[sb.size() - 1].is_tmp = true;
                    for (int k = 0; k < tmp_fun.size(); k++)
                    {
                        if (tmp_fun[k].x != NULL && tmp_fun[k].x == tmp_var_to_change[t])
                        {
                            tmp_fun[k].x = &sb.back();
                        }
                        if (tmp_fun[k].y != NULL && tmp_fun[k].y == tmp_var_to_change[t])
                        {
                            tmp_fun[k].y = &sb.back();
                        }
                        if (tmp_fun[k].z != NULL && tmp_fun[k].z == tmp_var_to_change[t])
                        {
                            tmp_fun[k].z = &sb.back();
                        }
                    }
                }

                for (int k = i + 1; k <= j; k++)
                {
                    if (ic[k].inter_type != PARA_PUSH && ic[k].inter_type != FUN_CALL && ic[k].inter_type != ASSIGN_RET)
                    {

                        tmp.push_back(ic[k]);
                        //printf("!\n");
                    }
                    else if (ic[k].inter_type == PARA_PUSH)
                    {

                        for (int t = 0; t < tmp_fun.size(); t++)
                        {
                            if (tmp_fun[t].inter_type == PARA_DEFINE)
                            {
                                entry *para = tmp_fun[t].x;
                                char new_tmp_name[NAME_LEN];
                                get_tmp_name(new_tmp_name);
                                sb.push_back(entry(VAR, tmp_fun[t].x->type, new_tmp_name, level = 2, 0, 0));
                                sb[sb.size() - 1].is_tmp = true;

                                inter_code itcd = inter_code(ASSIGN_WITHOUT_ARRAY, &sb.back(), '=', ic[k].x);
                                tmp_fun[t] = itcd;
                                for (int n = t + 1; n < tmp_fun.size(); n++)
                                {
                                    if (tmp_fun[n].x != NULL && tmp_fun[n].x == para)
                                        tmp_fun[n].x = &sb.back();
                                    if (tmp_fun[n].y != NULL && tmp_fun[n].y == para)
                                        tmp_fun[n].y = &sb.back();
                                    if (tmp_fun[n].z != NULL && tmp_fun[n].z == para)
                                        tmp_fun[n].z = &sb.back();
                                }
                                tmp.push_back(tmp_fun[t]);
                                tmp_fun.erase(tmp_fun.begin() + t);
                                break;
                            }
                        }
                    }
                    else if (ic[k].inter_type == FUN_CALL)
                    {
                        for (int t = 0; t < tmp_fun.size(); t++)
                        {
                            tmp.push_back(tmp_fun[t]);
                        }
                    }
                    if (ic[k].inter_type == ASSIGN_RET && k == j)
                    {
                        entry *return_value = tmp[tmp.size() - 1].x;
                        tmp[tmp.size() - 1] = inter_code(ASSIGN_WITHOUT_ARRAY, ic[k].x, '=', return_value);
                    }
                    if (ic[k].inter_type == FUN_CALL && k == j && tmp[tmp.size() - 1].inter_type == RET)
                    {
                        tmp.pop_back();
                    }
                }

                for (int k = 0; k <= j - i; k++)
                {
                    ic.erase(ic.begin() + i);
                }
                for (int k = tmp.size() - 1; k >= 0; k--)
                {
                    ic.insert(ic.begin() + i, tmp[k]);
                }
                for (int i = 0; i < ic.size(); i++)
                {
                    if (ic[i].inter_type == FUN_DEFINE && is_in_vector(ic[i].x, inline_fun))
                    {
                        ic[i].x->intermidiate_start = i;
                        for (int j = i + 1; j < ic.size(); j++)
                        {
                            if (ic[j].inter_type == END_OF_FUN)
                            {
                                ic[i].x->intermidiate_finish = j;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (flag == false)
            break;
    }
}
void calculate_last_appear_time()
{
    for (int i = ic.size() - 1; i >= 0; i--)
    {
        if (ic[i].x != NULL && ic[i].x->last_appear_time == -1)
        {
            //printf("%s ",ic[i].x->name);
            ic[i].x->last_appear_time = i;
        }
        if (ic[i].y != NULL && ic[i].y->last_appear_time == -1)
        {
            //printf("%s ",ic[i].y->name);
            ic[i].y->last_appear_time = i;
        }
        if (ic[i].z != NULL && ic[i].z->last_appear_time == -1)
        {
            //printf("%s ",ic[i].z->name);
            ic[i].z->last_appear_time = i;
        }
        //printf("\n");
    }
    for (int i = 0; i < ic.size(); i++)
    {
        if (ic[i].inter_type == SET)
        {
            for (int j = i + 1; j < ic.size(); j++)
            {
                if (ic[j].inter_type == _BEQ || ic[j].inter_type == _BNE || ic[j].inter_type == _LSS || ic[j].inter_type == _LEQ || ic[j].inter_type == _GRE || ic[j].inter_type == _GEQ)
                {
                    if (ic[j].z == ic[i].x)
                    {
                        for (int k = i; k <= j; k++)
                        {
                            if (ic[k].x != NULL && !ic[k].x->is_tmp && strcmp(ic[k].x->category, VAR) == 0 && ic[k].x->last_appear_time < j)
                            {
                                //printf("%s %d",ic[k].x->name,j+1);
                                ic[k].x->last_appear_time = j;
                            }
                            if (ic[k].y != NULL && !ic[k].y->is_tmp && strcmp(ic[k].y->category, VAR) == 0 && ic[k].y->last_appear_time < j)
                            {
                                //printf("%s ",ic[k].y->name);
                                ic[k].y->last_appear_time = j;
                            }
                            if (ic[k].z != NULL && !ic[k].z->is_tmp && strcmp(ic[k].z->category, VAR) == 0 && ic[k].z->last_appear_time < j)
                            {
                                //printf("%s ",ic[k].z->name);
                                ic[k].z->last_appear_time = j;
                            }
                        }
                    }
                }
            }
        }
    }
}
bool is_in_vector(entry *tmp, vector<entry *> ve)
{
    for (int i = 0; i < ve.size(); i++)
    {
        if (tmp == ve[i])
            return true;
    }
    return false;
}
