#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <limits.h>

int postOrderApply(char* path,int sizepathfun(char *path1));
int is_regular_file(const char *path);
int is_dir(const char *path);
int is_link(const char *path);
int is_pipe(const char *path);
int sizepathfun(char *path);
int z=0;
char tut[256];
char s[256];
char pc[256];
int main(int argc, char** argv) {
    
    char cwd[256];
    int size;
    FILE* file;
    int p=0;
    DIR *dr;
    getcwd(tut,sizeof(tut));
    strcat(tut,"/141044012sizes.txt");
    getcwd(s,sizeof(s));
    strcat(s,"/size.txt");
    getcwd(pc,sizeof(pc));
    strcat(pc,"/pc.txt");

    file=fopen(pc,"w");
    fprintf(file, "%d",p);
    fclose(file);
    if(argc==3){
        if(!strcmp(argv[1],"-z")){
            z=1;

                if(argv[2][0]=='/'){
                    strcpy(cwd,argv[2]);
                }
                else{
                    getcwd(cwd,sizeof(cwd));
                    strcat(cwd,"/");
                    strcat(cwd,argv[2]);
                }
            
            size=postOrderApply(cwd,sizepathfun);
            
            file = fopen(pc,"r");
            flock(fileno(file),1);
            fscanf(file,"%d",&p);
            flock(fileno(file),8);
            fclose(file);
            
            file = fopen (tut, "a");
            fprintf(file,"pid:%d -- %d B -- %s\n",getpid(),size,cwd);
            fprintf(file, "%d child processes created\n", p);        
            fclose(file);

            printf("pid:%d -- %d B -- %s\n",getpid(),size,cwd);
            printf("%d child processes created\n", p);
        }
        else{
            printf("Wrong Format! Usage: ./buNeDu -z Path or ./buNeDu Path\n");
        }

    }
    else if(argc==2){
        z=0;
        if(!strcmp(argv[1],"-z")){
            printf("Wrong Format! Usage: ./buNeDu -z Path or ./buNeDu Path\n");
        }
        else{    

                if(argv[1][0]=='/'){
                    strcpy(cwd,argv[1]);
                }
                else{
                    getcwd(cwd,sizeof(cwd));
                    strcat(cwd,"/");
                    strcat(cwd,argv[1]);
                }
            


            size=postOrderApply(cwd,sizepathfun);

            file = fopen(pc,"r");
            flock(fileno(file),1);
            fscanf(file,"%d",&p);
            flock(fileno(file),8);
            fclose(file);

            file = fopen (tut, "a");
            fprintf(file,"pid:%d -- %d B -- %s\n",getpid(),size,cwd);
            fprintf(file, "%d child processes created\n", p);            
            fclose(file);

            printf("pid:%d -- %d B -- %s\n",getpid(),size,cwd);
            printf("%d child processes created\n", p);
        }
        
    }
    else{
        printf("Wrong Format! Usage: ./buNeDu -z Path or ./buNeDu Path \n");
    }
        
    return 0;
    
}

int postOrderApply(char* path,int sizepathfun(char *path1)){
    
    struct dirent *de;
    char cwd[256];
    char old[256];
    int size1=0;
    int size2=0;
    int size3=0;
    int totalsize=0;
    int sum=0;

    FILE* file;
    FILE* sfile;
    FILE* pfile;
    strcpy(cwd,path);
    chdir(cwd);
    DIR *dr = opendir(cwd); 
    strcpy(old,cwd);
    struct stat statbuf;
    if (dr == NULL)  
    { 

        printf("Could not open current directory -> %s",cwd); 
        return 0; 
        
    } 
  
    else{
        while ((de = readdir(dr)) != NULL){
        strcat(cwd,"/");
        strcat(cwd,de->d_name);
        if(is_regular_file(cwd) && !is_link(cwd) && !is_pipe(cwd)){             
            stat(cwd,&statbuf);
            
            file = fopen (tut, "a");
            size1+=sizepathfun(cwd);
            sum+=size1;
            totalsize+=size1;
            flock(fileno(file),2);
            fprintf(file,"pid:%d -- %d B -- ",getpid(),size1);
            fprintf(file,"%s\n",cwd);
            flock(fileno(file),8);
            fclose(file);

            printf("pid:%d -- %d B -- ",getpid(),size1);
            printf("%s\n",cwd);
            size1=0;
            
        }
        else if(is_dir(cwd)){
            if(strcmp(de->d_name,".")!=0 && strcmp(de->d_name,"..")!=0){             
                stat(cwd,&statbuf);
                
                if(fork()==0){                                
                    size2+=postOrderApply(cwd,sizepathfun);                    

                    file = fopen (tut, "a");
                    flock(fileno(file),2);
                    fprintf(file,"pid:%d -- %d B -- ",getpid(),size2);
                    fprintf(file,"%s\n",cwd);     
                    flock(fileno(file),8);               
                    fclose(file);

                    printf("pid:%d -- %d B -- ",getpid(),size2);
                    printf("%s\n",cwd);

                    sfile = fopen(s,"w");
                    flock(fileno(sfile),2);
                    fprintf(sfile,"%d",size2);
                    flock(fileno(sfile),8);
                    fclose(sfile);            
                    exit(0);

                }
                else{
                    wait(NULL);
                    int p;
                    pfile = fopen(pc,"r");
                    flock(fileno(pfile),1);
                    fscanf(pfile,"%d",&p);
                    flock(fileno(pfile),8);
                    fclose(pfile);  

                    pfile = fopen(pc,"w");
                    flock(fileno(pfile),2);
                    fprintf(pfile,"%d",++p);
                    flock(fileno(pfile),8);
                    fclose(pfile); 
                    
                }

                sfile = fopen(s,"r");
                flock(fileno(sfile),1);
                fscanf(sfile,"%d",&size2);
                flock(fileno(sfile),8);
                fclose(sfile);
                totalsize+=size2;
                
                size2=0;


                
                
            }
        }
        else if(is_link(cwd) || is_pipe(cwd)){            
            stat(cwd,&statbuf);
            file = fopen (tut, "a");
            flock(fileno(file),2);
            fprintf(file,"pid:%d -- Special File  ",getpid());
            fprintf(file,"%s\n",cwd);
            flock(fileno(file),8);
            fclose(file);

            printf("pid:%d -- Special File  ",getpid());
            printf("%s\n",cwd); 

            size3+=0;
            totalsize+=size3;
            size3=0;
            
        }
        strcpy(cwd,old);           
    }

    }
    if(z==0){
        totalsize=sum;
    }

    closedir(dr);
    return totalsize;
}
int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
int is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
int is_link(const char *path)
{
    struct stat path_stat;
    lstat(path, &path_stat);
    return S_ISLNK(path_stat.st_mode);
}
int is_pipe(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISFIFO(path_stat.st_mode);
}
int sizepathfun(char *path){
    struct stat statbuf;
    if(stat(path, &statbuf) == -1){
        perror("Failed to get file status");
        return -1;
    }
    else if(S_ISREG(statbuf.st_mode) > 0){
        return statbuf.st_size;
    }
    else if(S_ISDIR(statbuf.st_mode) > 0){
        return 0;
    }

    return 0;
            
}
