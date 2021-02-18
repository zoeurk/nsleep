#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define TOOLONG(i, t, msg, prog)\
if(i >= t){\
fprintf(stderr, "\'%s\' is too long for me...\nShow usage \'%s -?\'\n", msg, prog);\
exit(EXIT_FAILURE);\
}

struct timespec rem = {0, 0};

void stop_usleep(int signum){
	unsigned long int n = 0, s = 0, M  = 0, m = 0, h = 0, d = 0, temp = 0;
	temp = rem.tv_sec;
	d = rem.tv_sec/(60*60*24);
	temp = temp%(60*60*24);
	h = temp/(60*60);
	temp = temp%(60*60);
	m = temp/60;
	s = temp%60;
	temp = rem.tv_nsec;
	M = temp/1000000;
	temp = temp%1000000;
	n = temp;
	printf("days:\t\t%lu\nhours:\t\t%lu\nminutes:\t%lu\nseconds:\t%lu\nmilliseconds:\t%lu\nnanoseconds:\t%lu\n", d, h, m, s, M, n);
	exit(EXIT_SUCCESS);
}
void check_temp(long int i, char *temp){
	if(i < 0){
		fprintf(stderr, "%li < 0 for %s\n",i , temp);
		exit(EXIT_FAILURE);
	}
}
void arguments(char **args, int i){
	if(*args[i+1] == '-' && (args[i+1][1] < 48 || args[i+1][1] > 57)){
		fprintf(stderr, "Missing arguments for %s\n", args[i]);
		exit(EXIT_FAILURE);
	}
}
void usage(char **args){
	printf("%s [OPTIONS]\n", *args);
	printf("Options:\n");
	printf("\t-d day         day (max: 106751)\n");
	printf("\t-H hours       hours (max: 23)\n");
	printf("\t-m minutes     minutes (max: 59)\n");
	printf("\t-s seconds     seconds (max: 59)\n");
	printf("\t-M millisconds milliseconds (max: 999)\n");
	printf("\t-n nanoseconds nanoseconds (max: 999999)\n");
	printf("\t-(h|?)         show this message\n");
	printf("Bug repport: zoeurk@gmail.com\n");
	exit(EXIT_SUCCESS);
}
void missing_arguments(char *args){
	fprintf(stderr,"Missing arguments for \'%s\'\n", args);
	exit(EXIT_FAILURE);
}
int args(int argc, char **argv, struct timespec *req){
	long int temp = 0;
	int i;
	char msg[24];
	memset(msg,0,24);
	for(i = 1; i < argc; i++){
		if(strlen(argv[i]) > 2){
			fprintf(stderr,"Global failure, see %s -? for the usage\n",argv[0]);
			exit(EXIT_FAILURE);
		}
		switch(argv[i][1])
		{
			case 'd':
				if(argc <= i+1)
					missing_arguments(argv[i]);
				arguments(argv, i);
				temp = atol(argv[i+1]);
				sprintf(msg, "%lu days", temp);
				TOOLONG(temp,106751, msg, argv[0]);
				check_temp(temp, "day");
				req->tv_sec += temp*60*60*24;
				i++;
				break;
			case 'H':
				if(argc <= i+1)
					missing_arguments(argv[i]);
				arguments(argv, i);
				temp = atol(argv[i+1]);
				sprintf(msg, "%lu hours", temp);
				TOOLONG(temp, 24, msg, argv[0]);
				check_temp(temp, "hour");
				req->tv_sec += temp*60*60;
				i++;
				break;
			case 'm':
				if(argc <= i+1)
					missing_arguments(argv[i]);
				arguments(argv, i);
				temp = atol(argv[i+1]);
				sprintf(msg, "%lu minutes", temp);
				TOOLONG(temp, 60, msg, argv[0]);
				check_temp(temp, "minutes");
				req->tv_sec += temp*60;
				i++;
				break;
			case 's':
				if(argc <= i+1)
					missing_arguments(argv[i]);
				arguments(argv, i);
				temp = atol(argv[i+1]);
				sprintf(msg, "%lu seconds", temp);
				TOOLONG(temp, 60, msg, argv[0]);
				check_temp(temp,"seconds");
				req->tv_sec += temp;
				i++;
				break;
			case 'M':
				if(argc <= i+1)
					missing_arguments(argv[i]);
				arguments(argv, i);
				temp = atol(argv[i+1]);
				check_temp(temp, "milliseconds");
				sprintf(msg, "%lu milliseconds", temp);
				TOOLONG(temp, 1000, msg, argv[0]);
				req->tv_nsec += temp*1000;
				i++;
				break;
			case 'n':
				if(argc <= i+1)
					missing_arguments(argv[i]);
				arguments(argv, i);
				temp = atol(argv[i+1]);
				check_temp(temp, "nanoseconds");
				sprintf(msg, "%lu nanoseconds", temp);
				TOOLONG(temp, 1000000, msg, argv[0]);
				req->tv_nsec += temp;
				i++;
				break;
			case 'h':
			case '?':
				usage(argv);
			default:
				if(*argv[i] == '-'){
					fprintf(stderr, "Unknow arguments: -%c\n", argv[i][1]);
					fprintf(stderr, "Try: %s -? for show usage\n", argv[0]);
					exit(EXIT_FAILURE);
				}else{
					usage(argv);
				}
		}
	}
	return 0;
}
int main(int argc, char **argv){
	struct timespec req = {0, 0};
	args(argc, argv, &req);
	signal(SIGINT, stop_usleep);
	signal(SIGQUIT, stop_usleep);
	signal(SIGTERM, stop_usleep);
	if(nanosleep(&req, &rem) < 0){
		perror("nanousleep()");
		exit(EXIT_FAILURE);
	}
	return 0;
}
