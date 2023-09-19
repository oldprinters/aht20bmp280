#include <Arduino.h>
#include "FilterES.h"

//********************************************
FilterES::FilterES(int size, double Alpha = 0.1):nSize(size), Alpha(Alpha){
  arVect.resize(nSize);
  begin1 = it = arVect.begin();
  end1 = arVect.end();
}
//********************************************
double FilterES::FilterAnalog(double val){
  double sum = 0.;
  for(auto& itT:arVect)sum += itT;  //сумма всех элементов массива
  *it = Alpha * val + (1-Alpha) * sum / nSize;
  it++;
  if(it == end1) it = begin1;
  return *it;
}
//********************************************
double FilterES::getValue(){return *it;}