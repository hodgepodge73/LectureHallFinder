#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

struct course {
    char subject[BUFFER], building[BUFFER], room[BUFFER], days[BUFFER], courseno[BUFFER], from[BUFFER], to[BUFFER];
};

long N_to_I(FILE * indexf, FILE * textf, char *name, long hash_table[ HASHSIZE ])
//turn name into index by looking it up in the hashtable
{
    long returnIndex;
    returnIndex = hash_lookup( name, hash_table, indexf, textf );
    return returnIndex;
}
 
void I_to_N(char returnName[BUFFER], FILE * indexf, FILE * textf, long index, long hash_table[ HASHSIZE ])
//turns index into a name by reading from the location of the index and finding that in the file
//inspired by code from get_string written by Stefan Kremer PhD
{
    
    char buffer[BUFFER];
    int temp;
    fseek( indexf, sizeof(long)*index, SEEK_SET );
    if ( fread( &temp, sizeof(long), 1, indexf ) != 1 )
    {
       fprintf( stderr, "Error: invalid index\n" );
       exit(-1);
    }
    fseek( textf, temp, SEEK_SET );
    fgets( buffer, BUFFER, textf );
    buffer[strlen(buffer)-1] = '\0';
    strcpy(returnName, buffer);
    
}

int hasAlready(char ** intersection, int len, char * key)
//checks an array of chars to see if it already contains the key
//used in the end to remove duplicates
{
    for (int i =0; i < len; i++)
    {
        if (strcmp(key, intersection[i])==0)
        {
            return 1;
        }
    }
    return 0;
}

//Sourced from the query.c file
//Written by Stefan Kremer PhD
int entries( char *filebase )
{
  char filename[BUFFER];
  int ent;

  strcpy( filename, filebase );
  strcat( filename, ".idx" );
  FILE *fp = fopen( filename, "rb" );
  fseek( fp, 0, SEEK_END );
  ent = ftell( fp ) / sizeof(long);
  fclose(fp);
  return ent;
}

void relarr_to_singlearr(int len,int len2, long index, char * array_code_single, char * array_code)
//turns the two dimensional array from the rel file into one dimensional array
{
    for (int j=0;j<len;j++)
    {
      int index2 = index +j*len2;
      array_code_single[j] = *(array_code + index2);
    }
}

void intersection(char ** intersection, char **a, char **b, int alen, int blen)
//performs the intersection function on a and b
{
    //assume intersection size is the sizeof(a) + sizeof(b)
    int k = 0;
    for (int i = 0; i < alen; i ++)
    {
        for (int j = 0; j < blen; j++)
        {
            if (strcmp(a[i],b[j])==0)
            {
                for (int p = 0; p < k; p++)
                {
                    if (hasAlready(intersection, k, a[i]) == 0)
                    {
                        strcpy(intersection[k], a[i]);
                    }
                }
            }
        }
    }
}

int compar(const void * a, const void *b)
//compar function for determinin order in qsort
{
    const struct course *coursea, *courseb;
    coursea = a;
    courseb = b;
    
    if (strcmp(coursea->days, courseb->days)==0)
    {
        //same day now just the times
        char * am1, *am2;
        am1 = strchr(coursea->from, 'A');
        am2 = strchr(courseb->from, 'A');
        if (am1 == NULL &&  am2 == NULL)//pm
        {
            char *toka, *tokb;
            char tempa[BUFFER], tempb[BUFFER];
            strcpy(tempa, coursea->from);
            strcpy(tempb, courseb->from);
            toka = strtok(tempa, ":");
            tokb = strtok(tempb, ":");
            int a_int, b_int;
            a_int = atoi(toka);
            b_int = atoi(tokb);
            if (a_int == 12)
            {
                a_int -= 12;
            }
            if (b_int == 12)
            {
                b_int -= 12;
            }
            return a_int - b_int;
        }
        else if (am1 == NULL)
        {//a less
            return 1;
        }
        else if (am2 == NULL)
        {//b less
            return -1;
        }
        else
        {//am
            char *toka, *tokb;
            char tempa[BUFFER], tempb[BUFFER];
            strcpy(tempa, coursea->from);
            strcpy(tempb, courseb->from);
            toka = strtok(tempa, ":");
            tokb = strtok(tempb, ":");
            int a_int, b_int;
            a_int = atoi(toka);
            b_int = atoi(tokb);
            //printf("%s, %s, %d\n", tempa, tempb, a_int-b_int);
            return a_int - b_int;
        }
    }
    else
    {
        int ascore, bscore;
        if (strcmp(coursea->days, "Mon")==0)
        {
            ascore = 1;
        }
        else if (strcmp(coursea->days, "Tues")==0)
        {
            ascore = 2;
        }
        else if (strcmp(coursea->days, "Wed")==0)
        {
            ascore = 3;
        }
        else if (strcmp(coursea->days, "Thur")==0)
        {
            ascore = 4;
        }
        else if (strcmp(coursea->days, "Fri")==0)
        {
            ascore = 5;
        }
        else if (strcmp(coursea->days, "Sat")==0)
        {
            ascore = 6;
        }
        else
        {
            ascore = 7;
        }

        if (strcmp(courseb->days, "Mon")==0)
        {
            bscore = 1;
        }
        else if (strcmp(courseb->days, "Tues")==0)
        {
            bscore = 2;
        }
        else if (strcmp(courseb->days, "Wed")==0)
        {
            bscore = 3;
        }
        else if (strcmp(courseb->days, "Thur")==0)
        {
            bscore = 4;
        }
        else if (strcmp(courseb->days, "Fri")==0)
        {
            bscore = 5;
        }
        else if (strcmp(courseb->days, "Sat")==0)
        {
            bscore = 6;
        }
        else
        {
            bscore = 7;
        }
    
        if (ascore!=bscore)
        {
            return ascore - bscore;
        }
        else
        {
            return 0;
        }
    }
}

