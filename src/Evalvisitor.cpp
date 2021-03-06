#include "Python3BaseVisitor.h"
#include "Scope.h"
#include "Exception.h"
#include "utils.h"
#include "Evalvisitor.h"
#include "int2048.h"
#include <map>
#include "Def.h"

Scope scope;
Def def;
std::vector<antlrcpp::Any> return_tests;

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
    std::string func_name = ctx->NAME()->getText();
    std::vector<std::string> func_para;
    std::vector<Var> func_init;
    auto typedargslist = ctx->parameters()->typedargslist();
    if (!typedargslist) {
        def.registerFunc(func_name, func_para, func_init, ctx->suite()); // throw if function names are the same
        return Var().setEmpty();
    }
    auto var_name_array = typedargslist->tfpdef();
    auto var_data_array = typedargslist->test();
    for (auto x : var_name_array)
        func_para.push_back(x->NAME()->getText());
    for (int i = 0; i < var_name_array.size() - var_data_array.size(); ++i)
        func_init.push_back(Var().setEmpty());
    for (auto x : var_data_array)
        func_init.push_back(visitTest(x));
    def.registerFunc(func_name, func_para, func_init, ctx->suite()); // throw if function names are the same
    return Var().setEmpty();
}

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
    auto simple_stmt = ctx->simple_stmt();
    if (simple_stmt)
        return visitSimple_stmt(simple_stmt);
    return visitCompound_stmt(ctx->compound_stmt());
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
    return visitSmall_stmt(ctx->small_stmt());
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
    auto expr_stmt = ctx->expr_stmt();
    if (expr_stmt) {
        visitExpr_stmt(expr_stmt);
        return Var().setEmpty();
    }
    return visitFlow_stmt(ctx->flow_stmt());
}

antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
    auto testlist_array = ctx->testlist();
    if (testlist_array.size() == 1) { // function return ;
        return visitTestlist(testlist_array[0]);
    }
    if (!ctx->augassign()) {
        auto var_data_array = testlist_array[testlist_array.size() - 1]->test();
        std::vector<Var> treated_var_data_array;
        for (int i = 0; i < var_data_array.size(); ++i)
            treated_var_data_array.push_back(visitTest(var_data_array[i]).as<Var>());
        if (var_data_array.size() < testlist_array[0]->test().size()) {
            for (int i = testlist_array.size() - 2; i >= 0; --i) {
                auto var_name_array = testlist_array[i]->test();
                for (int j = 0; j < var_name_array.size(); ++j) {
                    auto var_name = var_name_array[j]->getText();
                    scope.registerVar(var_name, return_tests[j].as<Var>());
                }
            }
            return_tests.clear();
            return Var().setEmpty();
        }
        for (int i = testlist_array.size() - 2; i >= 0; --i) {
            auto var_name_array = testlist_array[i]->test();
            for (int j = 0; j < var_name_array.size(); ++j) {
                auto var_name = var_name_array[j]->getText();
                scope.registerVar(var_name, treated_var_data_array[j]);
            }
        }
        return Var().setEmpty();
    }
    auto signal = ctx->augassign();
    auto var_name = testlist_array[0]->test()[0]->getText();
    auto var_delta = visitTest(testlist_array[1]->test()[0]).as<Var>();
    std::string temp = signal->getText();
    if (temp == "+=") {
        auto res = scope.queryVar(var_name); //throw * 2
        scope.registerVar(var_name, res.second + var_delta);
    }
    else if (temp == "-=") {
        auto res = scope.queryVar(var_name); //throw * 2
        scope.registerVar(var_name, res.second - var_delta);
    }
    else if (temp == "*=") {
        auto res = scope.queryVar(var_name); // throw * 2
        scope.registerVar(var_name, res.second * var_delta);
    }
    else if (temp == "/=") {
        auto res = scope.queryVar(var_name); // throw * 3
        if (res.second.type <= 1)
            res.second.setFloat(res.second.toFloat());
        scope.registerVar(var_name, res.second / var_delta);
    }
    else if (temp == "//=") {
        auto res = scope.queryVar(var_name); // throw * 3
        scope.registerVar(var_name, res.second / var_delta);
    }
    else if (temp == "%=") {
        auto res = scope.queryVar(var_name); // throw * 3
        scope.registerVar(var_name, res.second % var_delta);
    }
    return Var().setEmpty();
}

antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
    if (ctx->break_stmt())
        return Var().setBreak();
    if (ctx->continue_stmt())
        return Var().setContinue();
    auto testlist = ctx->return_stmt()->testlist();
    if (!testlist)
        return Var().setReturn();
    auto test = testlist->test();
    if (test.size() == 1)
        return visitTest(test[0]);
    for (auto x : test)
        return_tests.push_back(visitTest(x));
    return Var().setReturn();
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
    return visitChildren(ctx);
}

