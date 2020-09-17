/**
 * \tokenize.c
 * \Himani Munshi (hxm3443@rit.edu) 
 * \CSCI - 243 - 05
 * \Date: 10/ 11/ 2018
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "classes.h"

int MAX_SIZE = 256;            //Maximum size of the buffer
int MAX_TOKEN_SIZE = 63;      //Maximum length of a token
int ROW;                     //Represents the number of states in the matrix
int START;                  //Represents the start state 
int ACCEPT;                //Represents the accept state
int ERROR = 99;           //Represents the error state
int REJECT = -5;         //Returned from scanner if token is rejected
char ACT_DISCARD = 'd'; //Action -- Discard character
char ACT_SAVE = 's';   //Action -- Save character in a buffer

/**
 * A structure which contains the next transition state and the corresponding
 * action (either save or discard).  
 * */
typedef struct {
    int nextS;
    char trans_action;
} tm_data;

/**
 * This function takes in a 2-D array of struct as its parameter and builds 
 * the transition matrix. Essentially, our 2-D array contains pointers wherein 
 * each of these pointers point to a struct. 
 * Initially, we allocated memory to all the pointers in the matrix.
 * Using pointers we set all the structs to a default state wherein 
 * next transition state contains the error state 99 and the action contains 
 * discard 'd'. We then set each of these pointers as equal to the corresponding
 * element in the 2-D array. 
 * We then read from a file line by line and break the line into tokens. Each 
 * line's first token is the current row and then the corresponding 12 values 
 * of the form '%d/%d%c' tell us the current column, next transition row, and
 * the action to be performed respectively for that row. We keep saving the 
 * next transition state and the action in the respective struct based on 
 * current row and column. We keep doing this for every line in the file
 * and eventually build our transition matrix. Lastly, we print out our 
 * transition matrix.     
 *
 * \param *fp - Represents a file pointer
 * \param buf - Buffer (char array)  holding a line from the file each time
 * \param *t_matrix - 2-D matrix of a struct       
 * */
void build_transition_matrix( FILE *fp, char buf[], tm_data *t_matrix[][N_CC] ) {
    char *ptr, trans_data[MAX_SIZE];
    for( int i = 0; i < ROW; i++ ) {
        for ( int j =0; j < N_CC; j++ ) {
            tm_data *ptr_st = ( tm_data * ) malloc( sizeof(tm_data) );

            ptr_st->nextS = ERROR;
            ptr_st->trans_action = ACT_DISCARD;
            t_matrix[i][j] = ptr_st;
        }
    }

    while( (ptr = fgets(buf, MAX_SIZE, fp)) != NULL ) {
        strcpy( trans_data, buf );

        int curr_row;
        char *get_token = strtok( trans_data, " " );

        sscanf( get_token, "%d", &curr_row );
        get_token = strtok( NULL, " " );
        
        for ( int i = 0; i < N_CC && get_token != NULL; i++ ) {
            int  curr_col, next;
            char t_action;

            sscanf( get_token, "%d/%d%c", &curr_col, &next, &t_action );

            tm_data *ptr_st = t_matrix[curr_row][curr_col];
            ptr_st->nextS = next;
            ptr_st->trans_action = t_action;

            get_token = strtok( NULL, " " );
        }
    }
    
    printf("Scanning using the following matrix:\n");
    for ( int j = 0; j < N_CC; j++ ) {
        if ( j == 0 ) {
            printf( "%6d", j );
        }
        else {
            printf( "%5d", j );
        }
    }
    printf( "\n" );
    for ( int i = 0; i < ROW; i++ ) {
        printf( "%2d", i );
        for ( int j = 0; j < N_CC; j++ ) {
            printf( "%4d%c", t_matrix[i][j]->nextS, t_matrix[i][j]->trans_action );
        }
        printf( "\n" );
    }
}

/**
 * This function takes in an input character and returns the associated 
 * character class (behaves as the column index in the matrix). The returned
 * macros are defined in the header file called 'classes.h'.
 *
 * \param input_char - character being read from the standard input
 *
 * \return returns corresponding character class (macro)  
 * */
int characterClass( char input_char ) {
    if( input_char == '\n' )
        return CC_NEWLINE;
    else if( isspace(input_char) )
        return CC_WS;
    else if( isalpha(input_char) || input_char == '_' )
        return CC_ALPHA;
    else if( input_char == '0' )
        return CC_DIG_0;
    else if( isdigit(input_char) && (input_char <= '7' && input_char >= '1') )
        return CC_DIG_1_7;
    else if( isdigit(input_char) && (input_char == '8' || input_char == '9') )
        return CC_DIG_8_9;
    else if( input_char == '/' )
        return CC_CHAR_SLASH;
    else if( input_char == '*' )
        return CC_CHAR_STAR;
    else if( input_char == '+' || input_char == '-' || input_char == '%' )
        return CC_ARITH_OP;
    else if( input_char == EOF )
        return CC_EOF;
    else if( !(input_char >= 0) )
        return CC_ERROR;
    else
        return CC_OTHER;
}

