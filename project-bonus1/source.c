/*
1700001623 - Mehmet Faruk Koc
1401020023 - Ugurcan Topcu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_USR 32

typedef struct { 	       // idx file record structure
	char actname [32+1];   // account name string +'\0'
	long int pwdoffset;    // passwd file offset
	int  rel;              // record length in ./passwd
} IdxRec, *pIdxRec;

int indexof(char *str,char ch, int startindex, int length) {
	if(startindex < 0 | startindex > length) return -1;
	for(;startindex<length;startindex++)
	{
		if(str[startindex] == ch)
			return startindex;
	}
	return -1;
}

int main() 
{	
	char ch, *username = malloc(sizeof(char) * (MAX_USR + 1));
	int fdidx, fdpwd, i;
	struct stat bufstat;
	size_t size,len;
	
   	if ( ( fdidx = open("pwd.idx", O_RDONLY , 0) ) == -1){
		fprintf(stderr,"\nOpening pwd.idx failed\n");
		return 1;
	}
	if ( ( fdpwd = open("passwd.srt", O_RDWR , 0) ) == -1){
		fprintf(stderr,"\nOpening passwd failed\n");
		return 2;
	}
	if(stat("./pwd.idx",&bufstat)==-1)
	{
		fprintf(stderr,"stat function failed");
		return 3;
	}
	size = bufstat.st_size;
	len = size / sizeof(IdxRec);
	pIdxRec IdxTab;
	if((IdxTab = malloc(size))==NULL)
	{
		fprintf(stderr,"malloc failed");
		return 4;
	}
	
	read(fdidx,IdxTab,size);
	
	while(scanf("%s", username) != EOF)
	{	
		for(i = 0; i<len; i++) 
		{	
			if(strcmp((IdxTab+i)->actname,username)==0)
			{
				char *buf = malloc((IdxTab+i)->rel-1);
				char *input=malloc(1024);
				if(buf == NULL || input == NULL)
				{fprintf(stderr,"malloc failed\n"); return -1;}
				input[1024] = '\0';
				int j, i1=-1, i2;
				buf[(IdxTab+i)->rel-1] = '\0';
				//*(buf+(IdxTab+i)->rel-1) = '\0';
			
				
				if(pread(fdpwd,buf,(IdxTab+i)->rel-1,(IdxTab+i)->pwdoffset) == -1)
				{fprintf(stderr,"pread passwd.srt\n"); return 5;}
				
				fprintf(stdout,"Record:%s ",buf);
				
					
				for(j=0; j<4;j++)
				{
					i1 = indexof(buf,':',i1+1,(IdxTab+i)->rel);
				}
					i2 = indexof(buf,':',i1+1,(IdxTab+i)->rel);
				
				fprintf(stdout,"Size:%d\n",i2-i1-1);
				if((i2-i1-1)!=0) 
				{
					getchar();
					gets(input);
					
					if(strlen(input) == i2-i1-1) 
					{
						
						if(pwrite(fdpwd,input,i2-i1-1,i1 + 1 + (IdxTab+i)->pwdoffset) == -1)
						{fprintf(stderr,"pwrite passwd.srt\n"); return 6;}
						if(pread(fdpwd,buf,(IdxTab+i)->rel-1,(IdxTab+i)->pwdoffset) == -1)
						{fprintf(stderr,"pread passwd.srt\n"); return 7;}
						fprintf(stdout,"Record After Change:%s \n",buf);
					}
					else 
					{
						fprintf(stdout,"Size do not fit\n");
					}
					
					
				}
				free(buf);
				free(input);
				break;
			}
		}
		if(i==len)
			fprintf(stderr,"username not found\n");
	}
	free(username);
	free(IdxTab);
	return 0;
}
