#pragma once
class Object {
};

class Number : public Object {
    
};

class String : public Object {
public:
    std::string val;
    String(std::string str) : val(str) {}
    
};

class Integer : public Number {
public:
    Integer(int val) : val(val) {}
    int val;
};

class Float : Number {
public:
    float val;
};

class Double : Number {
public:
    double val;
};