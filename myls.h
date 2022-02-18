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

static int l = 0;
static int i = 0;
static int R = 0;
static int symbolicLink = 0;
static int printDir = 0;
static char arrow[100] = "->";
static char nameBuf[512];

static void infoHelper(struct stat stats, char* name);
static void printFromPath(char* path);
static void getPermissions(char* string, struct stat fileStat);
static void getDate(char* string, struct stat fileStat);
static void printLHelper(char* path, char* name);
static void printRHelper(char* path);
static void printIHelper(char* path, char* name);
static void printHelper(char* path, char* name);

typedef struct
{
	char* ino;
	char* perm;
	char* numLinks;
	char* owner;
	char* group;
	char* size;
	char* date;
	char* filename;
	char* linkChar;
	char symLink[1024];
	int isSymLink;
}containsInfo;

static int ind = 0;
containsInfo info[5000];

static int inoSize = 1;
static int permSize = 1;
static int numLinksSize = 1;
static int ownerSize = 1;
static int groupSize = 1;
static int sizeSize = 1;
static int dateSize = 1;
static int filenameSize = 1;

static void printAll(int n)
{
	for(int j = 0; j < n; j ++)
	{
		if(i == 1 && l == 1)
			if(info[j].isSymLink == 1)
				printf("%*s %*s %*s %*s %*s %*s %*s %s %s %s\n", inoSize, info[j].ino, permSize, info[j].perm, numLinksSize, info[j].numLinks, ownerSize, info[j].owner, groupSize, info[j].group, sizeSize, info[j].size, dateSize, info[j].date, info[j].filename, arrow, info[j].symLink);
			else
				printf("%*s %*s %*s %*s %*s %*s %*s %s\n", inoSize, info[j].ino, permSize, info[j].perm, numLinksSize, info[j].numLinks, ownerSize, info[j].owner, groupSize, info[j].group, sizeSize, info[j].size, dateSize, info[j].date, info[j].filename);
		else if(i == 1 && l == 0)
			printf("%*s %*s\n", inoSize, info[j].ino, filenameSize, info[j].filename);
		else
		{
			if(info[j].isSymLink == 1)
				printf("%*s %*s %*s %*s %*s %*s %s %s %s\n", permSize, info[j].perm, numLinksSize, info[j].numLinks, ownerSize, info[j].owner, groupSize, info[j].group, sizeSize, info[j].size, dateSize, info[j].date, info[j].filename, arrow, info[j].symLink);
			else
				printf("%*s %*s %*s %*s %*s %*s %s\n", permSize, info[j].perm, numLinksSize, info[j].numLinks, ownerSize, info[j].owner, groupSize, info[j].group, sizeSize, info[j].size, dateSize, info[j].date, info[j].filename);
		}
	}

	for(int j = 0; j < n; j ++)
	{
		free(info[j].perm);
		free(info[j].numLinks);
		free(info[j].owner);
		free(info[j].size);
		free(info[j].group);
		free(info[j].date);
		free(info[j].filename);
		free(info[j].ino);
	}

	memset(info, 0, sizeof(info));
	ind = 0;
}

static void infoHelper(struct stat stats, char* name)
{

	if(S_ISLNK(stats.st_mode))
	{
		char sym[1024];
		info[ind].isSymLink = 1;
		symbolicLink = 1;
		ssize_t len;

		if((len = readlink(nameBuf, sym, sizeof(sym) - 1)) != -1)
			sym[len] = '\0';

		strncpy(info[ind].symLink, sym, sizeof(sym));

	}
	char* inoString = (char*)malloc(sizeof(char*));
	if(i == 1)
	{
		sprintf(inoString, "%lu", stats.st_ino);
		info[ind].ino = inoString;

		if(strlen(inoString) > inoSize)
			inoSize = strlen(inoString);
		
	}


	char* permissionString = (char*)malloc(sizeof(char*) * 5000);
	getPermissions(permissionString, stats);
	char* dateString = (char*)malloc(sizeof(char*) * 5000);
	getDate(dateString, stats);
	char* linkChar = (char*)malloc(sizeof(char) * 5000);
	char* ownerChar = (char*)malloc(sizeof(char) * 5000);
	char* groupChar = (char*)malloc(sizeof(char) * 5000);
	char* sizeChar = (char*)malloc(sizeof(char) * 5000);
	char* nameChar = (char*)malloc(sizeof(char) * 5000);

	if(strlen(permissionString) > permSize)
		permSize = strlen(permissionString);

	info[ind].perm = permissionString;
	info[ind].date = dateString;

	if(strlen(dateString) > dateSize)
		dateSize = strlen(dateString);


	sprintf(linkChar, "%lu", stats.st_nlink);
	info[ind].numLinks = linkChar;
	if(strlen(linkChar) > numLinksSize)
		numLinksSize = strlen(linkChar);

	sprintf(ownerChar, "%s", getpwuid(stats.st_uid)->pw_name);
	info[ind].owner = ownerChar;
	if(strlen(ownerChar) > ownerSize)
		ownerSize = strlen(ownerChar);
		
	sprintf(groupChar, "%s", getgrgid(stats.st_gid)->gr_name);
	info[ind].group = groupChar;
	if(strlen(groupChar) > groupSize)
		groupSize = strlen(groupChar);

	sprintf(sizeChar, "%lu", stats.st_size);
	info[ind].size = sizeChar;
	if(strlen(sizeChar) > sizeSize)
		sizeSize = strlen(sizeChar);

	sprintf(nameChar, "%s", name);
	info[ind].filename = nameChar;
	if(strlen(nameChar) > filenameSize)
		filenameSize = strlen(nameChar);
	
	ind ++;

	if(symbolicLink == 1)
		symbolicLink = 0;

}

