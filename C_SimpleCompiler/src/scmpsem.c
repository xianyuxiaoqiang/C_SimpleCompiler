/** scmpsem.c
====================================================================
This is the base source file for Semantic Analyze


====================================================================
 */

#include "scmpsem.h"
int sem_deal_with_header();
int sem_deal_with_data();
int sem_deal_with_code();
int sem_deal_with_DEFINITION(SynNode_P pSynNode);
int sem_deal_with_BODY(SynNode_P pSynNode);
int sem_deal_with_FUNC_DEF(SynNode_P pSynNode);
int sem_deal_with_STATEMENT_LIST(SynNode_P pSynNode);
int sem_deal_with_ASSIGN_STMT(SynNode_P pSynNode);
int sem_deal_with_IF_STMT(SynNode_P pSynNode);
int sem_deal_with_FOR_STMT(SynNode_P pSynNode);
int sem_deal_with_WHILE_STMT(SynNode_P pSynNode);
int sem_deal_with_FUNC_CALL(SynNode_P pSynNode);
int sem_deal_with_FORMULA(SynNode_P pSynNode);
int sem_deal_with_BOOL_FORMULA(SynNode_P pSynNode);

static int push( SEM_STACK_NODE *in_node );
static int pop( SEM_STACK_NODE *out_node );
#define MY_PUSH( p_in_node )\
    {\
        if( 0 == push( p_in_node ) ){\
           printf("POP error at line '%d' \n", __LINE__ );\
           return 0;\
        }\
    }
#define MY_POP( p_out_node )\
    {\
        if( 0 == pop( p_out_node ) ){\
           printf("POP error at line '%d' \n", __LINE__ );\
           return 0;\
        }\
    }
static int check_caculate( SEM_STACK_NODE *cur_node );
static int caculate_AX_BX_by_op( SEM_STACK_NODE *op_node );

int initialize_SemEnv(){
    g_SemEnv.n_SignCurrentId = 0;
    g_SemEnv.pAsmFile = NULL;
    fopen_s( &g_SemEnv.pAsmFile, ASM_FILE_NAME , "w" );
    if( NULL == g_SemEnv.pAsmFile ){
        printf("Can't open source file '%s'. exit.\n", ASM_FILE_NAME );
        return 0;
    }
    g_SemEnv.s_sem_stack.n_num = 0;
    g_SemEnv.s_sem_stack.sem_node_list = ( SEM_STACK_NODE * )malloc( sizeof(SEM_STACK_NODE) * MAX_STACK_NUM );
    if( NULL == g_SemEnv.s_sem_stack.sem_node_list ){
        printf("Memery not enough while initializing semantic stack.\n" );
        return 0;
    }
    return 1;
}
int finalize_SemEnv(){
    g_SemEnv.n_SignCurrentId = 0;
    MY_HELP_CLOSE( g_SemEnv.pAsmFile );
    g_SemEnv.s_sem_stack.n_num = 0;
    MY_HELP_FREE( g_SemEnv.s_sem_stack.sem_node_list );
    return 1;
}


