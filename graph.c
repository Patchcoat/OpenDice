#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "graph.h"

void print_graph_array(Graph *graph_array, int size){
    for (int i = 0; i < size; i++) {
        printf("|%p", &graph_array[i]);
    }
    printf("\n");
}

double add(double a, double b) {
    return a + b;
}
double subtract(double a, double b){
    return a - b;
}
double multiply(double a, double b){
    return a * b;
}
double divide(double a, double b){
    return a / b;
}
double exponent(double a, double b){
    return pow(a, b);
}

double negation(double n) {
    return -n;
}
// TODO replace with a better algorithm, such as prime swing
double factorial(double n) {
    return n <= 1 ? 1 : n * factorial(n-1);
}

Graph graph_combine(int on_graph, int prev_graph, Graph *graph_array, Graph result_graph,
      int stack_top, double *num_stack, double (*opp)()) {
    if (on_graph && prev_graph) {
        Graph temp_graph;
        init_graph(&temp_graph, 
                graph_array[stack_top].used + graph_array[stack_top - 1].used);
        for (int i = 0; i < graph_array[stack_top].used; i++) {
            for (int j = 0; j < graph_array[stack_top-1].used; j++) {
                GraphLine *on_line = &graph_array[stack_top].graphLines[i];
                GraphLine *prev_line = &graph_array[stack_top-1].graphLines[j];
                double line = opp(on_line->line, prev_line->line);
                double probability = on_line->probability * prev_line->probability;
                int index = find_graph_line(&temp_graph, 0, temp_graph.used-1, line);
                if (index == -1) {
                    GraphLine graph_line = {.line = line, .probability = probability};
                    insert_into_graph_sorted(&temp_graph, &graph_line);
                } else {
                    temp_graph.graphLines[index].probability += probability;
                    probability = temp_graph.graphLines[index].probability;
                }
                // set max
                if (probability > temp_graph.max)
                    temp_graph.max = probability;
                // set min
                if (probability < temp_graph.min)
                    temp_graph.min = probability;
            }
        }
        free_graph(&graph_array[stack_top - 1]);
        graph_array[stack_top - 1] = temp_graph;
        result_graph = graph_array[stack_top - 1];
        free_graph(&graph_array[stack_top]);
        stack_top--;
    } else if (on_graph || prev_graph) {
        for (int i = 0; i < result_graph.used; i++) {
            if (on_graph && !prev_graph) {
                graph_array[stack_top].graphLines[i].line = 
                    opp(graph_array[stack_top].graphLines[i].line, num_stack[stack_top - 1]);
            } else if (!on_graph && prev_graph) {
                graph_array[stack_top - 1].graphLines[i].line = 
                    opp(graph_array[stack_top - 1].graphLines[i].line, num_stack[stack_top]);
            }
        }
        if (on_graph) {
            result_graph = graph_array[stack_top];
            free_graph(&graph_array[stack_top - 1]);
        } else {
            result_graph = graph_array[stack_top - 1];
            free_graph(&graph_array[stack_top]);
        }
        stack_top--;
    }
    return result_graph;
}

Graph graph_unary(Graph *graph_array, Graph result_graph, int stack_top, double (*opp)()) {
    Graph temp_graph;
    init_graph(&temp_graph, graph_array[stack_top].used);
    for (int i = 0; i < graph_array[stack_top].used; i++) {
        GraphLine *on_line = &graph_array[stack_top].graphLines[i];
        GraphLine graph_line = {.line = opp(on_line->line), .probability = on_line->probability};
        insert_into_graph_sorted(&temp_graph, &graph_line);
    }
    free_graph(&graph_array[stack_top]);
    graph_array[stack_top] = temp_graph;
    result_graph = graph_array[stack_top];
    return result_graph;
}

void combine_coin_die_graph(Graph *temp_graph, Graph *merge_graph, double divisor) {
    double probability = 0;
    for (int i = 0; i < temp_graph->used; i++) {
        GraphLine line = temp_graph->graphLines[i];
        probability = line.probability / divisor;
        line.probability = probability;
        int index = find_graph_line(merge_graph, 0, merge_graph->used-1, line.line);
        if (index == -1) {
            insert_into_graph_sorted(merge_graph, &line);
        } else {
            merge_graph->graphLines[index].probability += probability;
            probability = merge_graph->graphLines[index].probability;
        }
        // set max
        if (probability > merge_graph->max)
            merge_graph->max = probability;
        // set min
        if (probability < merge_graph->min)
            merge_graph->min = probability;
    }
}

