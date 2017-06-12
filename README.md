# OpenDice
OpenDice is a small and simple die rolling app written in C

## License
OpenDice is licensed under GPL v3.0
More information can be found in the LICENCE file or at the following link
https://www.gnu.org/licenses/gpl-3.0.en.html

## How to Use
The core of OpenDice is the expression system.
Input is treated like a mathematical expression, reading the numbers, sumbols, and characters in a way that follows order of operations.

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

You can roll dice to tell OpenDice how many dice it should roll    
OpenDice 3d6d20    
The above rolls 3d6, then rolls that many d20. This can be continued indefinitely.

### Tags
Adding the -i tag to show individual rolls    
OpenDice 2d6 -i    
The above will show the result of each d6

## Known Issues
* dx doesn't roll a single die of type x
* There is no effort made to clean up the input before trying to read it
* The randomness is not garanteed to be cryptographically secure
* Some lines of code are indented more than three times

## Planned Features
* Fix the Known Issues
* Add Paranthasis to allow for using a non-standard order of operations
* Add -d tag to let the user delay the result (-d 1 delays the result for one second)
* Add -h tag to let the user hide the result until they press a key
* Add coin flipping
* Add expression saving. An expression can be saved under a name using -s, then rolled under than name with the -r tag
* Impliment a fully featured GUI
