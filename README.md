_Final project in the C course in the Open University._

Assembler Compiler 
--
This project is based on the double-pass assembler model.  
___
**The Mission** 


1. Receive assembly files (.as).
2. Expend Macro then output new file (.am).  
2. Parse and interpret the files according to given syntax and semantic rules.
3. output results in three files (if compilation succeeded): 

      .ob - memory image and instructions (hex)

      .ent - symbol table

      .ext - list of external values

___
**The Guideline**

* Balancing between endless small utility function and the specific syntax and semantic rules of this assembler. 
* Trying to sperate each of the four main point in "The Mission" to independent stages (Pipeline not implemented).
___
**The Execution**

Run on all Linux based OS/VMS. (Compiled in Ubuntu)

Use makefile to compile the project like this:
```
>   make
```
After preparing assembly files **with an `.as` extension**, open *terminal* and pass file names as arguments (without the file extensions) as following:

```
# For pro.as and test.as files

> assm.c pro test
```
The assembler will first generate output file with the same filenames and the following extensions:
- `.am` - after macro expanding.

Then if the file obeying all the rules, output files:  
- `.ob` - Object file
- `.ent` - Entries file
- `.ext` - Externals file

Examples of input and output files can be found under the 'Tests' folder.
___