/**
 * This function is responsible for recognizing tokens. Whenever this 
 * function is called, it figures out the next transition state for the 
 * given character from standard input based on the calculated current
 * row and column. Also, if the corresponding action says 'save', then 
 * it stores the token in a buffer. If the next transition state happens to
 * be the accept state, then it prints out a message saying that the token has
 * been recognized and resets the buffer. 
 * On the other hand, if the next state happens to be the error state, then it 
 * prints out rejected and resets the buffer and returns from the function. 
 * Lastly, if the character happens to be the end-of-file, then it returns -1
 * to let the main() function know that its time to stop processing the input.
 *
 * \param c - input character being read from the standard input
 * \param token - buffer (char array) storing a token
 * \param t_matrix - 2-D array of a struct 
 * 
 * \return returns 0 if normal behavior
 * \return returns -1 if end-of-file has been reached
 * \return returns -5 if a token has been rejected 
 * */
int scanner( char c, char token[], tm_data *t_matrix[][N_CC] ) {

    static int next, counter = 0, index =0;
    int currRow;
    char action;
 
    int charClass = characterClass(c);

    if( counter == 0 ) {
        currRow = START;
        counter++;
    }
    else {
        currRow = next;
    }

    printf( "%d ", currRow );

    next = t_matrix[currRow][charClass]->nextS;
    action = t_matrix[currRow][charClass]->trans_action;

    if( action == ACT_SAVE ) {
        token[index] = c;
        index++;
    }

    if( next == ACCEPT ) {
        token[index] = '\0';
        if ( c != EOF )
            printf( "%d recognized \'%s\'\n", next, token );
        else
            printf( "%d EOF\n", next );
        index = 0;
        counter = 0;
    }
    
    if( next == ERROR ) {
        printf( "%d rejected\n", ERROR );
        index = 0;
        counter = 0;
        return REJECT;
    }

    if ( c == EOF ) {
        return -1;
    }

    return 0;
}

/**
 * This function is responsible for freeing the memory which was allocated 
 * to every pointer in the transition matrix. 
 *
 * \param t_matrix - 2-D matrix of a struct
 * */
void free_memory(tm_data *t_matrix[][N_CC]) {
    for( int i = 0; i < ROW; i++ ) {
        for ( int j = 0; j < N_CC; j++ ) {
            free(t_matrix[i][j]);
        }
    }
}

/**
 * The main() function verifies the commandline arguments. If the expected
 * number of arguments are not passed, then prints a usage message for the 
 * user. Then the main() function opens the file in reading mode specified 
 * by the user. If the file does not exist, prints out an error message.
 * It then reads the first three lines from the file to extract the number
 * of states, the start, and the accept states from the file for the matrix.
 * Based on the number of states, declares a matrix which is then passed as 
 * a parameter to the functions. After the transition matrix is ready, it then
 * loops and reads character by character from the standard input and makes
 * repeated calls to the scanner() function so that it can recognize the tokens.
 * It breaks out of the loop after the scanner processes the end-of-file 
 * character. Eventually, it closes the file pointer and frees the memory 
 * allocated to every pointer inside the 2-D matrix.
 *
 * \param argc - number of commandline arguments
 * \param *argv - array of pointers to strings where the strings are the 
 *                commandline arguments     
 * */
int main( int argc, char *argv[] ) {
    if( argc != 2 ) {
        fprintf( stderr, "usage: ./tokenize tmfile\n" );
        exit( EXIT_FAILURE );
    }

    FILE *fp;
    fp = fopen( argv[1], "r" );
    if( fp == NULL ) {
        //something went wrong
        perror( argv[1] );
        exit( EXIT_FAILURE );
    }

    char *ptr, buf[MAX_SIZE];
    int count = 0;
    
    //Retrieving the number of rows, start state, and the accept state from file
    while ( count <= 2 && ( ( ptr = fgets( buf, MAX_SIZE, fp ) ) != NULL)  ) {

        if ( count == 0 ) {
            sscanf( buf, "%*s %d", &ROW );
        }
        else if ( count == 1 ) {
            sscanf( buf, "%*s %d", &START );
        }
        else {
            sscanf( buf, "%*s %d", &ACCEPT );
        }

        count++;
    }
    
    tm_data *trans_matrix[ROW][N_CC];
    
    build_transition_matrix( fp, buf, trans_matrix );

    char token[MAX_TOKEN_SIZE+1];

    while( 1 ) {
        int c = getchar(), data;
        data = scanner(c, token, trans_matrix);
        
	//Token has been rejected so keep looping until you find space or newline
        if ( data == REJECT ) {
            do {
                c = getchar();
            }
            while ( !isspace(c) && c != EOF );
        }
        
	//Reached end-of-file
	if ( data == -1 )
             break;
    }

    fclose( fp );    //closing the file pointer
    free_memory(trans_matrix);    //calling function to free memory


return 0;
}
