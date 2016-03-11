#include "expr.h"
#include <iostream>
#include <ctype.h>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
using namespace std;
map<string, vector<pair<string,string> > > function;
map<string, string> todo;
vector<double> range;
Expression * parse(const char ** strp,string evaname,int sign);
Expression * abst(const char ** strp,int sign);

void skipSpace(const char ** strp) {
  while(isspace(**strp)) {
    *strp = *strp + 1;
  }
}

Expression * makeExpr(char op, Expression * lhs, Expression * rhs) {
  switch(op) {
  case '+':
    return new PlusExpression(lhs,rhs);
  case '-':
    return new MinusExpression(lhs,rhs);
  case '*':
    return new TimesExpression(lhs,rhs);
  case '/':
    return new DivExpression(lhs,rhs);
  case 'p':
    return new PowExpression(lhs,rhs);
  case 's':
    return new SqrtExpression(lhs);
  default:
    std::cerr << "Impossible op char: " << op << "\n";
    abort();
  }
}
bool isValidOp(char c) {
  return strchr("+-*/", c) != NULL;
}


Expression * parseOp(const char ** strp,string evaname,int sign) {
  skipSpace(strp);
  const char** help=strp;// to help decide which operation
  *help=*help+1;
  char op;
  Expression * lhs;
  Expression * rhs;
  if(**help==' '){//+-*/
    *help=*help-1;
  op = **strp;
  // cout<<"op"<<op<<endl;
  if (!isValidOp(op)) {
    std::cerr << "Invalid op: "<< op<< "\n";
    return NULL;
  }
  *strp = *strp + 1;
  lhs=parse(strp,evaname,sign);
  if (lhs == NULL) {
    return NULL;
  }
  rhs = parse(strp,evaname,sign);
  if (rhs == NULL) {
    delete lhs;
    return NULL;
  }
  }
  else{
    *help=*help-1;
    if(**help=='p'){
      if(((*help)[1]=='o')&&((*help)[2]=='w')&&((*help)[3]==' ')){
        op='p';
        *strp=*strp+3;
      }
      else{
        std::cerr << "Invalid op"<<"\n";
        return NULL;
      }
      lhs=parse(strp,evaname,sign);
      if (lhs == NULL) {
        return NULL;
      }
      rhs = parse(strp,evaname,sign);
      if (rhs == NULL) {
        delete lhs;
        return NULL;
      }
    }
    else if(**help=='s'){
      if(((*help)[1]=='q')&&((*help)[2]=='r')&&((*help)[3]=='t')&&((*help)[4]==' ')){
        op='s';
        *strp=*strp+4;
      }
      else{
        std::cerr << "Invalid op"<<"\n";
        return NULL;
      }
      lhs=parse(strp,evaname,sign);
      if (lhs == NULL) {
        return NULL;
      }
      rhs=NULL;
    }
  }
  skipSpace(strp);
  if (**strp == ')') {
    *strp = *strp + 1;
    return makeExpr(op,lhs,rhs);
  }
  std::cerr <<"Expected ) but found " << *strp << "\n";
  delete lhs;
  delete rhs;
  return NULL;
}
Expression * parse(const char ** strp,string evaname,int sign) {
  skipSpace(strp);
  if (**strp == '\0') {
    std::cerr << "End of line found mid expression!\n";
    return NULL;
  }
  else if (**strp == '(') {
    // (op E E)
    string judge;//+- or g f
    char op=(*strp)[1];
    size_t i=1;
    while((*strp)[i]!=' '){
      judge+=(*strp)[i];
      i++;
    }
    map<string,vector<pair<string,string> > >::iterator it2;
    it2=function.find(judge);
    if(isValidOp(op)||(judge=="pow")||(judge=="sqrt")){
    *strp = *strp + 1;
    return parseOp(strp,evaname,sign);
    }
    else if(it2!=function.end()){
      for(size_t i=0;i<it2->second.size();i++){
        it2->second[i].second=it2->second[i].first;
      }
      Expression* abs=abst(strp,0);
      *strp=*strp+1;
      return abs;
    }
    else{
      cerr<<"Invalid op or function name"<<endl;
      return NULL;
    }
  }
  else if(((**strp>='0')&&(**strp<='9'))||(**strp=='-')){
    //number
    char * endp;
    double num = strtod(*strp, &endp);
    if (endp == *strp) {
      std::cerr << "Expected a number, but found " << *strp << "\n";
      return NULL;
    }
    *strp = endp;
    //cout<<"0-9: "<<num<<endl;
    return new NumExpression(num);
      // return NULL;
  }
  else{//string x y etc.
    string vari;
    while((**strp!=' ')&&(**strp!=')')){
      vari+=**strp;
      *strp=*strp+1;
    }
    //cout<<vari<<endl;
    map<string, vector<pair<string,string> > >::iterator it;
    it=function.find(evaname);
    size_t size=it->second.size();
    size_t i;
    int flag=-1;
    if(sign==0){
      for(i=0;i<size;i++){
        if(it->second[i].first==vari){
          vari=it->second[i].second;
          break;
        }
      }
    }
    if(sign==1){
    for(i=0;i<size;i++){
      if(it->second[i].first==vari){
        flag=1;
      }
    }
    if(flag==-1){
      cerr<<"incorrect variables in the function"<<endl;
      return NULL;
    }
    }
    return new StrExpression(vari,evaname);
  }
}

