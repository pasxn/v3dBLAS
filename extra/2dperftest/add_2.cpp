#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void add(Int rows, Int columns, Int::Ptr x, Int::Ptr y, Int::Ptr z) {
  Int n= rows*columns;
  For (Int i=0, i<n, i+=16)
    Int a = *x;
    Int b = *y;
    *z = a+b;
    x = x+16;
    y = y+16;
    z = z+16;
  End
}


int main(int argc, const char *argv[]) {

	int columns =5;
	int rows = 5;
	int size = rows*columns;
	int iterations = 5;
	
	Int::Array a(rows*columns);
  Int::Array b(rows*columns);
  Int::Array r(rows*columns);

  for (int i = 0; i < (rows*columns); i++) {
    a[i] = i;
    b[i] = i;
  }
  auto k = compile(add);

  for(int y = 0; y < iterations ; y++) {
    k.load(rows, columns, &a, &b, &r);
  }
  settings.process(k);
  
  for(int i=0;i<(rows*columns);i++){
  	printf("The value of results is %i\n",r[i]);
  
  }
  
  return 0;
  
}
