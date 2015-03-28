#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <string>
#include "tokens.h"

using namespace std;

/** Function to add a new token to the queue based off a value and type
  * @param string value 
  * @param queue<Token *> * pointer to a queue that holds tokens
  * @param enum TokenType type
  * @return void
  */
void add_token(queue<Token *> * token_queue, string value, TokenType type, int col, int row)
{
  Token * tmp = new Token;
  tmp->value = value;
  tmp->type = type;
  tmp->col_num = col;
  tmp->row_num = row;
  token_queue->push(tmp);
}

/**
* Function to parse through each character in a file and analyze if the 
* characters and following characters form a particular token
* @param char * filename (ie char array)
* @param queue<Token *> * queue of all the tokens found FIFO
*/
queue<Token *> * 
scanner(char * filename){
  string line;
  unsigned int i;
  unsigned int lineNum = 0;
  bool inMacro = false;
  queue<Token *> * token_queue = new queue<Token *>;

  ifstream input(filename, ifstream::in);
  while(getline(input, line))
  {
    for(i = 0; i < (line.size() + 1); i++)
    {
      if(inMacro)
      {
        if(i == line.size()){ 
          inMacro = false;
        }else if(line.compare(i, strlen(FOR_LOOP), FOR_LOOP) == 0){
          add_token(token_queue, "FOR", FOR_LOOP_STATEMENT, i, lineNum);
          i += (strlen(FOR_LOOP) - 1);
        }else if(line.compare(i, strlen(IF), IF) == 0){
          add_token(token_queue, "IF", IF_CONDITION, i, lineNum);
          i += (strlen(IF) -1);
        }else if(line.compare(i, strlen(ELSE), ELSE) == 0){
          add_token(token_queue, "ELSE", ELSE_CONDITION, i, lineNum);
          i += (strlen(ELSE) - 1);
        }else if(line[i] == LP){
          char ch[2] = {line[i], '\0'};
          add_token(token_queue, ch, OPEN_PAREN, i, lineNum);
        }else if(line[i] == RP){
          char ch[2] = {line[i], '\0'};
          add_token(token_queue, ch, CLOSE_PAREN, i, lineNum);
        }else if(line[i] == LB){
          char ch[2] = {line[i], '\0'};
          add_token(token_queue, ch, OPEN_BLOCK, i, lineNum);
        }else if(line[i] == RB){
          char ch[2] = {line[i], '\0'};
          add_token(token_queue, ch, CLOSE_BLOCK, i, lineNum);
        }else if(line[i] == SEMICOLON){
          char ch[2] = {line[i], '\0'};
          add_token(token_queue, ch, END_STATEMENT, i, lineNum);
        }else if(line[i] == STRING_QUOTE_DOUBLE){
          int start, end;
          i++;
          start = i;
          while(line[i] != STRING_QUOTE_DOUBLE){
             i++;
          }
          end = i;
          add_token(token_queue, line.substr(start, end - start), STRING, i, lineNum);
        }else if(line[i] <= NUMBER_NINE && line[i] >= NUMBER_ZERO){
          // Assumes numbers are not concatenated with letters or anything
          int start, end;
          start = i;
          while(line[i] <= NUMBER_NINE && line[i] >= NUMBER_ZERO){
            i++;
          }
          end = i;
          i--;
          add_token(token_queue, line.substr(start, end - start), NUMBER, i, lineNum);
        }else if(IS_SYMBOL(line[i])){
          int start, end;
          start = i;
          if(IS_SYMBOL(line[i+1])){
            i++;
          }
          end = i + 1;
         
          add_token(token_queue, line.substr(start, end - start), SYMBOL, i, lineNum); 
        }else if(line[i] != WS && line[i] != TAB){
          int start, end;
          start = i; 
          while(IS_VALID_IDENTIFIER(line[i])){
            i++;
          }
          end = i;
          i--;
          add_token(token_queue, line.substr(start, end - start), IDENTIFIER, i, lineNum);
        }
      }else{
        if(line.compare(i, strlen(OPEN_MACRO), OPEN_MACRO) == 0){
          i += (strlen(OPEN_MACRO) - 1);
          inMacro = true;
        }else if(line.size() > 0){
          int start, end;
          start = i;
          while(i < line.size()){
            if(i <= (line.size() - strlen(INLINE_OPEN)) && line.compare(i, strlen(INLINE_OPEN), INLINE_OPEN) == 0){
              end = i;
              if(end != start){  
                add_token(token_queue, line.substr(start, end - start), PRINT, i, lineNum);
              }
            
              add_token(token_queue, line.substr(i, strlen(INLINE_OPEN)), INLINE_START, i, lineNum);
              i += (strlen(INLINE_OPEN));

              while(line.compare(i, strlen(INLINE_CLOSE), INLINE_CLOSE)){
                if(line[i] != WS && line[i] != TAB){
                  int start, end;
                  start = i;
                  while(IS_VALID_IDENTIFIER(line[i])){
                    i++;
                  }
                  end = i;
                  add_token(token_queue, line.substr(start, end - start), IDENTIFIER, i, lineNum);
                }
                i++;
              }

              add_token(token_queue, line.substr(i, strlen(INLINE_CLOSE)), INLINE_END, i, lineNum);
              i += (strlen(INLINE_CLOSE)-1);
              start = i + 1;
            }
            i++;
          }
          end = i;
          if(end != start){  
            add_token(token_queue, line.substr(start, end - start), PRINT, i, lineNum);
          }
        }else{
          add_token(token_queue, "\n", PRINT, i, lineNum);
        }
      }
    }
    lineNum++;
  }
  add_token(token_queue, "Program End", EOP, -1, -1);
  return token_queue;
}