string multi(const char** strp){
  string evaname;
  while(**strp!=' '){
    evaname+=**strp;
    *strp+=1;
  }
  //map<string, vector<pair<string,string> > > function2=function;
  //map<string, string> todo2=todo;
  map<string,string>::iterator it;
  it=todo.find(evaname);
  if(it==todo.end()){
    cout<<"cannot find function"<<endl;
    return "R";
  }
  string what=it->second;//what to do with the function
  //cout<<"what"<<what<<endl;
  const char* fromwhat=what.c_str();
  size_t varnum;//numer of variables
  map<string, vector<pair<string,string> > >::iterator it2;
  it2=function.find(evaname);
  varnum=it2->second.size();
  //cout<<varnum;
  skipSpace(strp);
  vector<string> temp;
  while(**strp!=')'){
    string number;  //actual number of variables
    if(**strp=='('){
      *strp=*strp+1;
      number=multi(strp);
      if(number=="R"){
        return "R";
      }
      while(**strp!=')'){
        *strp=*strp+1;
      }
      *strp=*strp+1;
    }
    else{
      while((**strp!=' ')&&(**strp!=')')){
        number+=**strp;
        *strp=*strp+1;
      }
    }
    //cout<<"multi"<<number<<endl;
    //it2->second[i].second=number;
    temp.push_back(number);
    skipSpace(strp);
  }
  skipSpace(strp);
  if(temp.size()!=varnum){
    cout<<"numbers cannot match variables"<<endl;
    return "R";
  }
  for(size_t i=0;i<varnum;i++){
    it2->second[i].second=temp[i];
  }
  Expression* expr=parse(&fromwhat,evaname,0);
  if(expr!=NULL){
    //std::cout << "Parsed expression to: " << expr->toString() << "\n";
    //std::cout << "Evaluated expression to: " << expr->evaluate() << "\n";
  }
  else{
    std::cout << "Could not parse expression, please try again.\n";
  }
  double convert=expr->evaluate(); //the result of this function
  //cout<<"convert"<<convert<<endl;
  delete expr;
  stringstream s;
  s<<convert;
  //cout<<"s"<<s.str()<<endl;
  return s.str();
}

