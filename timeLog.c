#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>   
#include <stdbool.h>
#include <stdlib.h>

#define START_CMD "start"
#define STOP_CMD "stop"
#define DISP_CMD "display"
#define NEW_LOG_FILE "new"
#define CONFIG_PATH "config.txt"
#define LOG_PATH_LINE 0
#define EXIT "exit"
#define NUM_CMD 2
#define MAX_LINE_LENGTH 50
#define MAX_DAY_STRING 11

typedef struct Record_t{
    char cmd[100];
    char wday[11]; 
    int day;
    int mon;
    int year;
    int hour;
    int min; 
    int sec; 
    double hwork;
}Record_t;

bool file_exists (char *filename);
void getDayOfWeek(char* buff,int daySinceSunday);
void timeToRecord(struct tm* timeinfo,char* cmd,Record_t* rc);
void fprintRecord(FILE* fp,Record_t* rc);
void printRecord(Record_t* rc);
double getHoursWorked(Record_t* last, Record_t* rc);
void hangForUser();
void getConfig(char* buff,int val);
void getUserInput(char* cmd,char* arg1);
void stopCommand(char* logPath, bool usingCmdArg);
int displayCommand(char* logPath,bool usingCmdArg);
int replace_string_on_line(int line_num, char* newline,char* file_path);
void makePath(char* dir, char* name, char* buff);
void newLogFile(char* newPath);
void getRecord(Record_t* record,char* cmd);
void startCommand(char* logPath,bool usingCmdArg);

/**
 * Returns if a file exists
 * @filename the path to the file.
 */

bool file_exists (char *filename) {
    struct stat buffer;   
    return (stat (filename, &buffer) == 0);
}
/**
 * Assigns the day of the week to the buffer
 * @buff the output buffer
 * @daySinceSunday the days since sunday E [0,6]
 */
void getDayOfWeek(char* buff,int daySinceSunday)
{
    switch(daySinceSunday)
    {
        case 0: strcpy(buff,"Sunday");      break;
        case 1: strcpy(buff,"Monday");      break;
        case 2: strcpy(buff,"Tuesday");     break;
        case 3: strcpy(buff,"Wednesday");   break;
        case 4: strcpy(buff,"Thursday");    break;
        case 5: strcpy(buff,"Friday");      break;
        case 6: strcpy(buff,"Saturday");    break;
    }
}
/**
 * convert struct tm* to local Record_t format
 * @timeinfo the struct to be converted from
 * @rc the new record format.
 */
void timeToRecord(struct tm* timeinfo,char* cmd,Record_t* rc){
    char wday[MAX_DAY_STRING];
    getDayOfWeek(wday,timeinfo->tm_wday);
    strcpy(rc->cmd,cmd);
    strcpy(rc->wday,wday);
    rc->day = timeinfo->tm_mday;  
    rc->mon = timeinfo->tm_mon+1; 
    rc->year = timeinfo->tm_year+1900; 
    rc->hour = timeinfo->tm_hour; 
    rc->min = timeinfo->tm_min;
    rc->sec = timeinfo->tm_sec; 
}

/**
 * prints out a time record to file
 * @fp a pointer to the file to be writen to
 * @rc the record to be printed
 */
void fprintRecord(FILE* fp,Record_t* rc)
{   
    fprintf (fp,"%s \t %s \t %d \t %d \t %d \t %d:%d:%d",rc->cmd,
                                                                rc->wday,
                                                                rc->day,  
                                                                rc->mon,  
                                                                rc->year, 
                                                                rc->hour, 
                                                                rc->min,  
                                                                rc->sec);
    if(!strcmp(rc->cmd,STOP_CMD)){
        fprintf(fp,"\t %.2f\n",rc->hwork);
    }
    else{
        fprintf(fp,"\n");    
    }
}
void printRecord(Record_t* rc)
{
    printf ("%s \t %s \t %d \t %d \t %d \t %d:%d:%d",rc->cmd,
                                                            rc->wday,
                                                            rc->day,  
                                                            rc->mon,  
                                                            rc->year, 
                                                            rc->hour, 
                                                            rc->min,  
                                                            rc->sec);
    if(!strcmp(rc->cmd,STOP_CMD))
    {  
        printf("\t\t %.2f\n",rc->hwork);
    }
    else{
        printf("\n");    
    }
}
/**
 * Get the hours worked between the two records
 * @rc the current record
 * @last the previous record
 *  To DO: update to deal with more times, ie dif days, months, years.
*/
double getHoursWorked(Record_t* last, Record_t* rc){
    int years = rc->year-last->year;
    int months = rc->mon-last->mon;
    int days = rc->day-last->day;
    int hours = rc->hour-last->hour;
    int mins = rc->min-last->min;
    int sec = rc->sec-last->sec;

    if(!days){ //i.e same day
        return hours+mins/60+sec/360;
    }
    else //i.e dif days
    { 
        printf("Go to bed bro timelogger is a on break");
        printf("WARNING: logger assumes that you work within 24hr day.\n");
        printf("i.e midninght to midnight log may be inacurate.");
        return -1;
    }
}
/**
 * Waits for the user to hit enter
 */
