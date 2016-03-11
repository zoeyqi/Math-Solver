#include<sstream>
#include <iostream>
#include <ctype.h>
#include <cstdlib>
#include <string.h>
#include <stdio.h>

class Expression{
 public:
  virtual std::string toString() const = 0;
  virtual long evaluate() const = 0;
  virtual ~Expression(){}
};

class NumExpression: public Expression{
 private:
  long num;
 public:
 NumExpression(long a):num(a){}
  virtual std::string toString() const{
    std::stringstream s;
    s<<num;
    return s.str();
  }
  virtual long evaluate() const{
    return num;
  }
};

class PlusExpression: public Expression{
 private:
  Expression* lhs;
  Expression* rhs;
 public:
 PlusExpression(Expression * lhs, Expression * rhs):lhs(lhs),rhs(rhs) {}
  virtual std::string toString() const{
    std::stringstream s;
    s<<"(";
    s<<lhs->toString();
    s<<" + ";
    s<<rhs->toString();
    s<<")";
    return s.str();
  }
  virtual long evaluate() const{
    return (lhs->evaluate()+rhs->evaluate());
  }
  virtual ~PlusExpression(){
    delete lhs;
    delete rhs;
  }
};

class MinusExpression: public Expression{
 private:
  Expression* lhs;
  Expression* rhs;
 public:
 MinusExpression(Expression * lhs, Expression* rhs):lhs(lhs),rhs(rhs) {}
  virtual std::string toString() const{
    std::stringstream s;
    s<<"(";
    s<<lhs->toString();
    s<<" - ";
    s<<rhs->toString();
    s<<")";
    return s.str();
  }
  virtual long evaluate() const{
    return (lhs->evaluate()-rhs->evaluate());
  }
  virtual ~MinusExpression(){
    delete lhs;
    delete rhs;
  }
};

class TimesExpression: public Expression{
 private:
  Expression* lhs;
  Expression* rhs;
 public:
 TimesExpression(Expression * lhs, Expression * rhs):lhs(lhs),rhs(rhs) {}
  virtual std::string toString() const{
    std::stringstream s;
    s<<"(";
    s<<lhs->toString();
    s<<" * ";
    s<<rhs->toString();
    s<<")";
    return s.str();
  }
  virtual long evaluate() const{
    return (lhs->evaluate()*rhs->evaluate());
  }
  virtual ~TimesExpression(){
    delete lhs;
    delete rhs;
  }
};

class DivExpression: public Expression{
 private:
  Expression* lhs;
  Expression* rhs;
 public:
 DivExpression(Expression * lhs, Expression * rhs):lhs(lhs),rhs(rhs) {}
  virtual std::string toString() const{
    std::stringstream s;
    s<<"(";
    s<<lhs->toString();
    s<<" / ";
    s<<rhs->toString();
    s<<")";
    return s.str();
  }
  virtual long evaluate() const{
    return (lhs->evaluate()/rhs->evaluate());
  }
  virtual ~DivExpression(){
    delete lhs;
    delete rhs;
  }
};