double numint(vector<double> dparas,const char** fromnumwhat,size_t i,string numname){
  //cout<<fromnumwhat<<endl;
  string origin(*fromnumwhat);
  const char* strp;
  double sum=0;
  double step=dparas[0];
  // double area=pow(step,i);//dxdy...
  //double j;
  //stringstream s;
  string ss;
  Expression* expr;
  map<string, vector<pair<string,string> > >::iterator it;
  it=function.find(numname);
  if(i==1){
   double j=dparas[1];
    while(j<=(dparas[2]-step+0.00001)){
      stringstream s;
      //cout<<"j1"<<j<<endl;
      strp=origin.c_str();
      s<<(j+step/2);
      ss=s.str();
      //cout<<"ss"<<ss<<endl;
      it->second[i-1].second=ss;
      expr=parse(&strp,numname,0);
      sum+=expr->evaluate();
      //cout<<"sum="<<sum<<endl;
      j+=step;
      delete expr;
    }
    return sum*step;
  }
  else{
    double j=dparas[2*i-1];
    while(j<=(dparas[2*i]-step+0.00001)){
      //cout<<"j2"<<j<<endl;
      stringstream s;
      strp=origin.c_str();
      s<<(j+step/2);
      ss=s.str();
      it->second[i-1].second=ss;
      // expr=parse(&strp,numname);
      sum+=numint(dparas,&strp,i-1,numname);
      j+=step;
    }
    sum=sum*step;
  }
  return sum;
}

Expression* abst(const char** strp,int sign){
  map<string,vector<pair<string,string> > >::iterator it;
  map<string,string>::iterator it2;
  vector<pair<string,string> > temp;
  vector<string> change;
  string name;
  skipSpace(strp);
  if(**strp=='('){
    *strp=*strp+1;
  }
  skipSpace(strp);
  while(**strp!=' '){
    name+=**strp;
    *strp=*strp+1;
  }
  it=function.find(name);
  temp=it->second;
  it2=todo.find(name);
  skipSpace(strp);
  while(**strp!=')'){
    string help;
    while((**strp!=' ')&&(**strp!=')')){
      if(**strp=='('){
        Expression* helper=parse(strp,name,0);
        skipSpace(strp);
        //stringstream s;
        help=helper->toString();
      delete helper;
      break;
      }
    else{
      help+=**strp;
      *strp=*strp+1;
    }
  }
  skipSpace(strp);
  //cout<<"help: "<<help<<endl;
  change.push_back(help);
  }
  if(change.size()!=(it->second).size()){
    cerr<<"the number of inputs does not match function"<<endl;
    return 0;
  }
  for(size_t j=0;j<change.size();j++){
    temp[j].second=change[j];
    //cout<<"change"<<j<<change[j]<<endl;
  }
  it->second=temp;
  const char* abstract=(it2->second).c_str();
  // cout<<"abstract:"<<abstract<<endl;
  Expression* fromabs=parse(&abstract,name,0);
  //cout<<"fromabs:"<<fromabs->toString()<<endl;
  return fromabs;
}

void mchelper(vector<double> dparas,const char** fromnumwhat,size_t i,string numname){
  string origin(*fromnumwhat);
  const char* strp;
  double step=0.01;
  string ss;
  Expression* expr;
  map<string, vector<pair<string,string> > >::iterator it;
  it=function.find(numname);
  if(i==1){
    double j=dparas[1];
    while(j<=(dparas[2]-step+0.000001)){
      stringstream s;
      //cout<<"j1"<<j<<endl;
      /* printf("j1=%0.16f\n",j);
      printf("step=%0.16f\n",step);
      printf("dparas=%0.16f\n",dparas[2]);
      printf("judge=%0.16f\n",dparas[2]-step);*/
      strp=origin.c_str();
      s<<(j+step/2.0);
      ss=s.str();
      //cout<<"ss"<<ss<<endl;
      it->second[i-1].second=ss;
      expr=parse(&strp,numname,0);
      range.push_back(expr->evaluate());
      //cout<<"element="<<expr->e<<endl;
      j+=step;
      delete expr;
    }
  }
  else{
    double j=dparas[2*i-1];
    while(j<=(dparas[2*i]-step+0.000001)){
      /*printf("j2=%0.16f\n",j);
      printf("step=%0.16f\n",step);
      printf("dparas=%0.16f\n",dparas[2*i]);
      printf("judge=%0.16f\n",dparas[2*i]-step);*/
      stringstream s;
      strp=origin.c_str();
      s<<(j+step/2.0);
      ss=s.str();
      it->second[i-1].second=ss;
      mchelper(dparas,&strp,i-1,numname);
      j=j+step;
    }
  }
}