void hangForUser(){
    printf("Hit enter to close\n");
    fflush(stdin);   
    getchar();

}
/**
 * gets the ocnfig value stord at index val.
 */
void getConfig(char* buff,int val){
    FILE* fp = fopen(CONFIG_PATH,"r");
    for(int i=0;i<val;i++){
        char c;
        while(c=fgetc(fp)!='\n' ){
            if(c==EOF) break;
        }
    }
    fscanf(fp,"%s",buff);
    fclose(fp);
}


/**
 * Gets the input from the user when opened from file explorer
 * @cmd the command from the user
 * @arg1 buff to contain the second input argument.
 */

void getUserInput(char* cmd,char* arg1)
{    
        printf("G'day mate, another day on the grind, aye!\n\n");
        while(strcmp(cmd,STOP_CMD)){
            if(!strcmp(cmd,START_CMD)) break;
            if(!strcmp(cmd,DISP_CMD)) break;
            if(!strcmp(cmd,EXIT)) exit(-1);
            printf("Usage: type \"start + enter\" to sign in.\n");
            printf("       type \"stop + enter\" to sign out.\n");
            printf("       type \"display + enter\" show timesheet.\n");
            printf("       type \"new + enter\" to start a new timesheet.\n");
            printf("       type \"exit + enter\" to abort.\n");
            
            scanf("%s",cmd);
            if(!strcmp(cmd,NEW_LOG_FILE)){
                scanf("%s",arg1);
            }
        }
}
/**
 * Handels a request to sign out by the user
 * @logPath the path to the timesheet file
 * @usingCmdArg indicates if the app was launched from 
 * command line or from a file explorer
 */
void stopCommand(char* logPath, bool usingCmdArg)
{
    Record_t last;
    Record_t newRecord;
    getRecord(&newRecord,STOP_CMD);

    int backoff = 1;
    FILE* fp;
    if(!(fp=fopen(logPath,"r")))
    {
        printf("cant open file");
    }

    printf("Last record:\n");
    printf("Status \t DayOfTheWeek \t Day \t Month \t Year \t Hour:Minuit:Second \t HoursWorked\n");
        
    while(strcmp(last.cmd,START_CMD)) //find the last start cmd
    {
        fseek(fp,-backoff*MAX_LINE_LENGTH,SEEK_END); //go back a bit over 1 line from the end
        //printf("  fp = %d",backoff);printf("  fp = %d",fp);
        if(ftell(fp)-MAX_LINE_LENGTH<0){
            printf("Mate, you gotta start working before you can nock off!\n");
            printf("i.e.File does not contain start record\n");
            if(!usingCmdArg) hangForUser();
            fclose(fp);
            exit(-1);
        }

        char buff[MAX_LINE_LENGTH];     
        fgets(buff,MAX_LINE_LENGTH,fp); //consume the rest of the line
        
        fscanf(fp,"%s \t %s \t\t %d \t %d \t \t %d \t %d:%d:%d\n",last.cmd,
                                                                last.wday,
                                                                &(last.day),  
                                                                &(last.mon),  
                                                                &(last.year), 
                                                                &(last.hour), 
                                                                &(last.min),  
                                                                &(last.sec));
        backoff++;
    }

    if(backoff>2){
        printf("Mate, you gotta start working before you can nock off!\n");
        printf("i.e.last record was a stop\n");
        if(!usingCmdArg) hangForUser();
        fclose(fp);
        exit(-1);
    }
    fclose(fp);
    newRecord.hwork = getHoursWorked(&last,&newRecord);
    printRecord(&last);
    printRecord(&newRecord);
    
    fp = fopen(logPath,"a");
    fprintRecord(fp,&newRecord);
    fclose(fp);
    exit(0);
}
/**
 * Displays the contents of the current timesheet
 * @logPath the path to the timesheet
 * @usingCmdArg indicats if the app was launched from command line 
 * or a file explorer.
 */
int displayCommand(char* logPath,bool usingCmdArg){
    FILE* fp = fopen(logPath,"r");
    char c;
    while((c=fgetc(fp))!='\n'); //consume 2 lines
    while((c=fgetc(fp))!='\n');
    Record_t frecord;
    printf("Displaying records for %s\n",logPath);
    printf("Status \t DayOfTheWeek \t Day \t Month \t Year \t Hour:Minuit:Second \t HoursWorked\n");
    while(fscanf(fp,"%s \t %s \t\t %d \t %d \t \t %d \t %d:%d:%d",frecord.cmd,
                                                            frecord.wday,
                                                            &(frecord.day),  
                                                            &(frecord.mon),  
                                                            &(frecord.year), 
                                                            &(frecord.hour), 
                                                            &(frecord.min),  
                                                            &(frecord.sec))!=EOF){
        if(!strcmp(frecord.cmd,STOP_CMD))
        {  
            fscanf(fp,"%lf",&(frecord.hwork));
        }    
        printRecord(&frecord);
    }
    fclose(fp);
    if(!usingCmdArg) hangForUser();
    exit(0);
}



/**
 * Replaces a line in a file
 * @line_num the line to replac 
 * @to_string the new line
 * @file_path the file path 
 */
