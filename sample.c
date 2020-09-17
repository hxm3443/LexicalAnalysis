#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void build_transition_matrix( FILE *fp ) {
    
    typedef struct {
        int currS;
        int nextS;
        char trans_action;
    } tm_data;
    
    char *ptr, buf[256], trans_data[256];
        int row, col = 12, count = 0, accept, start;
        while ( count <= 2 && ((ptr = fgets(buf, 256, fp)) != NULL)  ) {
        if (count == 0) {
                sscanf(buf, "%*s %d", &row);
        }
        else if (count == 1) {
        sscanf(buf, "%*s %d", &start);
        }
        else {
        sscanf(buf, "%*s %d", &accept);
        }

                count++;


        }
    // printf("%d %d %d %d\n", row, col, start, accept);   
    tm_data *t_matrix[row][col];
    for(int i = 0; i < row; i++) {
        for (int j =0; j < col; j++) {
            t_matrix[i][j] = NULL;
        }
    }

    while( (ptr = fgets(buf, 256, fp)) != NULL ) {
        strcpy(trans_data, buf);
        int curr_row;
        char *get_token = strtok( trans_data, " " );
        sscanf( get_token, "%d", &curr_row );
        get_token = strtok( NULL, " " );
        while( get_token != NULL ) {
            for ( int i = 0; i < col && get_token != NULL; i++ ) {
                int curr_col, next;
                char t_action;
                printf("curr:%d  i:%d\n", curr_row, i);

                sscanf( get_token, "%d/%d%c", &curr_col, &next, &t_action );
                tm_data *ptr_st = (tm_data *) malloc(sizeof(tm_data));
                 
                ptr_st->currS = curr_col;
                ptr_st->nextS = next;
                ptr_st->trans_action = t_action;
                t_matrix[curr_row][curr_col] = ptr_st;
            
                        
                get_token = strtok( NULL, " " );
            }
            for(int i = 0; i < row; i++) {
                for (int j =0; j < col; j++) {
                    if (t_matrix[i][j] == NULL) {
                        tm_data *ptr_st = (tm_data *) malloc(sizeof(tm_data));
                        ptr_st->currS = j;
                        ptr_st->nextS = 99;
                        ptr_st->trans_action = 'd';
                        t_matrix[i][j] = ptr_st;

                    }
                }
            }
    }
    
    printf("Scanning using the following matrix:\n");
    for ( int j = 0; j < col; j++ ) {
        if ( j == 0 ) {
                printf("%6d", j);
        }
        else {
            printf("%5d", j);
        }
    }
    printf("\n");   
    for ( int i = 0; i < row; i++ ) {
        printf("%2d", i);
        for ( int j = 0; j < col; j++ ) {
        	printf("%4d%c", t_matrix[i][j]->nextS, t_matrix[i][j]->trans_action); 
        }   
        printf("\n");
    }
}
}

int main( int argc, char *argv[] ) {
    if( argc != 2 ) {
        printf("usage: %s tmfile", argv[0]);
        exit( EXIT_FAILURE );
    }
    
    FILE *fp;
    fp = fopen(argv[1], "r");
    if( fp == NULL ) {
        //something went wrong 
        perror( argv[1] );
        exit( EXIT_FAILURE );
    }

    build_transition_matrix( fp );
               
return 0;       
}
