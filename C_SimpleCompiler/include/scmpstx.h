/** scmpstx.h
====================================================================
This is the base head file for Syntax Analyze


====================================================================
 */
#ifndef SCMPSTX_H_INCLUDE
#define SCMPSTX_H_INCLUDE


#ifndef STD_COMMON_H_INCLUDE
#define STD_COMMON_H_INCLUDE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#endif

#define MAX_SYN_NODE_NUM  32768          /* the max syntax node number */
#define MAX_ID_LENGTH     32             /* the max length of indentifier */
/* The identifier id */
typedef struct tag_identifier_id{
    short n_mode;                        /* the mode : SYN_NODE_TAIL / SYN_NODE_VARID / SYN_NODE_FUNID */
    short dummy;                         /* not used */
    int n_next_identifier_id;            /* the next identifier in current second-level list */
    char str_name[ MAX_ID_LENGTH+1 ];
}IdentId;
typedef IdentId * IdentId_P;
/* The identifier table */
typedef struct tag_id_table{
    unsigned int n_id_num;               /*============================================================
                                         The number of identifiers in the table.
                                         It's also the first free place for store a new identifier.
                                         Using n_id_num,  pIdList acts as a stack, and n_id_num is
                                         it's stack top.
                                         If n_id_num >= MAX_SYN_NODE_NUM, the stack is full.
                                         ============================================================*/
    IdentId_P pIdList;                   /*============================================================
                                         the head pointer of the identifier list.
                                         malloc MAX_SYN_NODE_NUM IdentId.
                                         ============================================================*/
    unsigned int two_level_index[52][62];        /*============================================================
                                         First index: using the first byte of the name
                                             'A' to 'Z' = 0 to 25
                                             'a' to 'z' = 26 to 51
                                         Second index: using the second byte of the name
                                             'A' to 'Z' = 0 to 25
                                             'a' to 'z' = 26 to 51
                                             '0' to '9' = 52 to 61
                                         Default value: two_level_index[i][j] = -1;
                                         if two_level_index[i][j] is not -1, it shows this second-level
                                         list in the pIdList is started at pIdList[two_level_index[i][j]]
                                         ============================================================*/            
}IdTable;
IdTable g_IdTable;                       /* the identifier table */
/* The node for syntax tree */
typedef struct tag_SyntaxNode{
    short n_mode;                        /* the mode of SyntaxNode */
    unsigned int n_identifier_id;        /*============================================================
                                         The index in the Id Table for this identifier.
                                         If it's not identifier, n_identifier_id = INVALID_ID;
                                         ============================================================*/
    unsigned int n_left_brother_id;      /*============================================================
                                         The index of the left brother in the syntax tree.
                                         If it's the first son of its father, n_left_brother_id = INVALID_ID;
                                         ============================================================*/
    unsigned int n_right_brother_id;     /*============================================================
                                         The index of the right brother in the syntax tree.
                                         If it's the last son of its father, n_right_brother_id = INVALID_ID;
                                         ============================================================*/
    unsigned int n_father_id;            /*============================================================
                                         The index of the father in the syntax tree.
                                         If it's the root, n_father_id = INVALID_ID;
                                         ============================================================*/
    unsigned int n_son_id;               /*============================================================
                                         The index of the son in the syntax tree.
                                         If it has no son, n_son_id = INVALID_ID;
                                         ============================================================*/
    int n_int_value;                       /* the value of integer */
}SynNode;
typedef SynNode * SynNode_P;
typedef struct tat_SyntaxTree{
    unsigned int n_node_num;             /*============================================================
                                         The number of SynNode in the SyntaxTree.
                                         It's also the first free place for store a new SynNode.
                                         Using n_node_num,  pSynNodeList acts as a stack, and n_node_num is
                                         it's stack top.
                                         If n_node_num >= MAX_SYN_NODE_NUM, the stack is full.
                                         ============================================================*/
    SynNode_P pSynNodeList;              /*============================================================
                                         the head pointer of the syntax tree node list.
                                         malloc MAX_SYN_NODE_NUM SynNode.
                                         ============================================================*/
}SynTree;
SynTree g_SynTree;                       /* the syntax tree */
/* Syntax Node mode definitions */
#define SYN_NODE_TAIL                 0          /* the tail node */
#define SYN_NODE_HEAD                 1          /* the head node */
#define SYN_NODE_VARID                2          /* the identifier of variabale */
#define SYN_NODE_FUNID                3          /* the identifier of function */
#define SYN_NODE_MAINFUNID            4          /* the identifier of main function */
#define SYN_NODE_BODY                 11         /* the BODY of a function */
#define SYN_NODE_DEFINITION           12         /* DEFINITION */
#define SYN_NODE_VAR_DEF              13         /* VAR_DEF */
#define SYN_NODE_FUNC_DEF             14         /* FUNC_DEF */
#define SYN_NODE_VAR_DEFINITION       15         /* VAR_DEFINITION */
#define SYN_NODE_STATEMENT_LIST       16         /* STATEMENT_LIST */
#define SYN_NODE_ASSIGN_STMT          17         /* ASSIGN_STMT */
#define SYN_NODE_IF_STMT              18         /* IF_STMT */
#define SYN_NODE_FOR_STMT             19         /* FOR_STMT */
#define SYN_NODE_WHILE_STMT           20         /* WHILE_STMT */
#define SYN_NODE_FORMULA              21         /* FORMULA */
#define SYN_NODE_BOOL_FORMULA         22         /* BOOL_FORMULA */
#define SYN_NODE_FORMULA_FOLLOW       23         /* FORMULA_FOLLOW */
#define SYN_NODE_NUMBER               24         /* numbers */
#define SYN_NODE_L_parenthesis        25         /* '(' */
#define SYN_NODE_R_parenthesis        26         /* ')' */
#define SYN_NODE_ADD                  27         /* '+' */
#define SYN_NODE_SUB                  28         /* '-' */
#define SYN_NODE_MUT                  29         /* '*' */
#define SYN_NODE_DIV                  30         /* '/' */
#define SYN_NODE_AND                  31         /* '&&' */
#define SYN_NODE_OR                   32         /* '||' */
#define SYN_NODE_LESS                 33         /* '<' */
#define SYN_NODE_MORE                 34         /* '>' */
#define SYN_NODE_EQ                   35         /* '==' */
#define SYN_NODE_NOT_LESS             36         /* '>=' */
#define SYN_NODE_NOT_MORE             37         /* '<=' */
#define SYN_NODE_NOT_EQ               38         /* '!=' */
#define SYN_NODE_BOOL_FORMULA_FOLLOW  39         /* BOOL_FORMULA_FOLLOW */
#define SYN_NODE_FUNC_CALL            40         /* FUNC_CALL */
#define SYN_NODE_EMPTY               100         /* An empty node */

