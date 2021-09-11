#include <iostream>
#include "Tokenizer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>

#include <map>

using namespace std;

map<string, int> symbol_table_map;

static const char *errstrings[] = {
    "",
    " Error: Absolute address exceeds machine size; zero used",
    " Error: Relative address exceeds module size; zero used",
    " Error: External address exceeds length of uselist; treated as immediate",
    " Error: %d is not defined; zero used",
    " Error: Illegal opcode; treated as 9999",
    " Error: Illegal immediate value; treated as 9999",
};

void Pass1(Tokenizer *t)
{
    int module_number = 1;
    int module_offset = 0;
    int num_instr = 0;
    vector<string> multiple_sym_def;
    vector<string> sym_table_order;

    while (true)
    {
        map<string, int> this_module_symbol_table_map;
        int this_module_size = 0;
        int defcount = t->readInt();
        if (t->getEof() && defcount == -1)
        {
            break;
        }
        if (defcount > 16)
        {
            t->__parseerror(4);
            exit(1);
        }
        for (int i = 0; i < defcount; i++)
        {
            string sym = t->readSym();
            int val = t->readInt();
            if (t->getEof() && val == -1)
            {
                t->__parseerror(0);
                exit(1);
            }
            int abs_val = val;
            val = val + module_offset;
            if (symbol_table_map.find(sym) != symbol_table_map.end())
            {
                multiple_sym_def.push_back(sym);
            }
            else
            {
                this_module_symbol_table_map.insert(pair<string, int>(sym, abs_val));
            }
            symbol_table_map.insert(pair<string, int>(sym, val));
            if (std::find(sym_table_order.begin(), sym_table_order.end(), sym) == sym_table_order.end())
            {
                sym_table_order.push_back(sym);
            }
        }
        int usecount = t->readInt();
        if (t->getEof() && usecount == -1)
        {
            t->__parseerror(0);
            exit(1);
        }
        if (usecount > 16)
        {
            t->__parseerror(5);
            exit(1);
        }
        for (int i = 0; i < usecount; i++)
        {
            string sym = t->readSym();
        }

        int instcount = t->readInt();
        if (t->getEof() && instcount == -1)
        {
            t->__parseerror(0);
            exit(1);
        }
        num_instr = num_instr + instcount;
        if (num_instr > 512)
        {
            t->__parseerror(6);
            exit(1);
        }
        for (int i = 0; i < instcount; i++)
        {
            t->ReadIEAR();
            int operand = t->readInt();
            if (t->getEof() && operand == -1)
            {
                t->__parseerror(0);
                exit(1);
            }
            module_offset++;
            this_module_size++;
        }

        for (vector<string>::iterator t_2 = sym_table_order.begin(); t_2 != sym_table_order.end(); t_2++)
        {
            map<string, int>::iterator sym_tab_it_2 = this_module_symbol_table_map.find(*t_2);
            if (sym_tab_it_2 != this_module_symbol_table_map.end())
            {
                if (sym_tab_it_2->second > instcount - 1)
                {
                    printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", module_number, sym_tab_it_2->first.c_str(), sym_tab_it_2->second, instcount - 1);
                    symbol_table_map.erase(sym_tab_it_2->first);
                    symbol_table_map.insert(pair<string, int>(sym_tab_it_2->first, (module_offset - this_module_size)));
                }
            }
        }
        module_number++;
    }
    cout << "Symbol Table\n";

    for (vector<string>::iterator t = sym_table_order.begin(); t != sym_table_order.end(); t++)
    {
        
        map<string, int>::iterator sym_tab_it = symbol_table_map.find(*t);
        if (std::find(multiple_sym_def.begin(), multiple_sym_def.end(), sym_tab_it->first) != multiple_sym_def.end())
        {
            cout << sym_tab_it->first << "=" << sym_tab_it->second << " Error: This variable is multiple times defined; first value used" << endl;
        }
        else
        {
            cout << sym_tab_it->first << "=" << sym_tab_it->second << endl;
        }
    }
}

