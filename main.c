#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arena.h"
#include "bcd.h"
#include "operator.h"

typedef struct _Element Element;

typedef struct {
    size_t length;
    long int assoc;
    Element* elements;
} Multiset;

typedef struct _Element {
    union {
    Num num;
    Multiset mset;
    };
    bool is_num;
} Element;

char* get_number(Arena *arena, Num *num, char* string) {
    char* itr = string;
    if (*itr == '-') {
        num->negative = true;
        itr++;
    }
    while (*itr != '\0') {
        if (ISDIGIT(*itr) || (*itr == '.' && !num->decimal)) {
            add_digit(arena, num, *itr);
        } else {
            break;
        }
        itr++;
    }
    return itr;
}

bool compare_operator(Operator *opt1, Operator *opt2) {
    if (opt1->length != opt2->length) {
        return false;
    }
    size_t length = opt1->length;
    for (size_t index; index < length; index++) {
        if (opt1->string[index] != opt2->string[index]) {
            return false;
        }
    }
    return true;
}

char* get_operator(Arena* arena, Operator *opt, char* string) {
    char* itr = string;
    do {
        add_symbol(arena, opt, *itr);
        itr++;
        if (ISDIGIT(*itr) || *itr == '-' || *itr == '.' || *itr == ' ') {
            break;
        }
    } while (*itr != '\0');

    return itr;
}

/*
Operator List

(1d6, 1d20, 1d10, 1d8, 1d4) | ($| highest 2 @, sum ,-> sdif $| # highest)
Roll four different dice. Return the sum of the two highest roll, and of the remaining dice, 
return the number of sides of the one with the most sides.

.d.
.d!.
.d!!.
.exp.           Takes in a multiset, and explodes using the associated multiset value for every value equal to value on the right.
                    If no value is provided, default to exploding on the associated value.
                    After exploding, add the new values into the multiset. If the multiset contains one value, add it to that value.
.imp.           As above, but implodes and on the smallest value
.exp!.          Explode once. Only explode once for each value in the multiset. Don't keep explodning if you roll that value again.
.imp!.
.add. .+.
.sub. .-.
.div. ./.
.mult. .*. .x.
.mod. .%.
.pipe. .|.      Pipe left into each individual element in the multiset right. Store in the '|' register.
.$. .get.       Get the value stored in register '.', which may be a multiset or single non alphabet character
.$!. .get!.     Get a value in register '.' then clear the register
.@. .store.     Store a value in register '.'
.@!. .store.    Clear the value stored in the register.
.! .factorial
.p.             Permutation of left to right
.c.             Combination of left to right
.^. .**. .exp. .pow.
.rt. .root.     Rights root to left
.abs
.round
.ceil
.floor
.count      Get the number of values in a multiset
.high       Get the largest value in the multiset
.low        Get the smallest value in the multiset
.keep[inequality].      Keep all the values at match the inequality relative to right.
.sum        Add together all the values in the multiset
.trim       Trim repeat values. Make the set a multiset
.u.         Multiset Union.
.i.         Multiset Intersection
.diff.      Multiset difference
.diff!.     Symetric difference
.mrg.       Merge the multisets
.#          Get the value associated with the multiset
.#->.       Associate a value with the multiset
()          Define a multiset
n..m        Generate a multiset of n, m, and all the integers between n and m
.:.         Generate a multiset of left repeated right times
.::(.)      Generate a multiset by evaluating the expression in parathasis left times
.,.         Seperate values inside the multiset
.,->.       Pass the value from the previous value in the multiset to the next value in the multiset. 
                Stores it into the ',' register if there isn't already a value in there.
                Passes the value in ',' to the awaiting operator if there is one.
.![!..]     Where . is an operator, modify the operator op with one or more
.~          Where . is an operator, reverse the effect of the input: m/n = n/~m
= =! < > <= >= =
.' .` .~
&& || and or nand nor xor
*/

int main(int argc, char *argv[]) {
    Arena arena = new_arena(10);
    Arena op_arena = new_arena(100);

    // TODO fill op arena
    Operator *opts[80];
    opts[0] = new_operator(&op_arena);

    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[0]);
    }
    char* string = "-2023.2391add2865";
    char* itr = string;

    // Load operation into an arena
    Num* num = new_num(&arena);
    itr = get_number(&arena, num, string);
    Operator* opt = new_operator(&arena);
    itr = get_operator(&arena, opt, itr);
    if (opt->is_biterm) {
        Num* num2 = new_num(&arena);
        itr = get_number(&arena, num, itr);
    }
    // Compute the result and load that into another arena.
    // Pop off the arena to the point where the equation started, and load the result in at that point
    // Clear the result arena and prepare for the next result

    return 0;
}