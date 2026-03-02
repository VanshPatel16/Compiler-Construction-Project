# Group 2 - Compiler Documentation

## Design Choices and Assumptions

We have listed out all the assumptions and design choices we have made in this section. The functions differ slightly from the given blueprint, as done to fit well with our implementation choices.

- The lexer and parser work in parallel, as the parser parses the stream of tokens while the lexer is analyzing the source code. This is achieved by the parser calling the lexer to retrieve the next token tuple from the buffer, rather than storing token tuples in the memory before parsing. 
- The lexer tokenizes the comments, but the comment tokens are not parsed by the parser, as they are not a part of the Grammar.
- The lexical errors are also not parsed into the parser as they are not a part of the Grammar.
- As suggested in the announcements, we have added some keywords to the synchronization set to better error handling.



## Lexer

The implementation of the lexer is in the following three files:

- lexerDef.h : Definitions of the structures and enums.
- lexer.h : Function declarations.
- lexer.c : Function definitions.

### lexerDef.h

In this file we have defined the structures and enums used for the lexer. It has the following definitions

- #### `typedef enum token_enum Token`
	
These represent the token ids that the lexer will tokenize the input source code into
	
- #### `struct tokeninfo`

This is the output datatype of a tokenized token, as accepted by the DFA implemented by the lexer. It contains the token, respective lexeme, line number, and a numValue field that captures the numeric values if the token is a numeric type.

	```C
	typedef struct tokeninfo{
    	Token token;
    	char* lexeme;
    	int lineNo;
    	double numValue;
	} TokenInfo;
	```


- #### `struct twinbuffer`

This is the buffer used by the lexer to store the input source code. It defines the buffer as a character array, maintains the start of the next half of the buffer (tbStart), start of the current buffer (basePtr), a forward pointer that points to the start of the next lexeme (forwardPtr), a file pointer to the input source code (fp), and a boolean flag to indicate if the end of the file has been reached to handle the buffer loading accordingly (isLastChunk).

	```C
	typedef struct twinbuffer{
	    char buffer[TBSIZE];
	    int tbStart;
		int basePtr;
		int forwardPtr;
		FILE* fp;
		bool isLastChunk;
	} TwinBuffer;
	```

### lexer.c

This file implements the functions and logic of the lexer according to the DFA.

- #### `void init(const char* fileName, TwinBuffer* tb)`

	This function initializes the `TwinBuffer` by opening the source file and loading the first chunk into the buffer. It also resets the global line counter.

- #### `void getFileStream(TwinBuffer* tb)`

	Used to fill the `TwinBuffer`. It reads a chunk of data from the file and places it in the appropriate half of the buffer. It also handles the end of file condition by appending an `EOF` marker.

- #### `char getNextChar(TwinBuffer* tb)`

	Gets the next character from the buffer while maintaining the `forwardPtr`. It automatically calls `getFileStream` when the pointer crosses a buffer boundary.

- #### `TokenInfo* getNextToken(TwinBuffer* tb)`

	The overall driver function of the lexer, it tokenizes the input source code according the the DFA and returns the token tuple. Handles the lexical errors and prints the relevant error message.

- #### `Token tokenize[Category](char* lexeme, TwinBuffer* tb)`

	These functions (e.g., `tokenizeNumber`, `tokenizeIDAndKeyword`, `tokenizeLogicalOp`) implement the state transitions for the various branches of the DFA, called from the main `getNextToken` function. Created for ease of implementation.

- #### `TokenInfo* createToken(Token token, char* lexeme, int lineNo, double numValue)`

	Utility function called by `getNextToken` to create a `TokenInfo` structure.

- #### `void removeComments(const char* testcaseFile)`

	Utility function to remove comments and print the clean file on terminal.

## Parser

The implementation of the parser is in the following three files:

- parserDef.h : Definitions of the structures and enums.
- parser.h : Function declarations.
- parser.c : Function definitions.

### parserDef.h

This file contains the data structures and enums used by the parser.

- #### `typedef enum grammarSymbol`

	Enum representing both terminals and non-terminals in our grammar, along with special symbols like `EPSILON`, `DOLLAR`, `SYN` and `PARSER_ERROR`.

- #### `struct productionRule`

	This structure stores an individual grammar rule. It includes the Left Hand Side (LHS) non terminal and multiple Right Hand Side (RHS) expansions. `tlen` stores the length of each expansion, and `hasEPSILON` indicates if the rule can derive epsilon.

	```C
	typedef struct productionRule{
		#define MAXRLEN 10
		#define MAXTLEN 10
	    grammarSymbol RHS[MAXRLEN][MAXTLEN];
	    int tlen[MAXRLEN];
	    grammarSymbol LHS;
	    int rlen;
	    bool hasEPSILON;
	} productionRule;
	```

- #### `struct Grammar`

	The structure holding the grammar rules and computed sets. It includes the `parseTable` for parsing, `first` and `follow` bitmaps, and boolean flags like `isFirstComputed` used during set calculation.

	```C
	typedef struct Grammar{
	    productionRule rules[NUMRULES];
	    grammarSymbol parseTable[NUMSYMBOLS][NUMTOKENS][MAXTLEN];
	    int parseTableRuleLen[NUMSYMBOLS][NUMTOKENS];
	    bool first[NUMSYMBOLS][NUMTOKENS];
	    bool follow[NUMSYMBOLS][NUMTOKENS];
	    bool hasEPSILONFirst[NUMSYMBOLS];
	    bool isFirstComputed[NUMSYMBOLS];
	} Grammar;
	```

- #### `struct Node`

	Represents an entry in the Parse Tree. It stores both the grammar symbol and the actual token data (lexeme, line number, numeric value), along with an array of pointers to its child nodes.

	```C
	typedef struct Node{
	    char* lexeme;
	    double numValue;
	    grammarSymbol parentSymbol;
	    grammarSymbol nodeSymbol;
	    int lineNo;
	    struct Node* children[MAXTLEN]; 
	    int numChildren;
	    bool isLeafNode;
	} Node;
	```

- #### `struct Stack`

	A simple array-based stack used for non-recursive parsing. It stores `Node*` pointers representing subtrees yet to be expanded or matched.

	```C
	typedef struct stack{
	    Node* arr[MAXSTACKSIZE];
	    int currentSize;
	} Stack;
	```

### parser.c

This file contains the implementation of the parser's logic, from grammar construction to the parsing driver.

- #### `Grammar constructGrammar(char* inputFile)`

	Reads from the input grammar file to populate our `Grammar` structure with production rules.

- #### `void calculateFirstSets(Grammar* G)` and `void calculateFollowSets(Grammar* G)`

	Compute the First and Follow sets for each non-terminal, needed to populate the parse table.

- #### `void populateParseTable(Grammar* G)`

	Fills the parse table using the first and Follow sets. It also handles synchronization states (`SYN`) for panic mode recovery.

- #### `Node constructParseTree(Grammar* G, const char* inputFileName)`

	The main driver that implements the LL1 parsing algorithm using a stack to build the parse tree.

- #### `TokenInfo errorHandler(...)`

	Implements panic mode error recovery, enabling the parser to continue by synchronizing at SYN states.

- #### `void printParseTree(Node* root, Node* parent, FILE* fp)`

	Prints the constructed parse tree in the specified format (in-order traversal) for analysis.

	
## Testcases and Analysis

## References