Graph coin_flip_coin(Graph *top_graph) {
    Graph merge_graph;
    init_graph(&merge_graph, top_graph->size * 2);
    for (int i = 0; i < top_graph->used; i++) {
        if (top_graph->graphLines[i].line == 0)
            continue;
        Graph temp_graph = graph('c', top_graph->graphLines[i].line, 2);
        combine_coin_die_graph(&temp_graph, &merge_graph, top_graph->used);
        free_graph(&temp_graph);
    }
    free_graph(top_graph);

    return merge_graph;
}

// order
// 1 = top_graph represents the type of die to roll: 2d(2d6)
// 0 = top_graph represents the number of dice to roll: (2d6)d6
Graph die_roll_die(Graph *top_graph, double num, short order) {
    Graph merge_graph;
    init_graph(&merge_graph, top_graph->used * num);

    for (int i = 0; i < top_graph->used; i++) {
        Graph temp_graph;
        if (order) {// graph is the type of die
            temp_graph = graph('d', num, top_graph->graphLines[i].line);
        } else {// graph is the number of dice
            temp_graph = graph('d', top_graph->graphLines[i].line, num);
        }
        combine_coin_die_graph(&temp_graph, &merge_graph, top_graph->used);
        free_graph(&temp_graph);
    }
    free_graph(top_graph);
    return merge_graph;
}

// use one graph as the die count, and another graph as the die value
Graph die_merge_die(Graph *top_graph, Graph *prev_graph) {
    Graph merge_graph;
    init_graph(&merge_graph, top_graph->used * prev_graph->used);

    for (int i = 0; i < top_graph->used; i++) {
        for (int j = 0; j < prev_graph->used; j++) {
            Graph temp_graph;
            temp_graph = graph('d', top_graph->graphLines[i].line, prev_graph->graphLines[j].line);
            combine_coin_die_graph(&temp_graph, &merge_graph, top_graph->used * prev_graph->used);
            free_graph(&temp_graph);
        }
    }

    free_graph(top_graph);
    free_graph(prev_graph);
    return merge_graph;
}

