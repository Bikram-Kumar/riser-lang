#pragma once

#include "../Object.cpp"
#include <vector>
#include <map>
#include <iostream>


enum struct Type {
    UNDEFINED,
    STRING,
    INT
};

enum struct Operator {
    PLUS,
    MINUS,
    STAR,
    SLASH
};

class IdData {
public:
    Type type;
    int index;
    IdData(Type type, int index) : type(type), index(index) {}
};

extern std::map<std::string,IdData*> idMap;

extern std::vector<int> intVars;
extern std::vector<std::string> stringVars;

class ASTNode {
public:
    virtual void run() = 0;
    virtual ~ASTNode() {};
};

class ASTCodeLine: public ASTNode {
public:
    ASTNode* stmnt;
    ASTCodeLine(ASTNode* stmnt) : stmnt(stmnt) { }
    
    virtual void run() override {
        stmnt->run();
    }
    
    ~ASTCodeLine() {
        delete stmnt;
    }
};


class ASTCodeBlock: public ASTNode {
public:
    std::vector<ASTCodeLine*> lines;
    
    virtual void run() override {
        for (ASTCodeLine* line: lines) {
            line->run();
        }
    }
    
    ~ASTCodeBlock() {
        for (ASTCodeLine* line: lines) {
            delete line;
        }
    }
};



class ASTId: public ASTNode {
public:
    std::string id;
    IdData* idData;
    
    ASTId(std::string id) : id(id) {}
    
    virtual void run() override {
        if (idMap.find(id) != idMap.end()) {
            idData = idMap[id];
        } else {
            std::cerr << id << " not defined\n";
        }
    }
    
    ~ASTId () {
        delete idData;
    }
    
};


class ASTExpression : public ASTNode {
public:
    Type type = Type::UNDEFINED;
    ASTExpression* lhs;
    Operator op;
    ASTExpression* rhs;
    
    Object* value;
    ASTId* id; // if this expression is a variable name
    
    ASTExpression(ASTExpression* lhs, Operator op, ASTExpression* rhs) {
        if (lhs->type != Type::UNDEFINED && rhs->type != Type::UNDEFINED) {
            if (lhs->type != rhs->type) {
                std::cerr << "Type mismatch!\n";
            }
            this->type = lhs->type;
        }
        this->lhs = lhs;
        this->op = op;
        this->rhs = rhs;
    }
    
    ASTExpression(int num): value(new Integer(num)), type(Type::INT) {
    }
    
    ASTExpression(std::string str): value(new String(str)), type(Type::STRING) {}
    
    ASTExpression(ASTId* id): id(id) {}
    
    virtual void run() override {
        
        if (this->id != nullptr) {
            this->id->run();
            type = this->id->idData->type;
            switch (type) {
                case Type::INT :
                    this->value = new Integer(intVars[this->id->idData->index]);
                    break;
                case Type::STRING :
                    this->value = new String(stringVars[this->id->idData->index]);
                    break;
            }
            return;
        }
        
        
        if (!lhs || !rhs) return;  // no need to do anything
        lhs->run();
        rhs->run();
        
        if (lhs->type != rhs->type) {
                std::cerr << "Type mismatch!\n";
        }
        type = lhs->type;
        
        
        //std::cout << ((Integer*)rhs->value)->val << "\n";
        switch(type) {
            case Type::STRING :
                if (op != Operator::PLUS) {
                    std::cerr << "Invalid operation for String\n";
                    return;
                }
                this->value = new String(((String*)lhs->value)->val + ((String*)rhs->value)->val);
                break;
            case Type::INT :
                switch (op) {
                    case Operator::PLUS :
                        this->value = new Integer(((Integer*)lhs->value)->val + ((Integer*)rhs->value)->val);
                        break;
                    case Operator::MINUS :
                        this->value = new Integer(((Integer*)lhs->value)->val - ((Integer*)rhs->value)->val);
                        break;
                    case Operator::STAR :
                        this->value = new Integer(((Integer*)lhs->value)->val * ((Integer*)rhs->value)->val);
                        break;
                    case Operator::SLASH :
                        this->value = new Integer(((Integer*)lhs->value)->val / ((Integer*)rhs->value)->val);
                        break;
                }
                break;
        }
        

    }
    
    ~ASTExpression() {
        delete lhs; delete rhs; delete value;
    }
    
    
};


class ASTDeclaration : public ASTNode {
public:
    Type type;
    std::string id;
    
    // "int var;" type constructor
    ASTDeclaration(Type type, std::string id) : type(type), id(id) {}
    
    // add constructor for "int var = 6;" type
    
    virtual void run() override {
        // allocate a new variable
        int index;
        switch (type) {
            case Type::STRING :
                index = stringVars.size();
                stringVars.push_back("");
                break;
            case Type::INT :
                index = intVars.size();
                intVars.push_back(0);
                break;
        }
        idMap[id] = new IdData(type, index);
    }
    
};





class ASTAssignment: public ASTNode {
public:
    std::string id;
    ASTExpression* expr;
    
    ASTAssignment(std::string id, ASTExpression* expr): id(id), expr(expr) {}
    
    virtual void run() override {
        if (idMap.find(id) == idMap.end()) {
            std::cerr << id + " not declared.\n";
            return;
        }
        expr->run();
        IdData* idData = idMap[id];
        switch (idData->type) {
            case Type::STRING :
                stringVars[idData->index] = ((String*)expr->value)->val;
                break;
            case Type::INT :
                intVars[idData->index] = ((Integer*)expr->value)->val;
                break;
        }
    }
    
    ~ASTAssignment () {
        delete expr;
    }
    
};




class ASTPrintStatement : public ASTNode {
public:
    ASTExpression* expr;
    ASTPrintStatement(ASTExpression* expr) : expr(expr) {}
    
    virtual void run() override {
        expr->run();
        if (expr->type == Type::STRING) {
            std::cout << ((String*)expr->value)->val << std::endl;
        } else {
            std::cout << ((Integer*)expr->value)->val << std::endl;
        }
    }
    
    ~ASTPrintStatement () {
        delete expr;
    }
};



class ASTIfStatement : public ASTNode {
public:
    ASTExpression* condition;
    ASTCodeBlock* block;
    
    ASTIfStatement(ASTExpression* condition, ASTCodeBlock* block) : condition(condition), block(block) {
        
    }
    
    virtual void run() override {
        condition->run();
        if (condition->type == Type::INT && ((Integer*)condition->value)->val) {
            block->run();
        }
    }
    
    ~ASTIfStatement() {
        delete block; delete condition;
    }

};

class ASTElseStatement : public ASTNode {
public:
    
};

class ASTWhileStatement : public ASTNode {
public:
    ASTExpression* condition;
    ASTCodeBlock* block;
    
    ASTWhileStatement(ASTExpression* condition, ASTCodeBlock* block) : condition(condition), block(block) {
        
    }
    
    virtual void run() override {
        condition->run();
        if (condition->type != Type::INT) {
            std::cerr << "Invalid condition\n";
            return;
        }
        
        while (((Integer*)condition->value)->val) {
            block->run();
            condition->run();
        }
    }
    
    ~ASTWhileStatement() {
        delete block; delete condition;
    }
    
};

class ASTForStatement : public ASTNode {
public:
    
};
