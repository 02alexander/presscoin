#define __USE_XOPEN
#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define RESET_TIME 20 /* Time the coin resets. */

/* Writes the current time to file. */
void update_file(char *file);

/* Returns seconds to wait from ptm untill it is tomorrow. */
time_t time_until_tomorrow(struct tm *ptm);

void wait_reset_time(char *file);

/* Gets the time stored in file. */
struct tm *read_time(char *file);

struct tm *get_current_time();

void print_time(struct tm *t);

int run();

int main(int argc, char const *argv[])
{
	char path[strlen(argv[0])];
	strcpy(path, argv[0]);
	int i;
	for (i = strlen(path); i > 0 && path[i] != '/'; --i)
		;

	if (i != 0) {
		path[i+1] = '\0';
		printf("%s\n", path);
		if (chdir(path) == -1) {
			fprintf(stderr, "main() chdir error: %s\n", strerror(errno));
			return 1;
		}
		char *wd = getcwd(NULL, 0);
		printf("%s\n", wd);
		free(wd);
	}

	while (1) {
		wait_reset_time("lastrun"); // lastrun contains the last time run() was run.
		run();
		update_file("lastrun");	
	}
	return 0;
}

int run()
{
	printf("executing run");
	return system("./run");
}

void wait_reset_time(char *file)
{
	// NOTE TO SELF:
	// Don't touch this, it works.
	time_t tmp = mktime(read_time(file)) - (RESET_TIME * 3600);
	struct tm lsttm = *localtime(&tmp);
	tmp = mktime(get_current_time()) - (RESET_TIME * 3600);
	struct tm curtm = *localtime(&tmp);

	printf("lsttm ");
	print_time(&lsttm);

	printf("curtm ");
	print_time(&curtm);

	if (lsttm.tm_mday == curtm.tm_mday)
		sleep(time_until_tomorrow(&curtm));
	else if (lsttm.tm_mday < curtm.tm_mday)
		return;
}

time_t time_until_tomorrow(struct tm *ptm)
{
	struct tm tmrw = *ptm;
	tmrw.tm_hour = 0;
	tmrw.tm_min = 0;
	tmrw.tm_sec = 0;
	time_t t = mktime(&tmrw) + (3600 * 24) - (long)mktime(ptm);
	return (t <= 0) ? 0 : t;
}

void update_file(char *file)
{
	FILE *fp = fopen(file, "w");

	if (fp == NULL) 
		fprintf(stderr, "update_file() fopen error: %s\n", strerror(errno));

	char buf[256];
	strftime(buf, 256, "%c", get_current_time());
	fprintf(fp, "%s\n", buf);
	fclose(fp);
}

struct tm *read_time(char *file)
{
	FILE *fp = fopen(file, "r");
	if (fp == NULL) 
		fprintf(stderr, "read_time() fopen error: %s\n", strerror(errno));
	static struct tm t;
	char *line = NULL;
	size_t len = 0;

	fseek(fp, 0L, SEEK_SET);
	if (getline(&line, &len, fp) == -1)
		fprintf(stderr, "read_time() getline error: %s\n", strerror(errno));

	if (len <= 0) {
		printf("couldn't read file\n");
		t = *get_current_time();
		update_file(file);
	} else {
		strptime(line, "%c", &t);
		printf("%s", line);
	}

	free(line);

	return &t;
}

struct tm *get_current_time()
{
	time_t nixt = time(NULL);
	return localtime(&nixt);
}

void print_time(struct tm *t)
{
	printf("sec=%d, min=%d, hour=%d, mday=%dn\n", t->tm_sec, t->tm_min, t->tm_hour, t->tm_mday);
}