Graph evaluate_equation_graph(Equation *equation, struct arguments *arguments) {
    double *num_stack = malloc(sizeof(double*) * (equation->num_count + 1));
    int num_count = 0;
    int stack_top = -1;
    Graph result_graph;
    Graph graph_array[equation->num_count + 1];
    for (int i = 0; i < equation->op_count; i++) {
        if (equation->operators[i] == '.') {
            num_stack[++stack_top] = equation->numbers[num_count++];
            init_graph(&graph_array[stack_top], 0);
            graph_array[stack_top].null = 1;
        } else {
            double result = 0;
            print_graph_array(graph_array, stack_top+1);
            int on_graph = 0;
            int prev_graph = 0;
            if (stack_top >= 0) {
                on_graph = !graph_array[stack_top].null;
                if (stack_top >= 1) {
                    prev_graph = !graph_array[stack_top - 1].null;
                }
            }
            switch(equation->operators[i]) {
            case '+': { // addition
                if (on_graph || prev_graph) {
                    result_graph = graph_combine(on_graph, prev_graph, graph_array, result_graph, 
                            stack_top, num_stack, &add);
                } else {
                    result = num_stack[stack_top - 1] + num_stack[stack_top];
                    num_stack[--stack_top] = result;
                    graph_array[stack_top].null = 1;
                }
            } break;
            case '-': {
                if (on_graph || prev_graph) {
                    result_graph = graph_combine(on_graph, prev_graph, graph_array, result_graph, 
                            stack_top, num_stack, &subtract);
                } else {
                    result = num_stack[stack_top - 1] - num_stack[stack_top];
                    num_stack[--stack_top] = result;
                    graph_array[stack_top].null = 1;
                }
            } break;
            case '*': {
                if (on_graph || prev_graph) {
                    result_graph = graph_combine(on_graph, prev_graph, graph_array, result_graph, 
                            stack_top, num_stack, &multiply);
                } else {
                    result = num_stack[stack_top - 1] * num_stack[stack_top];
                    num_stack[--stack_top] = result;
                    graph_array[stack_top].null = 1;
                }
            } break;
            case '/': {
                if (on_graph || prev_graph) {
                    result_graph = graph_combine(on_graph, prev_graph, graph_array, result_graph, 
                            stack_top, num_stack, &divide);
                } else {
                    result = num_stack[stack_top - 1] / num_stack[stack_top];
                    num_stack[--stack_top] = result;
                    graph_array[stack_top].null = 1;
                }
            } break;
            case '^': {
                if (on_graph || prev_graph) {
                    result_graph = graph_combine(on_graph, prev_graph, graph_array, result_graph, 
                            stack_top, num_stack, &exponent);
                } else {
                    result = pow(num_stack[stack_top - 1], num_stack[stack_top]);
                    num_stack[--stack_top] = result;
                    graph_array[stack_top].null = 1;
                }
            } break;
            case '!': {
                if (on_graph) {
                    result_graph = graph_unary(graph_array, result_graph, stack_top, &factorial);
                } else {
                    result = factorial(num_stack[stack_top]);
                    num_stack[stack_top] = result;
                    graph_array[stack_top].null = 1;
                }
            } break;
            case 'n': {
                if (on_graph) {
                    result_graph = graph_unary(graph_array, result_graph, stack_top, &negation);
                } else {
                    result = -num_stack[stack_top];
                    num_stack[stack_top] = result;
                    graph_array[stack_top].null = 1;
                }
            } break;
            case 'p': {
            } break;
            case 'd': { // roll dice
                if (on_graph || prev_graph) {
                    if (on_graph && prev_graph) {
                        result_graph = die_merge_die(&graph_array[stack_top], &graph_array[stack_top-1]);
                    } else {
                        if (on_graph) {
                            result_graph = die_roll_die(&graph_array[stack_top], num_stack[stack_top-1], 1);
                            free_graph(&graph_array[stack_top-1]);
                        } else {
                            result_graph = die_roll_die(&graph_array[stack_top-1], num_stack[stack_top], 0);
                            free_graph(&graph_array[stack_top]);
                        }
                    }
                } else {
                    free_graph(&graph_array[stack_top - 1]);
                    graph_array[stack_top - 1] = graph('d', num_stack[stack_top - 1], num_stack[stack_top]);
                    result_graph = graph_array[stack_top - 1];
                    free_graph(&graph_array[stack_top]);
                    stack_top--;
                }
            } break;
            case 'c': {
                if (on_graph) {
                    result_graph = coin_flip_coin(&graph_array[stack_top]);
                } else {
                    if (stack_top < 0) {
                        stack_top++;
                        graph_array[0] = graph('c', 1, 2);
                        result_graph = graph_array[stack_top];
                    } else {
                        free_graph(&graph_array[stack_top]);
                        graph_array[stack_top] = graph('c', num_stack[stack_top], 2);
                        result_graph = graph_array[stack_top];
                    }
                }
            } break;
            default:
                break;
            }
        }
    }

    free(num_stack);
    return result_graph;
}

double choose(double n, double k) {
    if (k > n) {
        return 0;
    } else if (k == 0 || n == k) {
        return 1;
    }
    return factorial(n) / (factorial(k) * factorial(n - k));
}

double calculate_probability(double value, double left, double right) {
    double total = 0;
    int max = floor((value-left)/right);
    for (int i = 0; i <= max; i++) {
        // neg is 1 if i is even, and -1 if i is odd
        double neg = (double) 1 + (-2 * (i & 1));
        double choose1 = choose(left, i);
        double choose2 = choose(value - (right * i) - 1, left - 1);
        total += neg * choose1 * choose2;
    }
    return (1 / pow(right, left)) * total;
}

