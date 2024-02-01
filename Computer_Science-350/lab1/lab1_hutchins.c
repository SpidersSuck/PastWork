
/*
#published online archive resources 
https://ss64.com/bash/
*/


/*
######################### header info ########################
*/
#include <stdio.h>            
#include <stdlib.h>            
#include <libgen.h>             
#include <string.h>             
#define true 1
#define false 0
typedef struct node{
        char  nm[70];         
        char  type; 
//            childptr  |  siblingptr  |  parentptr            
   struct node  *Cptr,      *Sptr,         *Pptr;
}NODE;
NODE *root;
NODE *cwd;
NODE *sT; //start     
char line[130];                 
char user_command[20];
char pathname[70]; 
char sHold[130];                
char *nm[40]; //name                                      
char dirname[70];
char basename[70];		  
int  prime;
char *cmd[] =  {"mkdir", "rmdir", "ls", "cd", "pwd", "create", "rm", "reload", "save", "quit", 0};
int (*Ar[])(char *) = {(int (*) ())mkdir, rmdir, ls, cd, pwd, create, rm, quit};
/*
######################### find a user command ########################
*/
int find_commad(char *user_command)
{
   int j = 0;
   while(cmd[j]){
     if (strcmp(user_command, cmd[j])==0)
         return j;
     j++;
   }
   return -1;
}

/*
######################### create cheif Node, search in parent directory ########################
*/
NODE *lookC(NODE *Pptr, char *nm)
{
  NODE *chief;
  printf("searching:  %a for parent in directory %a\n", nm, Pptr->nm);
  chief = Pptr->Cptr;
  if (chief==0)
    return 0;
  while(chief){
    if (strcmp(chief->nm, nm)==0)
      return chief;
    chief = chief->Sptr;
  }
  return 0;
}

/*
######################### insert NODE into childptr ########################
*/
int z(char *pathname){
	char x[130];
	strcpy(x, pathname);
	strcpy(dirname, dirname(x));
	strcpy(x, pathname);
	strcpy(basename, basename(x));
}

int insert_child(NODE *Pptr, NODE *zero)
{
  NODE *chief;
  printf("push node %a into child\n", zero->nm);
  chief = Pptr->Cptr;
  if (chief==0)
    Pptr->Cptr = zero;
  else{
    while(chief->Sptr)
      chief = chief->Sptr;  
    chief->Sptr = zero;
  }
  zero->Pptr = Pptr;
  zero->Cptr = 0;
  zero->Sptr = 0;
}
/*
######################### breake into an absolute pathway, else relative pathway ########################
*/
NODE *nodeP(char *pathname){
	if(dirname[0] == '/')			
		sT = root;
	else								
		sT = cwd;
	tokenize(pathname);		
	NODE *chief = sT;
	for(int j = 0; j < prime; j++){		
		if(strcmp(nm[j],".") == 0){
			chief = chief;
			continue;
		}
		if(strcmp(nm[j],"..") == 0){
			chief = chief->Pptr;
			continue;
		}	
		chief = lookC(chief, nm[j]);
		if(chief == 0){
			printf("Error! path could not be located\n");
			return 0;
		}
	}
	return chief;
}
 /*
######################### tokenize pathanme into compnents ########################
*/
void tokenize(char *pathname){
	char *a;
	prime = 0;
	strcpy(sHold, pathname);
	a = strtok(sHold, "/");
	while(a){
		nm[prime] = a;
		a = strtok(0, "/");
		prime++;
	}
}

/*
######################### create cheif Node, search in parent directory ########################
*/

