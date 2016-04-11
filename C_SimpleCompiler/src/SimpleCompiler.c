/** SimpleCompiler.c
====================================================================
This is the base source file for Main Function


====================================================================
 */

#include "scmppre.h"
#include "scmpstx.h"
#include "scmpsem.h"
int main(int arg_num, char ** args){
    int ret = 0;
    FILE *srcFP = NULL;
    FILE *outFP = NULL;
    char *inputFile = "test.c";
    char *outputFile = "test.c.tmp";
    FileField srcFileField;
#ifdef DEBUG_SYNTAX
    char *debugFile = "test.c.stx";
    FILE * debugfp = NULL;
#endif
    /* Global initializing */
    ret = initialize_IdTable();
    ret &= initialize_SynTree();
    if( 0 == ret ){
        printf("Program exit with initializing error.\n");
        return 0;
    }
    fopen_s( &srcFP, inputFile , "r" );
    if( NULL == srcFP ){
        printf("Can't open source file '%s'. Program exit.\n", inputFile );
        return 0;
    }
    fopen_s( &outFP, outputFile, "w" );
    if( NULL == outFP ){
        printf("Can't open temp file '%s'. Program exit.\n", outputFile );
        return 0;
    }
    /** pre-compiler **/
    ret = pre_compiler(srcFP, outFP);
    if( 0 == ret ){
        printf("Program exit with pre-compiler error.\n");
        goto ERROR_END;
    }
    MY_HELP_CLOSE( outFP );
    MY_HELP_CLOSE( srcFP );
    /** Syntax analyzing **/
    /* using the temp file as the source */
    fopen_s( &srcFP, outputFile, "r" );
    if( NULL == srcFP ){
        printf("Can't open temp file '%s'. Program exit.\n", outputFile );
        goto ERROR_END;
    }
    srcFileField.fp = srcFP;
    srcFileField.n_current_line = 1;
    ret = syntax_analyzer( &srcFileField );
    if( 0 == ret ){
        printf("Program exit with syntax_analyzer error.\n");
        goto ERROR_END;
    }
#ifdef DEBUG_SYNTAX
    fopen_s(&debugfp, debugFile, "w");
    if( NULL != debugfp ){
        debug_syntax( debugfp );
        fclose( debugfp );
    }
#endif
    /** semantic_analyzer **/
    ret = semantic_analyzer();
    if( 0 == ret ){
        printf("Program exit with semantic_analyzer error.\n");
        goto ERROR_END;
    }

    MY_HELP_CLOSE( outFP );
    MY_HELP_CLOSE( srcFP );
    release_IdTable();
    release_SynTree();
    return 1;
ERROR_END:
    MY_HELP_CLOSE( outFP );
    MY_HELP_CLOSE( srcFP );
    release_IdTable();
    release_SynTree();
    return 0;
}
