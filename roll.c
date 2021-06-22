#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <math.h>
#include "graph.h"
#include "blumblumshub.h"

#define ISNUM(index) arg[index] >= '0' && arg[index] <= '9'
#define ISNUMEXTENDED(index) (arg[index] >= '0' && arg[index] <= '9') || \
                             arg[index] == '.'
#define ISINEQ(index) arg[index] == '=' || arg[index] == '<' || \
                      arg[index] == '!' || arg[index] == '>'
#define ISOPERATOR(index) arg[index] == '+' || arg[index] == '-' || \
                          arg[index] == '*' || arg[index] == '/' || \
                          arg[index] == 'd' || arg[index] == 'c' || \
                          arg[index] == '(' || arg[index] == ')' || \
                          arg[index] == '!' || arg[index] == '^'
#define MAXEQUATIONNUMSIZE 256
#define MAXGRAPHBARSIZE 10

const char *argp_program_version = "OpenDice 0.2";
static char doc[] = "Documentation";
static char args_doc[] = "EQUATION";

/***********************************************************
 * argument handling
 ***********************************************************/
static struct argp_option options[] = {
    {"verbose",  'v', 0, 0, "Produce verbose output"},
    {"best",     'b', "TYPENUM", OPTION_ARG_OPTIONAL, 
        "Keep the best NUM rolls in an equation and set the rest to 0. A TYPE, h (highest) or l (lowest), and NUM of rolls to select can be specified, in that order. TYPE is h and NUM is 1 by default"},
    {"target",   't', "[INEQUALITY]NUM", 0, 
        "Compare the result against NUM using INEQUALITY. INEQUALITY is '=' by default"},
    {"multiple", 'm', "NUM", 0, "Repeat the given equations NUM times"},
    {"graph", 'g', "INEQUALITY", OPTION_ARG_OPTIONAL,
        "Graph the probability of every possible result. INEQUALITY is '=' by default meaning the probability a roll is equal to a given result"},
    {"round", 'r', "TYPE", OPTION_ARG_OPTIONAL, 
        "Round the final result to the nearest integer. TYPE is what direction to round, (u)p, (d)own, (c)losest. Defaults to closest."},
    {0}
};

