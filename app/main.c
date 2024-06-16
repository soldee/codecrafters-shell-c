#include <stdio.h>

void remove_trailing_newline(char* str) {
	int i = 0;
	while (str[i] != '\n' && str[i] != '\0') {
		i++;
	}
	if (str[i] == '\n') {
		str[i] = '\0';
	}
}


int main() {
	printf("$ ");
	fflush(stdout);

	// Wait for user input
	char input[100];
	fgets(input, 100, stdin);

	remove_trailing_newline(input);

	printf("%s: command not found\n", input);
	fflush(stdout);

	return 0;
}