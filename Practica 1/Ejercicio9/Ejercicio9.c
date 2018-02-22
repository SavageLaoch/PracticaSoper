#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define NUM_PROC 4
#define MAX 256
int main (void)
{
  int pid,i,j;
  int a[2],b[2],d;
  float c;
  int status,n,m,k;
  int fd1[2], fd2[2], fd3[2], fd4[2], fd5[2], fd6[2], fd7[2], fd8[2], pipe_status;
  char string[MAX], buffer[MAX];
  char *c1, *c2;
  
  pipe_status=pipe(fd1);
  if(pipe_status==-1){
    printf("Error creando la tuberia 1");
    exit(EXIT_FAILURE);
  }
  pipe_status=pipe(fd2);
  if(pipe_status==-1){
    printf("Error creando la tuberia 2");
    exit(EXIT_FAILURE);
  }
  pipe_status=pipe(fd3);
  if(pipe_status==-1){
    printf("Error creando la tuberia 3");
    exit(EXIT_FAILURE);
  }
  pipe_status=pipe(fd4);
  if(pipe_status==-1){
    printf("Error creando la tuberia 4");
    exit(EXIT_FAILURE);
  }
  pipe_status=pipe(fd5);
  if(pipe_status==-1){
    printf("Error creando la tuberia 5");
    exit(EXIT_FAILURE);
  }
  pipe_status=pipe(fd6);
  if(pipe_status==-1){
    printf("Error creando la tuberia 5");
    exit(EXIT_FAILURE);
  }
  pipe_status=pipe(fd7);
  if(pipe_status==-1){
    printf("Error creando la tuberia 5");
    exit(EXIT_FAILURE);
  }
  pipe_status=pipe(fd8);
  if(pipe_status==-1){
    printf("Error creando la tuberia 5");
    exit(EXIT_FAILURE);
  }

  for (i=0 ; i < NUM_PROC; i++){
      if ((pid=fork()) <0 ){
          printf("Error al emplear fork\n");
          exit(EXIT_FAILURE);

      /*Hijos*/
      }else if (pid == 0){
          if (i==0){
            /*Lectura*/
            close(fd1[1]);
            read(fd1[0],buffer,sizeof(buffer));
            c1=strtok(buffer,",");
            c2=strtok(NULL,",");
            b[0]=atoi(c1);
            b[1]=atoi(c2);

            /*Operacion*/
            d=pow(b[0],b[1]);

            /*Escritura*/
            sprintf(string, "Datos enviados a traves de la tuberia por el proceso %d. Operando 1: %d. Operando 2: %d. Potencia de %d elevado a %d: %d",getpid(),b[0],b[1],b[0],b[1],d);
            close(fd2[0]);
            write(fd2[1],string,strlen(string));
            break;
          }else if (i==1){
            /*Lectura*/
            close(fd3[1]);
            read(fd3[0],buffer,sizeof(buffer));
            c1=strtok(buffer,",");
            c2=strtok(NULL,",");
            b[0]=atoi(c1);
            b[1]=atoi(c2);

            /*Operacion*/
            if ((b[1]==0)||(b[0]<1)){
              sprintf(string, "Datos enviados a traves de la tuberia por el proceso %d. Operando 1: %d. Operando 2: %d. El factorial de %d entre %d no se puede calcular",getpid(),b[0],b[1],b[0],b[1]);
              close(fd4[0]);
              write(fd4[1],string,strlen(string));
              break;
            }
            c=1;
            for (j=b[0]; j>1; j--){
              c=c*j;
            }
            c=c/b[1];

            /*Escritura*/
            sprintf(string, "Datos enviados a traves de la tuberia por el proceso %d. Operando 1: %d. Operando 2: %d. Factorial de %d entre %d: %f",getpid(),b[0],b[1],b[0],b[1],c);
            close(fd4[0]);
            write(fd4[1],string,strlen(string));
            break;
          }else if (i==2){
            /*Lectura*/
            close(fd5[1]);
            read(fd5[0],buffer,sizeof(buffer));
            c1=strtok(buffer,",");
            c2=strtok(NULL,",");
            b[0]=atoi(c1);
            b[1]=atoi(c2);

            /*Operacion*/
            if (b[0]<b[1]){
              sprintf(string, "Datos enviados a traves de la tuberia por el proceso %d. Operando 1: %d. Operando 2: %d. El numero combinatorio no se puede calcular",getpid(),b[0],b[1]);
              close(fd6[0]);
              write(fd6[1],string,strlen(string));
              break;
            }
            n=1;
            for (j=b[0]; j>1; j--){
              n=n*j;
            }
            k=1;
            for (j=b[1]; j>1; j--){
              k=k*j;
            }
            m=1;
            for (j=b[1]-b[0]; j>1; j--){
              m=m*j;
            }
            c=n/(k*m);

            /*Escritura*/
            sprintf(string, "Datos enviados a traves de la tuberia por el proceso %d. Operando 1: %d. Operando 2: %d. Potencia: %f",getpid(),b[0],b[1],c);
            close(fd6[0]);
            write(fd6[1],string,strlen(string));
            break;
          }else{
            /*Lectura*/
            close(fd7[1]);
            read(fd7[0],buffer,sizeof(buffer));
            c1=strtok(buffer,",");
            c2=strtok(NULL,",");
            b[0]=atoi(c1);
            b[1]=atoi(c2);

            /*Operacion*/
            if (b[0]<0){
              b[0]=-b[0];
            }
            if (b[1]<0){
              b[1]=-b[1];
            }
            d=b[0]+b[1];

            /*Escritura*/
            sprintf(string, "Datos enviados a traves de la tuberia por el proceso %d. Operando 1: %d. Operando 2: %d. Valor absoluto de %d mas valor absoluto de %d: %d",getpid(),b[0],b[1],b[0],b[1],d);
            close(fd8[0]);
            write(fd8[1],string,strlen(string));
            break;
          }
      /*Padre*/
      }else{
          if (i==0){
            /*Leo de teclado los valores*/  
            printf("El padre tiene el id %d\n",getpid());         
            printf("Introduzca los operandos: ");
            fscanf(stdin,"%d %d",&a[0],&a[1]);
            
            /*Meto en la cadena los valores*/
            sprintf(string, "%d,%d",a[0],a[1]);

            /*Paso los datos al hijo*/
            close(fd1[0]);
            write(fd1[1],string,strlen(string));
            
            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd2[1]);
            read(fd2[0],buffer,sizeof(buffer));
            printf("%s\n",buffer);
            
          }else if (i==1){
            /*NO HACE FALTA VOLVER A PEDIR LOS NUMEROS*/

            /*Paso los datos al hijo*/
            close(fd3[0]);
            write(fd3[1],string,strlen(string));

            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd4[1]);
            read(fd4[0],buffer,sizeof(buffer));
            printf("%s\n",buffer);

          }else if (i==2){
            /*NO HACE FALTA VOLVER A PEDIR LOS NUMEROS*/

            /*Paso los datos al hijo*/
            close(fd5[0]);
            write(fd5[1],string,strlen(string));

            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd6[1]);
            read(fd6[0],buffer,sizeof(buffer));
            printf("%s\n",buffer);

          }else{
            /*NO HACE FALTA VOLVER A PEDIR LOS NUMEROS*/

            /*Paso los datos al hijo*/
            close(fd7[0]);
            write(fd7[1],string,strlen(string));

            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd8[1]);
            read(fd8[0],buffer,sizeof(buffer));
            printf("%s\n",buffer);
          }

      }
  }
	
  exit(EXIT_SUCCESS);
}
