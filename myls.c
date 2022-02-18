#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "myls.h"

int main(int argc, char *argv[]) 
{
	if(argc == 1)
	{
		struct dirent** namelist;
		int n;
		n = scandir(".", &namelist, NULL, alphasort);
		if(n == -1)
			return 0;
		int index = 0;
		while(index < n)
		{
			if(namelist[index]->d_name[0] != '.')
				printf("%s\n", namelist[index]->d_name);
			free(namelist[index]);
			index ++;
		}
		free(namelist);
		return 0;
  	}

  	/* get flags */
  	int index = 1;
  	char* temp = argv[index];
  	while(index < argc && temp[0] == '-')
  	{
  		
  		for(int j = 1; j < strlen(temp); j ++)
  		{
  			if(temp[j] == 'l')
  				l = 1;
  			else if(temp[j] == 'i')
  				i = 1;
  			else if(temp[j] == 'R')
  				R = 1;
  		
		  	else
		  	{
		  		perror("Unsupport flag");
		  		exit(1);
		  	}
		}
		index ++;
		temp = argv[index];
  	}
  	if(argc - index > 1 || R == 1)
  		printDir = 1;
  	
  	if(index >= argc)
  		printFromPath(".");
  	
  	else
  	{
	  	for(int i = index; i < argc; i ++)
	  	{
	  		temp = argv[i];
	  		printFromPath(temp);
	  	}

	}
	return 0;
}