static void printFromPath(char* path)
{
	char buff[5000];
	memset(buff, 0, sizeof(buff));
	strncpy(buff, path, strlen(path));

	struct stat pathStat;
	stat(path, &pathStat);

	/* if path is a file */
	if(S_ISREG(pathStat.st_mode))
	{
		infoHelper(pathStat, path);
		goto l1;
	}
	struct dirent** namelist;

	int n;
	n = scandir(path, &namelist, NULL, alphasort);
	if(n == -1)
		exit(1);

	int index = 0;
	if(printDir == 1)
		printf("%s: \n", path);

	while(index < n)
	{
		if(namelist[index]->d_name[0] != '.')
		{
			printHelper(path, namelist[index]->d_name);
		}
		index ++;
        free(namelist[index - 1]);
	}
	free(namelist);

	l1:;
	if(index == n && l == 1 || i == 1)
	{
		printAll(ind);
	}

	if(R == 1)
		printRHelper(path);

}

static void getPermissions(char* string, struct stat fileStat)
{
	memset(string, 0, 12);
	if(symbolicLink == 1)
		string[0] = 'l';
	else
		string[0] = (S_ISDIR(fileStat.st_mode) ? 'd' : '-');

	string[1] = (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
	string[2] = (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
	string[3] = (fileStat.st_mode & S_IXUSR) ? 'x' : '-';
	string[4] = (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
	string[5] = (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
	string[6] = (fileStat.st_mode & S_IXGRP) ? 'x' : '-';
	string[7] = (fileStat.st_mode & S_IROTH) ? 'r' : '-';
	string[8] = (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
	string[9] = (fileStat.st_mode & S_IXOTH) ? 'x' : '-';

}

static void getDate(char* string, struct stat fileStat)
{
	memset(string, 0, 30);
	strftime(string, 30, "%b %e %H:%M", localtime(&fileStat.st_mtime));
}

void printLHelper(char* path, char* name)
{
	struct stat stats;
	memset(&stats, 0, sizeof(struct stat));

	memset(nameBuf, 0, 512);
	strncpy(nameBuf, path, strlen(path));
	if(nameBuf[strlen(nameBuf)] != '/')
		nameBuf[strlen(nameBuf)] = '/';

	strncpy(nameBuf + strlen(nameBuf), name, strlen(name));

	if(lstat(nameBuf, &stats) == -1)
	{
		perror("");
		exit(1);
	}
	infoHelper(stats, name);
}

static void printRHelper(char* path)
{
	DIR* dir;
	struct dirent *dp;
    printf("\n");

	char buff[5000];
    memset(buff, 0, sizeof buff);
	strncpy(buff, path, strlen(path));

    if (buff[strlen(buff)] != '/' && buff[strlen(buff) - 1] != '/' ){
	    buff[strlen(buff)] = '/';
    }

    char buff2[5000];


	if((dir = opendir(path)) != NULL)
	{
		while((dp = readdir(dir)) != NULL)
		{
            memset(buff2, 0, sizeof buff2);
            strncpy(buff2, buff, strlen(buff));
			if(dp->d_type == DT_DIR && strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
				strncpy(buff2 + strlen(buff2), dp->d_name, strlen(dp->d_name));
				printFromPath(buff2);
			}
		}
		closedir(dir);
	}


}

static void printIHelper(char* path, char* name)
{
	struct stat fileStat;
	char buff[512];
	memset(buff, 0, 512);
	strncpy(buff, path, strlen(path));
	if(buff[strlen(buff)] != '/')
		buff[strlen(buff)] = '/';

	strncpy(buff + strlen(buff), name, strlen(name));

	if(stat(buff, &fileStat) == -1)
	{
		perror("");
		exit(1);
	}
	char* ino = (char*)malloc(sizeof(char*)*5000);
	sprintf(ino, "%ld", fileStat.st_ino);
	char* nameC = (char*)malloc(sizeof(char*)*5000);
	sprintf(nameC, "%s", name);

	info[ind].ino = ino;
	info[ind].filename = nameC;
	if(strlen(ino) > inoSize)
		inoSize = strlen(ino);
	ind ++;
}

static void printHelper(char* path, char* name){
	if(l == 1)
		printLHelper(path, name);
	else if(i == 1)
		printIHelper(path, name);
	else
		printf("%s\n", name);

}