int semantic_analyzer(){
    int ret = 0;
    char * func_name="semantic_analyzer";

    ret = initialize_SemEnv();
    if( 0 == ret ){
        printf("semantic_analyzer exit with initialize error.\n");
        finalize_SemEnv();
        return 0;
    }
    /* [ 1 ] deal with header */
    sem_deal_with_header();
    /* [ 2 ] deal with data */
    ret = sem_deal_with_data();
    if( 0 == ret ){
        printf("semantic_analyzer exit with internal error.\n");
        finalize_SemEnv();
        return 0;
    }
    /* [ 3 ] deal with code */
    ret =  sem_deal_with_code();
    if( 0 == ret ){
        printf("semantic_analyzer exit with internal error.\n");
        finalize_SemEnv();
        return 0;
    }
    /* [ 4 ] deal with Ending */
    fprintf( g_SemEnv.pAsmFile, ASM_END_MAIN );

    finalize_SemEnv();
    return 1;
}
int sem_deal_with_header(){
    fprintf( g_SemEnv.pAsmFile, ASM_HEADER );

    return 1;
}
int sem_deal_with_data(){
    unsigned int index = 0;
    char * p_VarName = NULL;

    /* [ 1 ] data header */
    fprintf( g_SemEnv.pAsmFile, ASM_DATA_HEADER );
    /* [ 2 ] Variable definition */
    for( index = 0; index < g_IdTable.n_id_num; index++ ){
        if( SYN_NODE_VARID == g_IdTable.pIdList[ index ].n_mode ){
            p_VarName = g_IdTable.pIdList[ index ].str_name;
            fprintf( g_SemEnv.pAsmFile, ASM_VAR_DEF, p_VarName, p_VarName, p_VarName );
        }
    }

    return 1;
}
int sem_deal_with_code(){
    int ret = 0;
    SynNode_P pCurSynNode = NULL;

    /* [ 1 ] code header */
    fprintf( g_SemEnv.pAsmFile, ASM_CODE_HEADER );
    /* [ 2 ] deal with DEFINITION */
    /* [ 2.1 ] Get the first node of DEFINITION */
    pCurSynNode = &g_SynTree.pSynNodeList[1];
    ret = sem_deal_with_DEFINITION( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 3 ] deal with main */
    /* [ 3.1 ] Get the node of 'main' */
    fprintf( g_SemEnv.pAsmFile, "Main  PROC\nMOV AX, DGROUP\nMOV DS, AX\n" );
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    /* [ 3.2 ] Get the node of 'BODY' */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_BODY( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
#ifdef DEBUG_SEM
    debug_sem();
#endif
    /* [ 3.3 ] Tail of 'main' */
    fprintf( g_SemEnv.pAsmFile, "MOV  AX, 4C00H\nINT  21H\nMain  ENDP\n" );
    return 1;
}
int sem_deal_with_DEFINITION( SynNode_P pSynNode ){
    SynNode_P pCurSynNode = NULL;
    int ret = 0;

    /* [ 1 ] Get the first node of DEFINITION */
    pCurSynNode = pSynNode;
    while( pCurSynNode != NULL && pCurSynNode->n_son_id != INVALID_ID ){
        /* [ 1.1 ] check the son */
        pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_son_id ];
        if( SYN_NODE_FUNC_DEF == pCurSynNode->n_mode ){
            /* [ 1.1.1 ] it's FUNC definition, deal with it */
            ret = sem_deal_with_FUNC_DEF( pCurSynNode );
            if( 0 == ret ){
                return 0;
            }

        }
            /* [ 1.1.2 ] deal with right brother of it */
        if( pCurSynNode->n_right_brother_id != INVALID_ID ){
            pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
        } else{
            pCurSynNode = NULL;
        }
    }
    return 1;
}
int sem_deal_with_FUNC_DEF(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    IdentId_P pId = NULL;
    int ret = 0;
    /* [ 1 ] Function Header */
    /* [ 1.1 ] Get the function id */
    CHECK_SYN_NODE_ID( pSynNode->n_son_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    CHECK_SYN_NODE_ID( pCurSynNode->n_identifier_id )
    pId = &g_IdTable.pIdList[ pCurSynNode->n_identifier_id ];
    fprintf( g_SemEnv.pAsmFile, "%s  PROC\n", pId->str_name );
    /* [ 1.2 ] Save the current Registers */
    fprintf( g_SemEnv.pAsmFile, "PUSH  BP\n", pId->str_name );
    fprintf( g_SemEnv.pAsmFile, "MOV  BP, SP\n", pId->str_name );
    fprintf( g_SemEnv.pAsmFile, "PUSH  DS\n", pId->str_name );
    /* [ 2 ] deal with 'BODY' */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_BODY( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 3 ] Function Tail */
    /* [ 3.1 ] Backup the current Registers */
    fprintf( g_SemEnv.pAsmFile, "POP  DS\n", pId->str_name );
    fprintf( g_SemEnv.pAsmFile, "MOV  SP, BP\n", pId->str_name );
    fprintf( g_SemEnv.pAsmFile, "POP  BP\n", pId->str_name );
    fprintf( g_SemEnv.pAsmFile, "RET\n%s  ENDP\n", pId->str_name );
    return 1;
}
int sem_deal_with_BODY(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    int ret = 0;
    
    /* [ 1 ] get node of 'VAR_DEFINITION' */
    CHECK_SYN_NODE_ID( pSynNode->n_son_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    /* [ 2 ] deal with  'STATEMENT_LIST' */
    if( INVALID_ID != pCurSynNode->n_right_brother_id ){
        pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
        ret = sem_deal_with_STATEMENT_LIST( pCurSynNode );
        if( 0 == ret ){
            return 0;
        }
    }
    return 1;
}
int sem_deal_with_STATEMENT_LIST(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    int ret = 0;

    if( INVALID_ID == pSynNode->n_son_id ){
        /* empty statement list */
        return 1;
    }
    /* [ 1 ] deal with first statement */
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    switch( pCurSynNode->n_mode ){
        case SYN_NODE_ASSIGN_STMT:
            ret = sem_deal_with_ASSIGN_STMT( pCurSynNode );
            break;
        case SYN_NODE_IF_STMT:
            ret = sem_deal_with_IF_STMT( pCurSynNode );
            break;
        case SYN_NODE_FOR_STMT:
            ret = sem_deal_with_FOR_STMT( pCurSynNode );
            break;
        case SYN_NODE_WHILE_STMT:
            ret = sem_deal_with_WHILE_STMT( pCurSynNode );
            break;
        case SYN_NODE_FUNC_CALL:
            ret = sem_deal_with_FUNC_CALL( pCurSynNode );
            break;
    }
    if( 0 == ret ){
        return 0;
    }
    /* [ 2 ] deal with following statement */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_STATEMENT_LIST( pCurSynNode );
    return ret;
}
int sem_deal_with_ASSIGN_STMT(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    int ret = 0;
    IdentId_P pId = NULL;
    /* [ 1 ] get the var id */
    CHECK_SYN_NODE_ID( pSynNode->n_son_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    CHECK_SYN_NODE_ID( pCurSynNode->n_identifier_id )
    pId = &g_IdTable.pIdList[ pCurSynNode->n_identifier_id ];

    /* [ 2 ] deal with FORMULA */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_FORMULA( pCurSynNode );
    if( 0 == ret ){
        return ret;
    }
    /* [ 3 ] deal with assign */
    fprintf( g_SemEnv.pAsmFile, "MOV  %s, AX\n", pId->str_name );

    return 1;
}
int sem_deal_with_IF_STMT(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    int ret = 0;
    char str_cur_sign[20];
    /* [ 1 ] deal with BOOL_FORMULA */
    CHECK_SYN_NODE_ID( pSynNode->n_son_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    ret = sem_deal_with_BOOL_FORMULA( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 2 ] make a sign name. if AX is 0, jump to it. */
    sprintf_s( str_cur_sign, sizeof(str_cur_sign), "SIGN_%d", g_SemEnv.n_SignCurrentId );
    g_SemEnv.n_SignCurrentId++;
    fprintf( g_SemEnv.pAsmFile, "CMP  AX,0\nJE %s\n",  str_cur_sign );
    /* [ 3 ] deal with STATEMENT_LIST */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_STATEMENT_LIST( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 4 ] make the sign at the end */
    fprintf( g_SemEnv.pAsmFile, "%s:\n",  str_cur_sign );
    return 1;
}
int sem_deal_with_FOR_STMT(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    SynNode_P pSecondAssign = NULL;
    int ret = 0;
    char str_first_sign[20];
    char str_second_sign[20];
    /* [ 1 ] deal with ASSIGN_STMT */
    CHECK_SYN_NODE_ID( pSynNode->n_son_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    if( SYN_NODE_ASSIGN_STMT == pCurSynNode->n_mode ){
        ret = sem_deal_with_ASSIGN_STMT( pCurSynNode );
        if( 0 == ret ){
            return 0;
        }
    }
    /* [ 2 ] make a sign before BOOL_FORMULA */
    sprintf_s( str_first_sign, sizeof(str_first_sign), "SIGN_%d", g_SemEnv.n_SignCurrentId );
    g_SemEnv.n_SignCurrentId++;
    fprintf( g_SemEnv.pAsmFile, "%s:\n",  str_first_sign );
    /* [ 3 ] deal with BOOL_FORMULA */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_BOOL_FORMULA( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 4 ] make a sign at the end, if AX=0,jump to it */
    sprintf_s( str_second_sign, sizeof(str_first_sign), "SIGN_%d", g_SemEnv.n_SignCurrentId );
    g_SemEnv.n_SignCurrentId++;
    fprintf( g_SemEnv.pAsmFile, "CMP  AX,0\nJE %s\n",  str_second_sign );
    /* [ 5 ] store the second Assign Statement node */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    pSecondAssign = pCurSynNode;
    /* [ 6 ] deal with STATEMENT_LIST */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_STATEMENT_LIST( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 7 ] deal with the second Assign Statement */
    if( SYN_NODE_ASSIGN_STMT == pSecondAssign->n_mode ){
        ret = sem_deal_with_ASSIGN_STMT( pSecondAssign );
        if( 0 == ret ){
            return 0;
        }
    }
    /* [ 8 ] jump to first sign */
    fprintf( g_SemEnv.pAsmFile, "JMP %s\n",  str_first_sign );
    /* [ 9 ] store to second sign */
    fprintf( g_SemEnv.pAsmFile, "%s:\n",  str_second_sign );

    return 1;
}
int sem_deal_with_WHILE_STMT(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    int ret = 0;
    char str_first_sign[20];
    char str_second_sign[20];
    /* [ 1 ] make a sign before BOOL_FORMULA */
    sprintf_s( str_first_sign, sizeof(str_first_sign), "SIGN_%d", g_SemEnv.n_SignCurrentId );
    g_SemEnv.n_SignCurrentId++;
    fprintf( g_SemEnv.pAsmFile, "%s:\n",  str_first_sign );
    /* [ 2 ] deal with BOOL_FORMULA */
    CHECK_SYN_NODE_ID( pSynNode->n_son_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    ret = sem_deal_with_BOOL_FORMULA( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 3 ] make a sign at the end, if AX=0,jump to it */
    sprintf_s( str_second_sign, sizeof(str_first_sign), "SIGN_%d", g_SemEnv.n_SignCurrentId );
    g_SemEnv.n_SignCurrentId++;
    fprintf( g_SemEnv.pAsmFile, "CMP  AX,0\nJE %s\n",  str_second_sign );
    /* [ 4 ] deal with STATEMENT_LIST */
    CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
    ret = sem_deal_with_STATEMENT_LIST( pCurSynNode );
    if( 0 == ret ){
        return 0;
    }
    /* [ 5 ] jump to first sign */
    fprintf( g_SemEnv.pAsmFile, "JMP %s\n",  str_first_sign );
    /* [ 6 ] store to second sign */
    fprintf( g_SemEnv.pAsmFile, "%s:\n",  str_second_sign );

    return 1;
}
int sem_deal_with_FUNC_CALL(SynNode_P pSynNode){
    SynNode_P pCurSynNode = NULL;
    IdentId_P pId = NULL;

    CHECK_SYN_NODE_ID( pSynNode->n_son_id )
    pCurSynNode = &g_SynTree.pSynNodeList[ pSynNode->n_son_id ];
    CHECK_SYN_NODE_ID( pCurSynNode->n_identifier_id )
    pId = &g_IdTable.pIdList[ pCurSynNode->n_identifier_id ];
    fprintf( g_SemEnv.pAsmFile, "CALL  %s\n", pId->str_name );

    return 1;
}
int sem_deal_with_FORMULA(SynNode_P pSynNode){
    char * func_name="sem_deal_with_FORMULA";
    SynNode_P pCurSynNode = NULL;
    IdentId_P pId = NULL;
    int ret = 0;
    unsigned int n_CurrentNodeId = 0;
    SEM_STACK_NODE sem_node;
    SEM_STACK_NODE op_node;
    n_CurrentNodeId = pSynNode->n_son_id;
    while( 1 ){
        /* [ 1 ] check the first Node of the Formula */
        CHECK_SYN_NODE_ID( n_CurrentNodeId )
        pCurSynNode = &g_SynTree.pSynNodeList[ n_CurrentNodeId ];
        /* [ 1.1 ] deal with '(' FORMULA ')' */
        if( SYN_NODE_L_parenthesis == pCurSynNode->n_mode ){
            /* [ 1.1.1 ] push '(' into Stack */
            sem_node.n_mode = 7;
            sem_node.n_value = SYN_NODE_L_parenthesis;
            MY_PUSH( &sem_node );
            /* [ 1.1.2 ] deal with FORMULA recursively */
            CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
            pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
            ret = sem_deal_with_FORMULA( pCurSynNode );
            if( 0 == ret ){
                return 0;
            }
            /* [ 1.1.3 ] deal with ')' */
            CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
            pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
            if( SYN_NODE_R_parenthesis != pCurSynNode->n_mode ){
                report_internal_error( func_name, __LINE__ );
                printf("Missing ')' in FORMULA .\n" );
                return 0;
            }
            /* [ 1.1.4 ] caculate current formula between '(' and ')' */
            op_node.n_mode = 0;
            op_node.n_value = SYN_NODE_R_parenthesis;
            ret = check_caculate( &op_node );
            if( 0 == ret ){
                return 0;
            }
        } else if( SYN_NODE_VARID == pCurSynNode->n_mode ){
        /* [ 1.2 ] deal with identifier */
            /* [ 1.2.1 ] make a sem node for the identifier */
            sem_node.n_mode = 0xffff;
            MY_PUSH( &sem_node );
            /* [ 1.2.2 ] make ASM code for the identifier */
            CHECK_SYN_NODE_ID( pCurSynNode->n_identifier_id )
            pId = &g_IdTable.pIdList[ pCurSynNode->n_identifier_id ];
            fprintf( g_SemEnv.pAsmFile, "MOV AX, %s\n", pId->str_name );
            fprintf( g_SemEnv.pAsmFile, "PUSH AX\n" );
        } else {
        /* [ 1.3 ] deal with number */
            /* [ 1.3.1 ] make a sem node for the number */
            sem_node.n_mode = 0xffff;
            MY_PUSH( &sem_node );
            /* [ 1.3.2 ] make ASM code for the number */
            fprintf( g_SemEnv.pAsmFile, "MOV AX, %d\n", pCurSynNode->n_int_value );
            fprintf( g_SemEnv.pAsmFile, "PUSH AX\n" );
        }
        /* [ 2 ]  deal with FORMULA_FOLLOW */
        CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
        pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
        if( INVALID_ID == pCurSynNode->n_son_id ){
            /*=================================================================
            Empty FORMULA_FOLLOW.Clear the stack and finish the caculating 
            =================================================================*/
            while( 1 ){
                MY_POP( &sem_node );
                if( 0 == g_SemEnv.s_sem_stack.n_num ){
                    /* This node is the only node in the stack , finished. */
                    fprintf( g_SemEnv.pAsmFile, "POP AX\n" );
                    return 1;
                }
                MY_POP( &op_node );
                if( 7 == op_node.n_mode ){/* Reach the Beginning of a FOMULA */
                    MY_PUSH( &op_node );
                    MY_PUSH( &sem_node );
                    return 1;
                } else{
                    MY_POP( &sem_node );
                    ret = caculate_AX_BX_by_op( &op_node );
                }
                /* Push the result */
                MY_PUSH( &sem_node );
                if( 0 == ret ){
                    return 0;
                }
                if( 0 == g_SemEnv.s_sem_stack.n_num ){
                    /* This node is the only node in the stack , finished. */
                    fprintf( g_SemEnv.pAsmFile, "POP AX\n" );
                    return 1;
                }
            }
        } else{
            /* [ 2.1 ] deal with operator */
            pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_son_id ];
            switch( pCurSynNode->n_mode ){
                case SYN_NODE_ADD:
                case SYN_NODE_SUB:
                    op_node.n_mode = 5;
                    break;
                case SYN_NODE_MUT:
                case SYN_NODE_DIV:
                    op_node.n_mode = 6;
                    break;
                case SYN_NODE_AND:
                    op_node.n_mode = 2;
                    break;
                case SYN_NODE_OR:
                    op_node.n_mode = 1;
                    break;
                case SYN_NODE_LESS:
                case SYN_NODE_MORE:
                case SYN_NODE_NOT_LESS:
                case SYN_NODE_NOT_MORE:
                    op_node.n_mode = 4;
                    break;
                case SYN_NODE_EQ:
                case SYN_NODE_NOT_EQ:
                    op_node.n_mode = 3;
                    break;
                default:
                    printf("Internal error! invalid operator mode '%d' at line '%d'\n", pCurSynNode->n_mode, __LINE__ );
                    return 0;
            }
            op_node.n_value = pCurSynNode->n_mode;
            ret = check_caculate( &op_node );
            if( 0 == ret ){
                return 0;
            }
            /* [ 2.2 ] deal with FORMULA using the while-looping */
            CHECK_SYN_NODE_ID( pCurSynNode->n_right_brother_id )
            pCurSynNode = &g_SynTree.pSynNodeList[ pCurSynNode->n_right_brother_id ];
            n_CurrentNodeId = pCurSynNode->n_son_id;
        }
    }

    return 1;
}
int sem_deal_with_BOOL_FORMULA(SynNode_P pSynNode){
    if( sem_deal_with_FORMULA( pSynNode ) == 0 ){
        return 0;
    }
    return 1;
}
static int push( SEM_STACK_NODE *in_node ){
    unsigned int num = 0;
    num = g_SemEnv.s_sem_stack.n_num;
    if( num >= MAX_STACK_NUM ){
        printf("Internal Error ! semantic stack overflow!\n");
        return 0;
    }
    memcpy( &g_SemEnv.s_sem_stack.sem_node_list[ num ], in_node, sizeof( SEM_STACK_NODE ) );
    g_SemEnv.s_sem_stack.n_num++;
    return 1;
}
static int pop( SEM_STACK_NODE *out_node ){
    unsigned int index = 0;
    index = g_SemEnv.s_sem_stack.n_num;
    index--;
    if( index >= MAX_STACK_NUM ){
        printf("Internal Error ! semantic stack empty!\n");
        return 0;
    }
    memcpy( out_node, &g_SemEnv.s_sem_stack.sem_node_list[ index ], sizeof( SEM_STACK_NODE ) );
    g_SemEnv.s_sem_stack.n_num--;
    return 1;
}
static int check_caculate( SEM_STACK_NODE *cur_node ){
    unsigned int num = 0;
    unsigned short cur_mode = cur_node->n_mode;
    unsigned short last_mode = 0;
    SEM_STACK_NODE sem_node;
    SEM_STACK_NODE op_node;
    num = g_SemEnv.s_sem_stack.n_num;
    if( 0 == cur_mode ){
        /* [ 1 ] deal with ')' */
        /* [ 1.1 ] pop the first node on top of the stack */
        MY_POP( &sem_node );
        if( 0xffff != sem_node.n_mode ){
            printf("Internal Error ! Missing number between '(' and ')' in FORMULA .\n" );
            return 0;
        }
        /* [ 1.2 ] pop the next node on top of the stack */
        MY_POP( &op_node );
        if( 7 == op_node.n_mode ){/* '(' */
            /* push the result of FORMULA between '(' and ')' */
            MY_PUSH( &sem_node );
        } else{/* operator */
            /* [ 1.2 ] pop the second number on top of the stack */
            MY_POP( &sem_node );
            /* [ 1.3 ] caculate current formula between '(' and ')' */
            if( 0 == caculate_AX_BX_by_op( &op_node ) ){
                return 0;
            }
            /* [ 1.4 ] pop the '(' */
            MY_POP( &op_node );
            /* [ 1.5 ] push the result of FORMULA between '(' and ')' */
            MY_PUSH( &sem_node );
        }
    } else{
        /* [ 2 ] deal with other operators */
        if( num >= 2 && num <= MAX_STACK_NUM ){
            last_mode = g_SemEnv.s_sem_stack.sem_node_list[ num-2 ].n_mode;
        }
        if( cur_mode <= last_mode && last_mode != 7 ){/* '(' can't caculate without ')' */
            MY_POP( &sem_node );
            MY_POP( &op_node );
            MY_POP( &sem_node );
            if( 0 == caculate_AX_BX_by_op( &op_node ) ){
                return 0;
            }
            MY_PUSH( &sem_node );
        } else{
            /* do nothing */
        }
        MY_PUSH( cur_node );
    }
    return 1;
}
static int caculate_AX_BX_by_op( SEM_STACK_NODE *op_node ){
    char * func_name = "caculate_AX_BX_by_op";
    char str_first_sign[20];
    char str_second_sign[20];
    /* [ 1 ] pop BX */
    fprintf( g_SemEnv.pAsmFile, "POP  BX\n" );
    /* [ 2 ] pop AX */
    fprintf( g_SemEnv.pAsmFile, "POP  AX\n" );
    /* [ 3 ] caculate */
    switch( op_node->n_mode ){
        case 1: /* || */
            fprintf( g_SemEnv.pAsmFile, "OR AX, BX\n" );
            break;
        case 2: /* && */
            fprintf( g_SemEnv.pAsmFile, "AND AX, BX\n" );
            break;
        case 3:
            if( SYN_NODE_EQ == op_node->n_value ){ /* == */
                fprintf( g_SemEnv.pAsmFile, ";Deal with '=='\n" );
                sprintf_s( str_first_sign, sizeof(str_first_sign), "SIGN_%d", g_SemEnv.n_SignCurrentId );
                g_SemEnv.n_SignCurrentId++;
                sprintf_s( str_second_sign, sizeof(str_second_sign), "SIGN_%d", g_SemEnv.n_SignCurrentId );
                g_SemEnv.n_SignCurrentId++;
                fprintf( g_SemEnv.pAsmFile, "CMP AX, BX\n" );
                fprintf( g_SemEnv.pAsmFile, "JNE %s\n",  str_first_sign );
                fprintf( g_SemEnv.pAsmFile, "MOV AX, 1\n" );
                fprintf( g_SemEnv.pAsmFile, "JMP %s\n", str_second_sign );
                fprintf( g_SemEnv.pAsmFile, "%s:\n",  str_first_sign );
                fprintf( g_SemEnv.pAsmFile, "MOV AX, 0\n" );
                fprintf( g_SemEnv.pAsmFile, "%s:\n",  str_second_sign );
            } else{ /* != */
                fprintf( g_SemEnv.pAsmFile, "XOR AX, BX\n" );
            }
            break;
        case 4:
            if( SYN_NODE_MORE == op_node->n_value ){ /* > */
                fprintf( g_SemEnv.pAsmFile, "SUB BX, AX\n" );
                fprintf( g_SemEnv.pAsmFile, "AND BX, 8000H\n" );
                fprintf( g_SemEnv.pAsmFile, "MOV AX, BX\n" );
            } else if( SYN_NODE_LESS == op_node->n_value ){ /* < */
                fprintf( g_SemEnv.pAsmFile, "SUB AX, BX\n" );
                fprintf( g_SemEnv.pAsmFile, "AND AX, 8000H\n" );
            } else if( SYN_NODE_NOT_LESS == op_node->n_value ){ /* >= */
                fprintf( g_SemEnv.pAsmFile, "SUB AX, BX\n" );
                fprintf( g_SemEnv.pAsmFile, "AND AX, 8000H\n" );
                fprintf( g_SemEnv.pAsmFile, "XOR AX, 8000H\n" );
            } else{ /* <= */
                fprintf( g_SemEnv.pAsmFile, "SUB BX, AX\n" );
                fprintf( g_SemEnv.pAsmFile, "AND BX, 8000H\n" );
                fprintf( g_SemEnv.pAsmFile, "XOR BX, 8000H\n" );
                fprintf( g_SemEnv.pAsmFile, "MOV AX, BX\n" );
            }
            break;
        case 5:
            if( SYN_NODE_ADD == op_node->n_value ){ /* + */
                fprintf( g_SemEnv.pAsmFile, "ADD AX, BX\n" );
            } else{ /* - */
                fprintf( g_SemEnv.pAsmFile, "SUB AX, BX\n" );
            }
            break;
        case 6:
            if( SYN_NODE_MUT == op_node->n_value ){ /* * */
                fprintf( g_SemEnv.pAsmFile, "MUL BX\n" );
            } else{ /* / */
                fprintf( g_SemEnv.pAsmFile, "MOV DX, 0\n" );
                fprintf( g_SemEnv.pAsmFile, "DIV BX\n" );
            }
            break;
        default:
            report_internal_error( func_name, __LINE__ );
            printf("Invalid operator mode '%d'\n", op_node->n_mode );
            return 0;
    }
    /* [ 4 ] push AX */
    fprintf( g_SemEnv.pAsmFile, "PUSH  AX\n" );

    return 1;
}
void debug_sem( ){
    unsigned int i = 0;
    IdentId_P pId = NULL;
    for( i = 0; i < g_IdTable.n_id_num; i++ ){
        pId = &g_IdTable.pIdList[ i ];
        if( SYN_NODE_VARID == pId->n_mode ){
            fprintf( g_SemEnv.pAsmFile, "LEA  AX, Name_%s\nMOV  DX, AX\nMOV  AH, 9\nINT  21H\n", pId->str_name );
            fprintf( g_SemEnv.pAsmFile, "MOV  AX, %s\nPRINTVAL  AX\n", pId->str_name );
            fprintf( g_SemEnv.pAsmFile, ASM_CHANGE_LINE );
        }
    }
}
