#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

/***********************************************************
 * check if a given string is a float, and return if the
 * float is bad.
 ***********************************************************/
bool isFloat(const char *s, float *dest) {
  if (s == NULL) {
    return false;
  }
  char *endptr;
  *dest = (float) strtod(s, &endptr);
  if (s == endptr) {
    return false; // no conversion
  }
  // Look at trailing text
  while (isspace((unsigned char ) *endptr))
    endptr++;
  return *endptr == '\0';
}

/***********************************************************
 * returns the target number and the inequalities
 ***********************************************************/
int targetNum(char *str, float *targetNum){
  int ineq = 0;
  int i = 0;
  // check for the inequalities
  while (str[i] == '=' || str[i] == '>' || str[i] == '<'){
    switch (str[i]){
    case '>':
      ineq += (ineq > 1 ? 0 : 4);
      break;
    case '<':
      ineq += (ineq > 1 ? 0 : 2);
      break;
    case '=':
      ineq += (ineq & 1 ? 0 : 1);
      break;
    default:
      break;
    }
    i++;
  }
  // default to equals
  if (ineq == 0){
    ineq = 1;
  }
  char *num = &str[i];
  if (isFloat(num, targetNum)){
    // Good float
  } else {
    // Bad float
    ineq = 0;
  }
  return ineq;
  /* !  0 000
   * =  1 001
   * <  2 010
   * <= 3 011
   * >  4 100
   * >= 5 101
   */
}

/***********************************************************
 * The Float Stack
 ***********************************************************/

struct Stack
{
    int top;
    unsigned capacity;
    char *array;
};

struct Stack *createStack(unsigned capacity){
    struct Stack *stack = (struct Stack*) malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (char*) malloc(stack->capacity * sizeof(char));
    return stack;
}

char isFull(struct Stack *stack)
{   return stack->top == stack->capacity - 1; }

char isEmpty(struct Stack *stack)
{   return stack->top == -1;  }

char top(struct Stack *stack)
{   return (isEmpty(stack) ? '\0' : stack->array[stack->top]);}

void push(struct Stack *stack, char item){
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}

char pop(struct Stack *stack){
    if (isEmpty(stack))
        return '\0';
    return stack->array[stack->top--];
}

/***********************************************************
 * Linked List for storing RPN output
 ***********************************************************/
typedef struct node{
  int which;
  float data;
  char dat;
  struct node *next;
} node;

typedef void (*callback)(node* data);

node* create(int which, float data, char dat, node* next){
  node* new_node = (node*)malloc(sizeof(node));
  if(new_node == NULL)
  {
    printf("Error creating a new node.\n");
    exit(0);
  }
  new_node->which = which;
  new_node->data = data;
  new_node->dat = dat;
  new_node->next = next;

  return new_node;
}

node* prepend(node* head, int which, float data, char dat){
  node* new_node = create(which,data,dat,head);
  head = new_node;
  return head;
}

node* append(node* head, int which, float data, char dat){
  /* if no head then prepend */
  if (head == NULL){
    return prepend(head, which, data, dat);
  }
  /* go to the last node */
  node* cursor = head;
  while(cursor->next != NULL)
    cursor = cursor->next;
  /* create a new node */
  node* new_node =  create(which,data,dat,NULL);
  cursor->next = new_node;
  return head;
}

node* insert_after(node *head, int which, float data, char dat, node* prev){
  if(head == NULL || prev == NULL)
    return NULL;
  /* find the prev node, starting from the first node*/
  node *cursor = head;
  while(cursor != prev)
    cursor = cursor->next;

  if(cursor != NULL)
  {
    node* new_node = create(which,data,dat,cursor->next);
    cursor->next = new_node;
    return head;
  }
  else
  {
    return NULL;
  }
}

node* insert_before(node *head, int which, float data, char dat, node* nxt){
  if(nxt == NULL || head == NULL)
    return NULL;

  if(head == nxt)
  {
    head = prepend(head,which,data,dat);
    return head;
  }

  /* find the prev node, starting from the first node*/
  node *cursor = head;
  while(cursor != NULL)
  {
    if(cursor->next == nxt)
      break;
    cursor = cursor->next;
  }

  if(cursor != NULL)
  {
    node* new_node = create(which,data,dat,cursor->next);
    cursor->next = new_node;
    return head;
  }
  else
  {
    return NULL;
  }
}