int str_to_ineq(char *arg, char *ineq) {
    int i = 0;
    while (ISINEQ(i)) {
        ineq[i] = arg[i];
        i++;
        if (i > 4)
            break;
    }
    ineq[i] = '\0';
    return i;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'v': {
            arguments->verbose = 1;
        } break;
        case 'b': {
            arguments->best = 1;
            if (arg == 0)
                break;
            int len = strlen(arg);
            if (len == 1) {
                if (arg[0] == 'b') {
                    arguments->best_type = 'b';
                } else if (arg[0] == 'l') {
                    arguments->best_type = 'l';
                } else if (ISNUM(0)) {
                    arguments->best_num = atoi(arg);
                }
            } else if (len > 1) {
                int type_len = 0;
                if (arg[0] == 'b') {
                    type_len = 1;
                    arguments->best_type = 'b';
                } else if (arg[0] == 'l') {
                    type_len = 1;
                    arguments->best_type = 'l';
                }
                if (ISNUM(type_len)) {
                    arguments->best_num = atoi(arg+type_len);
                }
            }
        } break;
        case 't': {
            arguments->target = 1;
            int len = strlen(arg);
            if (len == 1 && ISNUM(0)) {
                arguments->target_num = atof(arg);
            } else if (len > 1) {
                int ineq_len = 0;
                if (ISINEQ(0)) {
                    ineq_len = str_to_ineq(arg, arguments->target_inequality);
                }
                if (ISNUM(ineq_len)) {
                    arguments->target_num = atof(arg+ineq_len);
                }
            } else {
                return EINVAL;
            }
        } break;
        case 'm': {
            arguments->multiple = 1;
            arguments->multiple_num = atoi(arg);
        } break;
        case 'g': {
            arguments->graph = 1;
            if (arg != 0){
                if (ISINEQ(0)) {
                    str_to_ineq(arg, arguments->graph_inequality);
                }
            }
        } break;
        case 'r': {
            arguments->round = 1;
            if (arg == 0)
                break;
            if (arg[0] != 'u' && arg[0] != 'd' && arg[0] != 'c')
                return EINVAL;
            arguments->round_type = arg[0];
        } break;
        case ARGP_KEY_ARG: {
            if (state->arg_num >= 1)
                argp_usage(state);
            arguments->equation = arg;
        } break;
        case ARGP_KEY_END: {
            if (state->arg_num < 1)
                argp_usage(state);
        } break;
        default: {
        } return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

/***********************************************************
 * Equation parsing
 ***********************************************************/
void equation_count(char *arg, int *operator_count, int *number_count) {
    *operator_count = 0;
    *number_count = 0;

    int i = 0;
    while (arg[i] != '\0') {
        if (ISNUMEXTENDED(i)) {
            *number_count += 1;
            while (ISNUMEXTENDED(i))
                i++;
        } else if (ISOPERATOR(i)) {
            *operator_count += 1;
            i++;
        } else {
            i++;
        }
    }
}
int precedence(char *op) {
    switch(*op) {
    case '+':
        return 0;
    case '-':
        return 0;
    case '*':
        return 1;
    case '/':
        return 1;
    case 'd':
        return 3;
    case 'c':
        return 3;
    case '^':
        return 2;
    case '!':
        return 2;
    case 'n': // negation
        return 4;
    case 'p': // unary operator that leaves it's operand unchanged
        return 4;
    default:
        return 0;
    }
}
void display_equation(Equation *equation) {
    int num_count = 0;
    printf("Equation: ");
    for (int i = 0; i < equation->op_count; i++) {
        if (equation->operators[i] == '.') {
            printf("%f ", equation->numbers[num_count++]);
        } else {
            printf("%c ", equation->operators[i]);
        }
    }
    printf("\n");
}
Equation *parse_equation(struct arguments *arguments) {
    Equation *equation = malloc(sizeof(Equation));
    int operator_count;
    int number_count;

    equation_count(arguments->equation, &operator_count, &number_count);
    if (arguments->verbose) {
        printf("Operators: %d\nNumbers: %d\n", operator_count, number_count);
    }
    equation->operators = malloc(sizeof(char*) * (operator_count + number_count));
    equation->numbers = malloc(sizeof(double*) * (number_count));
    equation->op_count = 0;
    equation->num_count = 0;

    // shunting-yard algorithm
    int i = 0;
    char *arg = arguments->equation;
    char *op_stack = malloc(sizeof(char*) * operator_count);
    int stack_top = -1;
    while (arg[i] != '\0') {
        if (ISNUMEXTENDED(i)) {
            char num_str[MAXEQUATIONNUMSIZE];
            int j = 0;
            while (ISNUMEXTENDED(i))
                num_str[j++] = arg[i++];
            num_str[j] = '\0';
            equation->numbers[equation->num_count++] = atof(num_str);
            equation->operators[equation->op_count++] = '.';
        } else if (ISOPERATOR(i)) {
            if (arg[i] == '-' && (i == 0 || (ISOPERATOR(i-1)))) {
                arg[i] = 'n';
            } else if (arg[i] == '+' && (i == 0 || (ISOPERATOR(i-1)))) {
                arg[i] = 'p';
            } else if (arg[i] == '(' && i != 0 && ((ISNUMEXTENDED(i-1)) || arg[i-1] == ')')) {
                op_stack[++stack_top] = '*';
            } else if (arg[i] == 'd') {
                if (i == 0 || (i > 0 && !(ISNUMEXTENDED(i-1)) && arg[i-1] != ')')) {
                    equation->numbers = realloc(
                            equation->numbers, sizeof(double*) * (equation->num_count + 1));
                    equation->numbers[equation->num_count++] = 1;
                    equation->operators[equation->op_count++] = '.';
                }
                if (!(ISNUMEXTENDED(i+1)) && arg[i+1] != '(') {
                    equation->numbers = realloc(
                            equation->numbers, sizeof(double*) * (equation->num_count + 1));
                    equation->numbers[equation->num_count++] = 6;
                    equation->operators[equation->op_count++] = '.';
                }
            }
            if (stack_top < 0) {
                op_stack[++stack_top] = arg[i++];
            } else {
                if (arg[i] == '(') {
                    op_stack[++stack_top] = arg[i++];
                } else if (arg[i] == ')') {
                    while (stack_top >= 0 && op_stack[stack_top] != '(')
                        equation->operators[equation->op_count++] = op_stack[stack_top--];
                    if (op_stack[stack_top] == '(')
                        stack_top--;
                    if (ISNUMEXTENDED(i+1))
                        op_stack[++stack_top] = '*';
                    i++;
                } else {
                    while (stack_top >= 0) {
                        int stack_precedence = precedence(&op_stack[stack_top]);
                        int arg_precedence = precedence(&arg[i]);
                        if (((stack_precedence == arg_precedence && op_stack[stack_top] != '!' &&
                              op_stack[stack_top] != '^' && op_stack[stack_top] != 'c') ||
                              stack_precedence > arg_precedence) &&
                            op_stack[stack_top] != '(') {
                            equation->operators[equation->op_count++] = op_stack[stack_top--];
                        } else {
                            break;
                        }
                    }
                    op_stack[++stack_top] = arg[i++];
                }
            }
        } else {
            i++;
        }
    }
    while (stack_top >= 0)
        equation->operators[equation->op_count++] = op_stack[stack_top--];
    free(op_stack);

    if (arguments->verbose) {
        display_equation(equation);
    }

    return equation;
}

/***********************************************************
 * roll some type and number of dice
 ***********************************************************/
int partition(double *list, int lo, int hi) {
    double pivot = list[hi];
    int i = lo;
    for (int j = lo; j <= hi; j++) {
        if (list[j] < pivot) {
            double hold = list[j];
            list[j] = list[i];
            list[i] = hold;
            i++;
        }
    }
    double hold = list[hi];
    list[hi] = list[i];
    list[i] = hold;
    return i;
}
void quicksort(double *list, int lo, int hi) {
    if (lo < hi) {
        int p = partition(list, lo, hi);
        quicksort(list, lo, p - 1);
        quicksort(list, p + 1, hi);
    }
}
double roll(double count, double die, int coin, struct arguments *arguments) {
    double *rolls = malloc(sizeof(double*) * ceil(count));
    double result = 0;
    double count_int;
    double die_int;
    double count_frac = modf(count, &count_int);
    modf(die, &die_int);
    if (arguments->verbose) {
        if (coin)
            printf("\nFlipping %f coin%s\n", count, count == 1 ? "" : "s");
        else
            printf("\nRolling %f %d-sided di%se\n", count, (int) die_int,
                   count == 1 ? "" : "c");
    }

    for (int i = 0; i < count_int; i++) {
        rolls[i] = !coin + (rand_num() % (long) die_int);
        if (arguments->verbose) {
            if (coin)
                printf("Flip %d: %s\n", i+1, rolls[i] ? "heads" : "tails");
            else
                printf("Roll %d: %f\n", i+1, rolls[i]);
        }
    }
    if (count_frac > 0) {
        rolls[(int) floor(count)] = (!coin + rand_num() % (long) die_int) * count_frac;
    }

    int start = 0;
    int end = ceil(count)-1;
    if (arguments->best) {
        if (arguments->verbose)
            printf("Sorting %ss\n", coin ? "flip" : "roll");
        quicksort(rolls, start, end);
        if (arguments->verbose)
            printf("Selecting the %d %s %ss\n", arguments->best_num, 
                    arguments->best_type == 'h' ? "highest" : "lowest",
                    coin ? "flip" : "roll");
        switch(arguments->best_type) {
        case 'h': {
            start = end - (arguments->best_num - 1);
        } break;
        case 'l': {
            end = start + (arguments->best_num - 1);
        } break;
        default:
            break;
        }
    }

    if (arguments->verbose)
        printf("total = ");
    for (int i = start; i <= end; i++) {
        if (arguments->verbose)
            printf("%f%s", rolls[i], i == end ? "" : " + ");
        result += rolls[i];
    }
    if (arguments->verbose)
        printf("\n");

    free(rolls);

    return result;
}

/***********************************************************
 * Evaluate equation
 * graph_min_max
 * 0: provide the minimum value for every die roll
 * 1: provide the maximum value for every die roll
 * return
 * 0: success
 * char: not enough numbers for the operator
 ***********************************************************/
int evaluate_equation(double *result_out, Equation *equation, struct arguments *arguments) {
    double *num_stack = malloc(sizeof(double*) * (equation->num_count + 1));
    int num_count = 0;
    int stack_top = -1;
    for (int i = 0; i < equation->op_count; i++) {
        if (equation->operators[i] == '.') {
            num_stack[++stack_top] = equation->numbers[num_count++];
        } else {
            double result = 0;
            switch(equation->operators[i]) {
            case '+': { // addition
                if (stack_top < 1)
                    return '+';
                result = num_stack[stack_top - 1] + num_stack[stack_top];
                num_stack[--stack_top] = result;
            } break;
            case '-': { // subtraction
                if (stack_top < 1)
                    return '-';
                result = num_stack[stack_top - 1] - num_stack[stack_top];
                num_stack[--stack_top] = result;
            } break;
            case '*': { // multiplication
                if (stack_top < 1)
                    return '*';
                result = num_stack[stack_top - 1] * num_stack[stack_top];
                num_stack[--stack_top] = result;
            } break;
            case '/': { // division
                if (stack_top < 1)
                    return '/';
                result = num_stack[stack_top - 1] / num_stack[stack_top];
                num_stack[--stack_top] = result;
            } break;
            case '^': { // exponentiation
                if (stack_top < 1)
                    return '^';
                result = pow(num_stack[stack_top - 1], num_stack[stack_top]);
                num_stack[--stack_top] = result;
            } break;
            case '!': { // factorial
                if (stack_top < 0)
                    return '!';
                result = factorial(num_stack[stack_top]);
                num_stack[stack_top] = result;
            } break;
            case 'n': { // negation
                if (stack_top < 0)
                    return '-';
                result = -num_stack[stack_top];
                num_stack[stack_top] = result;
            } break;
            case 'p': { // change nothing
                if (stack_top < 0)
                    return '+';
            } break;
            case 'd': { // roll dice
                if (stack_top < 1)
                    return 'd';
                result = roll(num_stack[stack_top - 1], num_stack[stack_top], 0, arguments);
                num_stack[--stack_top] = result;
            } break;
            case 'c': {
                if (stack_top < 0) {
                    result = roll(1, 2, 1, arguments);
                    stack_top++;
                } else {
                    result = roll(num_stack[stack_top], 2, 1, arguments);
                }
                num_stack[stack_top] = result;
            } break;
            default:
                break;
            }
        }
    }

    *result_out = num_stack[0];

    free(num_stack);
    return 0;
}
int verify_equation(Equation *equation, struct arguments *arguments) {
    double *num_stack = malloc(sizeof(double*) * (equation->num_count + 1));
    int num_count = 0;
    int stack_top = -1;
    int roll_or_flip = 0;
    for (int i = 0; i < equation->op_count; i++) {
        if (equation->operators[i] == '.') {
            num_stack[++stack_top] = equation->numbers[num_count++];
        } else {
            switch(equation->operators[i]) {
            case '+': { // addition
                if (stack_top < 1)
                    return '+';
                --stack_top;
            } break;
            case '-': { // subtraction
                if (stack_top < 1)
                    return '-';
                --stack_top;
            } break;
            case '*': { // multiplication
                if (stack_top < 1)
                    return '*';
                --stack_top;
            } break;
            case '/': { // division
                if (stack_top < 1)
                    return '/';
                --stack_top;
            } break;
            case '^': { // exponentiation
                if (stack_top < 1)
                    return '^';
                --stack_top;
            } break;
            case '!': { // factorial
                if (stack_top < 0)
                    return '!';
            } break;
            case 'n': { // negation
                if (stack_top < 0)
                    return '-';
            } break;
            case 'p': { // change nothing
                if (stack_top < 0)
                    return '+';
            } break;
            case 'd': { // roll dice
                if (stack_top < 1)
                    return 'd';
                --stack_top;
                roll_or_flip = 1;
            } break;
            case 'c': {
                if (stack_top < 0) {
                    stack_top++;
                }
                roll_or_flip = 1;
            } break;
            default:
                break;
            }
        }
    }
    if (!roll_or_flip)
        return 'l';
    free(num_stack);
    return 0;
}

/***********************************************************
 * Target Inequality
 ***********************************************************/
int target_inequality(double result, struct arguments *arguments) {
    if (arguments->target_inequality[0] == '=') { // ==
        return result == arguments->target_num;
    } else if (arguments->target_inequality[0] == '<' &&
        arguments->target_inequality[1] == '=') { // <=
        return result <= arguments->target_num;
    } else if (arguments->target_inequality[0] == '>' &&
        arguments->target_inequality[1] == '=') { // >=
        return result >= arguments->target_num;
    } else if (arguments->target_inequality[0] == '!' &&
        arguments->target_inequality[1] == '=') { // !=
        return result != arguments->target_num;
    } else if (arguments->target_inequality[0] == '<') { // <
        return result < arguments->target_num;
    } else if (arguments->target_inequality[0] == '>') { // >
        return result > arguments->target_num;
    }
    return result == arguments->target_num;
}

/***********************************************************
 * Rounding
 ***********************************************************/
void rounding(struct arguments *arguments, double *result){
    if (arguments->round) {
        switch(arguments->round_type) {
        case 'u':{
            *result = ceil(*result);
            if (arguments->verbose)
                printf("Rounding Up\n");
        } break;
        case 'd':{
            *result = floor(*result);
            if (arguments->verbose)
                printf("Rounding Down\n");
        } break;
        case 'c':{
            *result = round(*result);
            if (arguments->verbose)
                printf("Rounding to the Closest\n");
        } break;
        }
    }
}

/***********************************************************
 * Graph
 * takes in an equation with a single roll or coin flip
 * return an array of probabilities, and the length
 ***********************************************************/
int verify_inequality(char *inequality) {
    if (inequality[1] == '\0') {
        if (!(inequality[0] == '=' || inequality[0] == '<' || inequality[0] == '>'))
            return 1;
    } else {
        if (!((inequality[0] == '<' && inequality[1] == '=') || 
                (inequality[0] == '=' && inequality[1] == '<') || 
                (inequality[0] == '>' && inequality[1] == '=') || 
                (inequality[0] == '=' && inequality[1] == '>') || 
                (inequality[0] == '!' && inequality[1] == '=')))
            return 1;
    }
    return 0;
}
void draw_graph(struct arguments *arguments, Equation *equation) {
    char *inequality = arguments->graph_inequality;
    int err = verify_inequality(inequality);
    if (err) {
        printf("ERROR: Inequality is not recognized\n");
        return;
    }
    err = verify_equation(equation, arguments);
    if (err != 0) {
        if (err == 'l') {
            printf("ERROR: Nothing to graph, must have a die or coin\n");
            return;
        }
        printf("ERROR: Not enough numbers for the %c operator\n", err);
        return;
    }
    Graph result = evaluate_equation_graph(equation, arguments);
    if (arguments->verbose) {
        if (inequality[1] == '\0') {
            if (inequality[0] == '=') {
                if (arguments->verbose)
                    printf("Graphing Equals\n");
            } else if (inequality[0] == '<'){
                if (arguments->verbose)
                    printf("Graphing Less Than\n");
            } else if (inequality[0] == '>') {
                if (arguments->verbose)
                    printf("Graphing Greater Than\n");
            }
        } else {
            if ((inequality[0] == '<' && inequality[1] == '=') || 
                    (inequality[0] == '=' && inequality[1] == '<')){
                if (arguments->verbose)
                    printf("Graphing Less Than or Equal To\n");
            } else if ((inequality[0] == '>' && inequality[1] == '=') || 
                    (inequality[0] == '=' && inequality[1] == '>')) {
                if (arguments->verbose)
                    printf("Graphing Greater Than or Equal To\n");
            } else if (inequality[0] == '!' && inequality[1] == '=') {
                if (arguments->verbose)
                    printf("Graphing Not Equal To\n");
            }
        }
        printf("Min: %f\n", result.min);
        printf("Max: %f\n", result.max);
    }
    // bar_count is probability / result.max * (MAXGRAPHBARSIZE - 1) + 1
    // a bit of algebra lets us precalculate so it doesn't have to be done every loop
    double mult = (MAXGRAPHBARSIZE - 1) / result.max;
    for (int i = 0; i < result.used; i++) {
        printf("%10f: ", result.graphLines[i].line);
        double probability = result.graphLines[i].probability;
        int bar_count = round(probability * mult + 1);
        if (result.max == result.min) {
            bar_count = 5;
        }
        for (int i = 0; i < bar_count; i++) {
            printf("#");
        }
        printf(" %f\n", probability);
    }
    free_graph(&result);
}

/***********************************************************
 * The main function
 ***********************************************************/
int main(int argc, char *argv[]){
    struct arguments arguments;

    // set defaults
    arguments.verbose = 0;

    arguments.best = 0;
    arguments.best_type = 'h';
    arguments.best_num = 1;

    arguments.target = 0;
    arguments.target_inequality[0] = '=';
    arguments.target_inequality[1] = '\0';
    arguments.target_num = 0;

    arguments.multiple = 0;
    arguments.multiple_num = 1;

    arguments.graph = 0;
    arguments.graph_inequality[0] = '=';
    arguments.graph_inequality[1] = '\0';

    arguments.round = 0;
    arguments.round_type = 'c';

    // parse arguments
    error_t err = argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (err == EINVAL) {
        printf("ERROR: Invalid Arguments\n");
        return EINVAL;
    }

    // parse equation
    int coin = 0;
    int coin_total = 0;
    int coin_count = 0;
    /*  0: no coin, or coin flip is part of a larger equation
     *  1: 1 coin
     * -1: multi-coin
     */
    Equation *equation = parse_equation(&arguments);
    if (equation->op_count == 1 && equation->operators[0] == 'c') {
        if (arguments.multiple == 0)
            coin = 1;
        else
            coin = -1;
    } else if (equation->op_count == 2 && equation->operators[1] == 'c') {
        coin = -1;
    }
    if (arguments.graph) {
        draw_graph(&arguments, equation);
        // free memory
        free(equation->operators);
        free(equation->numbers);
        free(equation);

        exit(0);
    }
    // evaluate equation
    time_t t;
    init_seed((unsigned) time(&t));
    int target_true = 0;
    int target_false = 0;
    double result = 0;
    for (int i = 0; i < arguments.multiple_num; i++) {
        int err = evaluate_equation(&result, equation, &arguments);
        if (err != 0) {
            printf("ERROR: Not enough numbers for the %c operator\n", err);
        } else {
            rounding(&arguments, &result);
            if (arguments.target) {
                if (arguments.verbose || !arguments.multiple) {
                    printf("%s\n", target_inequality(result, &arguments) ? "true" : "false");
                }
                if (arguments.multiple) {
                    if (target_inequality(result, &arguments)) {
                        target_true++;
                    } else {
                        target_false++;
                    }
                }
            } else {
                if (coin == 0) {
                    printf("%f\n", result);
                } else if (coin == 1) {
                    printf("%s\n", result ? "heads" : "tails");
                } else if (coin == -1) {
                    if (equation->num_count == 0)
                        coin_total += 1;
                    else
                        coin_total += equation->numbers[0];
                    coin_count += result;
                }
            }
        }
    }
    if (coin == -1)
        printf("Heads: %d\nTails: %d\n", coin_count, coin_total - coin_count);
    if (arguments.target && arguments.multiple)
        printf("True: %d\nFalse: %d\n", target_true, target_false);

    // free memory
    free(equation->operators);
    free(equation->numbers);
    free(equation);
    rand_clear();

    exit(0);
}
