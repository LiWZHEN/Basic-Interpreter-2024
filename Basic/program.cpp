/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"


Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
    // Replace this stub with your own code
    //todo
    info.clear(); // 清除所有的源代码

    // 清除所有的解析对象，确保删除指针避免内存泄漏
    for (auto &entry : storage) {
        delete entry.second;
    }
    storage.clear();

    // 清除执行次数
    executionCount.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    // Replace this stub with your own code
    //todo
    deleteParsedStatement(lineNumber);
    info[lineNumber] = line;
    executionCount[lineNumber] = 0;
}

void Program::removeSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    info.erase(lineNumber);
    deleteParsedStatement(lineNumber);
    executionCount.erase(lineNumber);
}

std::string Program::getSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    if (info.find(lineNumber) != info.end()) {
        return info[lineNumber];
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
    //todo
    if (storage.find(lineNumber) != storage.end()) { // 如果之前存在 Statement，需要删除以避免内存泄漏
        delete storage[lineNumber];
    }

    // 更新 storage 中该行的解析对象
    storage[lineNumber] = stmt;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
   // Replace this stub with your own code
   //todo
    if (storage.find(lineNumber) != storage.end()) { // 返回行号对应的解析后对象
        return storage[lineNumber];
    }
    return nullptr; // 找不到就返回空指针
}

int Program::getFirstLineNumber() {
    if (info.empty()) {
        return -1;
    }
    int minLine = INT_MAX;
    for (const auto &p : info) {
        minLine = std::min(minLine, p.first);
    }
    return minLine;
}

int Program::getNextLineNumber(int lineNumber) {
    int minLine = INT_MAX;
    bool found = false;
    for (const auto &p : info) {
        if (p.first > lineNumber) {
            minLine = std::min(minLine, p.first);
            found = true;
        }
    }
    return found ? minLine : -1; // 有下一行返回下一行行号，没有就返回-1
}

//more func to add
//todo

void Program::AddTimes(int lineNumber) {
    ++executionCount[lineNumber];
    if (executionCount[lineNumber] >= 4294967295) {
        error("SYNTAX ERROR");
        exit(1);
    }
}