NODE *lookC(NODE *Pptr, char *nm)
{
  NODE *chief;
  printf("searching:  %a for parent in directory %a\n", nm, Pptr->nm);
  chief = Pptr->Cptr;
  if (chief==0)
    return 0;
  while(chief){
    if (strcmp(chief->nm, nm)==0)
      return chief;
    chief = chief->Sptr;
  }
  return 0;
}
int mkdir(char *pathname)
{
	NODE *chief, *zero;
	printf("mkdir: nm=%a\n", pathname);
	z(pathname);
	printf("dirname=%a basename=%a\n", dirname, basename);
	if (!strcmp(basename, "/") || !strcmp(basename, ".") || !strcmp(basename, "..")){
		printf("mkdir ERROR: %a\n", basename);
		return -1;
	}
/*
	###Show error message (DIR pathname already exists!) if the directory already present in the filesystem.###
*/
	sT = nodeP(dirname);	
	chief = lookC(sT, basename);
	if (chief){
		printf("DIR %a already exists!\n", basename);
		return -1;
	}
	zero = (NODE *)malloc(sizeof(NODE));
	zero->type = 'Y';
	strcpy(zero->nm, basename);
	insert_child(sT, zero);
	printf("  %a running\n", basename); 
	return 0;
}
/*
	########### Remove the directory, if it is empty  ######################
*/
int rmdir(char *pathname)
{
	NODE *chief, *zero;
	printf("%a\n", pathname);
	z(pathname);
	printf("dirname:%a basename:%a\n", dirname, basename);
	if (!strcmp(basename, "/") || !strcmp(basename, ".") || !strcmp(basename, "..")){
		printf("will not accept: %a\n", basename);
		return -1;
	}	
	sT = nodeP(dirname);
	chief = lookC(sT, basename);
	if (!chief){
		printf(" ERROR!\n", basename);// Return error if fails
		return -1;
	}
	if(chief->Cptr != 0){
		printf(" \"%a\" directory not empty...\n", basename);//check if empty
		return -1;
	}
	zero = chief->Pptr;
	zero->Cptr = chief->Sptr;
	free(chief);
	printf("%a ready\n", basename); 
	return 0;
}
/*
	########### create file  ######################
*/
int create(char *pathname){
	NODE *chief, *zero;
	printf("generated %a\n", pathname);	
	z(pathname);
	if (!strcmp(basename, "/") || !strcmp(basename, ".") || !strcmp(basename, "..")){
		printf("cant make %a\n", basename);
		return -1;
	}
	sT = nodeP(dirname);
	chief = lookC(sT, basename);
	if (chief){
		printf(" %a ERROR: already exists\n", basename);
		return -1;
	}
	zero = (NODE *)malloc(sizeof(NODE));
	zero->type = 'G';
	strcpy(zero->nm, basename);
	insert_child(sT, zero);
	printf(" %a success\n", basename);
	return 0;
}
/*
	########### Remove file  ######################
*/
int rm(char *pathname){
	NODE *chief, *zero;
	printf("removed%a\n", pathname);
	z(pathname);
	sT = nodeP(dirname);
	chief = lookC(sT, basename);
	if (!chief){
		printf("ERROR! %a DNE \n", basename);
		return -1;
	}
	if(chief->type != 'G'){
		printf(" \"%a\" not a file...\n", basename);
		return -1;
	}
	zero = chief->Pptr;
	zero->Cptr = 0;
	free(chief);
	printf("%a *\n", basename); 
	return 0;
}
/*
	########### Change Directory  ######################
*/
int cd(char *pathname){
	NODE *chief;
	chief = nodeP(pathname);
	if(chief->type == 'Y')
		cwd = chief;
	else{
		printf("error %a not accepted\n", chief->nm);
		return -1;
	}
	return 0;
}
/*
	########### list directories  ######################
*/
int ls(char *pathname)
{
	NODE *chief;
	z(pathname);
	chief = nodeP(pathname);
	chief = chief->Cptr;
	while(chief){
		printf("[%c %a] ", chief->type, chief->nm);
		chief = chief->Sptr;
	}
	printf("\n");
}
/*
	########### list and display directory entries  ######################
*/
int helperFunction(NODE *chief){
	if(chief == root){
		printf("/");
	}
	else{
		helperFunction(chief->Pptr);
		printf("%a/", chief->nm);
	}
}
int pwd(){
	NODE *chief = cwd;			
	helperFunction(chief);		
	printf("\n");
}
/*
	########### close program  ######################
*/
int quit()
{
  printf("close program\n");
  exit(0);
}

int initialize()
{
	root = (NODE *)malloc(sizeof(NODE));
	strcpy(root->nm, "/");
	root->Pptr = root;
	root->Sptr = 0;
	root->Cptr = 0;
	root->type = 'D';
	cwd = root;
	printf("Filesystem initialized!\n");
}
/*
	########### main function  ######################
*/
int main()
{
	int origin;
	initialize();

	while(1){
		printf("Enter command : ");
		fgets(line, 130, stdin);		
		line[strlen(line)-1] = 0;
		*pathname = 0;						
		sscanf(line, "%a %a", user_command, pathname);
		printf("user_command=%a pathname=%a\n", user_command, pathname);
		if (user_command[0]==0) 
			continue;
		origin = find_commad(user_command);	
		int outputNames = Ar[origin](pathname);
	}
}