vector<double> maandmi(void){
  vector<double>::iterator it=range.begin();
  double max=*it;
  double min=*it;
  vector<double> mami;
  ++it;
  while(it!=range.end()){
    //cout<<"range:"<<*it<<endl;
    if((*it)>max){
      max=*it;
    }
    else if((*it)<min){
      min=*it;
    }
    else{
    }
    ++it;
  }
  mami.push_back(max);
  mami.push_back(min);
  return mami;
}

vector<double> gradient(vector<double> curr,string lastname,string lastwhat){
  vector<double> result(curr.size());
  double h=0.000000000001;
  double origin;
  double plush;
  string ss;
  const char* fromlast=lastwhat.c_str();
  map<string, vector<pair<string,string> > >::iterator it;
  Expression* expr;
  it=function.find(lastname);
  for(size_t i=0;i<curr.size();i++){
    char str[255];
    sprintf(str,"%.16f",curr[i]);
    //s<<curr[i];
    ss=str;
    it->second[i].second=ss;
  }
  expr=parse(&fromlast,lastname,0);
  origin=expr->evaluate();
  //cout<<"origin:"<<origin<<endl;
  delete expr;
  for(size_t i=0;i<curr.size();i++){
    char str[255];
    double m=curr[i]+h;
    //printf("m=%0.16f\n",m);
    sprintf(str,"%.16f",m);
    ss=str;
    //cout<<ss<<endl;
    it->second[i].second=ss;
    fromlast=lastwhat.c_str();
    expr=parse(&fromlast,lastname,0);
    plush=expr->evaluate();
    //printf("plush=%0.16f\n",plush);
    //cout<<"plush:"<<plush<<endl;
    delete expr;
    char str2[255];
    sprintf(str2,"%.16f",m-h);
    ss=str2;
    it->second[i].second=ss;
    result[i]=(plush-origin)/h;
    //cout<<i<<"result:"<<result[i]<<endl;
  }
  return result;
}

double dist(vector<double> curr,vector<double> pnew){
  double result=0;
  for(size_t i=0;i<curr.size();i++){
    result+=pow((pnew[i]-curr[i]),2);
  }
  result=sqrt(result);
  return result;
}

double gradhelper(vector<double> curr,vector<double> pnew,double gamma,double converge,string lastname,string lastwhat,double distorigin,int flag){
  double distance=distorigin;
  vector<double> grad(curr.size());
  map<string, vector<pair<string,string> > >::iterator it;
  it=function.find(lastname);
  while(distance>=converge){
    for(size_t i=0;i<curr.size();i++){
      curr[i]=pnew[i];
    }
    grad=gradient(curr,lastname,lastwhat);
    if(flag==1){
      for(size_t i=0;i<curr.size();i++){
        pnew[i]=curr[i]+gamma*grad[i];
        //cout<<i<<"pnew in helper:"<<pnew[i]<<endl;
      }
    }
    else if(flag==0){
      for(size_t i=0;i<curr.size();i++){
        pnew[i]=curr[i]-gamma*grad[i];
      }
    }
    distance=dist(curr,pnew);
    //cout<<"distance:"<<distance<<endl;
  }
  for(size_t i=0;i<pnew.size();i++){
    char str[255];
    string s;
    sprintf(str,"%.16f",pnew[i]);
    s=str;
    it->second[i].second=s;
    //cout<<"last:"<<pnew[i]<<endl;
  }
  const char* last=lastwhat.c_str();
  Expression* expr=parse(&last,lastname,0);
  double result=expr->evaluate();
  delete expr;
  return result;
}

