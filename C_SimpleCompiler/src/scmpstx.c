/** scmpstx.c
====================================================================
This is the base source file for Syntax Analyze


====================================================================
 */
#include "scmpstx.h"

int DEFINITION(FileField *fileField, int left_brother_ID, int father_ID);
int VAR_DEF(FileField *fileField, int left_brother_ID, int father_ID);
int FUNC_DEF(FileField *fileField, int left_brother_ID, int father_ID);
int IDENTIFY(FileField *fileField, int left_brother_ID, int father_ID);
int BODY(FileField *fileField, int left_brother_ID, int father_ID);
int VAR_DEFINITION(FileField *fileField, int left_brother_ID, int father_ID);
int STATEMENT_LIST(FileField *fileField, int left_brother_ID, int father_ID);
int ASSIGN_STMT(FileField *fileField, int left_brother_ID, int father_ID);
int IF_STMT(FileField *fileField, int left_brother_ID, int father_ID);
int FOR_STMT(FileField *fileField, int left_brother_ID, int father_ID);
int WHILE_STMT(FileField *fileField, int left_brother_ID, int father_ID);
int FORMULA(FileField *fileField, int left_brother_ID, int father_ID);
int BOOL_FORMULA(FileField *fileField, int left_brother_ID, int father_ID);
int FORMULA_FOLLOW(FileField *fileField, int left_brother_ID, int father_ID);
int BOOL_FORMULA_FOLLOW(FileField *fileField, int left_brother_ID, int father_ID);
int FUNC_CALL(FileField *fileField, int left_brother_ID, int father_ID);



void report_syntax_error(char *func_name, char *detail, int line_number){
    printf("Syntax error accured in function '%s()', at line '%d'.\nDetail: %s\n", func_name, line_number, detail);
    return;
}
void report_memery_error(char *func_name, int line_number){
    printf("Memery error accured in function '%s()', at line '%d'.\n", func_name, line_number);
    return;
}
void report_internal_error(char *func_name, int line_number){
    printf("Internal error accured in function '%s()', at line '%d'.\n", func_name, line_number);
    return;
}
int initialize_IdTable(){
    int i=0;
    int j=0;
    g_IdTable.n_id_num = 0;
    g_IdTable.pIdList = (IdentId_P)malloc( ( MAX_SYN_NODE_NUM + 1 ) * sizeof(IdentId) );
    if( NULL == g_IdTable.pIdList ){
        report_memery_error("initialize_IdTable", __LINE__);
        return 0;
    }
    for( i = 0; i < 51; i++ ){
        for( j = 0; j < 61; j++ ){
            g_IdTable.two_level_index[i][j] = INVALID_ID;
        }
    }
    return 1;
}
int initialize_SynTree(){
    g_SynTree.n_node_num = 0;
    g_SynTree.pSynNodeList = (SynNode_P)malloc( ( MAX_SYN_NODE_NUM + 1 ) * sizeof(SynNode) );
    if( NULL == g_SynTree.pSynNodeList ){
        report_memery_error("initialize_SynTree", __LINE__);
        return 0;
    }
    return 1;
}
int release_IdTable(){
    MY_HELP_FREE( g_IdTable.pIdList )
    g_IdTable.n_id_num = 0;
    return 1;
}
int release_SynTree(){
    MY_HELP_FREE( g_SynTree.pSynNodeList );
    g_SynTree.n_node_num = 0;
    return 1;
}

