#include <iostream>
#include "antlr4-runtime.h"
#include "Python3Lexer.h"
#include "Python3Parser.h"
#include "Evalvisitor.h"

using namespace antlr4;
//todo: regenerating files in directory named "generated" is dangerous.
//       if you really need to regenerate,please ask TA for help.
int main(int argc, const char* argv[]){
//    clock_t start = clock();
//    freopen("test.in", "r", stdin);
//    freopen("test.out", "w", stdout);
    //todo:please don't modify the code below the construction of ifs if you want to use visitor mode
    ANTLRInputStream input(std::cin);
    Python3Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();
    Python3Parser parser(&tokens);
    tree::ParseTree* tree=parser.file_input();
    EvalVisitor visitor;
    try {
        visitor.visit(tree);
    } catch(Exception& e) {
        std::cout << e.what() << '\n';
        return 0;
    }
//    clock_t stop = clock();
//    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
//    printf("\nTime elapsed: %.5f\n", elapsed);
}
