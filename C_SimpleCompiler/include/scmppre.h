/** scmppre.h
====================================================================
This is the base head file for pre-compiler


====================================================================
 */
#ifndef SCMPPRE_H_INCLUDE
#define SCMPPRE_H_INCLUDE

#ifndef STD_COMMON_H_INCLUDE
#define STD_COMMON_H_INCLUDE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#endif


#define MAX_SRC_LINE      1024           /* the max byte number of one line in the source file */

int pre_compiler(FILE * fpInput, FILE * fpOutput);



#endif