void traverse(node* head,callback f){
  node* cursor = head;
  while(cursor != NULL)
  {
    f(cursor);
    cursor = cursor->next;
  }
}

node* remove_front(node* head){
  if(head == NULL)
    return NULL;
  node *front = head;
  head = head->next;
  front->next = NULL;
  /* is this the last node in the list */
  if(front == head)
    head = NULL;
  free(front);
  return head;
}

node* remove_back(node* head){
  if(head == NULL)
    return NULL;

  node *cursor = head;
  node *back = NULL;
  while(cursor->next != NULL)
  {
    back = cursor;
    cursor = cursor->next;
  }

  if(back != NULL)
    back->next = NULL;

  /* if this is the last node in the list*/
  if(cursor == head)
    head = NULL;

  free(cursor);

  return head;
}

node* remove_any(node* head,node* nd){
  /* if the node is the first node */
  if(nd == head)
  {
    head = remove_front(head);
    return head;
  }

  /* if the node is the last node */
  if(nd->next == NULL)
  {
    head = remove_back(head);
    return head;
  }

  /* if the node is in the middle */
  node* cursor = head;
  while(cursor != NULL)
  {
    if((cursor->next = nd))
      break;
    cursor = cursor->next;
  }

  if(cursor != NULL)
  {
    node* tmp = cursor->next;
    cursor->next = tmp->next;
    tmp->next = NULL;
    free(tmp);
  }
  return head;
}

void dispose(node *head){
  node *cursor, *tmp;

  if(head != NULL)
    {
      cursor = head->next;
      head->next = NULL;
      while(cursor != NULL)
        {
          tmp = cursor->next;
          free(cursor);
          cursor = tmp;
        }
    }
}

void display(node* n){
  if(n != NULL){
    if (n->which == 0){
      printf("%f ", n->data);
    } else {
      printf("%c ", n->dat);
    }
  }
}

/***********************************************************
 * this function gets a float from a string and returns both
 * the float and a pointer to where the float ends in the
 * string
 ***********************************************************/
float getFloat(char *str, char **endPtr){
  float num = 0;
  float dec = 0;
  int i = 0;
  int neg = 1;
  // if the first character is '-' the float is negative
  if (str[0] == '-'){
    neg = -1;
    i++;
  }
  // go through everything that would come before a decimal
  // place and fill "num" with it.
  // this "pushing" everything in num when it encouters a
  // new digit, which it places in the ones place.
  while (i < strlen(str) && isdigit(str[i])){
    num *= 10;
    num += (str[i] - '0');
    i++;
  }
  // this does what the above function does, only in reverse
  // rather than pushing everything over by multiplying by
  // 10 it pushes just the last digit to the end
  if (str[i] == '.'){
    i++;
    float place = 0.1;
    while (i < strlen(str) && isdigit(str[i])){
      dec += (str[i] - '0') * place;
      place *= 0.1;
      i++;
    }
  }
  // give the position of the character right after the float
  *endPtr = &str[i];
  // add everything before and after the decimal, then apply
  // the correct sign
  return (num+dec) * neg;
}

/***********************************************************
 * roll the given dice
 ***********************************************************/

long random_at_most(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
    x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}

void findBest(int best, int bestNum, float input, float *bestNums){
  if (best == 1){
    // if the highest is considered the best
    float min = bestNums[0];
    int minPos = 0;
    for (int i = 0; i < bestNum; i++){
      if (bestNums[i] < min){
        min = bestNums[i];
        minPos = i;
      }
    }
    if (min < input){
      bestNums[minPos] = input;
    }
  } else if (best == -1){
    // if the lowest is considered the best
    float max = bestNums[0];
    int maxPos = 0;
    for (int i = 0; i < bestNum; i++){
      if (bestNums[i] > max){
        max = bestNums[i];
        maxPos = i;
      }
    }
    if (max > input){
      bestNums[maxPos] = input;
    }
  }
}

float roll(float num, float sides, int verb, int best, int bestNum){
  float total = 0;
  long roll = 0;
  best = (bestNum <= num ? best : 0);
  float bestNums[(int)bestNum];
  // loop through the number of dice
  for(int i = 0; i < num; i++){
    while (roll == 0){
      roll = random_at_most(sides);
    }
    if (verb == 1 && num > 1){
      printf("%ld", roll);
      if (i < num-1){
        printf("+");
      }else{
        printf("\n");
      }
    }
    // add roll to the array of the best
    if (best != 0 && i < bestNum){
      bestNums[i] = roll;
    } else if (best != 0) {
      findBest(best, bestNum, roll, bestNums);
    }
    total += roll;
    roll = 0;
  }
  // find the best
  if (best != 0){
    total = 0;
    for (int i = 0; i < bestNum; i++){
      total += bestNums[i];
    }
  }
  return total;
}

