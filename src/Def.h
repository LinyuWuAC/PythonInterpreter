//
// Created by Lynn on 2021/11/15.
//

#ifndef PYTHON_INTERPRETER_DEF_H
#define PYTHON_INTERPRETER_DEF_H

#include <map>
#include <vector>
#include <string>
#include "Scope.h"
#include "Python3BaseVisitor.h"

class Func {
public:
    std::vector<std::string> para;
    std::vector<Var> init;
    antlrcpp::Any suite;
    Func() {}
    Func(std::vector<std::string> input_para, std::vector<Var> input_init, antlrcpp::Any input_suite) {
        para = input_para;
        init = input_init;
        suite = input_suite;
    }
};

class Def {
private:
    std::map<std::string, Func> define_table;
public:
    void registerFunc(std::string &func_name, std::vector<std::string> func_para, std::vector<Var> func_init, antlrcpp::Any func_suite); // throw

    bool queryFuncName(std::string &func_name);

    antlrcpp::Any queryFuncSuite(std::string &func_name); //throw

    std::vector<std::string> queryFuncPara(std::string &func_name);

    std::vector<Var> queryFuncInit(std::string &func_name);
};

#endif //PYTHON_INTERPRETER_DEF_H