void Pass2(Tokenizer *t)
{
    int module_number = 1;
    int module_offset = 0;
    int size_count = 0;
    int memory_address = 0;
    int error_code = 0;
    vector<string> sym_table_order;

    map<string, int> symbol_unused;
    map<string, int>::iterator it_su;
    for (it_su = symbol_table_map.begin(); it_su != symbol_table_map.end(); it_su++)
    {
            symbol_unused.insert(pair<string, int>(it_su->first, -1));
    }   
    while (true)
    {
        size_count = 0;

        int defcount = t->readInt();
        if (t->getEof() && defcount == -1)
        {
            break;
        }
        for (int i = 0; i < defcount; i++)
        {
            string sym = t->readSym();
            t->readInt();
            if (symbol_unused.find(sym) != symbol_unused.end())
            {
                if(symbol_unused[sym] == -1){
                symbol_unused[sym] = module_number;
                }
            
            }
            if (std::find(sym_table_order.begin(), sym_table_order.end(), sym) == sym_table_order.end())
            {
                sym_table_order.push_back(sym);
            }
        }

        int usecount = t->readInt();
        string sym[usecount];
        set<int> un_sym;
        for (int i = 0; i < usecount; i++)
        {
            sym[i] = t->readSym();
            un_sym.insert(i);
        }

        int instcount = t->readInt();
        for (int i = 0; i < instcount; i++)
        {
            char addressmode = t->ReadIEAR();
            int operand = t->readInt();
            int opcode = operand / 1000;
            int operande = operand % 1000;
            if (addressmode == 'I')
            {
                if (operand >= 10000)
                {
                    operand = 9999;
                    error_code = 6;
                }
            }
            else if (opcode >= 10)
            {
                operand = 9999;
                error_code = 5;
            }
            else if (addressmode == 'E')
            {
                if (operande < usecount)
                {
                    if (symbol_table_map.find(sym[operande]) != symbol_table_map.end())
                    {
                        int symbol_val = symbol_table_map.find(sym[operande])->second;
                        operand = opcode * 1000 + symbol_val;
                        symbol_unused.erase(sym[operande]);
                        un_sym.erase(operande);
                    }
                    else
                    {
                        un_sym.erase(operande);
                        operand = opcode * 1000 + 0;
                        error_code = 4;
                    }
                }
                else
                {
                    error_code = 3;
                }
            }
            else if (addressmode == 'R')
            {

                if (operande >= instcount)
                {
                    operand = opcode * 1000 + 0 + module_offset;
                    error_code = 2;
                }
                else
                {
                    operand = operand + module_offset;
                }
            }

            else
            {

                if (operande >= 512)
                {
                    operand = opcode * 1000 + 0;
                    error_code = 1;
                }
            }
            size_count++;
            if (error_code == 4)
            {
                printf("%03d: %04d Error: %s is not defined; zero used\n", memory_address, operand, sym[operande].c_str());
            }
            else
            {
                printf("%03d: %04d%s\n", memory_address, operand, errstrings[error_code]);
            }
            error_code = 0;
            memory_address++;
        }
        set<int>::iterator itr;
        for (itr = un_sym.begin(); itr != un_sym.end(); itr++)
        {
            cout << "Warning: Module " << module_number << ": " << sym[*itr] << " appeared in the uselist but was not actually used\n";
        }
        module_number++;
        module_offset = module_offset + size_count;
    }
    printf("\n");
    for (vector<string>::iterator t = sym_table_order.begin(); t != sym_table_order.end(); t++)
    {
        map<string, int>::iterator sym_un_tab_it = symbol_unused.find(*t);
        if (sym_un_tab_it != symbol_unused.end())
        {
            printf("Warning: Module %d: %s was defined but never used\n", sym_un_tab_it->second, sym_un_tab_it->first.c_str());
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    Tokenizer *t1 = new Tokenizer(argv[1]);
    Pass1(t1);

    Tokenizer *t2 = new Tokenizer(argv[1]);
    cout << "\nMemory Map\n";
    Pass2(t2);
}