/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);
void runProgram(Program &program, EvalState &state);
void listProgram(Program &program);
/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (scanner.hasMoreTokens()) {
        std::string token = scanner.nextToken();

        if (isdigit(token[0])) { // 行号开头
            int lineNumber = stringToInteger(token);
            if (!scanner.hasMoreTokens()) { // 如果行号后面没有更多内容，表示是删除该行
                program.removeSourceLine(lineNumber);
            } else { // 有内容
                std::string stmtToken = scanner.nextToken(); // 辨别类型，以便根据不同类型创建 Statement 对象
                Statement *stmt = nullptr;

                if (stmtToken == "LET") {
                    std::string var = scanner.nextToken();
                    if (scanner.nextToken() != "=") {
                        error("SYNTAX ERROR");
                    }
                    Expression *exp = parseExp(scanner);
                    stmt = new LetStatement(var, exp);
                } else if (stmtToken == "PRINT") {
                    Expression *exp = parseExp(scanner);
                    stmt = new PrintStatement(exp);
                } else if (stmtToken == "INPUT") {
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    }
                    const std::string var = scanner.nextToken();
                    stmt = new InputStatement(var);
                } else if (stmtToken == "REM") {
                    std::string commentText = scanner.getRemainingInput(); // 获取 REM 后的所有内容
                    stmt = new RemStatement(commentText);  // 生成 REM 语句
                } else if (stmtToken == "GOTO") {
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 缺目标行
                    int targetLine = stringToInteger(scanner.nextToken());
                    if (program.getSourceLine(targetLine) == "") {
                        error("LINE NUMBER ERROR");
                    } // 目标行错误
                    if (scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 多了不该有的
                    stmt = new GotoStatement(targetLine);
                } else if (stmtToken == "IF") {
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 缺表达式
                    Expression *lhs = readE(scanner);
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 缺符号
                    std::string op = scanner.nextToken();
                    if (op != "<" && op != "=" && op != ">") {
                        error("SYNTAX ERROR");
                    } // 符号不对
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 符号后缺数
                    Expression *rhs = readE(scanner);
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 缺跳转
                    scanner.verifyToken("THEN");
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 缺目标行
                    int targetLine = stringToInteger(scanner.nextToken());
                    if (program.getSourceLine(targetLine) == "") {
                        error("LINE NUMBER ERROR");
                    } // 目标行错误
                    if (scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                    } // 多了不该有的
                    stmt = new IfStatement(lhs, op, rhs, targetLine);
                } else if (stmtToken == "END") {
                    stmt = new EndStatement();
                } else {
                    error("SYNTAX ERROR");
                }

                if (stmt != nullptr) { // stmt有效，存它
                    program.addSourceLine(lineNumber, line);
                    program.setParsedStatement(lineNumber, stmt);
                }
            }
        } else {
            // 处理命令的情况（即没有行号的命令）
            Statement *stmt = nullptr;

            if (token == "RUN") {
                runProgram(program, state);
            } else if (token == "LIST") {
                listProgram(program);
            } else if (token == "CLEAR") {
                program.clear();
                state.Clear();
            } else if (token == "QUIT") {
                exit(0);
            } else if (token == "LET") {
                const std::string var = scanner.nextToken();
                if (scanner.nextToken() != "=") {
                    error("SYNTAX ERROR");
                }
                Expression *exp = parseExp(scanner);
                stmt = new LetStatement(var, exp);
            } else if (token == "PRINT") {
                Expression *exp = parseExp(scanner);
                stmt = new PrintStatement(exp);
            } else if (token == "INPUT") {
                std::string var = scanner.nextToken();
                if (scanner.hasMoreTokens()) {
                    error("SYNTAX ERROR");
                }
                stmt = new InputStatement(var);
            } else {
                error("SYNTAX ERROR");
            }

            // 立即执行命令
            if (stmt != nullptr) {
                stmt->execute(state, program);
                delete stmt; // 删除立即执行的语句，避免内存泄漏
            }
        }
    }
}


void runProgram(Program &program, EvalState &state) {
    int lineNumber = program.getFirstLineNumber();
    while (lineNumber != -1) {
        Statement *stmt = program.getParsedStatement(lineNumber);
        if (stmt != nullptr) {
            program.AddTimes(lineNumber);
            if (const auto *gotoStmt = dynamic_cast<GotoStatement*>(stmt)) { // stmt是GotoStatement类型的
                lineNumber = gotoStmt->getTargetLine();
            } else if (const auto *ifStmt = dynamic_cast<IfStatement*>(stmt)) { // stmt是IfStatement类型的
                if (ifStmt->isConditionTrue(state)) {
                    lineNumber = ifStmt->getTargetLine();
                } else {
                    lineNumber = program.getNextLineNumber(lineNumber);
                }
            } else if (dynamic_cast<EndStatement*>(stmt)) { // stmt是EndStatement类型的
                lineNumber = -1;
            } else { // 其他，正常转移
                stmt->execute(state, program);
                lineNumber = program.getNextLineNumber(lineNumber);
            }
        }
    }
}

void listProgram(Program &program) {
    int lineNumber = program.getFirstLineNumber();
    while (lineNumber != -1) {
        std::cout << lineNumber << " " << program.getSourceLine(lineNumber) << std::endl;
        lineNumber = program.getNextLineNumber(lineNumber);
    }
}
