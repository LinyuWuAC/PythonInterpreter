//
// Created by Lynn on 2021/11/15.
//

#include "Def.h"

#include <utility>

void Def::registerFunc(std::string &func_name, std::vector<std::string> func_para, std::vector<Var> func_init, antlrcpp::Any func_suite) {
    define_table[func_name] = Func(std::move(func_para), std::move(func_init), func_suite);
}

antlrcpp::Any Def::queryFuncSuite(std::string &func_name) {
    return define_table[func_name].suite;
}

bool Def::queryFuncName(std::string &func_name) {
    return define_table.count(func_name);
}

std::vector<std::string> Def::queryFuncPara(std::string &func_name) {
    return define_table[func_name].para;
}

std::vector<Var> Def::queryFuncInit(std::string &func_name) {
    return define_table[func_name].init;
}




