#include<string.h>
#include <string>
#include "calc.h"
#include <vector>
#include <tuple>
#include <map>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <pthread.h>
#include <semaphore.h>
using std::vector;
using std::map;
using std::string;
  struct Calc{
   std::map<std::string,int> vars;
   std::vector<std::string> tokenize(const std::string & expr);
   pthread_mutex_t lock;
   int calcEval(const char *expr, int * result);
  };

extern "C" struct Calc* calc_create(void){
   struct Calc * myCalc =  new Calc();
   pthread_mutex_init(&myCalc->lock,NULL);
   return myCalc;
}
//function to divide input into tokens, taken from project description
std::vector<std::string>Calc:: tokenize(const std::string & expr){
  std::vector<std::string> vec;
  std::stringstream s(expr);
  std::string token;
  while (s >> token){
    vec.push_back(token);
  }
  return vec;
}
//eval function
int Calc::calcEval(const char *expr, int * result){
  std::vector<std::string> firstVec = tokenize(expr);
  std::vector<std::string> vec;
  std::vector<int> numbVec = {};
  std::vector<char> opVec = {};
  std::string variable;
  bool myBool = 0;
  pthread_mutex_lock(&lock);
    //check if variable is being initialized
  if(firstVec.size() > 1 && strcmp(firstVec[1].c_str(),"=") == 0){
    myBool = 1;
    variable = firstVec[0];
    for(uint i = 0; i < variable.size(); i++){
      if (!isalpha(variable[i])){
	return 0;
      }
    }
  }
    //if variable is being initialized, push operation statement after equal sign onto new vector
  //else give vec the operation statement
  if(myBool){
    for(uint i = 2; i < firstVec.size(); i++){
      vec.push_back(firstVec[i]);
    }
  }
  else
    vec = firstVec;
  
  for(uint i = 0; i < vec.size();i++){
    std::string myTemp = vec[i];
    bool isDig = 1;
    for(uint j = 0; j < myTemp.size(); j++){
      if(!isdigit(myTemp[j]))
	isDig = 0;
    }
    //this section organizes tokens by token type onto different vectors
    //if it is a digit, push back onto number vector
    //else if negative number, push back negative onto operator vector and digit onto number vector
    //else if single char token, check if operator -> push back onto operator vector, else if variable, push back number
          //that variable represents onto number vector
    //else: if negative number, push back negative integer representative, else look up variable and push back the number
          //it represents onto the number vector
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
      else{
	return 0;
      }
      }
    }
  }
  //invalid expression check      
  if(numbVec.size() != opVec.size()+1){
    return 0;
  }
  std::reverse(numbVec.begin(),numbVec.end());
  std::reverse(opVec.begin(),opVec.end()); 
  while(numbVec.size() > 1 ){
    //addition operation
    if(opVec.back() == 43){
      int one = numbVec.back();numbVec.pop_back();
      int two = numbVec.back();numbVec.pop_back();
      int myResult = one + two;
      numbVec.push_back(myResult);
      opVec.pop_back();
    }
    //subtraction operation
    else if(opVec.back() == 45){
      int one = numbVec.back();numbVec.pop_back();
      int two = numbVec.back();numbVec.pop_back();
      int myResult = one - two;
      numbVec.push_back(myResult);
      opVec.pop_back();
    }
    //multiplication operation
    else if (opVec.back() == 42){
      int one = numbVec.back();numbVec.pop_back();
      int two = numbVec.back();numbVec.pop_back();
      int myResult = one*two;
      numbVec.push_back(myResult);
      opVec.pop_back();
    }
    //division operation
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
  //initialize variable if not in the map already
  if(myBool){
    if(vars.find(variable) != vars.end())
      vars.at(variable) = numbVec.back();
    else
    vars.insert(std::pair<std::string,int>(variable,numbVec.back()));
  }
  pthread_mutex_unlock(&lock);
  return numbVec.back();
}
  
extern "C" void calc_destroy(struct Calc *calc){
  //remove pthread mutex
  pthread_mutex_destroy(&calc->lock);
  delete(calc);  
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result){
  return calc->calcEval(expr,result);
}

