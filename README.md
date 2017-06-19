# OpenDice
OpenDice is a small and simple die rolling app written in C

## License
OpenDice is licensed under GPL v3.0
More information can be found in the LICENCE file or at the following link
https://www.gnu.org/licenses/gpl-3.0.en.html

## How to Use
### Rolling
The core of OpenDice is the expression system.
Input is treated like a mathematical expression, reading the numbers, symbols, and characters in a way that follows order of operations.

Roll n number of x dice like so    
OpenDice ndx

So to roll 2 six sided dice    
OpenDice 2d6    
When rolling multiple dice like this each roll is added together and the user is given the total

Use multiplication and addition to modify the result    
OpenDice 2d6+2    
The above will roll two six sided dice, add the two rolls together, and then add two to that end result

Order of operations is as follows    
rolls (xdy)    
exponentiation (xey)    
multiplication (x\*y)   
addition (x+y)
subtraction (x-y)

Dice can be rolled to tell OpenDice how many dice it should roll    
OpenDice 3d6d20    
The above rolls 3d6, then rolls that many d20. This can be continued indefinitely.

### Coin
Use coin to flip a coin    
OpenDice coin    
The above will return with heads or tails

Flip x coins like so    
OpenDice coin x    
The x in the above can be any expression, not just a number


### Tags
Adding the -i tag to show individual rolls/coin flips    
OpenDice 2d6 -i    
The above will show the result of each d6

Adding the -h tag will "hide" the result, when in reality, as said by the actual program, it just waits for the user to push a key before it rolls the expression.
For those who ask "why does this feature exist?", this is suposed to emulate rolling dice under a cup. Once the roll is made there's no taking it back. The only thing you can do is remove the cup. While not a perfect emulation because the roll is made after you press the "any key", it surves it's purpose, in that after you've made your roll, you can't change it but you still don't know what it is.

## Known Issues
* dx doesn't roll a single die of type x
* There is no effort made to clean up the input before trying to read it
* The randomness is not garanteed to be cryptographically secure
* Some lines of code are indented more than three times

## Planned Features
* Fix the Known Issues
* Add Paranthasis to allow for using a non-standard order of operations
* Add -d tag to let the user delay the result (-d 1 delays the result for one second)
* Add -c tag to let the user count certain rolls
* Add help option
* Add expression saving. An expression can be saved under a name using -s, then rolled under than name with the -r tag
* Impliment a fully featured GUI