int syntax_analyzer(FileField *fileField){
    int ret = 0;
    char * func_name="syntax_analyzer";

    /** [ 1 ] initialize a node for whole program **/
    g_SynTree.n_node_num = 1;
    g_SynTree.pSynNodeList[0].n_father_id = INVALID_ID;
    g_SynTree.pSynNodeList[0].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[0].n_left_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[0].n_mode = SYN_NODE_HEAD;
    g_SynTree.pSynNodeList[0].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[0].n_son_id = INVALID_ID;
    /** [ 2 ] deal with DEFINITION **/
    ret = DEFINITION( fileField, INVALID_ID, 0 );
    /** [ 3 ] deal with 'main' **/
    if( 0 == ret ){
        printf("Syntax_analyzer exit with syntax error.\n");
        return 0;
    } else{
        /* some definition finished, the node 'DEFINITION' is the left brother of 'main' */
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
        g_SynTree.pSynNodeList[g_SynTree.n_node_num-1].n_father_id = 0;/* the index of 'main' is g_SynTree.n_node_num-1 */
        g_SynTree.pSynNodeList[g_SynTree.n_node_num-1].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[g_SynTree.n_node_num-1].n_left_brother_id = 1;/* the node 'DEFINITION' */
        g_SynTree.pSynNodeList[g_SynTree.n_node_num-1].n_mode = SYN_NODE_MAINFUNID;
        g_SynTree.pSynNodeList[g_SynTree.n_node_num-1].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[g_SynTree.n_node_num-1].n_son_id = INVALID_ID;
        g_SynTree.pSynNodeList[1].n_right_brother_id = g_SynTree.n_node_num-1;/* fix the right brother for node 'DEFINITION' */
    }
    /** [ 4 ] deal with '(' and ')' **/
    /* [ 4.1 ] jump the spaces after 'main' */
    JUMP_SPACES( fileField )
    /* [ 4.2 ] check '(' */
    if( '(' != fileField->current_ch ){
        report_syntax_error( "syntax_analyzer", "missing '('.", fileField->n_current_line );
        printf("Syntax_analyzer exit with syntax error.\n");
        return 0;
    }
    /* [ 4.3 ] jump the spaces after '(' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 4.4 ] check ')' */
    if( ')' != fileField->current_ch ){
        report_syntax_error( "syntax_analyzer", "missing ')'.", fileField->n_current_line );
        printf("Syntax_analyzer exit with syntax error.\n");
        return 0;
    }
    /* [ 4.5 ] jump the spaces after ')' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 4.6 ] check '{' */
    if( '{' != fileField->current_ch ){
        report_syntax_error( "syntax_analyzer", "missing '{'.", fileField->n_current_line );
        printf("Syntax_analyzer exit with syntax error.\n");
        return 0;
    }
    /** [ 5 ] deal with 'BODY' **/
    ret = BODY( fileField, g_SynTree.n_node_num-1, 0 );
    if( 0 == ret ){
        printf("Syntax_analyzer exit with syntax error.\n");
        return 0;
    }
    /* [ 6 ] jump the spaces after 'BODY' */
    JUMP_SPACES( fileField )
    /* [ 7 ] check '}' */
    if( '}' != fileField->current_ch ){
        report_syntax_error( "syntax_analyzer", "missing '}'.", fileField->n_current_line );
        printf("Syntax_analyzer exit with syntax error.\n");
        return 0;
    }
    /* [ 8 ] jump the spaces after '}' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    if( EOF != fileField->current_ch ){
        report_syntax_error( "syntax_analyzer", "Unexpected character.", fileField->n_current_line );
        printf("Syntax_analyzer exit with syntax error.\n");
        return 0;
    }
    return 1;
}


int DEFINITION(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    int id_length = 0;
    char *func_name="DEFINITION";
    unsigned int tempIndex = 0;

    /* [ 1 ] make a node for 'DEFINITION' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_DEFINITION;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )


    /* [ 3 ] deal with VAR_DEF */
    ret = VAR_DEF( fileField, INVALID_ID, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    if( 1000 == ret ){
        report_syntax_error( func_name, "Invalid identifier .", fileField->n_current_line );
        return 0;
    } else if( 100 == ret ){
        return 1;
    } else if( 200 == ret ){
        /* [ 4 ] deal with FUNC_DEF */
        ret = FUNC_DEF( fileField, INVALID_ID, tempIndex );
        if( 0 == ret ){
            return 0;
        }
    } else if( 300 == ret ){
        report_syntax_error( "syntax_analyzer", "Unexpected character.", fileField->n_current_line );
        return 0;
    }
    /* [ 5 ] deal with DEFINITION recursively */
    ret = DEFINITION( fileField, g_SynTree.pSynNodeList[ tempIndex ].n_son_id, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    return 1;
}
/** if it returns 1000, it's unkown character, maybe '}' **/
/** if it returns 100, an identifier name 'main' is stored in FileField.name_buf **/
/** if it returns 200, an identifier name ( not 'main' ) is stored in FileField.name_buf **/
/** if it returns 300, it's 'if' 'while' 'for' **/
int VAR_DEF(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="VAR_DEF";
    unsigned int tempIndex = 0;
    unsigned int ident_index = 0;
    int id_length = 0;
    /* [ 1 ] jump spaces in the head */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 2 ] get the first token */
    GET_IDENTIFY( fileField, fileField->name_buf, id_length )
    if( 0 == id_length ){
        return 1000;
    }
    CHECK_KEY_WORD( fileField->name_buf, ret )
    if( KEY_WORD_MAIN == ret ){
        /* no definition */
        return 100;
    } else if( KEY_WORD_NO == ret ){
        /* it's an identifier. maybe statment_list or syntax error. */
        return 200;
    }else if( KEY_WORD_INT == ret ){
        /* [ 3 ] deal with 'VAR_DEF' */
        /* [ 3.1 ] make a node for 'VAR_DEF' */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_VAR_DEF;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 3.2 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
        /* [ 3.3 ] get the identifier after 'int' */
        GET_IDENTIFY( fileField, fileField->name_buf, id_length )
        if( 0 == id_length ){
            report_syntax_error( func_name, "Invalid identifier .", fileField->n_current_line );
            return 0;
        }
        CHECK_KEY_WORD( fileField->name_buf, ret )
        if( KEY_WORD_NO != ret ){
            /* Invalid identifier */
            report_syntax_error( func_name, "Invalid identifier: key-word.", fileField->n_current_line );
            return 0;
        }
        /* [ 3.4 ] use a IdentId to store the identifier and make a syntax node */
        ret = add_to_IdTable(fileField, fileField->name_buf, id_length, SYN_NODE_VARID, &ident_index);
        if( 0 == ret ){
            return 0;
        }
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_father_id = tempIndex;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_identifier_id = ident_index;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_mode = SYN_NODE_VARID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_son_id = INVALID_ID;
        /* [ 3.5 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( tempIndex, INVALID_ID, g_SynTree.n_node_num )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
        /* [ 3.6 ] deal with ';' */
        JUMP_SPACES( fileField )
        if( ';' != fileField->current_ch ){
            report_syntax_error( func_name, "Missing ';' .", fileField->n_current_line );
            return 0;
        }
        return 1;
    } else{
        /* it's 'if' 'while' 'for' */
        return 300;
    }
}
int FUNC_DEF(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="FUNC_DEF";
    unsigned int tempIndex = 0;
    unsigned int ident_index = 0;
    int id_length = 0;
    /* [ 1 ] deal with FUNC_DEF */
    /* [ 1.1 ] make a node for FUNC_DEF */
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_mode = SYN_NODE_FUNC_DEF;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_son_id = INVALID_ID;
    /* [ 1.2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, g_SynTree.n_node_num )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 1.3 ] use a IdentId to store the identifier and make a syntax node */
    ret = add_to_IdTable(fileField, fileField->name_buf, (int)strlen( fileField->name_buf ), SYN_NODE_FUNID, &ident_index);
    if( 0 == ret ){
        return 0;
    }
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_father_id = g_SynTree.n_node_num - 1;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_identifier_id = ident_index;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_left_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_mode = SYN_NODE_FUNID;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_son_id = INVALID_ID;
    /* [ 1.4 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( g_SynTree.n_node_num - 1, INVALID_ID, g_SynTree.n_node_num )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 1.5 ] deal with '(' */
    JUMP_SPACES( fileField )
    if( '(' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '(' .", fileField->n_current_line );
        return 0;
    }
    /* [ 1.6 ] deal with ')' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    if( ')' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing ')' .", fileField->n_current_line );
        return 0;
    }
    /* [ 1.7 ] deal with '{' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    if( '{' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '{' .", fileField->n_current_line );
        return 0;
    }
    /* [ 1.8 ] deal with 'BODY' */
    ret = BODY( fileField, g_SynTree.n_node_num - 1, g_SynTree.n_node_num - 2 );
    if( 0 == ret ){
        return 0;
    }
    /* [ 1.9 ] deal with '}' */
    JUMP_SPACES( fileField )
    if( '}' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '}' .", fileField->n_current_line );
        return 0;
    }
    return 1;
}
int BODY(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="BODY";
    unsigned int tempIndex = 0;
    int id_length = 0;

    /* [ 1 ] make a node for 'BODY' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_BODY;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] deal with VAR_DEFINITION */
    ret = VAR_DEFINITION( fileField, INVALID_ID, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    if( 400 != ret ){
        /* [ 4 ] deal with STATEMENT_LIST */
        ret = STATEMENT_LIST( fileField, g_SynTree.pSynNodeList[ tempIndex ].n_son_id, tempIndex );
        if( 0 == ret ){
            return 0;
        }
    }

    return 1;
}
/** if it returns 200, an identifier name is stored in FileField.name_buf **/
/** if it returns 300, it's 'if' 'while' 'for' **/
/** if it returns 400, an empty statment_list is following **/
int VAR_DEFINITION(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="VAR_DEFINITION";
    unsigned int tempIndex = 0;
    /* [ 1 ] make a node for 'VAR_DEFINITION' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_VAR_DEFINITION;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] deal with VAR_DEF */
    ret = VAR_DEF( fileField, INVALID_ID, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    if( 1000 == ret ){
        if( '}' != fileField->current_ch ){
            report_syntax_error( func_name, "Invalid identifier .", fileField->n_current_line );
            return 0;
        } else{
            /* Empty statement_list */
            return 400;
        }
    } else if( 100 == ret ){
        report_syntax_error( "syntax_analyzer", "Unexpected key-word 'main'.", fileField->n_current_line );
        return 0;/* 'main' can't be here */
    } else if( 200 == ret ){
        /* [ 4 ] 'STATEMENT_LIST' is following */
        return 200;
    } else if( 300 == ret ){
        /* [ 5 ] 'STATEMENT_LIST' is following */
        return 300;
    }
    ret = VAR_DEFINITION( fileField, g_SynTree.pSynNodeList[ tempIndex ].n_son_id, tempIndex );
    return ret;
}
int STATEMENT_LIST(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="STATEMENT_LIST";
    unsigned int tempIndex = 0;
    unsigned int id_length = 0;
    /* [ 1 ] make a node for 'STATEMENT_LIST' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_STATEMENT_LIST;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] check the last byte */
    if( '}' == fileField->current_ch ){
        /* the statement list is empty */
        return 1;
    }
    /* [ 4 ] check the key-word which has already been read */
    CHECK_KEY_WORD( fileField->name_buf, ret )
    if( KEY_WORD_NO == ret ){
        JUMP_SPACES( fileField )
        if( '=' == fileField->current_ch ){
            /* [ 4.1 ] it's an ASSIGN_STMT */
            ret = ASSIGN_STMT( fileField, INVALID_ID, tempIndex );
            if( 0 == ret ){
                return 0;
            }
            JUMP_SPACES( fileField )
            if( ';' != fileField->current_ch ){
                /* check ';' */
                report_syntax_error( func_name, "Missing ';' .", fileField->n_current_line );
                return 0;
            }
        } else if( '(' == fileField->current_ch ){
            /* [ 4.2 ] it's an FUNC_CALL */
            ret = FUNC_CALL( fileField, INVALID_ID, tempIndex );
            if( 0 == ret ){
                return 0;
            }
        } else{
            report_syntax_error( func_name, "Unknown statement .", fileField->n_current_line );
            return 0;
        }
    } else if( KEY_WORD_IF == ret ){
        /* [ 4.2 ] deal with IF_STMT */
        ret = IF_STMT( fileField, INVALID_ID, tempIndex );
        if( 0 == ret ){
            return 0;
        }
    } else if( KEY_WORD_WHILE == ret ){
        /* [ 4.3 ] deal with WHILE_STMT */
        ret = WHILE_STMT( fileField, INVALID_ID, tempIndex );
        if( 0 == ret ){
            return 0;
        }
    } else if( KEY_WORD_FOR == ret ){
        /* [ 4.4 ] deal with FOR_STMT */
        ret = FOR_STMT( fileField, INVALID_ID, tempIndex );
        if( 0 == ret ){
            return 0;
        }
    } else {
        report_syntax_error( func_name, "Invalid identifier .", fileField->n_current_line );
        return 0;
    }
    /* [ 5 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 6 ] get the next token */
    GET_IDENTIFY( fileField, fileField->name_buf, id_length )
    /* [ 7 ] call STATEMENT_LIST() recursively */
    ret = STATEMENT_LIST( fileField, g_SynTree.pSynNodeList[ tempIndex ].n_son_id, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    
    return 1;
}
int ASSIGN_STMT(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="ASSIGN_STMT";
    unsigned int tempIndex = 0;
    unsigned int ident_index = 0;
    IdentId_P p_cur_id = NULL;
    /* [ 1 ] make a node for 'ASSIGN_STMT' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_ASSIGN_STMT;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] find the identifier in IdTable */
    ret = find_in_IdTable(fileField, fileField->name_buf, (int)strlen(fileField->name_buf), &ident_index);
    if( 0 == ret ){
        return 0;
    }
    p_cur_id = &g_IdTable.pIdList[ ident_index ];
    if( p_cur_id->n_mode != SYN_NODE_VARID ){
        report_syntax_error( func_name, "Identifier can't be assigned, it's a function name.", fileField->n_current_line );
        return 0;
    }
    /* [ 4 ] make a node for the identifier */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = tempIndex - 1;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = ident_index;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_VARID;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 5 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( tempIndex - 1, INVALID_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 6 ] check the '=' */
    JUMP_SPACES( fileField )
    if( '=' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '='.", fileField->n_current_line );
        return 0;
    }
    /* [ 7 ] deal with FORMULA */
    ret = FORMULA( fileField, tempIndex, tempIndex - 1 );
    if( 0 == ret ){
        return 0;
    }

    return 1;
}

int IF_STMT(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="IF_STMT";
    unsigned int tempIndex = 0;
    unsigned int id_length = 0;
    /* [ 1 ] make a node for 'IF_STMT' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_IF_STMT;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] check '('  */
    JUMP_SPACES( fileField )
    if( '(' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '('.", fileField->n_current_line );
        return 0;
    }
    /* [ 4 ] deal with BOOL_FORMULA */
    /* [ 4.1 ] jump spaces in the head for two bytes operator e.g. '>=' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    ret = BOOL_FORMULA( fileField, INVALID_ID, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    /* [ 5 ] check ')'  */
    JUMP_SPACES( fileField )
    if( ')' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing ')'.", fileField->n_current_line );
        return 0;
    }
    /* [ 6 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 7 ] check '{' */
    if( '{' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '{'.", fileField->n_current_line );
        return 0;
    }
    /* [ 8 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 9 ] get the next token */
    GET_IDENTIFY( fileField, fileField->name_buf, id_length )
    /* [ 10 ] call STATEMENT_LIST() */
    ret = STATEMENT_LIST( fileField, g_SynTree.pSynNodeList[ tempIndex ].n_son_id, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    /* [ 11 ] deal with '}' */
    JUMP_SPACES( fileField )
    if( '}' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '}' .", fileField->n_current_line );
        return 0;
    }

    return 1;
}
int FOR_STMT(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="FOR_STMT";
    unsigned int for_stmt_index = 0;
    unsigned int tempIndex = 0;
    unsigned int id_length = 0;
    /* [ 1 ] make a node for 'FOR_STMT' */
    tempIndex = g_SynTree.n_node_num;
    for_stmt_index = tempIndex;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_FOR_STMT;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] check '('  */
    JUMP_SPACES( fileField )
    if( '(' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '('.", fileField->n_current_line );
        return 0;
    }
    /* [ 4 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    if( ';' == fileField->current_ch ){
        /* [ 5 ] first statement is empty, make an empty node */
        tempIndex++;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = for_stmt_index;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_EMPTY;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 5.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( for_stmt_index, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else{
        /* [ 6 ] deal with ASSIGN_STMT */
        /* [ 6.1 ] get the next token */
        GET_IDENTIFY( fileField, fileField->name_buf, id_length )
        ret = ASSIGN_STMT( fileField, INVALID_ID, for_stmt_index );
        if( 0 == ret ){
            return 0;
        }
        JUMP_SPACES( fileField )
        if( ';' != fileField->current_ch ){
            /* check ';' */
            report_syntax_error( func_name, "Missing ';' .", fileField->n_current_line );
            return 0;
        }
    }
    /* [ 7 ] deal with BOOL_FORMULA */
    /* [ 7.1 ] jump spaces in the head for two bytes operator e.g. '>=' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    ret = BOOL_FORMULA( fileField, g_SynTree.pSynNodeList[ for_stmt_index ].n_son_id, for_stmt_index );
    if( 0 == ret ){
        return 0;
    }
    JUMP_SPACES( fileField )
    if( ';' != fileField->current_ch ){
        /* check ';' */
        report_syntax_error( func_name, "Missing ';' .", fileField->n_current_line );
        return 0;
    }
    /* [ 7.1 ] store the index of BOOL_FORMULA */
    tempIndex = g_SynTree.pSynNodeList[ g_SynTree.pSynNodeList[ for_stmt_index ].n_son_id ].n_right_brother_id;
    /* [ 8 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    if( ')' == fileField->current_ch ){
        /* [ 9 ] last statement is empty, make an empty node */
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_father_id = for_stmt_index;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_left_brother_id = tempIndex;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_mode = SYN_NODE_EMPTY;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_son_id = INVALID_ID;
        /* [ 9.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( for_stmt_index, tempIndex, g_SynTree.n_node_num )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else{
        /* [ 10 ] deal with ASSIGN_STMT */
        /* [ 10.1 ] get the next token */
        GET_IDENTIFY( fileField, fileField->name_buf, id_length )
        ret = ASSIGN_STMT( fileField, tempIndex, for_stmt_index );
        if( 0 == ret ){
            return 0;
        }
        JUMP_SPACES( fileField )
        if( ')' != fileField->current_ch ){
            /* check ')' */
            report_syntax_error( func_name, "Missing ')' .", fileField->n_current_line );
            return 0;
        }
    }
    /* [ 10.1 ] store the index of third child of FOR_STMT */
    tempIndex = g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id;
    /* [ 11 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 12 ] check '{' */
    if( '{' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '{' .", fileField->n_current_line );
        return 0;
    }
    /* [ 13 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 14 ] get the next token */
    GET_IDENTIFY( fileField, fileField->name_buf, id_length )
    /* [ 15 ] call STATEMENT_LIST() */
    ret = STATEMENT_LIST( fileField, tempIndex, for_stmt_index );
    if( 0 == ret ){
        return 0;
    }
    /* [ 16 ] deal with '}' */
    JUMP_SPACES( fileField )
    if( '}' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '}' .", fileField->n_current_line );
        return 0;
    }

    return 1;
}
int WHILE_STMT(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="WHILE_STMT";
    unsigned int tempIndex = 0;
    unsigned int id_length = 0;
    /* [ 1 ] make a node for 'WHILE_STMT' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_WHILE_STMT;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] check '('  */
    JUMP_SPACES( fileField )
    if( '(' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '('.", fileField->n_current_line );
        return 0;
    }
    /* [ 4 ] jump spaces */
    /* [ 5.1 ] jump spaces in the head for two bytes operator e.g. '>=' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 5 ] deal with BOOL_FORMULA */
    ret = BOOL_FORMULA( fileField, INVALID_ID, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    /* [ 6 ] check ')' */
    JUMP_SPACES( fileField )
    if( ')' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing ')'.", fileField->n_current_line );
        return 0;
    }
    /* [ 7 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 8 ] check '{' */
    if( '{' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '{'.", fileField->n_current_line );
        return 0;
    }
    /* [ 9 ] jump spaces */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 10 ] get the next token */
    GET_IDENTIFY( fileField, fileField->name_buf, id_length )
    /* [ 11 ] call STATEMENT_LIST() */
    ret = STATEMENT_LIST( fileField, g_SynTree.pSynNodeList[ tempIndex ].n_son_id, tempIndex );
    if( 0 == ret ){
        return 0;
    }
    /* [ 12 ] deal with '}' */
    JUMP_SPACES( fileField )
    if( '}' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '}' .", fileField->n_current_line );
        return 0;
    }


    return 1;
}
int FORMULA(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="FORMULA";
    unsigned int tempIndex = 0;
    unsigned int FORMULAIndex = 0;
    unsigned int id_length = 0;
    unsigned int ident_index = 0;
    int int_value = 0;
    IdentId_P p_cur_id = NULL;
    /* [ 1 ] make a node for 'FORMULA' */
    tempIndex = g_SynTree.n_node_num;
    FORMULAIndex = tempIndex;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_FORMULA;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] jump spaces in the head */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    /* [ 4 ] check the first byte */
    if( '(' == fileField->current_ch ){
        /* [ 5 ] make a node for '(' */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = FORMULAIndex;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_L_parenthesis;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 5.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( FORMULAIndex, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
        /* [ 5.2 ] deal with FORMULA recursively */
        ret = FORMULA( fileField, tempIndex, FORMULAIndex );
        if( 0 == ret ){
            return 0;
        }
        /* [ 5.3 ] check ')' */
        JUMP_SPACES( fileField )
        if( ')' != fileField->current_ch ){
            report_syntax_error( func_name, "Missing ')' .", fileField->n_current_line );
            return 0;
        }
        /* [ 5.4 ] make a node for ')' */
        tempIndex = g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id;
        g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_father_id = FORMULAIndex;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_left_brother_id = tempIndex;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_mode = SYN_NODE_R_parenthesis;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_son_id = INVALID_ID;
        /* [ 5.5 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( FORMULAIndex, tempIndex , g_SynTree.n_node_num )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
        /* [ 5.6 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
        /* [ 5.7 ] reflash the tempIndex */
        tempIndex = g_SynTree.n_node_num - 1;
    } else if( isalpha( fileField->current_ch ) ){
        /* [ 6 ] deal with identifier */
        GET_IDENTIFY( fileField, fileField->name_buf, id_length )
        if( 0 == id_length ){
            report_syntax_error( func_name, "Invalid identifier .", fileField->n_current_line );
            return 0;
        }
        CHECK_KEY_WORD( fileField->name_buf, ret )
        if( KEY_WORD_NO != ret ){
            report_syntax_error( func_name, "Invalid identifier :key-word.", fileField->n_current_line );
            return 0;
        }
        /* [ 6.1 ] find the identifier in IdTable */
        ret = find_in_IdTable(fileField, fileField->name_buf, (int)strlen(fileField->name_buf), &ident_index);
        if( 0 == ret ){
            return 0;
        }
        p_cur_id = &g_IdTable.pIdList[ ident_index ];
        if( p_cur_id->n_mode != SYN_NODE_VARID ){
            report_syntax_error( func_name, "Identifier can't be caculated, it's a function name.", fileField->n_current_line );
            return 0;
        }
        /* [ 6.2 ] make a node for the identifier */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = FORMULAIndex;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = ident_index;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_VARID;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 6.3 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( FORMULAIndex, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else{
        /* [ 7 ] deal with numbers */
        ret = get_number( fileField, &int_value );
        if( 0 == ret ){
            return 0;
        }
        /* [ 7.1 ] make a node for the integer */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = FORMULAIndex;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_NUMBER;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_int_value = int_value;
        /* [ 7.2 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( FORMULAIndex, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    }
    /* [ 8 ] deal with FORMULA_FOLLOW */
    ret = FORMULA_FOLLOW( fileField, tempIndex,  FORMULAIndex );
    if( 0 == ret ){
        return 0;
    }

    return 1;
};
int BOOL_FORMULA(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="BOOL_FORMULA";
    unsigned int tempIndex = 0;
    unsigned int BOOL_FORMULAIndex = 0;
    unsigned int id_length = 0;
    unsigned int ident_index = 0;
    int int_value = 0;
    IdentId_P p_cur_id = NULL;
    /* [ 1 ] make a node for 'BOOL_FORMULA' */
    tempIndex = g_SynTree.n_node_num;
    BOOL_FORMULAIndex = tempIndex;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_BOOL_FORMULA;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] jump spaces in the head */
    JUMP_SPACES( fileField )
    /* [ 4 ] check the first byte */
    if( '(' == fileField->current_ch ){
        /* [ 5 ] make a node for '(' */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = BOOL_FORMULAIndex;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_L_parenthesis;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 5.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( BOOL_FORMULAIndex, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
        /* [ 5.2 ] deal with BOOL_FORMULA recursively */
        /* jump spaces in the head for two bytes operator e.g. '>=' */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
        ret = BOOL_FORMULA( fileField, tempIndex, BOOL_FORMULAIndex );
        if( 0 == ret ){
            return 0;
        }
        /* [ 5.3 ] check ')' */
        JUMP_SPACES( fileField )
        if( ')' != fileField->current_ch ){
            report_syntax_error( func_name, "Missing ')' .", fileField->n_current_line );
            return 0;
        }
        /* [ 5.4 ] make a node for ')' */
        tempIndex = g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id;
        g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_father_id = BOOL_FORMULAIndex;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_left_brother_id = tempIndex;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_mode = SYN_NODE_R_parenthesis;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ g_SynTree.n_node_num ].n_son_id = INVALID_ID;
        /* [ 5.5 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( BOOL_FORMULAIndex, tempIndex , g_SynTree.n_node_num )
        tempIndex = g_SynTree.n_node_num;/* Fix Bug */
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
        /* [ 5.6 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( isalpha( fileField->current_ch ) ){
        /* [ 6 ] deal with identifier */
        GET_IDENTIFY( fileField, fileField->name_buf, id_length )
        if( 0 == id_length ){
            report_syntax_error( func_name, "Invalid identifier .", fileField->n_current_line );
            return 0;
        }
        CHECK_KEY_WORD( fileField->name_buf, ret )
        if( KEY_WORD_NO != ret ){
            report_syntax_error( func_name, "Invalid identifier :key-word.", fileField->n_current_line );
            return 0;
        }
        /* [ 6.1 ] find the identifier in IdTable */
        ret = find_in_IdTable(fileField, fileField->name_buf, (int)strlen(fileField->name_buf), &ident_index);
        if( 0 == ret ){
            return 0;
        }
        p_cur_id = &g_IdTable.pIdList[ ident_index ];
        if( p_cur_id->n_mode != SYN_NODE_VARID ){
            report_syntax_error( func_name, "Identifier can't be caculated, it's a function name.", fileField->n_current_line );
            return 0;
        }
        /* [ 6.2 ] make a node for the identifier */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = BOOL_FORMULAIndex;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = ident_index;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_VARID;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 6.3 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( BOOL_FORMULAIndex, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else{
        /* [ 7 ] deal with numbers */
        ret = get_number( fileField, &int_value );
        if( 0 == ret ){
            return 0;
        }
        /* [ 7.1 ] make a node for the integer */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = BOOL_FORMULAIndex;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_NUMBER;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_int_value = int_value;
        /* [ 7.2 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( BOOL_FORMULAIndex, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    }
    /* [ 8 ] deal with BOOL_FORMULA_FOLLOW */
    ret = BOOL_FORMULA_FOLLOW( fileField, tempIndex,  BOOL_FORMULAIndex );
    if( 0 == ret ){
        return 0;
    }

    return 1;
}
int FORMULA_FOLLOW(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="FORMULA_FOLLOW";
    unsigned int tempIndex = 0;
    /* [ 1 ] make a node for 'FORMULA_FOLLOW' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_FORMULA_FOLLOW;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] check the first byte */
    JUMP_SPACES( fileField )
    if( '+' == fileField->current_ch ){
        /* [ 4 ] make a node for '+' */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = tempIndex - 1;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_ADD;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 4.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( tempIndex - 1, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else if( '-' ==  fileField->current_ch ){
        /* [ 5 ] make a node for '-' */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = tempIndex - 1;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_SUB;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 5.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( tempIndex - 1, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else if( '*' == fileField->current_ch ){
        /* [ 6 ] make a node for '*' */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = tempIndex - 1;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_MUT;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 6.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( tempIndex - 1, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else if( '/' == fileField->current_ch ){
        /* [ 7 ] make a node for '/' */
        tempIndex = g_SynTree.n_node_num;
        g_SynTree.pSynNodeList[ tempIndex ].n_father_id = tempIndex - 1;
        g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_DIV;
        g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
        g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
        /* [ 7.1 ] fix for father and left brother */
        FIX_FATHER_LBROTHER( tempIndex - 1, INVALID_ID, tempIndex )
        g_SynTree.n_node_num++;
        CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    } else{
        /* empty follow */
        return 1;
    }
    /* [ 8 ] deal with FORMULA */
    ret = FORMULA( fileField, tempIndex, tempIndex - 1 );
    if( 0 == ret ){
        return 0;
    }

    return 1;
}
int BOOL_FORMULA_FOLLOW(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="BOOL_FORMULA_FOLLOW";
    unsigned int tempIndex = 0;
    short n_mode = 0;
    /* [ 1 ] make a node for 'BOOL_FORMULA_FOLLOW' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_BOOL_FORMULA_FOLLOW;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] check the operator, maybe two bytes */
    JUMP_SPACES( fileField )
    if( '|' == fileField->current_ch ){
        /* [ 4 ] deal with the operator '||' */
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
        if( '|' != fileField->current_ch ){
            report_syntax_error( func_name, "Invalid operator.", fileField->n_current_line );
            return 0;
        }
        /* [ 4.1 ] mode is '||' */
        n_mode = SYN_NODE_OR;
        /* [ 4.2 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( '&' == fileField->current_ch){
        /* [ 5 ] deal with the operator '&&' */
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
        if( '&' != fileField->current_ch ){
            report_syntax_error( func_name, "Invalid operator.", fileField->n_current_line );
            return 0;
        }
        /* [ 5.1 ] mode is '||' */
        n_mode = SYN_NODE_AND;
        /* [ 5.2 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( '<' == fileField->current_ch){
        /* [ 6 ] deal with the operator '<' or '<=' */
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
        if( '=' != fileField->current_ch ){
            /* [ 6.1 ] mode is '<' */
            n_mode = SYN_NODE_LESS;
        } else{
            /* [ 6.2 ] mode is '<=' */
            n_mode = SYN_NODE_NOT_MORE;
            /* [ 6.3 ] jump spaces in the end */
            GET_NEW_CHAR_JUMP_SPACE( fileField )
        }
    } else if( '>' == fileField->current_ch){
        /* [ 7 ] deal with the operator '>' or '>=' */
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
        if( '=' != fileField->current_ch ){
            /* [ 7.1 ] mode is '>' */
            n_mode = SYN_NODE_MORE;
        } else{
            /* [ 7.2 ] mode is '>=' */
            n_mode = SYN_NODE_NOT_LESS;
            /* [ 7.3 ] jump spaces in the end */
            GET_NEW_CHAR_JUMP_SPACE( fileField )
        }
    } else if( '=' == fileField->current_ch){
        /* [ 8 ] deal with the operator '==' */
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
        if( '=' != fileField->current_ch ){
            report_syntax_error( func_name, "Invalid operator.", fileField->n_current_line );
            return 0;
        }
        /* [ 8.1 ] mode is '==' */
        n_mode = SYN_NODE_EQ;
        /* [ 8.2 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( '!' == fileField->current_ch){
        /* [ 9 ] deal with the operator '!=' */
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
        if( '=' != fileField->current_ch ){
            report_syntax_error( func_name, "Invalid operator.", fileField->n_current_line );
            return 0;
        }
        /* [ 9.1 ] mode is '!=' */
        n_mode = SYN_NODE_NOT_EQ;
        /* [ 9.2 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( '+' == fileField->current_ch){
        /* [ 10 ] deal with the operator '+' */
        n_mode = SYN_NODE_ADD;
        /* [ 10.1 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( '-' == fileField->current_ch){
        /* [ 11 ] deal with the operator '-' */
        n_mode = SYN_NODE_SUB;
        /* [ 11.1 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( '*' == fileField->current_ch){
        /* [ 12 ] deal with the operator '*' */
        n_mode = SYN_NODE_MUT;
        /* [ 12.1 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else if( '/' == fileField->current_ch){
        /* [ 13 ] deal with the operator '/' */
        n_mode = SYN_NODE_DIV;
        /* [ 13.1 ] jump spaces in the end */
        GET_NEW_CHAR_JUMP_SPACE( fileField )
    } else{
        /* empty follow */
        return 1;
    }
    /* [ 14 ] make a node for the operator*/
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = tempIndex - 1;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = n_mode;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 15 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( tempIndex - 1, INVALID_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 16 ] deal with BOOL_FORMULA */
    ret = BOOL_FORMULA( fileField, tempIndex, tempIndex - 1 );
    if( 0 == ret ){
        return 0;
    }

    return 1;
}
int FUNC_CALL(FileField *fileField, int left_brother_ID, int father_ID){
    int ret = 0;
    char *func_name="FUNC_CALL";
    unsigned int tempIndex = 0;
    unsigned int ident_index = 0;
    IdentId_P p_cur_id = NULL;
    /* [ 1 ] make a node for 'FUNC_CALL' */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = father_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = left_brother_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_FUNC_CALL;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 2 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( father_ID, left_brother_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 3 ] find the identifier in IdTable */
    ret = find_in_IdTable(fileField, fileField->name_buf, (int)strlen(fileField->name_buf), &ident_index);
    if( 0 == ret ){
        return 0;
    }
    p_cur_id = &g_IdTable.pIdList[ ident_index ];
    if( p_cur_id->n_mode != SYN_NODE_FUNID ){
        report_syntax_error( func_name, "Function can't be called, it's an identifier name.", fileField->n_current_line );
        return 0;
    }
    /* [ 4 ] make a node for the func_id */
    tempIndex = g_SynTree.n_node_num;
    g_SynTree.pSynNodeList[ tempIndex ].n_father_id = tempIndex - 1;
    g_SynTree.pSynNodeList[ tempIndex ].n_identifier_id = ident_index;
    g_SynTree.pSynNodeList[ tempIndex ].n_left_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_mode = SYN_NODE_FUNID;
    g_SynTree.pSynNodeList[ tempIndex ].n_right_brother_id = INVALID_ID;
    g_SynTree.pSynNodeList[ tempIndex ].n_son_id = INVALID_ID;
    /* [ 5 ] fix for father and left brother */
    FIX_FATHER_LBROTHER( tempIndex - 1, INVALID_ID, tempIndex )
    g_SynTree.n_node_num++;
    CHECK_NODE_NUM( func_name, fileField, g_SynTree.n_node_num )
    /* [ 6 ] check the '(' */
    JUMP_SPACES( fileField )
    if( '(' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing '('.", fileField->n_current_line );
        return 0;
    }
    /* [ 7 ] check the ')' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    if( ')' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing ')'.", fileField->n_current_line );
        return 0;
    }
    /* [ 8 ] check the ';' */
    GET_NEW_CHAR_JUMP_SPACE( fileField )
    if( ';' != fileField->current_ch ){
        report_syntax_error( func_name, "Missing ';'.", fileField->n_current_line );
        return 0;
    }

    return 1;
}
int add_to_IdTable(FileField * fileField, char *id_name, int length, short mode, unsigned int * p_ident_index){
    char first_index = 0;
    char second_index = 0;
    IdentId_P cur_IdentId_list = NULL;
    unsigned int index = 0;
    unsigned int temp = 0;
    char *func_name = "add_to_IdTable";

    if( length <= 0 ){
        report_internal_error(func_name, __LINE__);
        return 0;
    }
    if( id_name[0] >= 'A' && id_name[0] <= 'Z' ){
        first_index = id_name[0] - 'A';
    } else if( id_name[0] >= 'a' && id_name[0] <= 'z' ){
        first_index = id_name[0] - 'a' + 26;
    } else{
        first_index = 51;
    }
    if( length > 1 ){
        if( id_name[1] >= 'A' && id_name[1] <= 'Z' ){
            second_index = id_name[1] - 'A';
        } else if( id_name[1] >= 'a' && id_name[1] <= 'z' ){
            second_index = id_name[1] - 'a' + 26;
        } else if( id_name[1] >= '0' && id_name[1] <= '9' ){
            second_index = id_name[1] - '0' + 52;
        } else{
            second_index = 61;
        }
    }
    temp = g_IdTable.two_level_index[ first_index ][ second_index ];

    while( temp != INVALID_ID ){
        /* a list is found */
        cur_IdentId_list = &g_IdTable.pIdList[temp];
        if( strcmp( cur_IdentId_list->str_name, id_name ) == 0 ){
            /* already exist */
            report_syntax_error( func_name, "Re-definition error.", fileField->n_current_line );
            return 0;
        }
        temp = cur_IdentId_list->n_next_identifier_id;
    }
    if( g_IdTable.n_id_num + 1 > MAX_SYN_NODE_NUM ){
        report_syntax_error( func_name, "Too many identifiers.", fileField->n_current_line );
        return 0;
    }
    index = g_IdTable.n_id_num;
    g_IdTable.n_id_num++;
    if( NULL != cur_IdentId_list ){
        cur_IdentId_list->n_next_identifier_id = index;
    } else{
        /* create a new list for this id */
        g_IdTable.two_level_index[ first_index ][ second_index ] = index;
    }
    g_IdTable.pIdList[index].n_mode = mode;
    g_IdTable.pIdList[index].n_next_identifier_id = INVALID_ID;
    strcpy_s( g_IdTable.pIdList[index].str_name, MAX_ID_LENGTH+1, id_name );
    *p_ident_index = index;

    return 1;
}

int find_in_IdTable(FileField * fileField, char *id_name, int length, unsigned int * p_ident_index){
    char first_index = 0;
    char second_index = 0;
    IdentId_P cur_IdentId_list = NULL;
    unsigned int index = 0;
    unsigned int temp = 0;
    char *func_name = "find_in_IdTable";

    if( length <= 0 ){
        report_syntax_error( func_name, "Identifier not specified.", fileField->n_current_line );
        return 0;
    }
    if( id_name[0] >= 'A' && id_name[0] <= 'Z' ){
        first_index = id_name[0] - 'A';
    } else if( id_name[0] >= 'a' && id_name[0] <= 'z' ){
        first_index = id_name[0] - 'a' + 26;
    } else{
        first_index = 51;
    }
    if( length > 1 ){
        if( id_name[1] >= 'A' && id_name[1] <= 'Z' ){
            second_index = id_name[1] - 'A';
        } else if( id_name[1] >= 'a' && id_name[1] <= 'z' ){
            second_index = id_name[1] - 'a' + 26;
        } else if( id_name[1] >= '0' && id_name[1] <= '9' ){
            second_index = id_name[1] - '0' + 52;
        } else{
            second_index = 61;
        }
    }
    temp = g_IdTable.two_level_index[ first_index ][ second_index ];

    while( temp != INVALID_ID ){
        /* a list is found */
        cur_IdentId_list = &g_IdTable.pIdList[temp];
        if( strcmp( cur_IdentId_list->str_name, id_name ) == 0 ){
            *p_ident_index = temp;
            return 1;
        }
        temp = cur_IdentId_list->n_next_identifier_id;
    }
    report_syntax_error( func_name, "Identifier not specified.", fileField->n_current_line );
    return 0;
}

int get_number( FileField * fileField, int * pOutValue){
    char *func_name = "get_number";
    int tempValue1 = 0;
    int tempValue2 = 0;
    int b_sign = 0;/* 0 ==> no sign ,  1 ==> '+' , -1 ==> '-'  */
    /* [ 1 ] check the sign */
    if( '-' == fileField->current_ch ){
        b_sign = -1;
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
    } else if( '+' == fileField->current_ch ){
        b_sign = 1;
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )
    }
    /* [ 2 ] check the first number */
    if( fileField->current_ch < '0' || fileField->current_ch > '9' ){
        report_syntax_error( func_name, "Invalid character.", fileField->n_current_line );
        return 0;
    }
    while( fileField->current_ch >= '0' && fileField->current_ch <= '9' ){
        tempValue1 = tempValue2;
        tempValue2 = tempValue2 * 10 + fileField->current_ch - '0';
        if( tempValue1 > tempValue2 ){
            /* [ 3 ] deal with overflow */
            report_syntax_error( func_name, "Integer overflow.", fileField->n_current_line );
            return 0;
        }
        fileField->current_ch = fgetc( fileField->fp );
        CHECK_NEW_LINE( fileField )        
    }
    if( -1 == b_sign ){
        tempValue2 = ~tempValue2 + 1;
    }
    if( tempValue2 > SHORT_MAX || tempValue2 < SHORT_MIN ){
        /* [ 4 ] deal with overflow. Note: using 16bit-int for EXE file */
        report_syntax_error( func_name, "Integer overflow.", fileField->n_current_line );
        return 0;
    }
    *pOutValue = (int)tempValue2;


    return 1;
}
static void print_syntax_node( FILE* fp, SynNode_P p_node ){
    char buffer[128];
    buffer[0] = '\0';
    switch( p_node->n_mode ){
    case SYN_NODE_HEAD:
        strcpy_s( buffer, 128, "head" );
        break;
    case SYN_NODE_VARID:
        strcpy_s( buffer, 128, "VARID" );
        break;
    case SYN_NODE_FUNID:
        strcpy_s( buffer, 128, "FUNID" );
        break;
    case SYN_NODE_MAINFUNID:
        strcpy_s( buffer, 128, "main" );
        break;
    case SYN_NODE_BODY:
        strcpy_s( buffer, 128, "BODY" );
        break;
    case SYN_NODE_DEFINITION:
        strcpy_s( buffer, 128, "DEFINITION" );
        break;
    case SYN_NODE_VAR_DEF:
        strcpy_s( buffer, 128, "VAR_DEF" );
        break;
    case SYN_NODE_FUNC_DEF:
        strcpy_s( buffer, 128, "FUNC_DEF" );
        break;
    case SYN_NODE_VAR_DEFINITION:
        strcpy_s( buffer, 128, "VAR_DEFINITION" );
        break;
    case SYN_NODE_STATEMENT_LIST:
        strcpy_s( buffer, 128, "STATEMENT_LIST" );
        break;
    case SYN_NODE_ASSIGN_STMT:
        strcpy_s( buffer, 128, "ASSIGN_STMT" );
        break;
    case SYN_NODE_IF_STMT:
        strcpy_s( buffer, 128, "IF_STMT" );
        break;
    case SYN_NODE_FOR_STMT:
        strcpy_s( buffer, 128, "FOR_STMT" );
        break;
    case SYN_NODE_WHILE_STMT:
        strcpy_s( buffer, 128, "WHILE_STMT" );
        break;
    case SYN_NODE_FORMULA:
        strcpy_s( buffer, 128, "FORMULA" );
        break;
    case SYN_NODE_BOOL_FORMULA:
        strcpy_s( buffer, 128, "BOOL_FORMULA" );
        break;
    case SYN_NODE_FORMULA_FOLLOW:
        strcpy_s( buffer, 128, "FORMULA_FOLLOW" );
        break;
    case SYN_NODE_NUMBER:
        strcpy_s( buffer, 128, "NUMBER" );
        break;
    case SYN_NODE_L_parenthesis:
        strcpy_s( buffer, 128, "(" );
        break;
    case SYN_NODE_R_parenthesis:
        strcpy_s( buffer, 128, ")" );
        break;
    case SYN_NODE_ADD:
        strcpy_s( buffer, 128, "+" );
        break;
    case SYN_NODE_SUB:
        strcpy_s( buffer, 128, "-" );
        break;
    case SYN_NODE_MUT:
        strcpy_s( buffer, 128, "*" );
        break;
    case SYN_NODE_DIV:
        strcpy_s( buffer, 128, "/" );
        break;
    case SYN_NODE_AND:
        strcpy_s( buffer, 128, "&&" );
        break;
    case SYN_NODE_OR:
        strcpy_s( buffer, 128, "||" );
        break;
    case SYN_NODE_LESS:
        strcpy_s( buffer, 128, "<" );
        break;
    case SYN_NODE_MORE:
        strcpy_s( buffer, 128, ">" );
        break;
    case SYN_NODE_EQ:
        strcpy_s( buffer, 128, "==" );
        break;
    case SYN_NODE_NOT_LESS:
        strcpy_s( buffer, 128, ">=" );
        break;
    case SYN_NODE_NOT_MORE:
        strcpy_s( buffer, 128, "<=" );
        break;
    case SYN_NODE_NOT_EQ:
        strcpy_s( buffer, 128, "!=" );
        break;
    case SYN_NODE_BOOL_FORMULA_FOLLOW:
        strcpy_s( buffer, 128, "BOOL_FORMULA_FOLLOW" );
        break;
    case SYN_NODE_EMPTY:
        strcpy_s( buffer, 128, "EMPTY" );
        break;
    case SYN_NODE_FUNC_CALL:
        strcpy_s( buffer, 128, "FUNC_CALL" );
        break;
    }

    fprintf( fp,"n_mode = '%s';", buffer);
    if( SYN_NODE_VARID == p_node->n_mode || SYN_NODE_FUNID == p_node->n_mode ){
        fprintf( fp, "id name = '%s';", g_IdTable.pIdList[ p_node->n_identifier_id ].str_name );
    }
    fprintf( fp,"father_id = '%d';", (int)p_node->n_father_id);
    fprintf( fp,"left_brother_id = '%d';", (int)p_node->n_left_brother_id);
    fprintf( fp,"right_brother_id = '%d';", (int)p_node->n_right_brother_id);
    fprintf( fp,"son_id = '%d';", (int)p_node->n_son_id);
    if( SYN_NODE_NUMBER == p_node->n_mode ){
        fprintf( fp, "value = '%d';", p_node->n_int_value );
    }
}
void debug_syntax( FILE* fp ){
    unsigned int i = 0;
    for( i = 0; i < g_SynTree.n_node_num; i++ ){
        fprintf( fp,"id = '%6d';", i);
        print_syntax_node( fp, &g_SynTree.pSynNodeList[ i ] );
        fprintf(fp,"\n*******************************************************************************\n");
    }
}

