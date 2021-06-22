# OpenDice
OpenDice is a small and simple die rolling app written in C

## License
OpenDice is licensed under GPL v3.0
More information can be found in the LICENCE file or at the following link
https://www.gnu.org/licenses/gpl-3.0.en.html

## How to Use
roll [OPTION...] EQUATION

Order of operations is as follows    
parentheses ((x+y)*(w-z))    
rolls and flips (xdy) (xc)   
exponentiation (x^y)    
multiplication and division (x\*y) (x/y)   
addition and subtraction (x+y) (x-y)    
NOTE: when using parentheses or exponentiation there must be single or double quotes around the equation, for example 'x^(y+z)'

### Rolling
The core of OpenDice is the equation system.    
Input is treated like a mathematical equation, reading the numbers, symbols, and characters in a way that follows order of operations.

Roll n number of x sided dice like so    
roll ndx

So to roll 2 six sided dice    
roll 2d6    
When rolling multiple dice like this each roll is added together and the user is given the total

Use multiplication and addition to modify the result    
roll 2d6+2    
The above will roll two six sided dice, add the two rolls together, and then add two to that end result

Dice can be rolled to tell OpenDice how many dice it should roll    
roll 3d6d20    
The above rolls 3d6, adds the results, then rolls that many d20. This can be continued indefinitely.

The default number of dice is 1, and the default number of sides is 6. So 3d is the same as 3d6.

### Coin
Use 'coin' or 'c' to flip a coin    
roll coin    
The above will return with heads or tails

Flip x coins like so    
roll xcoin    
which can be abbreviated to    
roll xc

NOTE: Coins must be in the format "roll xcoin" or "roll xc" to get a "Heads:/Tails:" display. If x is an equation it follows order of operations (parenthesis, then flip x coins/roll dice, then add/multiply/divide etc.) and it displays a number rather than a split between heads and tails. When used in an equation, heads is 1 and tails is 0, and it's recommended you use the --verbose tag in order to see the result of each flip.

### Tags
--help to see a breakdown of the commands.

The -b or --best tag will take a given number of **t**op or **b**ottom rolls and ignore the rest.    
roll 3d6 -bt2    
The above will execute the equation "3d6", but will only add the top two results together. To use a certain number of the bottom results, use the character b in place of the t.    
If no number is given the default is to use the single bottom or top results.    
If no t or b is given, the default is to find the top results.

The -g or --graph command takes in an optional inequality (or defaults to = if nothing is provided) and returns the probability of every possible result along side a graph of the probability spread. --verbose shows the minimum and maximum probability. Supported inequalities include the following    
=, <, >, ==, !=, <=, >=, =<, <=

The -m or --multiple tag will execute all the provided equations a given number of times.    
roll 2d6 -m 3    
The above will execute the equation "2d6" three times, and give back the three results added together.    
To see each individual results, add a -v or --verbose tag.

The -t or --target tag sets a target for an equation results and reports back whether or not that target was reached. Single or double quotes must be used if a >, < or = is present    
roll 2d6 -t'>=4'    
The above will execute the equation "2d6", then check if the result is greater than or equal to 4 with either "success" or "failure".    
If no inequality is given (>, <, or =) then it defaults to checking if the result is equal to the given number    
When combined with --multiple it displays a count of how many times it reached the target, and how many times it didn't.

The -v or --verbose tag shows individual rolls/coin flips, as well as a bunch more information such as the number of operators and numbers, and the equation in reverse polish notation    
roll 2d6 -v    
The above will display the following

Operators: 1    
Numbers: 2    
Equation: 2.000000 6.000000 d

Rolling 2.000000 6-sided dice    
Roll 1: 4.000000    
Roll 2: 6.000000    
total = 4.000000 + 6.000000    
10.000000