/***********************************************************
 * compute exponents
 ***********************************************************/

float expon(float bse, int pwr){
  if (pwr == 0){
    return 1;
  } else if (pwr == 1){
    return bse;
  }
  float tmp = expon(bse, pwr/2);
  tmp = tmp*tmp;
  if (pwr%2 == 0){
    return tmp;
  } else {
    if (pwr > 0){
      return bse*tmp;
    } else {
      return (tmp)/bse;
    }
  }
  return 0;
}

/***********************************************************
 * Takes in the die roll function, seperates it, and
 * executes the pieces.
 * d dice
 * ^ exponents
 * * multiplication
 * / division
 * + addition
 * - subtraction
 ***********************************************************/
/*
  For call-stack, for Shunting-yard
  For Prim's, for multi-thread
  For shortest path pathfinding
  We thank thee, Dijkstra
  Amen
 */
// equation, verbose, best (h/l), target number, inequality
float solve(char *eq, int verb, int best, int bestNum){
  char op;// operator variable
  float num;// number variable
  char lstChar = 'x';// stores the previous character. x is placeholder, \0 is number
  char *endPtr = eq;// for placeholding
  struct Stack* stack = createStack(strlen(eq));// operator Stack
  node *head = NULL;// the head of the linked list
  // This extracts the numers and operators and fills the
  // stack in reverse polish notation
  while(eq[0] != '\0'){
    if (lstChar != '\0'){
      num = getFloat(eq, &endPtr);
    }
    //printf("%s", eq);
    if (eq == endPtr && endPtr[0] != '\0'){
      op = endPtr[0];
      eq++;
      char oprtr;
      if (top(stack) == 'd' && op != '(' && lstChar != '\0' && lstChar != ')'){
        // if the last operator was a die, and the number
        // of sides wasn't chosen, then default to six sides
        head = append(head, 0, 6, '\0');
      }
      switch(op){
      case '(':
        push(stack, op);
        break;
      case ')':
        do{
          oprtr = pop(stack);
          if (oprtr != '(' && oprtr != '\0') {
            head = append(head, 1, 0, oprtr);
          }
        } while (oprtr != '(' && oprtr != '\0');
        // this handles (x)(y) = (x)*(y)
        if (endPtr[1] != '\0' && endPtr[1] == '('){
          endPtr[0] = '*';
          eq--;
        }
        break;
      case 'd':
        if (lstChar != ')' && lstChar != '\0'){
          // if the last get was an operator other than ')'
          // then put a 1 into the output queue
          head = append(head, 0, 1, '\0');
        }
        if (endPtr[1] == '\0'){
          // if the d is the last character in the string, put
          // a 6 in the output queue as the default number of
          // sides
          head = append(head, 0, 6, '\0');
        }
        oprtr = top(stack);
        while(oprtr == 'd'){
          head = append(head, 1, 0, pop(stack));
          oprtr = top(stack);
        }
        push(stack, op);
        break;
      case '^':
        oprtr = top(stack);
        while(oprtr == 'd'){
          head = append(head, 1, 0, pop(stack));
          oprtr = top(stack);
        }
        push(stack, op);
        break;
      case '*':
        oprtr = top(stack);
        while (oprtr == 'd' || oprtr == '^' || oprtr == '*' || oprtr == '/'){
          head = append(head, 1, 0, pop(stack));
          oprtr = top(stack);
        }
        push(stack, op);
        break;
      case '/':
        oprtr = top(stack);
        while (oprtr == 'd' || oprtr == '^' || oprtr == '*' || oprtr == '/'){
          head = append(head, 1, 0, pop(stack));
          oprtr = top(stack);
        }
        push(stack, op);
        break;
      case '+':
        oprtr = top(stack);
        while (oprtr == 'd' || oprtr == '^' || oprtr == '*' || oprtr == '/' || oprtr == '+' || oprtr == '-'){
          head = append(head, 1, 0, pop(stack));
          oprtr = top(stack);
        }
        push(stack, op);
        break;
      case '-':
        oprtr = top(stack);
        while (oprtr == 'd' || oprtr == '^' || oprtr == '*' || oprtr == '/' || oprtr == '+' || oprtr == '-'){
          head = append(head, 1, 0, pop(stack));
          oprtr = top(stack);
        }
        push(stack, op);
        break;
      default:
        // do nothing
        break;
      }
      lstChar = op;
    } else {
      //handles (x)y = x*y
      char oprtr = top(stack);
      if (lstChar == ')'){
        while (oprtr == 'd' || oprtr == '^' || oprtr == '*' || oprtr == '/'){
          head = append(head, 1, 0, pop(stack));
          oprtr = top(stack);
        }
        push(stack, '*');
      }
      // push the number into the queue
      head = append(head, 0, num, '\0');
      // this handles x(y) = x*(y)
      if (endPtr[0] != '\0' && endPtr[0] == '('){
        endPtr--;
        endPtr[0] = '*';
      }
      eq = endPtr;
      lstChar = '\0';
    }
  }
  // pop the remaining operators from the stack
  // append the operators to the list
  char oprtr;
  do{
    oprtr = pop(stack);
    if (oprtr != '\0'){
      head = append(head, 1, 0, oprtr);
    } else {
      break;
    }
  } while(oprtr != '\0');
  // print out the list
  // solve the equation
  node *eqHead = NULL;// the head of the equation list
  do{
    if (head == NULL){
      break;
    }
    if (head->which == 0){
      eqHead = prepend(eqHead, 0, head->data, '\0');
      head = remove_front(head);
    } else {
      // I know I'm using a linked list like a stack. I don't care!
      float num2 = eqHead->data;
      eqHead = remove_front(eqHead);
      float num1 = eqHead->data;
      eqHead = remove_front(eqHead);
      float ans = 0;
      switch(head->dat){
      case 'd':
        ans = roll(num1, num2, verb, best, bestNum);
        break;
      case '^':
        ans = expon(num1, (int) num2);
        break;
      case '*':
        ans = num1 * num2;
        break;
      case '/':
        ans = num1 / num2;
        break;
      case '+':
        ans = num1 + num2;
        break;
      case '-':
        ans = num1 - num2;
        break;
      default:
        break;
      }
      eqHead = prepend(eqHead, 0, ans, '\0');
      head = remove_front(head);
    }
  } while (head != NULL);
  return eqHead->data;
}

