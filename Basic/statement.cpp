/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

//todo

void InputStatement::execute(EvalState &state, Program &program) {
    int value;
    std::string input;

    while (true) {
        std::cout << " ? ";
        getline(std::cin, input);

        // 尝试转换输入为整数类型
        std::istringstream iss(input);
        if (iss >> value) {
            if (iss.eof()) {  // 确保输入只包含一个整数，没有其他多余的内容
                state.setValue(variable, value);
                break;
            }
        }
        std::cout << "INVALID NUMBER" << std::endl; // 如果输入不合法，提示用户重新输入
        iss.clear(); // 重置输入流，清除错误状态
    }
}

void GotoStatement::execute(EvalState &state, Program &program) {
    if (program.getSourceLine(targetLine).empty()) {
        error("LINE NUMBER ERROR");
    }
}

void IfStatement::execute(EvalState &state, Program &program) {
    if (program.getSourceLine(targetLine).empty()) {
        error("LINE NUMBER ERROR");
    }
}

bool IfStatement::isConditionTrue(EvalState &state) const {
    const int leftValue = lhs->eval(state);
    const int rightValue = rhs->eval(state);
    if (op == "=") {
        return leftValue == rightValue;
    } else if (op == "<") {
        return leftValue < rightValue;
    } else if (op == ">") {
        return leftValue > rightValue;
    } else {
        error("SYNTAX ERROR");
        return false;
    }

}
