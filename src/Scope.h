//
// Created by Lynn on 2021/11/12.
//

#ifndef PYTHON_INTERPRETER_SCOPE_H
#define PYTHON_INTERPRETER_SCOPE_H

#include <iostream>
#include <map>
#include <cstring>
#include <any>
#include <stack>
#include <utility>
#include <iomanip>
#include "int2048.h"

class Var {
public:
    int type; //Return: -4; Break: -3; Continue: -2; Empty: -1; bool: 0; int2048: 1; float: 2; str: 3; None: 4;
    bool bool_data;
    BigInt::int2048 int_data;
    double float_data;
    std::string str_data;

    Var(): type(), int_data(), bool_data(), float_data(), str_data() {
        int_data = BigInt::int2048();
        bool_data = 0;
        float_data = 0;
        str_data = "";
        type = -1;
    }

    void clear() {
        if (type == 0)
            bool_data = 0;
        if (type == 1)
            int_data = BigInt::int2048();
        if (type == 2)
            float_data = 0;
        if (type == 3)
            str_data = "";
        type = -1;
    }

    Var &setBool(const bool &a) {
        clear();
        type = 0;
        bool_data = a;
        return *this;
    }

    Var &setInt(const BigInt::int2048 &a) {
        clear();
        type = 1;
        int_data = a;
        return *this;
    }

    Var &setFloat(const double &a) {
        clear();
        type = 2;
        float_data = a;
        return *this;
    }

    Var &setStr(const std::string &a) {
        clear();
        type = 3;
        str_data = a;
        return *this;
    }

    Var &setNone() {
        clear();
        type = 4;
        return *this;
    }

    Var &setEmpty() {
        clear();
        type = -1;
        return *this;
    }

    Var &setContinue() {
        clear();
        type = -2;
        return *this;
    }

    Var &setBreak() {
        clear();
        type = -3;
        return *this;
    }

    Var &setReturn() {
        clear();
        type = -4;
        return *this;
    }

    bool isEmpty() {
        return type == -1;
    }

    bool isContinue() {
        return type == -2;
    }

    bool isBreak() {
        return type == -3;
    }

    bool isReturn() {
        return type == -4;
    }

    bool toBool() const {
        if (type == 0)
            return bool_data;
        if (type == 1)
            return int_data != BigInt::int2048(0);
        if (type == 2)
            return float_data != 0;
        if (type == 3)
            return str_data != "";
        return false;
    }

    BigInt::int2048 toInt() const {
        if (type == 0)
            return BigInt::int2048(bool_data);
        if (type == 1)
            return int_data;
        if (type == 2) {
            std::stringstream temp;
            temp << std::setprecision(8);
            return BigInt::int2048(temp.str());
        }
        if (type == 3)
            return BigInt::int2048(str_data);
        return BigInt::int2048();
    }

    double toFloat() const {
        if (type == 0)
            return double(bool_data);
        if (type == 1)
            return int_data.toDouble();
        if (type == 2)
            return float_data;
        if (type == 3) {
            double res = 0;
            for (int i = 0; i < str_data.size(); ++i)
                res = res * 10 + str_data[i] - '0';
            return res;
        }
        return 0;
    }

    std::string toStr() const {
        if (type == 0)
            return std::to_string(bool_data);
        if (type == 1)
            return int_data.toString();
        if (type == 2)
            return std::to_string(float_data);
        if (type == 3)
            return str_data;
        return "";
    }

    friend Var operator + (const Var &a, const Var &b) {
        if (a.type == 3 && b.type == 3)
            return Var().setStr(a.str_data + b.str_data);
        if (a.type == 3 || b.type == 3)
            return Var().setNone();
        int max_type = std::max(a.type, b.type);
        if (max_type == 2)
            return Var().setFloat(a.toFloat() + b.toFloat());
        if (max_type == 1)
            return Var().setInt(a.toInt() + b.toInt());
        return Var().setBool(a.toBool() + b.toBool());
    }

    friend Var operator - (const Var &a, const Var &b) {
        if (a.type == 3 || b.type == 3)
            return Var().setNone();
        int max_type = std::max(a.type, b.type);
        if (max_type == 2)
            return Var().setFloat(a.toFloat() - b.toFloat());
        if (max_type == 1)
            return Var().setInt(a.toInt() - b.toInt());
        return Var().setBool(a.toBool() - b.toBool());
    }