/***********************************************************
 * The main function. Reads arguments.
 ***********************************************************/
int main(int argc, char *argv[]){
  if (argc >= 2){
    // Obligitory handling of "version" and "help"
    if (!strcmp(argv[1], "--version")){
      printf("Open Dice 0.1\n");
      printf("Copyright (C) 2016 TheGreatS\n");
      printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
      printf("This is free software: you are free to change and redistribute it.\n");
      printf("There is NO WARRANTY, to the extent permitted by law.\n");
      // TODO get this information from an external file
      return 1;
    } else if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")){
      printf("Print Help File\n");
      // TODO print actual help file
      return 1;
    }
    // Store potential die rolls
    int dice[argc];
    for (int i = 0; i < argc; i++){
      dice[i] = 1;
    }
    // Check for relevant tags
    int verbose = 0;
    int best = 0;
    int bestNum = 0;
    int ineq = 0;
    int mult = 1;
    int heads = 0;
    int tails = 0;
    float target = 0;
    for (int i = 1; i < argc; i++){
      // check for tags
      if (!strcmp(argv[i], "--verbose") || !strcmp(argv[i], "-v")){
        dice[i] = 0;
        verbose = 1;
        continue;
      } else if (!strcmp(argv[i], "--target") || !strcmp(argv[i], "-t")){
        char *targetNumChar = (argv[i+1] != NULL ? argv[i+1] : "a");
        // set the potential dice
        dice[i+1] = 0;
        dice[i] = 0;
        ineq = targetNum(targetNumChar, &target);
        if (ineq == 0){
          printf("ERROR: target number not found\n");
          continue;
        }
        continue;
      } else if (!strcmp(argv[i], "--best") || !strcmp(argv[i], "-b")){
        char *bestArg = argv[i+1];
        // set the potential dice
        dice[i+1] = 0;
        dice[i] = 0;
        bestNum = 1;
        if (argv[i+1] == NULL){
          best = 0;
        } else {
          switch(bestArg[0]){
          case 't':
            best = 1;// get the top
            break;
          case 'b':
            best = -1;// get the bottom
            break;
          default:
            best = 0;// use the default
            break;
          }
        }
        if (best == 0){
          float bestFloat = 0;
          bool exists = isFloat(&bestArg[0], &bestFloat);
          bestNum = (exists ? (int)bestFloat : 1);
        } else {
          float bestFloat = 0;
          bool exists = isFloat(&bestArg[1], &bestFloat);
          bestNum = (exists ? (int)bestFloat : 1);
        }
        continue;
      } else if (!strcmp(argv[i], "--multiple") || !strcmp(argv[i], "-m")){
        char *multiArg = argv[i+1];
        dice[i+1] = 0;
        dice[i] = 0;
        if (argv[i+1] == NULL){
          mult = 1;
        } else {
          float multiple = 0;
          bool isNum = isFloat(&multiArg[0], &multiple);
          mult = (isNum ? (int)multiple : 1);
        }
        continue;
      } // IF ELSE CHAIN END
    }// FOR LOOP END
    // use the given equation
    int hasEq = 0;// was an equation provided?
    float ans = 0;
    float total = 0;
    // seed the random number generator using the current time
    srandom(time(NULL));// TODO find a better way to do this
    // iterate for the number of multiples
    int success = 0;
    for (int j = 0; j < mult; j++){
      // iterate through all the arguments to find equations
      for (int i = 1; i < argc; i++){
        if (dice[i] == 1){
          hasEq = 1; // there is an equation in the given input
          // check if it's a coin or die
          if (!strcmp(argv[i], "coin")){
            dice[i+1] = 0;
            float coinNum = 0;
            bool isNum = isFloat(&argv[i+1][0], &coinNum);
            if (argv == NULL || !isNum){
              coinNum = 1;
            }
            for (int i = 0; i < coinNum; i++){
              long side = random_at_most(1);
              if (side == 1){
                heads++;
                if (verbose == 1 && (coinNum > 1 || mult > 1))
                  printf("heads");
              } else {
                tails++;
                if (verbose == 1 && (coinNum > 1 || mult > 1))
                  printf("tails");
              }
              if (i < coinNum-1 && verbose == 1){
                printf("\n");
              }
            }
          } else {
            ans = solve(argv[i], verbose, best, bestNum);
          }
          success = 0;
          if (verbose == 1 && heads+tails == 0){
            printf("%f", ans);
          }
          if (ineq != 0){
            switch (ineq){
            case 1:// equal to
              if (ans == target){
                success = 1;
              }
              break;
            case 2:// less than
              if (ans < target){
                success = 1;
              }
              break;
            case 3:// less than or equal to
              if (ans <= target){
                success = 1;
              }
              break;
            case 4:// greater than
              if (ans > target){
                success = 1;
              }
              break;
            case 5:// greater than or equal to
              if (ans >= target){
                success = 1;
              }
              break;
            default:
              break;
            }
            total += success;
            if (verbose == 1){
              printf(": %s",(success == 0 ? "failure" : "success"));
            }
          } else if (ineq == 0 || verbose == 1) {
            total += ans;
          }// Inequality comparison end
          if (verbose == 1 && (heads+tails > 1 || mult > 1)){
            printf("\n");
          }
        }
      }// ARGUMENT LOOP END
      // default to 1d6
      if (hasEq == 0){
        ans = solve("1d6", verbose, best, bestNum);
        total += ans;
        printf("%f\n", ans);
      }
    }// MULT LOOP END
    // print the results
    if (mult > 1){
      if (heads == 0 && tails == 0){
        printf("total%s %f\n", (ineq != 0 ? " success:" : ":") , total);
      } else {
        printf("heads: %d\ntails: %d\n", heads, tails);
      }
    } else if (ineq == 0 && verbose == 0 && heads+tails == 0) {
      printf("%f\n", total);
    } else if (ineq != 0 && verbose == 0){
      printf("%s\n", (total > 0 ? "success" : "failure"));
    } else if (heads > 0 || tails > 0){
      if (heads + tails > 1){
        printf("heads: %d\ntails: %d\n", heads, tails);
      } else if (heads == 1){
        printf("heads\n");
      } else if (tails == 1){
        printf("tails\n");
      }
    }
  } else {
    // default to rolling 1d6
    srandom(time(NULL));
    float ans = solve("1d6", 0, 0, 0);
    printf("%f\n", ans);
  }
  return 1;
}
