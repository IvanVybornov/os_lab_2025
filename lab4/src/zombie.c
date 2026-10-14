#include <stdio.h>
#include <unistd.h>

int main(){
	printf("I'm parent!\n");

	pid_t child_pid = fork();

	if (child_pid == 0){
		printf("I'm child! In the next line my execution will be finished.\n");
		return 0;
	}

	printf("Parent here: In the next line my execution will be finished.\n");
	return 0;
}