int replace_string_on_line(int line_num, char* newline,char* file_path)
{
	
	FILE* fp; 
	if((fp = fopen(file_path,"r")) ==NULL){}
	FILE* new_fp;
	if((new_fp = fopen("tmp.txt","w"))==NULL){}
	
    char tmp[MAX_LINE_LENGTH]; 
	for(int i=0;i<line_num;i++){
		fputs(fgets(tmp,MAX_LINE_LENGTH,fp),new_fp);
	}
    strcat(newline,"\n");
	fputs(newline,new_fp);
	fgets(tmp,MAX_LINE_LENGTH,fp); //Consume replaced line

	while(fgets(tmp,MAX_LINE_LENGTH,fp)!=NULL){
		fputs(tmp,new_fp);
	}

	fclose(fp);
	fclose(new_fp);

	if(remove(file_path)){
		printf("file not removed succesfully\n");
	}

	if(rename("tmp.txt",file_path)){
		printf("error file not renamed\n");
	}

	return 0;
}

/**
 * concatinates a name and directory into a path
 * @dir the name of the directory
 * @name the name of the file
 * @buff the return buffer to contain the output.
 */
void makePath(char* dir, char* name, char* buff)
{
    strcpy(buff,"./");
    strcat(buff,dir);
    strcat(buff,"/");
    strcat(buff,name);
    size_t len = strlen(buff);

    while((buff[len-1] == '\n' || buff[len-1] == '\r'
           && len > 0))
    {
        buff[--len] = '\0';
    }
  
}
/**
 * creats a new timesheet 
 * @newPath the path to the file to be created includeing the name of the file.
 */
void newLogFile(char* newPath){

    FILE* fp;

    if(!(fp=fopen(newPath,"w"))){
        printf("cant create new file\n");
        exit(-1);
    }
    fprintf(fp,"Status \t DayOfTheWeek \t Day \t Month \t Year \t Hour:Minuit:Second \t HoursWorked\n");
    
    for(int i=0;i<84;i++) fprintf(fp,"-");
    fprintf(fp,"\n");

    fclose(fp);
}
/**
 * adds data to a record from the computers system clock
 * @record struct for the data
 * @cmd the use case specified by the user
 */
void getRecord(Record_t* record,char* cmd)
{
    time_t rawtime;
    struct tm* timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    timeToRecord(timeinfo,cmd,record);

}
/**
 * Handles a request to sign in to work
 * @logPath the path to the timesheet file
 * @usingCmdArg indicates if the program was launched from the comand line
 * or a file explorer
 */
void startCommand(char* logPath,bool usingCmdArg)
{
    Record_t record;
    FILE* fp = fopen(logPath,"a");
    
    printf("Last record:\n");
    printf("Status \t DayOfTheWeek \t Day \t Month \t Year \t Hour:Minuit:Second \t HoursWorked\n");
   
    getRecord(&record,START_CMD);
    fprintRecord(fp,&record);

    printRecord(&record);
    fclose(fp);
    exit(0);
}
/**
 * Create a new timesheet file if a request is made
 * @name the name of the file with no path.
 */
void newFileCommand(char* name)
{
    replace_string_on_line(LOG_PATH_LINE,name,CONFIG_PATH);
    char dir[_MAX_PATH];
    char path[_MAX_PATH];
    getConfig(dir,1);
    makePath(dir,name,path);
    
    newLogFile(path);
    printf("New Log file created: %s\n",path);
    exit(0);
}

int main(int argc,char** argv)
{   
    bool usingCmdArg = true;
    char* cmd;
    char* arg1 = argv[2]; 
    
    if(argc<NUM_CMD){
        char command[100];
        char argument1[100];
        usingCmdArg = false;
        getUserInput(command,arg1);
        cmd = command;
        arg1 = argument1;
        
    }
    else{
        cmd = argv[1];
    }

    //new file command
    if(!strcmp(cmd,NEW_LOG_FILE)){
        newFileCommand(arg1);
    }
    //get the logpath from the config file.
    char logName [_MAX_PATH];
    char logDir [_MAX_PATH];
    char logPath [_MAX_PATH];
    getConfig(logName,0);
    getConfig(logDir,1);

    makePath(logDir,logName,logPath);
    
    //if no log file create one.
    if(!file_exists(logPath)){
    
        newLogFile(logPath);
    }
    //display command
    if(!strcmp(cmd,DISP_CMD)){
       displayCommand(logPath,usingCmdArg);
    }
    if(!strcmp(cmd,STOP_CMD))
    {   
        stopCommand(logPath,usingCmdArg);
    }
    if(!strcmp(cmd,START_CMD)){
        startCommand(logPath,usingCmdArg);
    }

    printf("Error Command not found\n");
    printf("Usage: ./timeLog start #to sign in.\n");
    printf("       ./timelog stop #to sign out.\n");
    printf("       ./timelog display #to sign out.\n");
    printf("       ./timelog exit #to sign out.\n");
    printf("       ./timelog new #to start a new timesheet.\n");
    
    if(!usingCmdArg) hangForUser();
    return 0;

}