// function: visit th if statement
// return value: a Var showing messages like some signals including continue, break, return, return with value and empty
antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
    auto test_array = ctx->test();
    auto suite_array = ctx->suite();
    bool last = false;
    for (int i = 0; i < test_array.size(); ++i)
        if (visitTest(test_array[i]).as<Var>().toBool()) {
            last = true;
            auto flag = visitSuite(suite_array[i]).as<Var>();
            if (flag.type != -1)
                return flag;
            break;
        }
    if (suite_array.size() > test_array.size() && !last) {
        auto flag = visitSuite(suite_array.back()).as<Var>();
        if (flag.type != -1)
            return flag;
    }
    return Var().setEmpty();
}

// function: visit the while statement
// return value: a Var showing messages like some signals including return, return with value and empty
antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
    auto test = ctx->test();
    auto suite = ctx->suite();
    while (visitTest(test).as<Var>().toBool()) {
        auto flag = visitSuite(suite).as<Var>();
        if (flag.isContinue())
            continue;
        else if (flag.isBreak())
            break;
        else if (flag.isReturn() || !flag.isEmpty())
            return flag;
    }
    return Var().setEmpty();
}

antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
    auto simple_stmt = ctx->simple_stmt();
    if (simple_stmt) {
        auto flag = visitSimple_stmt(simple_stmt).as<Var>();
        return flag;
    }
    else {
        auto stmt = ctx->stmt();
        for (auto x : stmt) {
            auto flag = visitStmt(x).as<Var>();
            if (flag.type != -1)
                return flag;
        }
    }
    return Var().setEmpty();
}

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
    auto and_array = ctx->and_test();
    if (and_array.size() == 1)
        return visitAnd_test(and_array[0]);
    bool state = false;
    for (auto x : and_array) {
        state |= visitAnd_test(x).as<Var>().toBool();
        if (state)
            return Var().setBool(true);
    }
    return Var().setBool(false);
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
    auto not_array = ctx->not_test();
    if (not_array.size() == 1)
        return visitNot_test(not_array[0]);
    bool state = true;
    for (auto x : not_array) {
        state &= visitNot_test(x).as<Var>().toBool();
        if (!state)
            return Var().setBool(false);
    }
    return Var().setBool(true);
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
    if (!ctx->NOT())
        return visitComparison(ctx->comparison());
    return Var().setBool(!(visitNot_test(ctx->not_test()).as<Var>().toBool()));
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
    auto arith_array = ctx->arith_expr();
    auto op_array = ctx->comp_op();
    if (arith_array.size() == 1)
        return visitArith_expr(arith_array[0]);
    bool state = true;
    std::vector<Var> var_array;
    var_array.push_back(visitArith_expr(arith_array[0]).as<Var>());
    for (int i = 0; i < op_array.size(); ++i) {
        var_array.push_back(visitArith_expr(arith_array[i + 1]).as<Var>());
        std::string temp = op_array[i]->getText();
        if (temp == "<")
            state &= (var_array[i] < var_array[i + 1]);
        else if (temp == ">")
            state &= (var_array[i] > var_array[i + 1]);
        else if (temp == "==")
            state &= (var_array[i] == var_array[i + 1]);
        else if (temp == ">=")
            state &= (var_array[i] >= var_array[i + 1]);
        else if (temp == "<=")
            state &= (var_array[i] <= var_array[i + 1]);
        else if (temp == "!=")
            state &= (var_array[i] != var_array[i + 1]);
        if (!state)
            return Var().setBool(false);
    }
    return Var().setBool(true);
}

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
    auto term_array = ctx->term();
    if (term_array.size() == 1)
        return visitTerm(term_array[0]);
    auto op_array = ctx->addorsub_op();
    Var var_data = visitTerm(term_array[0]);
    for (int i = 0; i < op_array.size(); ++i) {
        std::string temp = op_array[i]->getText();
        if (temp == "+")
            var_data += visitTerm(term_array[i + 1]);
        else if (temp == "-")
            var_data -= visitTerm(term_array[i + 1]);
    }
    return var_data;
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
    auto factor_array = ctx->factor();
    if (factor_array.size() == 1)
        return visitFactor(factor_array[0]);
    auto op_array = ctx->muldivmod_op();
    Var res = visitFactor(factor_array[0]);
    for (int i = 0; i < op_array.size(); ++i) {
        std::string temp = op_array[i]->getText();
        if (temp == "*") {
            res *= visitFactor(factor_array[i + 1]);
        }
        else if (temp == "//" ) {
            res /= visitFactor(factor_array[i + 1]);
        }
        else if (temp == "/") {
            if (res.type != 2)
                res.setFloat(res.toFloat());
            res /= visitFactor(factor_array[i + 1]);
        }
        else if (temp == "%")
            res %= visitFactor(factor_array[i + 1]).as<Var>();
    }
    return res;
}

antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
    if (!ctx->factor())
        return visitAtom_expr(ctx->atom_expr());
    Var res;
    res.setInt(BigInt::int2048(ctx->MINUS() ? -1 : 1));
    auto factor = ctx->factor();
    auto atom_expr = ctx->atom_expr();
    if (factor)
        res *= visitFactor(factor).as<Var>();
    if (atom_expr)
        res *= visitAtom_expr(atom_expr).as<Var>();
    return res;
}

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
    if (!ctx->trailer())
        return visitAtom(ctx->atom());
    auto func_name = ctx->atom()->getText();
    if (def.queryFuncName(func_name)) {
        auto para_array = def.queryFuncPara(func_name);
        auto init_array = def.queryFuncInit(func_name);
        auto args_array = visitTrailer(ctx->trailer()).as<std::vector<std::pair<std::string, Var>>>(); // throw
        scope.intoFunc();
        for (int i = 0; i < para_array.size(); ++i)
            if (init_array[i].type != -1)
                scope.registerVarInNew(para_array[i], init_array[i]);
        if (!args_array.empty()) {
            for (int i = 0; i < args_array.size(); ++i)
                if (args_array[i].first.empty())
                    scope.registerVarInNew(para_array[i], args_array[i].second);
                else
                    scope.registerVarInNew(args_array[i].first, args_array[i].second);
        }
        auto res = visitSuite(def.queryFuncSuite(func_name)).as<Var>();
        scope.outOfFunc();
        if (res.type >= 0)
            return res;
        return Var().setEmpty();
    }
    auto args_array = visitTrailer(ctx->trailer()).as<std::vector<std::pair<std::string, Var>>>();
    if (func_name == "print") {
        for (auto x : args_array) {
            x.second.print();
            std::cout << " ";
        }
        std::cout << std::endl;
        return Var().setEmpty();
    }
    if (func_name == "bool")
        return Var().setBool(args_array[0].second.toBool()); // more than two args throw
    if (func_name == "int")
        return Var().setInt(args_array[0].second.toInt());
    if (func_name == "float")
        return Var().setFloat(args_array[0].second.toFloat());
    if (func_name == "str")
        return Var().setStr(args_array[0].second.toStr());
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
    auto arglist = ctx->arglist();
    if (arglist)
        return visitArglist(arglist).as<std::vector<std::pair<std::string, Var>>>();
    return std::vector<std::pair<std::string, Var>>();
}

/*
 *  function:
 *      deal with NAME, NUMBER(int and float), NONE(), TRUE(bool),
 */

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
    if (ctx->NAME()) {
        auto res = scope.queryVar(ctx->NAME()->getText());
        if (res.first)
            return res.second;
        throw Exception(ctx->NAME()->getText(), UNDEFINED);
    }
    else if (ctx->NUMBER()) {
        std::string temp = ctx->NUMBER()->getText();
        if (temp.find('.') != temp.npos)
            return Var().setFloat(std::stod(temp));
        return Var().setInt(BigInt::int2048(temp));
    }
    else if (ctx->NONE())
        return Var().setNone();
    else if (ctx->TRUE())
        return Var().setBool(1);
    else if (ctx->FALSE())
        return Var().setBool(0);
    else if (ctx->test())
        return visitTest(ctx->test());
    auto strings = ctx->STRING();
    std::string res = "";
    for (auto x: strings) {
        std::string temp = x->getText();
        res += temp.substr(1, temp.size() - 2);
    }
    return Var().setStr(res);
}

/*
 *  function:
 *      visit every test
 *  return value:
 *      Var of bool
 *      Var of anything
 */

antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
    return visitChildren(ctx);
}

/*
 *  function:
 *      combine argument into a vector
 *  return value:
 *      vector: pair: name and value
 */

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
//    std::cout << "Get visitArglist!" << std::endl;
    auto arg_array = ctx->argument();
    std::vector<std::pair<std::string, Var>> treated_arg_array;
    for (auto x : arg_array) {
        auto temp = visitArgument(x).as<std::pair<std::string, Var>>();
        if (!return_tests.empty()) {
            for (auto x : return_tests)
                treated_arg_array.push_back(std::make_pair("", x.as<Var>()));
            return_tests.clear();
        }
        else
            treated_arg_array.push_back(temp);
    }
    return treated_arg_array;
}

/*
 *  function:
 *      deal with the def argument
 *  return value:
 *      pair: name and value
 *      pair: "" and value
 */

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
    auto test_array = ctx->test();
    if (test_array.size() == 1) {
        return std::make_pair(std::string(),visitTest(test_array[0]).as<Var>());
    }
    std::string var_name = test_array[0]->getText();
    auto var_data = visitTest(test_array[1]).as<Var>();
//    scope.registerVar(var_name, var_data);
    return std::make_pair(var_name, var_data);
}
