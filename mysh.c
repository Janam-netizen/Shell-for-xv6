

#include "kernel/types.h"

#include "user/user.h"

#include "kernel/fcntl.h"
int alnum(char ch)
{

   if (ch == 65 || ch == 90 || ch == 97 || ch == 122 || (ch > 65 && ch < 90) || (ch > 97 && ch < 122))
   {

      return 0;
   }
   else
   {

      return -1;
   }
}
//NOTE: THE BELOW FUNCTION WAS COPIED FROM FILE user/sh.c which is part of the xv6 operating system.
int getcmd(char *buf, int nbuf)
{
   fprintf(2, ">>>");
   memset(buf, 0, nbuf);
   gets(buf, nbuf);
   if (buf[0] == 0) // EOF
      return -1;
   return 0;
}

//int argument_number = 1;

char **parse_buffer(char buf[100], int *argument_number)
{
   //int arg_index = 0;

   //char command[10];

   char **args = malloc(10 * sizeof(char *));
   for (int i = 0; i < 10; i++)
   {
      args[i] = malloc(100 * sizeof(char));
   }

   int j = 0;
   *argument_number = 0;

   for (int i = 0; i < strlen(buf); i++)
   {
      if (i == 0 && buf[i] == ' ')
      {

         continue;
      }

      else if (i > 0 && buf[i] == ' ' && buf[i - 1] == ' ')
      {

         continue;
      }

      else if (i > 0 && alnum(buf[i]) == 0 && buf[i - 1] == ' ')
      {
         args[*argument_number][j] = 0;

         *argument_number += 1;

         j = 0;

         args[*argument_number][j] = buf[i];

         j++;
      }
      else if (i > 0 && buf[i] == ' ' && alnum(buf[i - 1]) == 0)
      {

         continue;
      }

      else if (buf[i] == '\n')
      {

         break;
      }

      else
      {
         args[*argument_number][j] = buf[i];

         j++;
      }

      //return args;
   }

   args[(*argument_number) + 1] = 0;

   return args;
}

char symbols[] = "<|>";

char get_symbol(char buf[100], char *l_c, char *r_c)
{

   memset(l_c, 0, 100);

   memset(r_c, 0, 100);

   char symbol = 0;

   for (int i = 0; i < strlen(buf); i++)
   {

      if (i > 0 && strchr(symbols, buf[i]))
      {

         symbol = buf[i];

         int k = 0;

         for (int j = i + 2; j < strlen(buf); j++)
         {

            r_c[k] = buf[j];

            k += 1;
         }

         break;
      }
      l_c[i] = buf[i];
   }

   return symbol;
}

void execute(char **args, int fd)
{

   close(0);

   dup(fd);

   exec(args[0], args);
}

void handle_pipes(char buf[100])
{

   //dup(fd_in);

   char lc[100];

   char rc[100];

   char **argsv_l;

   int argc_l;

   char symbol = get_symbol(buf, lc, rc);

   if (symbol == '|')
   {
      int p[2];

      pipe(p);
      if (fork() == 0)
      {
         close(1);
         dup(p[1]);
         close(p[0]);
         close(p[1]);
         handle_pipes(lc);
      }
      if (fork() == 0)
      {
         close(0);
         dup(p[0]);
         close(p[0]);
         close(p[1]);
         handle_pipes(rc);
      }
      close(p[0]);
      close(p[1]);
      wait((int *)0);
      wait((int *)0);
   }

   else if (symbol == '>' || symbol == '<')
   {

      if (fork() == 0)
      {

         rc[strlen(rc) - 1] = 0;

         if (symbol == '>')
         {
            //fprintf(1, "Performing o/p redirection\n");

            open(rc, O_CREATE);

            close(1);
            open(rc, O_RDWR);

            handle_pipes(lc);
         }

         else if (symbol == '<')
         {
            //fprintf(1, "Performing i/p redirection\n");

            close(0);

            fprintf(1, "%d\n", open(rc, O_RDONLY));

            handle_pipes(lc);
         }
      }

      else
      {
         wait((int *)0);
      }
   }

   else
   {

      argsv_l = parse_buffer(buf, &argc_l);


   
         
      exec(argsv_l[0], argsv_l);

      

      /*argc_l = 0;

      argsv_l = parse_buffer(buf, &argc_l);

      //args = parse_buffer(buff);

      //char **args = parse_buffer(buf);
      if (strcmp(argsv_l[0], "cd") == 0)
      { // strip out the new line

         fprintf(1, "Executing cd\n");
         fprintf(1, "%s", argsv_l[1]);
         fprintf(1, "%d", strlen(argsv_l[1]));
         char *dir = argsv_l[1];

         //dir[strlen(dir) - 1] = 0;
         fprintf(1, "The len of dir is %d \n", strlen(dir));
         fprintf(1, "%d", chdir(dir));
         fprintf(1, "____________");
         //break;
      }

      else if (strcmp(argsv_l[0], "exit") == 0)
      {

         exit(1);
      }

      else
      {
         int pid = fork();

         if (pid == 0)
         {

            //fprintf(1, "Number of arguements is %d\n", argument_number);
            //args[argument_number + 1] = '\0';
            fprintf(1, "%d\n", exec(argsv_l[0], argsv_l));

            exit(0);
         }
         else
         {

            wait((int *)0);
         }
      }*/
   }
}

int main()
{
   char buf[100];

   while (getcmd(buf, sizeof(buf)) >= 0)

   {
      char **args;

      int argc_l;

      args=parse_buffer(buf,&argc_l);


      if(strcmp(args[0],"cd")==0){


           fprintf(1, "%d", chdir(args[1]));

      }
      
      
      else if (fork() == 0)
      {

         handle_pipes(buf);
      }


      else{

         wait((int*)0);

      }
   }

   //int p[2];

   //fprintf(1,"the file descriptors are %d,%d", p[0],p[1]);

   //close(2);

   //close(p[0]);
   //dup(1);

   /*int p[2];

   pipe(p);

   if (fork() == 0)
   {

      close(1);

      dup(p[1]);

      exec(argv_one[0], argv_one);
   }

   else
   {

      close(0);
      dup(0);
      close(p[1]);

      execute(argv_two, p[0]);
   }*/

   //char *argv_one[]={"ls",NULL};

   //char *argv_two[]={"wc",NULL};

   //char *argv_three={"cat",NULL};

   //fprintf(1,"%d,%d\n",p[0],p[1]);

   exit(1);
}