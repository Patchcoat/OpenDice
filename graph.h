#include <stddef.h>

typedef struct {
    double line;
    double probability;
} GraphLine;
typedef struct {
    GraphLine* graphLines;
    double max;
    double min;
    size_t used;
    size_t size;
} Graph;
typedef struct {
    char *operators;
    double *numbers;
    int op_count;
    int num_count;
} Equation;
struct arguments {
    char *equation;
    int verbose;

    int best;
    char best_type;
    int best_num;

    int target;
    char target_inequality[4];
    float target_num;

    int multiple;
    int multiple_num;

    int graph;
    char graph_inequality[4];

    int round;
    char round_type;
};


// evaluate_equation, but performed on an entire graph
Graph evaluate_equation_graph(Equation *equation, struct arguments *arguments);
// create and return a graph for a given roll
Graph graph(char op, double left, double right);
int find_graph_line(Graph* graph, int l, int r, double line);
// dynamic array functions
void init_graph(Graph* in, size_t initSize);
void insert_into_graph(Graph* in, GraphLine* element);
void free_graph(Graph* in);
// combines two graphs into a new one, then frees the input graphs
Graph* combine_graphs(char op, Graph* left, Graph* right);