#define SHORT_MAX                    32767       /* MAX VALUE OF SHORT */
#define SHORT_MIN                    -32768      /* MIN VALUE OF SHORT */

#define INVALID_ID            (unsigned int)0xFFFFFFFF         /* the invalid index */

typedef struct tag_fileField{
    FILE * fp;                                   /* the pointer to the source file */
    int n_current_line;                          /* the current line number */
    char current_ch;                             /* the current char */
    char name_buf[MAX_ID_LENGTH+1];              /* the buffer for an identifier name */
}FileField;

#define KEY_WORD_NO                   0          /* Not a key word */
#define KEY_WORD_MAIN                 1          /* 'main' */
#define KEY_WORD_INT                  2          /* 'int' */
#define KEY_WORD_IF                   3          /* 'if' */
#define KEY_WORD_WHILE                4          /* 'while' */
#define KEY_WORD_FOR                  5          /* 'for' */

#define CHECK_KEY_WORD( buffer, type ) \
    { \
        if( 0 == strcmp( buffer, "main" ) ){\
            type = KEY_WORD_MAIN;\
        } else if( 0 == strcmp( buffer, "int" ) ){\
            type = KEY_WORD_INT;\
        } else if( 0 == strcmp( buffer, "if" ) ){\
            type = KEY_WORD_IF;\
        } else if( 0 == strcmp( buffer, "while") ){\
            type = KEY_WORD_WHILE;\
        } else if( 0 == strcmp( buffer, "for") ){\
            type = KEY_WORD_FOR;\
        } else{\
            type = KEY_WORD_NO;\
        }\
    }
