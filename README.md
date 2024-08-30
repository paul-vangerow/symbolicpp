### Code Generator Functionality ####

arguments -> build type ( parser / lexer ), input specification ( path )
(technically two seperate things but I'll use this to reuse the input logic for it)


### MATH PARSER

Tokens:

T_HEAD
T_EQUATION
T_EOL
T_EXPR
T_EQ
T_NUM
T_VAR
T_BR
T_MUL
T_DIV
T_ADD
T_SUB
T_LBR
T_RBR
T_MUL_S
T_DIV_S
T_ADD_S
T_SUB_S

General structure

T_HEAD : (T_EQUATION T_EOL)* T_EQUATION?

T_EQUATION : T_EXPR T_EQ T_EXPR

T_EXPR :  T_NUM   | 
          T_VAR   | 
          T_BR    | 
          T_MUL   | 
          T_DIV   |
          T_ADD   |
          T_SUB

T_BR   :  T_LBR T_EXPR T_RBR

T_MUL  :  T_EXPR T_MUL_S T_EXPR

T_DIV  :  T_EXPR T_DIV_S T_EXPR

T_ADD  :  T_EXPR T_ADD_S T_EXPR

T_SUB  :  T_EXPR T_SUB_S T_EXPR