Graph graph(char op, double left, double right) {
    Graph graph;
    init_graph(&graph, (int) left * right);
    if (op == 'd') {
        double integral_left;
        double fractional_left = modf(left, &integral_left);
        double integral_right;
        modf(right, &integral_right);
        if (fractional_left == 0) {
            int min = (int) integral_left;
            int max = (int) integral_left * integral_right;
            for (int i = min; i <= max; i++) {
                GraphLine line;
                line.line = i;
                line.probability = calculate_probability(i, integral_left, integral_right);
                insert_into_graph(&graph, &line);

                // set max
                if (line.probability > graph.max)
                    graph.max = line.probability;
                // set min
                if (line.probability < graph.min)
                    graph.min = line.probability;
            }
        } else {
            int min = (int) integral_left;
            int max = (int) integral_left * integral_right;
            for (int i = min; i <= max; i++) {
                double probability = (1 / integral_right) * 
                    calculate_probability(i, integral_left, integral_right);
                for (int j = 1; j <= integral_right; j++) {
                    GraphLine line;
                    line.line = i + (fractional_left * j);
                    line.probability = probability;
                    int index = find_graph_line(&graph, 0, graph.used-1, line.line);
                    if (index == -1) {
                        insert_into_graph_sorted(&graph, &line);
                    } else {
                        graph.graphLines[index].probability += line.probability;
                        line.probability = graph.graphLines[index].probability;
                    }
                    // set max
                    if (line.probability > graph.max)
                        graph.max = line.probability;
                    // set min
                    if (line.probability < graph.min)
                        graph.min = line.probability;
                }
            }
        }
    } else if (op == 'c') {
        double integral;
        double fraction = modf(left, &integral);
        if (integral == 0)
            integral = 1;
        if (fraction == 0) {
            int min = 0;
            int max = (int)integral;
            double probability = 1 / (double) (max + 1);
            graph.max = probability;
            graph.min = probability;
            for (int i = min; i <= max; i++) {
                GraphLine line;
                line.line = i;
                line.probability = probability;
                insert_into_graph(&graph, &line);
            }
        } else {
            int min = 0;
            int max = (int) integral;
            for (int i = min; i <= max; i++) {
                double probability = 1.0 / (double) (max + 1);
                for (int j = 1; j <= 2; j++) {
                    GraphLine line;
                    line.line = i + (fraction * j);
                    line.probability = probability;
                    int index = find_graph_line(&graph, 0, graph.used-1, line.line);
                    if (index == -1) {
                        insert_into_graph_sorted(&graph, &line);
                    } else {
                        graph.graphLines[index].probability += line.probability;
                        line.probability = graph.graphLines[index].probability;
                    }
                    // set max
                    if (line.probability > graph.max)
                        graph.max = line.probability;
                    // set min
                    if (line.probability < graph.min)
                        graph.min = line.probability;
                }
            }
        }
    }
    return graph;
}

int find_graph_line(Graph* graph, int l, int r, double line) {
    if (graph->used == 0)
        return -1;
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (graph->graphLines[mid].line == line)
            return mid;

        if (graph->graphLines[mid].line > line)
            return find_graph_line(graph, l, mid - 1, line);

        return find_graph_line(graph, mid + 1, r, line);
    }
    return -1;
}
int find_insert_index(Graph* graph, int l, int r, double line) {
    // the way this function is used it's most common for the
    // correct answer to the at the end. So we create a check
    // for that specifically
    if (r >= 0 && graph->graphLines[r].line < line)
        return r+1;
    int mid = l;
    if (r >= l) {
        mid = l + (r - l) / 2;
        if (graph->graphLines[mid].line == line)
            return mid;

        if (graph->graphLines[mid].line > line)
            return find_insert_index(graph, l, mid - 1, line);

        return find_insert_index(graph, mid + 1, r, line);
    }
    return mid;
}

// dynamic array functions
void init_graph(Graph* in, size_t initSize) {
    in->graphLines = malloc(initSize * sizeof(GraphLine));
    in->used = 0;
    in->size = initSize;
    in->min = 1;
    in->max = 0;
    in->null = 0;
}
void insert_into_graph(Graph* in, GraphLine* element) {
    if (in->used == in->size) {
        in->size *= 2;
        in->graphLines = realloc(in->graphLines, in->size * sizeof(GraphLine));
    }
    in->graphLines[in->used].line = element->line;
    in->graphLines[in->used].probability = element->probability;
    in->used++;
}
void insert_into_graph_sorted(Graph* in, GraphLine* element) {
    if (in->used == in->size) {
        in->size *= 2;
        in->graphLines = realloc(in->graphLines, in->size * sizeof(GraphLine));
    }
    int index = find_insert_index(in, 0, in->used-1, element->line);
    for (int i = in->used; i > index; i--) {
        in->graphLines[i].line = in->graphLines[i-1].line;
        in->graphLines[i].probability = in->graphLines[i-1].probability;
    }
    in->graphLines[index].line = element->line;
    in->graphLines[index].probability = element->probability;
    in->used++;
}
void free_graph(Graph* in) {
    free(in->graphLines);
    in->graphLines = NULL;
    in->used = in->size = 0;
}
