/** scmpsem.h
====================================================================
This is the base head file for Semantic Analyze


====================================================================
 */
#ifndef SCMPSEM_H_INCLUDE
#define SCMPSEM_H_INCLUDE

#include "scmpstx.h"

#ifndef LINUX
/* For Windows only */
/* define the header of the MASM source file */
#define ASM_HEADER ".MODEL SMALL\n.STACK 4096h\nINCLUDE PrintVal.mac\n"
#define ASM_DATA_HEADER ".DATA\nASM_CHAGNE_LINE  DB  0DH,0AH,'$'\n"
/* the definition of varibale e.g. sprintf(buffer, MASM_VAR_DEF, "VAR1", "VAR1", "VAR1") */
#define ASM_VAR_DEF "%s  DW  0\nName_%s  DB '%s',20H,20H,'$'\n"
#define ASM_CODE_HEADER ".CODE\nCALL Main\n"
#define ASM_END_MAIN "END Main\n"
#define ASM_FILE_NAME "MyResult.asm"
#define ASM_CHANGE_LINE "LEA  AX, ASM_CHAGNE_LINE\nMOV  DX, AX\nMOV  AH, 9\nINT  21H\n"

#else
/* For Linux only */
#endif

#define MAX_STACK_NUM 32768               /* the max node number of sem_stack */
typedef struct tag_SEM_STACK_NODE{
    unsigned short n_mode;                /*====================================
                                          Type of the node
                                          0 ==> )
                                          1 ==> ||
                                          2 ==> &&
                                          3 ==> ==   !=
                                          4 ==> >   >=   <   <=
                                          5 ==> +  -
                                          6 ==> *  /
                                          7 ==> (
                                          0xffff ==> number
                                          =====================================*/
    short n_value;                        /*=====================================
                                          The value of the node,using definition
                                          in the scmpstx.h.
                                          e.g.
                                          SYN_NODE_NUMBER   24   numbers
                                          SYN_NODE_EQ       35   '=='
                                          =====================================*/
}SEM_STACK_NODE;
typedef struct tag_SEM_STACK{
    unsigned int n_num;
    SEM_STACK_NODE * sem_node_list;
}SEM_STACK;
typedef struct tag_SemEnv{
    unsigned int n_SignCurrentId;         /* the id of current sign */
    FILE * pAsmFile;                      /* the pointer to the output ASM file */
    SEM_STACK s_sem_stack;                /* the stack for semantic analyze */
}SemEnv;
SemEnv g_SemEnv;/* the semantic analyser environment */

int initialize_SemEnv();
int finalize_SemEnv();
int semantic_analyzer();
void debug_sem( );


#define CHECK_SYN_NODE_ID(id)  \
    if( INVALID_ID == (id) ){\
          printf("Internal Error ! Invalid syntax node id at line '%d'.\n", __LINE__);\
          return 0;\
    }
          

#endif