int main(int argc,char ** argv){
  if(argc!=2){
    fprintf(stderr,"Can't find the file name\n");
    return EXIT_FAILURE;
  }
  FILE* f=fopen(argv[1],"r");
  if(f==NULL){
    perror("Could not open file");
    fclose(f);
    return EXIT_FAILURE;
  }
  char * line = NULL;
  size_t sz;
  char def[]="define";
  char eva[]="evaluate";
  while (getline(&line, &sz, f)>=0) {
    const char * strp = line;
    skipSpace(&strp);
    if(strp[0]=='d'){//define
     const char* which=strstr(strp,def);
      if(which==NULL){
        cout<<"wrong format."<<"\n";
        free(line);
        fclose(f);
        return EXIT_FAILURE;
      }
      else{
        strp+=6;
        string output(strp);
        skipSpace(&strp);
        string name;
        if(strstr(strp,"=")==NULL){
          cerr<<"missing ="<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        const char* content=strstr(strp,"=")+1;
        //string keepcontent(content);
        if(content==NULL){
          cerr<<"missing things after ="<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        string fromcontent(content);
        if(*strp!='('){
          cout<<"missing ("<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        else{
          strp+=1;
          skipSpace(&strp);
          while(!isspace(*strp)){
            name+=*strp;
            strp++;
          }
          map<string, vector<pair<string,string> > >::iterator itin;
          itin=function.find(name);
          if(itin!=function.end()){
            cerr<<"cannot redefine"<<endl;
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
          skipSpace(&strp);
          string left; // x y z)
          while(*strp!='='){
            left+=*strp;
            strp++;
          }
          // string initial;
          if(left.find(")")==string::npos){
            cout<<"missing )"<<endl;
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
          left.erase(left.find(")"));
          left+=" ";
          string var;
          size_t length;
          while(left.size()>0){
            length=left.find(" ");
            for(size_t i=0;i<length;i++){
              var+=left[i];
            }
            // cout<<"cut"<<var<<endl;
            function[name].push_back(make_pair(var,var));
            var.erase(0);
            left.erase(0,length+1);
          }
          //fromcontent=abs(name,&content);
          //cout<<"fromcontent:"<<fromcontent<<endl;
          //content=fromcontent.c_str();
          Expression* check=parse(&content,name,0);
          if(check==NULL){
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
          fromcontent=check->toString();
          // cout<<"fromcontent:"<<fromcontent<<endl;
          const char* from=fromcontent.c_str();
          delete check;
          check=parse(&from,name,1);
          if(check==NULL){
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
          fromcontent=check->toString();
          // cout<<"fromcontent:"<<fromcontent<<endl;
          delete check;
          // map<string, vector<pair<string,string> > >::iterator itin;//to initialize
          for(itin=function.begin();itin!=function.end();++itin){
            for(size_t i=0;i<itin->second.size();i++){
              itin->second[i].second=itin->second[i].first;
            }
          }
          todo.insert(pair<string,string>(name,fromcontent));
          cout<<"defined";
          size_t k=0;
          while(output[k]!=')'){
            cout<<output[k];
            k++;
          }
          cout<<output[k]<<endl;
        }
        // cout<<fromcontent<<endl;
      }
    }
    else if(strp[0]=='e'){  //evalute
      //cout<<"eva"<<endl;
      const char* which=strstr(strp,eva);
      if(which==NULL){
        cout<<"wrong format."<<"\n";
        free(line);
        fclose(f);
        return EXIT_FAILURE;
      }
      else{
        strp+=8;
        skipSpace(&strp);
        string output2(strp);
        if(*strp!='('){
          cout<<"missing (."<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        strp++;
        string evaname;
        while(*strp!=' '){
          evaname+=*strp;
          strp+=1;
        }
        //cout<<evaname<<endl;
        map<string,string>::iterator it;
        it=todo.find(evaname);
        if(it==todo.end()){
          cout<<"cannot find function"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        string what=it->second;//what to do with the function
        //cout<<"what"<<what<<endl;
        const char* fromwhat=what.c_str();
        //cout<<fromwhat;
        size_t varnum;//numer of variables
        map<string, vector<pair<string,string> > >::iterator it2;
        it2=function.find(evaname);
        varnum=it2->second.size();
        //cout<<varnum;
        skipSpace(&strp);
        vector<string> temp;
        while(*strp!=')'){
          string number;  //actual number of variables
          if(*strp=='('){
            strp++;
            number=multi(&strp);
            if(number=="R"){
              free(line);
              return EXIT_FAILURE;
            }
            while(*strp!=')'){
              strp++;
            }
            strp++;
          }
          else{
          while((*strp!=' ')&&(*strp!=')')){
            number+=*strp;
            strp++;
          }
          }
          //cout<<"first"<<number<<endl;
          // it2->second[i].second=number;
          temp.push_back(number);
          skipSpace(&strp);
        }
        skipSpace(&strp);
        if(temp.size()!=varnum){
          cerr<<"numbers cannot match variables"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        for(size_t i=0;i<varnum;i++){
          it2->second[i].second=temp[i];
        }
        Expression* expr=parse(&fromwhat,evaname,0);
        if(expr!=NULL){
          //std::cout << "Parsed expression to: " << expr->toString() << "\n";
          //std::cout << "Evaluated expression to: " << expr->evaluate() << "\n";
          size_t k=0;
          size_t j=0;
          while(output2[k]!='\n'){
            k++;
          }
          while(output2[k]!=')'){
            k--;
          }
          while(j<=k){
            cout<<output2[j];
            j++;
          }
          //expr->evaluate();
          cout<<" = "<<expr->evaluate()<<endl;
          delete expr;
        }
        else{
          std::cout << "Could not parse expression, please try again.\n";
        }
      }
    }
    else if(strp[0]=='n'){
      ///cout<<"num"<<endl;
      const char* where=strstr(strp,"numint");
      if(where==NULL){
        cout<<"wrong format."<<"\n";
        free(line);
        fclose(f);
        return EXIT_FAILURE;
      }
      else{
        strp+=6;
        skipSpace(&strp);
        string str(strp);
        string numname; //function name
        while(*strp!=' '){
          numname+=*strp;
          strp+=1;
        }
        map<string,string>::iterator it4;
        map<string, vector<pair<string,string> > >::iterator it5;
        it4=todo.find(numname);
        it5=function.find(numname);
        if(it4==todo.end()){
          cout<<"cannot find function"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        string numwhat=it4->second;
        size_t numofvar=it5->second.size();//the actual number of variables
        size_t numnum=2*(it5->second.size())+1;//size of vectors
        //cout<<numnum<<endl;
        const char* fromnumwhat=numwhat.c_str();
        vector<string> paras;//to keep record of the parameters
        vector<double> dparas(numnum);//convert string to double
        skipSpace(&strp);
        while(*strp!='\0'){
          string para; //to get every number in the command line
          while((*strp!=' ')&&(*strp!='\n')){
            para+=*strp;
            strp++;
          }
          if(para.size()==0){
            cout<<"less parameters than expected"<<endl;
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
          paras.push_back(para);
          // cout<<paras[i]<<endl;
          skipSpace(&strp);
        }
        if(paras.size()!=numnum){
          cout<<"less parameters than expected"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        for(size_t i=0;i<numnum;i++){
          char* endp;
          const char* temp=paras[i].c_str();
          dparas[i]=strtod(temp,&endp);
          // cout<<dparas[i]<<endl;
        }
        for(size_t i=1;i<=numofvar;i++){
          if(dparas[2*i-1]>=dparas[2*i]){
            cout<<"Invalid range"<<endl;
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
        }
        for(size_t i=0;i<numofvar;i++){
          it5->second[i].second=paras[2*i+1];
        }
        double sum=numint(dparas,&fromnumwhat,numofvar,numname);
        cout<<"Numerical Integration of ";
        size_t i=0;
        while(str[i]!='\n'){
          cout<<str[i];
          i++;
        }
        cout<<" is "<<sum<<endl;
      }
    }
      else if(strp[0]=='m'){
        //cout<<"mcint"<<endl;
      const char* mc=strstr(strp,"mcint");
      const char* ma=strstr(strp,"max");
      const char* mi=strstr(strp,"min");
      if((mc==NULL)&&(ma==NULL)&&(mi==NULL)){
        cout<<"wrong format."<<endl;
        fclose(f);
        return EXIT_FAILURE;
      }
      else if(mc!=NULL){
        strp+=5;
        skipSpace(&strp);
        string str(strp);
        string mcname; //function name
        while(*strp!=' '){
          mcname+=*strp;
          strp+=1;
        }
        map<string,string>::iterator it6;
        map<string, vector<pair<string,string> > >::iterator it7;
        it6=todo.find(mcname);
        it7=function.find(mcname);
        if(it6==todo.end()){
          cout<<"cannot find function"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        string mcwhat=it6->second;
        size_t mcofvar=it7->second.size();//the actual number of variables
        size_t mcnum=2*(it7->second.size())+1;//size of vectors
        //cout<<numnum<<endl;
        const char* frommcwhat=mcwhat.c_str();
        vector<string> mparas;//to keep record of the parameters
        vector<double> mdparas(mcnum+2);//convert string to double
        skipSpace(&strp);
        while(*strp!='\0'){
          string para; //to get every number in the command line
          while((*strp!=' ')&&(*strp!='\n')){
            para+=*strp;
            strp++;
          }
          if(para.size()==0){
            cout<<"less parameters than expected"<<endl;
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
          mparas.push_back(para);
          // cout<<paras[i]<<endl;
          skipSpace(&strp);
        }
        if(mparas.size()!=mcnum){
          cout<<"less parameters than expected"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        for(size_t i=0;i<mcnum;i++){
          char* endp;
          const char* temp=mparas[i].c_str();
          //cout<<"temp:"<<temp<<endl;
          mdparas[i]=strtod(temp,&endp);
          // cout<<dparas[i]<<endl;
        }
        for(size_t i=1;i<=mcofvar;i++){
          if(mdparas[2*i-1]>=mdparas[2*i]){
            cout<<"Invalid range in Monte Carlo Integration"<<endl;
            free(line);
            fclose(f);
            return EXIT_FAILURE;
          }
        }
        for(size_t i=0;i<mcofvar;i++){
          it7->second[i].second=mparas[2*i+1];
        }
        mchelper(mdparas,&frommcwhat,mcofvar,mcname);
        //cout<<"range size"<<range.size()<<endl;
        vector<double> mami=maandmi();
        //cout<<"max"<<mami[0]<<endl;
        //cout<<"min"<<mami[1]<<endl;
        mdparas[mcnum]=mami[1];
        mdparas[mcnum+1]=mami[0];
        range.clear();
        //cout<<range.size()<<endl;
        //cout<<mdparas[0]<<endl;
        size_t a=mdparas[0];
        vector<vector<string> > trials(a);
        for(size_t i=0;i<a;i++){
          trials[i].resize(mcofvar+2);
        }
        for(size_t i=1;i<=(mcofvar+1);i++){
          //srand(time(NULL));
          double ran;
          for(size_t j=0;j<a;j++){
            //srand(j);
            stringstream s;
            //cout<<"dividor:"<<mdparas[2*i]<<"-"<<mdparas[2*i-1]<<endl;
            //cout<<"j:"<<j<<endl;
            //cout<<"i:"<<i<<endl;
            ran=(rand()/(double)RAND_MAX)*(mdparas[2*i]-mdparas[2*i-1]);
            ran=ran+(int)mdparas[2*i-1];
            //cout<<"random:"<<ran<<endl;
            s<<ran;
            string ss=s.str();
            trials[j][i-1]=ss;
          }
        }
        Expression* mc;
        string origin(frommcwhat);
        const char* strp;
        for(size_t i=0;i<a;i++){
          stringstream s;
          string ss;
          strp=origin.c_str();
          for(size_t j=0;j<mcofvar;j++){
            it7->second[j].second=trials[i][j];
            // cout<<"trials[i][j]:"<<trials[i][j]<<endl;
          }
          mc=parse(&strp,mcname,0);
          //cout<<"evaluate:"<<mc->evaluate()<<endl;
          s<<mc->evaluate();
          ss=s.str();
          //cout<<"value:"<<ss<<endl;
          trials[i][mcofvar+1]=ss;
          delete mc;
        }
        double under=0;
        double above=0;
        double random=0;
        double calculate=0;
        double total=mdparas[0];
        double area=1;
        for(size_t i=1;i<=(mcofvar+1);i++){
          area=area*(mdparas[2*i]-mdparas[2*i-1]);
          //cout<<"area:"<<area<<endl;
        }
        //cout<<"area:"<<area<<endl;
        if(mami[1]>=0){
          for(size_t i=0;i<a;i++){
            char* endp;
            const char* temp;
            temp=trials[i][mcofvar].c_str();
            random=strtod(temp,&endp);
            temp=trials[i][mcofvar+1].c_str();
            calculate=strtod(temp,&endp);
            if(random<=calculate){
              under++;
            }
          }
        }
        else{
          for(size_t i=0;i<a;i++){
            char* endp;
            const char* temp;
            temp=trials[i][mcofvar].c_str();
            random=strtod(temp,&endp);
            temp=trials[i][mcofvar+1].c_str();
            calculate=strtod(temp,&endp);
            if((random>=0)&&(random<=calculate)&&(calculate>=0)){
              under++;
            }
            else if((random<0)&&(calculate<0)&&(random>=calculate)){
              above++;
            }
        }
        }
        double inter=0;//the result;
        inter=area*((under-above)/total);
        cout<<"Monte Carlo Integration of ";
        size_t i=0;
        while(str[i]!='\n'){
          cout<<str[i];
          i++;
        }
        cout<<" is "<<inter<<endl;
      }
      else{//command max or min
        strp+=3;
        skipSpace(&strp);
        string str(strp);
        string lastname;
        while(*strp!=' '){
          lastname+=*strp;
          strp+=1;
        }
        map<string,string>::iterator l1;
        map<string, vector<pair<string,string> > >::iterator l2;
        l1=todo.find(lastname);
        if(l1==todo.end()){
          cout<<"cannot find function"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        l2=function.find(lastname);
        string lastwhat=l1->second;
        size_t parameters=l2->second.size()+2; //number of the command except name
        vector<string> commands;
        vector<double> dcommands(parameters);
        skipSpace(&strp);
        while(*strp!='\0'){
          string para; //to get every number in the command line
          while((*strp!=' ')&&(*strp!='\n')){
            para+=*strp;
            strp++;
          }
          commands.push_back(para);
          skipSpace(&strp);
        }
        if(commands.size()!=parameters){
          cout<<"less parameters than expected"<<endl;
          free(line);
          fclose(f);
          return EXIT_FAILURE;
        }
        for(size_t i=0;i<parameters;i++){
          char* endp;
          const char* temp=commands[i].c_str();
          dcommands[i]=strtod(temp,&endp);
        }
        double gamma=dcommands[0];
        double converge=dcommands[1];
        vector<double> curr(parameters-2);
        vector<double> pnew(parameters-2);
        vector<double> gradorigin(parameters-2);
        for(size_t i=0;i<(parameters-2);i++){
          curr[i]=dcommands[i+2];
        }
        double result;
        double disorigin;
        gradorigin=gradient(curr,lastname,lastwhat);
        if(ma!=NULL){
          for(size_t i=0;i<curr.size();i++){
            pnew[i]=curr[i]+gamma*gradorigin[i];
          }
          disorigin=dist(curr,pnew);
          result=gradhelper(curr,pnew,gamma,converge,lastname,lastwhat,disorigin,1);
          cout<<"Max of ";
          size_t i=0;
          while(str[i]!='\n'){
            cout<<str[i];
            i++;
          }
          cout<<" is "<<result<<endl;
        }
        else{
          for(size_t i=0;i<curr.size();i++){
            pnew[i]=curr[i]-gamma*gradorigin[i];
          }
          disorigin=dist(curr,pnew);
          result=gradhelper(curr,pnew,gamma,converge,lastname,lastwhat,disorigin,0);
          cout<<"Min of ";
          size_t i=0;
          while(str[i]!='\n'){
            cout<<str[i];
            i++;
          }
          cout<<" is "<<result<<endl;
        }
      }
      }
    else{  //neither
      cout<<"wrong format."<<endl;
      free(line);
      fclose(f);
      return EXIT_FAILURE;
    }
  }
  if (fclose(f) != 0) {
    perror("Failed to close the input file!");
    free(line);
    return EXIT_FAILURE;
  }
  free(line);
  return EXIT_SUCCESS;
}