int main( int argc, char **argv )
{
    //create arrays to store names of the files
    char idxname_building[BUFFER];
    char txtname_building[BUFFER];
    char idxname_room[BUFFER];
    char txtname_room[BUFFER];
    char idxname_code[BUFFER];
    char txtname_code[BUFFER];

    char idxname_courseno[BUFFER];
    char txtname_courseno[BUFFER];
    char idxname_days[BUFFER];
    char txtname_days[BUFFER];
    char idxname_from[BUFFER];
    char txtname_from[BUFFER];
    char idxname_to[BUFFER];
    char txtname_to[BUFFER];
    char idxname_subject[BUFFER];
    char txtname_subject[BUFFER];

    char *value_building;
    long hash_table_building[ HASHSIZE ];
    char *value_room;
    long hash_table_room[ HASHSIZE ];
    long hash_table_code[ HASHSIZE ];

    long hash_table_courseno[ HASHSIZE ];
    long hash_table_days[ HASHSIZE ];
    long hash_table_from[ HASHSIZE ];
    long hash_table_to[ HASHSIZE ];
    long hash_table_subject[ HASHSIZE ];
    if (argc!=3)
    {
        fprintf( stderr, "Not enough arguments passed");
        exit(-1);
    }
    // copy the file names
    strcpy( idxname_building, "building.idx");
    strcpy( txtname_building, "building.txt" );
    strcpy( idxname_room, "room.idx");
    strcpy( txtname_room, "room.txt" );
    strcpy( idxname_code, "code.idx");
    strcpy( txtname_code, "code.txt" );

    strcpy( idxname_courseno, "courseno.idx");
    strcpy( txtname_courseno, "courseno.txt" );
    strcpy( idxname_days, "days.idx");
    strcpy( txtname_days, "days.txt" );
    strcpy( idxname_from, "from.idx");
    strcpy( txtname_from, "from.txt" );
    strcpy( idxname_to, "to.idx");
    strcpy( txtname_to, "to.txt" );
    strcpy( idxname_subject, "subject.idx");
    strcpy( txtname_subject, "subject.txt" );

    value_building = argv[1];
    value_room = argv[2];

    get_hashtable( "building", hash_table_building );
    get_hashtable( "room", hash_table_room );
    get_hashtable( "code", hash_table_code );

    get_hashtable( "courseno", hash_table_courseno );
    get_hashtable( "days", hash_table_days );
    get_hashtable( "from", hash_table_from );
    get_hashtable( "to", hash_table_to );
    get_hashtable( "subject", hash_table_subject );

    FILE *idxfile_building = fopen( idxname_building, "r" );
    FILE *txtfile_building = fopen( txtname_building, "r" );
    FILE *idxfile_room = fopen( idxname_room, "r" );
    FILE *txtfile_room = fopen( txtname_room, "r" );

    FILE *idxfile_courseno = fopen( idxname_courseno, "r" );
    FILE *txtfile_courseno = fopen( txtname_courseno, "r" );
    FILE *idxfile_days = fopen( idxname_days, "r" );
    FILE *txtfile_days = fopen( txtname_days, "r" );
    FILE *idxfile_from = fopen( idxname_from, "r" );
    FILE *txtfile_from = fopen( txtname_from, "r" );
    FILE *idxfile_to = fopen( idxname_to, "r" );
    FILE *txtfile_to = fopen( txtname_to, "r" );
    FILE *idxfile_subject = fopen( idxname_subject, "r" );
    FILE *txtfile_subject = fopen( txtname_subject, "r" );

    long index_building = N_to_I(idxfile_building, txtfile_building, value_building, hash_table_building);
    long index_room = N_to_I(idxfile_room, txtfile_room, value_room, hash_table_room);
    char code_building_rel[BUFFER];
    char code_room_rel[BUFFER];

    char code_courseno_rel[BUFFER];
    char code_days_rel[BUFFER];
    char code_from_rel[BUFFER];
    char code_to_rel[BUFFER];
    char code_subject_rel[BUFFER];
    

    strcpy( code_building_rel, "code_building.rel");
    strcpy( code_room_rel, "code_room.rel");
    
    strcpy( code_courseno_rel, "code_courseno.rel");
    strcpy( code_days_rel, "code_days.rel");
    strcpy( code_from_rel, "code_from.rel");
    strcpy( code_to_rel, "code_to.rel");
    strcpy( code_subject_rel, "code_subject.rel");

    FILE *cbfile = fopen( code_building_rel, "rb" );
    FILE *crfile = fopen( code_room_rel, "rb" );

    FILE *ccnfile = fopen( code_courseno_rel, "rb" );
    FILE *cdfile = fopen( code_days_rel, "rb" );
    FILE *cffile = fopen( code_from_rel, "rb" );
    FILE *ctfile = fopen( code_to_rel, "rb" );
    FILE *csfile = fopen( code_subject_rel, "rb" );
   
    int n1 = entries( "code" );
    int n2_b = entries( "building" );
    int n2_r = entries( "room" );
    
    int n2_cn = entries( "courseno" );
    int n2_d = entries( "days" );
    int n2_f = entries( "from" );
    int n2_t = entries( "to" );
    int n2_s = entries( "subject" );

    char *array_code_building = malloc( n1*n2_b );
    char *array_code_room = malloc( n1*n2_r );

    char *array_code_courseno = malloc( n1*n2_cn );
    char *array_code_days = malloc( n1*n2_d );
    char *array_code_from = malloc( n1*n2_f );
    char *array_code_to = malloc( n1*n2_t );
    char *array_code_subject = malloc( n1*n2_s );


    fread( array_code_building, 1, n1*n2_b, cbfile );
    fread( array_code_room, 1, n1*n2_r, crfile );

    fread( array_code_courseno, 1, n1*n2_cn, ccnfile );
    fread( array_code_days, 1, n1*n2_d, cdfile );
    fread( array_code_from, 1, n1*n2_f, cffile );
    fread( array_code_to, 1, n1*n2_t, ctfile );
    fread( array_code_subject, 1, n1*n2_s, csfile );
    

    char *array_code_building_single = malloc( n1);
    char *array_code_room_single = malloc( n1 );
    


    relarr_to_singlearr(n1, n2_b,  index_building, array_code_building_single, array_code_building);
    relarr_to_singlearr(n1, n2_r,  index_room, array_code_room_single, array_code_room);
    
    char *array_intersection = malloc( n1 );
    for (int j=0;j<n1;j++)
    {
        if (((*(j+array_code_room_single)) + '0' == '1') && (*(j+array_code_building_single) + '0' == '1'))
        {
            array_intersection[j] = 1;
        }
        else
        {
              array_intersection[j] = 0;
        }
    }  
    int count_ones=0;
    for (int i = 0; i < n1; i++)
    {
         if (*(i+array_intersection) == 1)
         {
             count_ones++;
         }
    }
    struct course courseList [count_ones];
    int count_courseList_index = 0;
    char buffer_code[BUFFER];
    int k =0;
    int in = 0;
    for (int i = 0; i < n1; i++)
    {
        if (k > 0)
        {
            in += 1;
            k =0;
        }
         if (*(i+array_intersection) == 1)
         {
            for (int j = 0; j < n2_cn; j++)
            {
                int index3 = i*n2_cn+j;
                if (*(array_code_courseno + index3) == 1)
                {
                    I_to_N(buffer_code, idxfile_courseno, txtfile_courseno, j, hash_table_courseno);
                    strcpy(courseList[in].courseno, buffer_code);
                    //printf("%s", courseList[k].courseno);
                    k+=1;
                }
            }
            //find days
            k = 0;
            for (int j = 0; j < n2_d; j++)
            {
                int index3 = i*n2_d+j;
                if (*(array_code_days+ index3) == 1){
                    I_to_N(buffer_code, idxfile_days, txtfile_days, j, hash_table_days);
                    strcpy(courseList[in].days, buffer_code);
                    //printf("%s, %d", courseList[k].days, k);
                    k = k +1;
                }
            }
            //find from
            k = 0;
            for (int j = 0; j < n2_f; j++)
            {
                int index3 = i*n2_f+j;
                if (*(array_code_from+ index3) == 1){
                    I_to_N(buffer_code, idxfile_from, txtfile_from, j, hash_table_from);
                    strcpy(courseList[in].from, buffer_code);
                    //printf("%s", buffer_code);
                    k+=1;
                }
            }
            k = 0;
            for (int j = 0; j < n2_t; j++)
            {
                int index3 = i*n2_t+j;
                if (*(array_code_to+ index3) == 1){
                    I_to_N(buffer_code, idxfile_to, txtfile_to, j, hash_table_to);
                    strcpy(courseList[in].to, buffer_code);
                    //printf("%s", buffer_code);
                    k+=1;
                }
            }
            //subject
            k = 0;
            for (int j = 0; j < n2_s; j++)
            {
                int index3 = i*n2_s+j;
                if (*(array_code_subject+ index3) == 1){
                    I_to_N(buffer_code, idxfile_subject, txtfile_subject, j, hash_table_subject);
                    strcpy(courseList[in].subject, buffer_code);
                    //printf("%s %d", courseList[k].subject, k);
                    k+=1;
                }
            }
            //I_to_N(buffer_code, idxfile_code, txtfile_code, i, hash_table_code);
            count_courseList_index++;
         }
        //printf("%d %d\n", i, n1);

    }
    for (int j = 0; j < count_ones; j++)
    {
        strcpy(courseList[j].room, value_room);
    }
    //building
    for (int j = 0; j < count_ones; j++)
    {
        strcpy(courseList[j].building, value_building);
    }
    struct course courseList2 [count_ones];
    int count_newlist = 0;
    int exists = 0;
    for (int i = 0; i < count_ones; i++)
    {
        exists = 0;
        for(int j = 0; j < count_newlist; j ++)
        {
            if ((strcmp(courseList2[j].courseno, courseList[i].courseno) == 0) && (strcmp(courseList2[j].subject, courseList[i].subject) == 0))
            {
                exists = 1;
            }
        }
        if (!exists){
            strcpy(courseList2[count_newlist].building, courseList[i].building);
            strcpy(courseList2[count_newlist].courseno, courseList[i].courseno);
            strcpy(courseList2[count_newlist].days, courseList[i].days);
            strcpy(courseList2[count_newlist].from, courseList[i].from);
            strcpy(courseList2[count_newlist].to, courseList[i].to);
            strcpy(courseList2[count_newlist].room, courseList[i].room);
            strcpy(courseList2[count_newlist].subject, courseList[i].subject);
            count_newlist++;
            //printf("%d", count_newlist);
        }
        
    }
    int count_days_total = 0;
    char *token;
    for (int i = 0; i < count_newlist; i++){
        char day_temp[BUFFER];
        strcpy(day_temp, courseList2[i].days);
        token = strtok(day_temp, ", ");
        while (token != NULL){
            count_days_total ++;
            token = strtok(NULL, ", ");
        }
    }
    struct course courseList3 [count_days_total];
    int counter =0;
    for (int i = 0; i < count_newlist; i++){
        char day_temp[BUFFER];
        strcpy(day_temp, courseList2[i].days);
        token = strtok(day_temp, ", ");
        while (token != NULL){
            strcpy(courseList3[counter].building, courseList2[i].building);
            strcpy(courseList3[counter].courseno, courseList2[i].courseno);
            strcpy(courseList3[counter].days, token);
            strcpy(courseList3[counter].from, courseList2[i].from);
            strcpy(courseList3[counter].to, courseList2[i].to);
            strcpy(courseList3[counter].room, courseList2[i].room);
            strcpy(courseList3[counter].subject, courseList2[i].subject);
            counter ++;
            token = strtok(NULL, ", ");
        }
    }
    
    //printf("%d", count_days_total);
    qsort(courseList3, count_days_total, sizeof(struct course), compar );
    for (int i = 0; i < count_days_total; i++){
        printf("%s*%s %s %s - %s\n", courseList3[i].subject, courseList3[i].courseno, courseList3[i].days, courseList3[i].from, courseList3[i].to);
    }

    free(array_code_building);
    free(array_code_room);
    free(array_intersection);
    free(array_code_courseno);
    free(array_code_days);
    free(array_code_from);
    free(array_code_to);
    free(array_code_subject);

    free(array_code_building_single);
    free(array_code_room_single);

    fclose(idxfile_building);
    fclose(txtfile_building);
    fclose(idxfile_room);
    fclose(txtfile_room);

    fclose(idxfile_courseno);
    fclose(txtfile_courseno);
    fclose(idxfile_days);
    fclose(txtfile_days);
    fclose(idxfile_from);
    fclose(txtfile_from);
    fclose(idxfile_to);
    fclose(txtfile_to);
    fclose(idxfile_subject);
    fclose(txtfile_subject);

    fclose(cbfile);
    fclose(crfile);

    fclose(ccnfile );
    fclose(cdfile );
    fclose(cffile );
    fclose(ctfile );
    fclose(csfile);
    
    return 0;
}
