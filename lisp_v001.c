#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char *readline(char *prompt)
{
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char *cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy) - 1] = '\0';
	return cpy;
}

void add_history(char *unused) {}

#else
#include <editline.h>
#endif

int main()
{
	puts("Lispy v001");
	puts("Press Ctrl+C to exit\n");

	while (1)
	{
		char *input = readline("lispy> ");
		add_history(input);
		printf("You put in %s\n", input);
	}
	return 0;
}