    friend Var operator * (const Var &a, const Var &b) {
        if ((a.type == 1 && b.type == 3) || (a.type == 3 && b.type == 1)) {
            int times = a.type == 1 ? a.int_data.toInt() : b.int_data.toInt();
            std::string res = "";
            std::string delta = a.type == 3 ? a.str_data : b.str_data;
            for (int i = 0; i < times; ++i)
                res = res + delta;
            return Var().setStr(res);
        }
        if (a.type == 3 || b.type == 3)
            return Var().setNone();
        int max_type = std::max(a.type, b.type);
        if (max_type == 2)
            return Var().setFloat(a.toFloat() * b.toFloat());
        if (max_type == 1)
            return Var().setInt(a.toInt() * b.toInt());
        return Var().setBool(a.toBool() * b.toBool());
    }

    friend Var operator / (const Var &a, const Var &b) {
        if (a.type == 3 || b.type == 3 || a.type == 0 || b.type == 0)
            return Var().setNone();
        int max_type = std::max(a.type, b.type);
        if (max_type == 2)
            return Var().setFloat(a.toFloat() / b.toFloat());
        return Var().setInt(a.toInt() / b.toInt());
    }

    friend Var operator % (const Var &a, const Var &b) {
        if (a.type == 1 && b.type == 1)
            return Var().setInt(a.int_data % b.int_data);
        return Var().setNone();
    }

    friend bool operator < (const Var &a, const Var &b) {
        if (a.type == 3 && b.type == 3)
            return a.str_data < b.str_data;
        if (a.type == 3 || b.type == 3)
            return false; // throw
        int max_type = std::max(a.type, b.type);
        if (max_type == 2)
            return a.toFloat() < b.toFloat();
        if (max_type == 1)
            return a.toInt() < b.toInt();
        return a.bool_data < b.bool_data;
    }

    friend bool operator <= (const Var &a, const Var &b) {
        if (a.type == 3 && b.type == 3)
            return a.str_data <= b.str_data;
        if (a.type == 3 || b.type == 3)
            return false; // throw
        int max_type = std::max(a.type, b.type);
        if (max_type == 2)
            return a.toFloat() <= b.toFloat();
        if (max_type == 1)
            return a.toInt() <= b.toInt();
        return a.bool_data <= b.bool_data;
    }

    friend bool operator == (const Var &a, const Var &b) {
        if (a.type == 3 && b.type == 3)
            return a.str_data == b.str_data;
        if (a.type == 3 || b.type == 3)
            return false; // throw
        int max_type = std::max(a.type, b.type);
        if (max_type == 2)
            return a.toFloat() == b.toFloat();
        if (max_type == 1)
            return a.toInt() == b.toInt();
        return a.bool_data == b.bool_data;
    }

    friend bool operator > (const Var &a, const Var &b) {
        return !(a <= b);
    }

    friend bool operator >= (const Var &a, const Var &b) {
        return !(a < b);
    }

    friend bool operator != (const Var &a, const Var &b) {
        return !(a == b);
    }

    void print() const {
        if (type == 0)
            std::cout << (bool_data ? "True" : "False");
        else if (type == 1)
            int_data.print();
        else if (type == 2)
            std::cout << float_data;
        else if (type == 3)
            std::cout << str_data;
        else
            std::cout << "Get None! ";
    }
};

class Scope {
private:
    std::map<std::string, Var> global_var_table;
    std::deque<std::map<std::string, Var>> local_var_stack;
public:
    Scope(): global_var_table(), local_var_stack() {}

    void intoFunc() {
        local_var_stack.emplace_back();
    }

    void registerVar(const std::string &var_name, const Var &var) {
        if (global_var_table.count(var_name) || local_var_stack.empty())
            global_var_table[var_name] = var;
        else
            local_var_stack.back()[var_name] = var;
    }

    std::pair<bool, Var> queryVar(const std::string &var_name) {
        if (!local_var_stack.empty() && local_var_stack.back().count(var_name))
            return std::make_pair(true, local_var_stack.back()[var_name]);
        if (global_var_table.count(var_name))
            return std::make_pair(true, global_var_table[var_name]);
        return std::make_pair(false, Var().setEmpty());
    }

    void outOfFunc() {
        if (!local_var_stack.empty())
            local_var_stack.pop_back();
    }
};

#endif //PYTHON_INTERPRETER_SCOPE_H
