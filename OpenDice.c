#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// takes in a number of dice, type of dice, and returns the total
// as well as what each individual roll was
int roll(int num, int type, int *rolls)
{
	int total = 0;
	int i;
	printf("%dd%d\n", num, type); // display the roll
	for (i = 0; i < num; i++){ // roll the given number of dice the given number of times
		rolls[i] = rand()%(type-1) + 1; // roll the die
		printf("%d", rolls[i]); // display the roll
		printf(i == num-1 ? "\n" : "+"); // display the "+" between numbers, but "\n" at the end 
		total += rolls[i]; // add that roll to the running total for display later
	}
	return total;
}

// multiply a number by itself a number of times
int exponentiate(int bse, int xpnt) // base and exponent
{
	int total = 1;
	int i;
	for (i = 0; i < xpnt; i++){
		total *= bse;
	}
	return total;
}

// get the length of an integer
int get_int_len (int val)
{
	int l = 1;
	while (val > 9){
		l++;
		val/=10;
	}
	return l;
}

// returns the numbers on either side of a character
char* find_bookend_numbers(int nums[], char symbol, char *str)
{
	char *p = str; // for iterating through
	char *q = str; // same as above
	char *start = str; // the position to return from
	int found = 0; //whether or not the symbol is found
	int dgts = 0; // how many digits are foud

	while(*q){// while there are more characters to find
		if (*q == symbol){ // if the symbol is found
			found = 1; // mark the symbol as found
			break; // exit the loop
		} else if (!isdigit(*q)){ // if some other symbol is found
			p = q; // begin the search for the bookend numbers from this character
			start = q; // mark that position as the posititon to return to
			q++; // advance to the next character
		} else {
			q++; // advance to the next character
		}
	}
	if(!isdigit(*start)){
		start++;
	}
	if (found == 0){ // The symbol isn't found...
		nums[0] = -1; // ...so set the return values to -1
		return start; // and exit the function
	}
	while(*p && dgts < 2){ // while there are more characters but before there are two numbers
		if (isdigit(*p)){ // upon finding a digit
			long val = strtol(p, &p, 10); // read a number
			nums[dgts] = val; // and store it in the array
			dgts++;
		} else { // otherwise advance to the next character
			p++;
		}
	}
	return start;
}

// takes in a pointer to where to start, how many characters to replace, and what to replace it with
void str_replace(char* start, int end, char* rep){
	char *buffer = start + end; // set the buffer to the end of the string
	int i;
	// go through and replace each character
	for (i = 0; i < end; i++){
		if (!*rep){ // leave the loop before the null character is added to the string
			break;
		}
		*start = *rep;
		start++;
		rep++;
	}
	// add the the previously saved end of the string back to itself, but this time a little eairlier
	do{
		*start = *buffer;
		start++;
		buffer++;
	}while(*buffer);
	*start = '\0';
}

int main(int argc, char *argv[])
{
	int nums[2]; // number on either side of input
	int num; // number of dice
	int type; // type of dice
	int ans; // the result of some function
	int indi; // display individual rolls
	char *start; // where the specific piece of a function starts
	char *command; // the roll the user enters
	srand(time(NULL));
	if (argc == 2){
		command = malloc(strlen(argv[1])); // allocate some memory for the command
		strcpy(command, argv[1]);
		// run code as normal
	} else if (argc > 2) {
		int i;
		command = malloc(strlen(argv[1])); // allocate some memory for the command
		strcpy(command, argv[1]);
		// TODO fix below loop for detecting tags
		for (i = 2; i < argc; i++){
			if (strcmp(argv[i], "-i") != 0){
				printf(argv[i]);
			}
		}
		printf("\n");
		// TODO combine arguments together
		//return 0;
	} else {// cannot run without input
		printf("Error: No Input\n");
		return 1;
	}
	// Order of operations! Randy eats many apples
	// roll, exponent, multiply, add
	// in this case the order of operations are hardcoded
	/*
	 * roll
	 */
	while(1){
		// find the numbers on either side of the given character
		start = find_bookend_numbers(nums, 'd', command);
		num = nums[0];
		type = nums[1];
		if (nums[0] == -1){
			break;
		}
		int rolls[num];
		ans = roll(num, type, rolls); // roll the dice
		char ans_str[get_int_len(ans)];
		sprintf(ans_str, "%d", ans);
		str_replace(start, get_int_len(nums[0])+get_int_len(nums[1]) + 1, ans_str);
		// refortmat the answer so the command can be reused
		printf("%d\n", ans);
		printf("\n");
	}
	/*
	 * multiply
	 */
	while(1){
		start = find_bookend_numbers(nums, '*', command);
		num = nums[0];
		type = nums[1];
		if (nums[0] == -1){
			break;
		}
		int rolls[num];
		ans = num * type;
		char ans_str[get_int_len(ans)];
		sprintf(ans_str, "%d", ans);
		str_replace(start, get_int_len(nums[0])+get_int_len(nums[1])+1, ans_str);
	}
	/*
	 * add
	 */
	while(1){
		start = find_bookend_numbers(nums, '+', command);
		num = nums[0];
		type = nums[1];
		if (nums[0] == -1){
			break;
		}
		int rolls[num];
		ans = num + type;
		char ans_str[get_int_len(ans)];
		sprintf(ans_str, "%d", ans);
		str_replace(start, get_int_len(nums[0])+get_int_len(nums[1])+1, ans_str);
	}
	printf(command);
	realloc(command, 0); // free the memory
	return 0;
}
