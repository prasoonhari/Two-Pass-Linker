#include "Tokenizer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

char line[1024];
string nextline = "\n";
char const *sep = " \n\t";
char *word, *phrase, *phrase_2, *brkt, *brkb;
char *buffer;
int offset_2;
int current_buffer_length = -1;
size_t linecap = 0;

Tokenizer::Tokenizer(char *t)
{
  myfile = fopen(t, "r");
  if(myfile==NULL){
    cout<<"Not a valid inputfile <"<<t<<">\n";
    exit(1);
  }
  set = false;
  eof = false;
  offset = 1;
  line_number = 1;
}

char *Tokenizer::getToken()
{
  if (!set)
  {
    set = true;
    current_buffer_length = getline(&buffer, &linecap, myfile);
    if (current_buffer_length != -1)
    {
      word = strtok_r(buffer, sep, &brkt);
      if (word == NULL)
      {
        offset = current_buffer_length;
        offset_2 = current_buffer_length;
        while (word == NULL)
        {
          current_buffer_length = getline(&buffer, &linecap, myfile);
          if (current_buffer_length != -1)
          {
            line_number = line_number + 1;
            offset = 1;
            offset_2 = (unsigned)strlen(buffer);
            word = strtok_r(buffer, sep, &brkt);
            if (word != NULL)
            {
              offset = word - buffer + 1;
              offset_2 = offset + (unsigned)strlen(word);
              return word;
            }
          }
          else
          {
            fclose(myfile);
            eof = true;
            return NULL;
          }
        }
      }
      else
      {
        offset = word - buffer + 1;
        offset_2 = offset + (unsigned)strlen(word);
        return word;
      }
    }
    else
    {
      fclose(myfile);
      eof = true;
      return NULL;
    }
  }
  else
  {
    word = strtok_r(NULL, sep, &brkt);
    if (word == NULL)
    {
      offset = current_buffer_length;
      offset_2 = current_buffer_length;
      while (word == NULL)
      {
        current_buffer_length = getline(&buffer, &linecap, myfile);
        if (current_buffer_length != -1)
        {
          line_number = line_number + 1;
          offset = 1;
          offset_2 = (unsigned)strlen(buffer);
          word = strtok_r(buffer, sep, &brkt);
          if (word != NULL)
          {
            offset = word - buffer + 1;
            offset_2 = offset + (unsigned)strlen(word);
            return word;
          }
        }
        else
        {
          fclose(myfile);
          eof = true;
          return NULL;
        }
      }
    }
    else
    {
      offset = word - buffer + 1;
      offset_2 = offset + (unsigned)strlen(word);
      return word;
    }
  }
}

int Tokenizer::getLineNumber()
{
  return line_number;
}

int Tokenizer::getOffset()
{
  return offset;
}

int Tokenizer::getOffset2()
{
  return offset_2;
}

bool Tokenizer::getEof()
{
  return eof;
}

int Tokenizer::readInt()
{
  char *int_word = getToken();
  if (int_word != NULL)
  {
    for (phrase = int_word; *phrase != '\0'; phrase++)
    {
      if (isdigit(*phrase))
      {
      }
      else
      {
        __parseerror(0);
        exit(1);
      }
    }
    if (atoi(int_word) >= 1073741824 || atoi(int_word)<0)
    {
      __parseerror(0);
      exit(1);
    }
    return atoi(int_word);
  }
  else
  {
    return -1;
  }
}
string Tokenizer::readSym()
{

  char *char_word = getToken();
  if (char_word != NULL && *char_word != '\0')
  {
    
    phrase_2 = char_word;
    if (isalpha(*phrase_2))
    {
      phrase_2++;
    }
    else
    {
      __parseerror(1);
      exit(1);
    }
    for (phrase_2; *phrase_2 != '\0'; phrase_2++)
    {
      if (!isalnum(*phrase_2))
      {
        __parseerror(1);
        exit(1);
      }
    }
    int len = (unsigned)strlen(char_word);
    if (len > 16)
    {
      __parseerror(3);
      exit(1);
    }
    return char_word;
  }
  else
  {
    offset = offset_2;
    __parseerror(1);
    exit(1);
  }
}

char Tokenizer::ReadIEAR()
{
  char *iear_word = getToken();
  if (iear_word != NULL)
  {
    if (*iear_word == 'I' || *iear_word == 'E' || *iear_word == 'A' || *iear_word == 'R')
    {
      return iear_word[0];
    }
    else
    {
      __parseerror(2);
      exit(1);
    }
  }
  else
  {
    offset = offset_2;
    __parseerror(2);
    exit(1);
  }
}

void Tokenizer::__parseerror(int errcode)
{
  static const char *errstr[] = {
      "NUM_EXPECTED",
      "SYM_EXPECTED",
      "ADDR_EXPECTED",
      "SYM_TOO_LONG",
      "TOO_MANY_DEF_IN_MODULE",
      "TOO_MANY_USE_IN_MODULE",
      "TOO_MANY_INSTR",
  };
  printf("Parse Error line %d offset %d: %s\n", line_number, offset, errstr[errcode]);
}
