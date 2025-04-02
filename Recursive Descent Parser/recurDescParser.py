import re
from functools import *

"""

Single Programmer Affidavit
I the undersigned promise that the submitted assignment is my own work. While I was
free to discuss ideas with others, the work contained for coding and non-coding parts
of the assignment is my own. I recognize that should this not be the case; I will be
subject to penalties as outlined in the course syllabus.
Name: Liam Hayes
Red ID: 827393886



A LL(1) recursive descent parser for validating simple expressions.

# Part A:
You would need to first write the grammar rules (non-terminals) in EBNF 
according to the token patterns and grammar rules specified in the prompt,
put the rules in a separate PDF file, see prompt. 

# Part B:
You would then write the recursive descent parsing procedures for 
validating expressions according to the rules from Part A. 
(Refer to the recursive descent parsing algorithm in lecture slides)

It implements one parsing procedure for each one of the 
non-terminals (grammar rules), starting from the top of the parse tree, 
then drilling into lower hierachical levels.

The procedures work together to handle all combinations of the grammar 
rules, and they automatically handle the nested compositions of terms 
with multi-level priority brackets. 

----------------------------------------------------------------------------
Usage (Refer to the prompt for more examples - both positive and negative cases)

r = recDecsent('7 - 17')
print(r.validate()) # will print True as '7 - 17' is a valid expression

r = recDecsent('7 - ')
print(r.validate()) # will print False as '7 - ' is an invalid expression

----------------------------------------------------------------------------
Follows are examples of valid expressions based on the expression patterns specified above:
•	7 - 17
•	> 90
•	(1 - 100 and not 50) or > 200
•	(7 - 17) or > 90
•	> 50 or == 20
•	1 - 100 and != 50
•	(5 - 100) and (not 50) or (>= 130 or (2 - 4))

Examples of invalid expressions:
•	>
•	2 - - 4
•	- 7
•	7 -
•	= 6
•	(!= 5) and
•	2 - 4 and >< 300
•	>= 5) nand < 10

"""

class recDescent:

    # IMPORTANT:
    # You MUST NOT change the signatures of 
    # the constructor, lex(self) and validate(self)
    # Otherwise, autograding tests will FAIL

    # constructor to initialize and set class level variables
    def __init__(self, expr = ""):

        # string to be parsed
        self.expr = expr

        # tokens from lexer tokenization of the expression
        self.tokens = []

        #Start position at 0
        self.pos = 0

    # lexer - tokenize the expression into a list of tokens
    # the tokens are stored in an list which can be accessed by self.tokens
    # do NOT edit any piece of code in this function

    def lex(self):
        self.tokens = re.findall("[-\(\)]|[!<>=]=|[<>]|\w+|[^ +]\W+", self.expr)
        # transform tokens to lower case, and filter out possible spaces in the tokens
        self.tokens = list(filter((lambda x: len(x)), 
                           list(map((lambda x: x.strip().lower()), self.tokens))))    
    
    # parser - determine if the input expression is valid or not
    
    # validate() function will return True if the expression is valid, False otherwise 
    # do NOT change the method signature

    #Get token from self
    def get_current_token(self):
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return None
    
    #Advance pos of token
    def advance(self):
        self.pos+= 1
    
    #Determine if there are any more tokens
    def is_at_end(self):
        return self.pos >= len(self.tokens)

    #Standard error message
    def error(self, msg=""):
        raise ValueError("Parsing error. " + msg)

    def validate(self):
        # Using the tokens from lex() tokenization,
        # this validate would first call lex() to tokenize the expression,
        # then call the top level parsing procedure for the outermost rule
        # and go from there
        
        self.lex()

        try:
            self.parse_expr()
            if not self.is_at_end():
                return False
            return True
        except ValueError:
            return False
        
    def parse_expr(self):
        
        # <expr> ::= <term> { logic_op <term> }

        self.parse_term()
        while not self.is_at_end():
            current = self.get_current_token()
            if current in ["and", "or", "nand"]:
                self.parse_logic_op()               #Parse and/or/nand
                self.parse_term()                   #Parse next <term>
            else:
                break
    
    def parse_term(self):
        
        # <term> ::= <range_expr> | <rel_expr> | “(“ <expr> “)”

        #Check if there is a current token
        if self.is_at_end():
            self.error("No term available.")
        current = self.get_current_token()

        if current == "(":
            self.advance()                          #Parse '('
            self.parse_expr()                       #Parse the sub expression
            if self.is_at_end() or self.get_current_token() != ")":
                self.error("Missing closing parenthesis.")
            self.advance()                          #Parse ')'
            return
        
        #Relop + int
        if current in ["<", ">", "<=", ">=", "==", "!=", "not"]:
            self.parse_rel_expr()
            return
        
        #int - int
        if current[0].isdigit():
            self.parse_range_expr()
            return
        
        #Return an error if none found
        self.error("Unexpected token '" + current + "' in <term>.")


    def parse_range_expr(self):

        # <range_expr> ::= <int> “-” <int>

        self.parse_int()                            #Parse first int
        if self.is_at_end() or self.get_current_token() != "-":
            self.error("Expected -.")
        self.advance()                              #Parse -
        self.parse_int()                            #Parse second int

    def parse_rel_expr(self):
        
        # <rel_expr> ::= <relop> <int>
       
        self.parse_relop()                          #Parse <relop>
        self.parse_int()                            #Parse int
        

    def parse_logic_op(self):

        # <logic_op> ::= “and” | “or” | “nand”

        current = self.get_current_token()
        if current in ["and", "or", "nand"]:
            self.advance()                          #Parse and/or/nand
        else:
            self.error("Expected logic operator.")  #Return error if and/or/nand found


    def parse_relop(self):

        # <relop> ::= “<=” | “>=” | “==” | “!=” | “<” | “>” | “not”

        #Check if there is a valid relop
        if self.is_at_end():
            self.error("Relational operator expected.")
        current = self.get_current_token()
        if current not in ["<", ">", "<=", ">=", "==", "!=", "not"]:
            self.error("Invalid relational operator.")
        self.advance()                              #Parse


    def parse_int(self):

        # <int> ::= <digit> { <digit> }

        #Parses a positive int token
        if self.is_at_end():
            self.error("No int available.")
        token = self.get_current_token()
        for vd in token:                               #Valid digit
            self.parse_digit(vd)                       #Parse valid digit
        self.advance()                                 #Next token
        
    
    def parse_digit(self, vd):

        # <digit> ::= “0” | “1” | “2” | “3” | “4” | “5” | “6” | “7” | “8” | “9”

        #Check number to make sure it is valid
        if vd not in "0123456789":
            self.error("Invalid digit")

        

    
    # TODO: Write your parsing procedures corresponding to the grammar rules - follow Figure 5.17

# sample test code, use examples from the prompt for more testing
# positive tests: validation of the expression returns True
# negative tests: validation of the expression returns False

r = recDescent('5 - 100') 
print(r.validate()) # should return True

r = recDescent('5 - ') 
print(r.validate()) # should return False