#define MY_HELP_FREE( pValue )  \
    if( NULL != pValue ){\
        free(pValue);\
        pValue = NULL;\
    }
#define MY_HELP_CLOSE( pFILE )  \
    if( NULL != pFILE ){\
        fclose(pFILE);\
        pFILE = NULL;\
    }
#define CHECK_NEW_LINE( fileField ) \
    if( '\n' == fileField->current_ch ){\
        fileField->n_current_line++;\
    }
#define JUMP_SPACES( fileField ) \
    {\
        while( isspace( fileField->current_ch ) ){\
            fileField->current_ch = fgetc( fileField->fp );\
            CHECK_NEW_LINE( fileField )\
        }\
    }
#define GET_IDENTIFY( fileField, buffer, length )\
    {\
        length = 0;\
        JUMP_SPACES( fileField );\
        if( !isalpha(fileField->current_ch) ){/* the fisrt char must be alphabet */\
            buffer[0] = '\0';\
            length = 0;\
        } else{\
            buffer[0] = fileField->current_ch;\
            length = 1;\
            while( 1 ){\
                fileField->current_ch = fgetc( fileField->fp );\
                CHECK_NEW_LINE( fileField )\
                if( isalnum( fileField->current_ch ) ){\
                    buffer[length] = fileField->current_ch;\
                    length++;\
                    if( length > MAX_ID_LENGTH ){/* too logn identifier */\
                        buffer[0] = '\0';\
                        length = 0;\
                        break;\
                    }\
                } else{\
                    buffer[length] = '\0';\
                    break;\
                }\
            }\
        }\
    }
#define GET_NEW_CHAR_JUMP_SPACE( fileField ) \
    {\
        fileField->current_ch = fgetc( fileField->fp );\
        CHECK_NEW_LINE( fileField )\
        JUMP_SPACES( fileField )\
    }
#define CHECK_NODE_NUM( function, fileField, num ) \
    {\
        if( num > MAX_SYN_NODE_NUM ){\
            report_syntax_error( function, "Too many syntax nodes.", fileField->n_current_line );\
            return 0;\
        }\
    }
#define FIX_FATHER_LBROTHER( father_ID, left_brother_ID, current_ID ) \
    {\
        if( father_ID != INVALID_ID ){\
            if( INVALID_ID == g_SynTree.pSynNodeList[father_ID].n_son_id ){\
                g_SynTree.pSynNodeList[father_ID].n_son_id = current_ID;\
            }\
        }\
        if( left_brother_ID != INVALID_ID ){\
            if( INVALID_ID == g_SynTree.pSynNodeList[left_brother_ID].n_right_brother_id ){\
                g_SynTree.pSynNodeList[left_brother_ID].n_right_brother_id = current_ID;\
            }\
        }\
    }
int syntax_analyzer(FileField *fileField);
int initialize_IdTable();
int initialize_SynTree();
int release_IdTable();
int release_SynTree();
void report_syntax_error(char *func_name, char *detail, int line_number);
void report_memery_error(char *func_name, int line_number);
void report_internal_error(char *func_name, int line_number);
int add_to_IdTable(FileField * fileField, char *id_name, int length, short mode, unsigned int * p_ident_index);
int find_in_IdTable(FileField * fileField, char *id_name, int length, unsigned int * p_ident_index);
int get_number( FileField * fileField, int * pOutValue);
void debug_syntax( FILE* fp );

#endif
