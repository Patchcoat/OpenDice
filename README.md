# OpenDice
OpenDice is a small and simple die rolling app written in C

## License
OpenDice is licensed under GPL v3.0
More information can be found in the LICENCE file or at the following link
https://www.gnu.org/licenses/gpl-3.0.en.html

## How to Use
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

Order of operations is as follows    
parentheses ((x+y)*(w-z))    
rolls (xdy)    
exponentiation (x^y)    
multiplication and division (x\*y) (x/y)   
addition and subtraction (x+y) (x-y)    
NOTE: when using parentheses or exponentiation there must be single quotes around the equation, for example 'x^(y+z)'

Dice can be rolled to tell OpenDice how many dice it should roll    
roll 3d6d20    
The above rolls 3d6, adds the results, then rolls that many d20. This can be continued indefinitely.

### Coin
Use coin to flip a coin    
roll coin    
The above will return with heads or tails

Flip x coins like so    
roll coin x    
The x in the above must be a number. It cannot be an equation.

NOTE: Coins must be in the format "roll coin x". Flipping coins and rolling dice at the same time is not recmmended as all the information from the die rolls may not show up.


### Tags
The -v or --verbose tag shows individual rolls/coin flips    
roll 2d6 -v    
The above will show the result of each d6 like so    
3+5    
8

The -t or --target tag sets a target for an equation results and reports back whether or not that target was reached single quotes must be used if a >, < or = is present    
roll 2d6 -t '>=4'    
The above will execute the equation "2d6", then check if the result is greater than or equal to 4 with either "success" or "failure".    
If no inequality is given (>, <, or =) then it defaults to checking if the result is equal to the given number

The -b or --best tag will take a given number of **t**op or **b**ottom rolls and ignore the rest.    
roll 3d6 -b t2    
The above will execute the equation "3d6", but will only add the top two results together. To use a certain number of the bottom results, use the character b in place of the t.    
If no number is given the default is to use the single bottom or top results.    
If no t or b is given, the default is to find the top results.

The -m or --multiple tag will execute all the provided equations a given number of times.    
roll 2d6 -m 3    
The above will execute the equation "2d6" three times, and give back the three results added together.    
To see each individual results, add a -v or --verbose tag.
