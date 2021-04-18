#include<string.h>
#include <string>
#include "calc.h"
#include <vector>
#include <tuple>
#include <map>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
using std::vector;
using std::map;
using std::string;
  struct Calc{
   std::map<std::string,int> vars;
   std::vector<std::string> tokenize(const std::string & expr);
   int calcEval(const char *expr, int * result);
  };

extern "C" struct Calc* calc_create(void){
   return new Calc();
}

std::vector<std::string>Calc:: tokenize(const std::string & expr){
  std::vector<std::string> vec;
  std::stringstream s(expr);
  std::string token;
  while (s >> token){
    vec.push_back(token);
  }
  return vec;
}
int Calc::calcEval(const char *expr, int * result){
  std::vector<std::string> firstVec = tokenize(expr);
  std::vector<std::string> vec;
  std::vector<int> numbVec = {};
  std::vector<char> opVec = {};
  std::string variable;
  bool myBool = 0;
  if(firstVec.size() > 1 && strcmp(firstVec[1].c_str(),"=") == 0){
    myBool = 1;
    variable = firstVec[0];
    for(int i = 0; i < variable.size(); i++){
      if (!isalpha(variable[i])){
	return 0;
      }
    }
  }
  if(myBool){
    for(int i = 2; i < firstVec.size(); i++){
      vec.push_back(firstVec[i]);
    }
  }
  else
    vec = firstVec;
  
  for(int i = 0; i < vec.size();i++){
    std::string myTemp = vec[i];
    bool isDig = 1;
    for(int j = 0; j < myTemp.size(); j++){
      if(!isdigit(myTemp[j]))
	isDig = 0;
    }
    if(isDig){
      numbVec.push_back(stoi(vec[i]));
    }
    else if(i == 0 && vec[i].size() == 1 && vec[i].front() == 45){
      numbVec.push_back(0);
      opVec.push_back(vec[i].front());
    }
    else if(vec.at(i).size() == 1){
      char myChar = vec[i].front();
      if(myChar == 47 || myChar == 45 || myChar == 42 || myChar == 43)
	opVec.push_back(myChar);
      else if(vars.find(vec[i]) != vars.end()){
	numbVec.push_back(vars.find(vec.at(i))->second);
	}
    }
    else{
      if(vec.at(i).front() == 45){
	numbVec.push_back(stoi(vec.at(i)));
      }
      else{
      std::string checker = vec[i];
      if(vars.find(checker) != vars.end()){
	numbVec.push_back(vars.find(checker)->second);
      }
      else
	return 0;
      }
    }
  }
      
  if(numbVec.size() != opVec.size()+1){
    return 0;
  }
  std::reverse(numbVec.begin(),numbVec.end());
  std::reverse(opVec.begin(),opVec.end()); 
  while(numbVec.size() > 1 ){
    if(opVec.back() == 43){
      int one = numbVec.back();numbVec.pop_back();
      int two = numbVec.back();numbVec.pop_back();
      int myResult = one + two;
      numbVec.push_back(myResult);
      opVec.pop_back();
    }
    else if(opVec.back() == 45){
      int one = numbVec.back();numbVec.pop_back();
      int two = numbVec.back();numbVec.pop_back();
      int myResult = one - two;
      numbVec.push_back(myResult);
      opVec.pop_back();
    }
    else if (opVec.back() == 42){
      int one = numbVec.back();numbVec.pop_back();
      int two = numbVec.back();numbVec.pop_back();
      int myResult = one*two;
      numbVec.push_back(myResult);
      opVec.pop_back();
    }
    else{
      int one = numbVec.back();numbVec.pop_back();
      int two = numbVec.back();numbVec.pop_back();
      if(two == 0)
	return 0; 
      int myResult = one/two;
      numbVec.push_back(myResult);
      opVec.pop_back();
    }
  }
  
  *result = numbVec.back();
  if(myBool){
    vars.insert(std::pair<std::string,int>(variable,numbVec.back()));
  }
  return numbVec.back();
}
  
extern "C" void calc_destroy(struct Calc *calc){
  delete(calc);  
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result){
  return calc->calcEval(expr,result);
}

