/** scmppre.c
====================================================================
This is the base source file for pre-compiler


====================================================================
 */

#include "scmppre.h"


static int report_pre_error(char *msg, int code){
    printf("Error : %s\nCode : %d\n", msg, code);
    return 0;
}

#define CHECK_INDEX( index )    \
    if( index >= MAX_SRC_LINE ){\
        report_pre_error("Too long line.", index);\
        return 0;\
    }
/*=======================================================
The main function of pre-compiler.
return     1 : success
           0 : fail
========================================================*/
int pre_compiler(FILE * fpInput, FILE * fpOutput){
    char str_line_buffer[MAX_SRC_LINE + 1];
    int index = 0;
    char current_char;
    current_char = fgetc(fpInput);
    while( current_char != EOF ){
        /*========================================================
        search the '//' . if exists, jump the rest of the line.
        ========================================================*/
        if( '/' == current_char ){
            current_char = fgetc(fpInput);
            if( '/' == current_char ){
                while( current_char != EOF ){
                    current_char =  fgetc(fpInput);
                    if( '\n' == current_char ){
                        /* end of a line */
                        str_line_buffer[index] = '\n';
                        index++;
                        CHECK_INDEX( index )
                        break;/* stop loop */
                    }
                }
                /* output a line */
                str_line_buffer[index] = '\0';
                fputs(str_line_buffer, fpOutput);
                index = 0;
            } else{
                /* a single '/', just store it and its following byte */
                str_line_buffer[index] = '/';
                index++;
                CHECK_INDEX( index )
                if( EOF == current_char ){ /* end of file */
                    str_line_buffer[index] = '\0';
                    fputs(str_line_buffer, fpOutput);
                    index = 0;
                    return 1;
                } else if( '\n' == current_char ){
                    str_line_buffer[index] = current_char;
                    index++;
                    CHECK_INDEX( index )
                    str_line_buffer[index] = '\0';
                    fputs(str_line_buffer, fpOutput);
                    index = 0;
                } else{
                    str_line_buffer[index] = current_char;
                    index++;
                    CHECK_INDEX( index )
                }
            }
        } else if( '\n' == current_char ){
            str_line_buffer[index] = current_char;
            index++;
            CHECK_INDEX( index )
            str_line_buffer[index] = '\0';
            fputs(str_line_buffer, fpOutput);
            index = 0;
        } else{
            str_line_buffer[index] = current_char;
            index++;
            CHECK_INDEX( index )
        }
        current_char = fgetc(fpInput);
    }

    str_line_buffer[index] = '\0';
    fputs(str_line_buffer, fpOutput);
    index = 0;

    return 1;
}
