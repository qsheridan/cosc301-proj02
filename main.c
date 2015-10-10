#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>



char *string_list[] = {
  "cd",
  "help",
  "exit"
};

int cd(char **args);
int help(char **args);
int exiting(char **args);


int (*string_func[]) (char **) = {
  &cd,
  &help,
  &exiting
};


int string_list_size() {
  return (sizeof(string_list) / sizeof(char *));
}




int help(char **args)
{
  printf("This is the help command\n"); 
  printf("These are the following commands you can use\n"); 

  for (int i = 0; i < string_list_size(); i++) {
    printf("  %s\n", string_list[i]);
  }

  return 1;
}


int exiting(char **args)
{
  return 0;
}

int launch_shell(char **args)
{
  pid_t p;
  int status;

  p = fork();
  if (p == 0) {
    if (execv(args[0], args) == -1) {
      perror("error");
    }
    exit(EXIT_FAILURE);
  } else if (p < 0) {
    perror("error");
  } else {
    do {
      waitpid(p, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


int execute(char **arguments)
{

  if (arguments[0] == NULL) {
    return 1;
  }

  int length = sizeof(arguments);
  printf("%d\n", length); 
  for(int x = 0; x < length-1; x++) {
	printf("%s\n", arguments[x]);
	
  }

 //for(int j = 0; j < (length-1); j++){
     for (int i = 0; i < string_list_size(); i++) {
       if (strcmp(arguments[0], string_list[i]) == 0) {
         return (*string_func[i])(arguments);
       }
	else {
	 return launch_shell(arguments);
	}
     }
  // } 

  //return lsh_launch(args);
}

#define buffersize 1024

char *reading_line(void)
{
  int buf = buffersize;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buf);
  int c;

  if (!buffer) {
    fprintf("error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {

    c = getchar();


    if (c == EOF || c == '\n' || c == '#') {
      buffer[position] = '\0';
      return buffer;
    }
    else {
      buffer[position] = c;
    }
    position++;


    if (position >= buf) {
      buf += buffersize;
      buffer = realloc(buffer, buf);
    }
  }
}



char **tokenify(char *s) {
   char *newS = strdup(s);
   int count = 0; 
   const char *whitespace = " ;\t\n";
   char *token;
   for(token = strtok(newS, whitespace); token!=NULL; token=strtok(NULL,whitespace)) {
     count++;			
   }  
	free(newS);
	newS = strdup(s);
	//printf("%d\n", count); 
	char **arr = malloc(sizeof(char*) * (count + 1)); 
	//printf("%d\n", sizeof(arr)); 
	int i = 0;
	for(token = strtok(newS, whitespace); token!=NULL; token=strtok(NULL,whitespace))
	{	
		arr[i] = strdup(token);
		//printf("%s\n", token); 
		i++;
	}
	free(newS);
	
	arr[i] = NULL;
       	/*for (int i = 0; i < (sizeof(arr)-1); i++) {
		printf("%s\n", arr[i]); 
	}  */ 
	return arr;	

}

void loop(void)
{
  char *line;
  char **args;
  int isdone;

  do {
    printf("> ");
    line = reading_line();
    args = tokenify(line);
    isdone = execute(args);

    free(line);
    free(args);
  } while (isdone);
}


int main(int argc, char **argv)
{
  loop();

  return 0;
}
