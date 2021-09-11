
#include <fstream>

using namespace std;

class Tokenizer
{
public:
  Tokenizer(char* t);        
  char* getToken(); 
  int getLineNumber(); 
  int getOffset(); 
  int getOffset2(); 
  bool getEof();
  int readInt();
  string readSym();
  char ReadIEAR();
  void __parseerror(int errcode);
private:
  char*  member;
  char test[800];
  bool set;
  int offset;
  int line_number;
  bool eof;
  FILE *myfile;
};