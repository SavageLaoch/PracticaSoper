#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>

#define NUM_PROC 4
int main (void)
{
  int pid,i,j;
  float a[2],b[2],c,d;
  int status;
  int fd1[2], fd2[2], fd3[2], fd4[2], fd5[2], fd6[2], fd7[2], fd8[2], pipe_status;

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
      }else if (pid == 0){
          if (i==0){
            /*Lectura*/
            printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
            close(fd1[1]);
            read(fd1[0],b,2*sizeof(float));
            printf("He leido %f %f \n",b[0],b[1]);

            /*Operacion*/
            c=pow(b[0],b[1]);

            /*Escritura*/
            close(fd2[0]);
            write(fd2[1],&c,sizeof(float));
            break;
          }else if (i==1){
            /*Lectura*/
            printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
            close(fd3[1]);
            read(fd3[0],b,2*sizeof(float));
            printf("He leido %f %f \n",b[0],b[1]);

            /*Operacion*/
            if ((b[1]==0)||(b[0]/b[1]<1)){
              c=0;
              close(fd4[0]);
              write(fd4[1],&c,sizeof(float));
              break;
            }
            j=b[0]/b[1];
            c=1;
            while (j!=1){
              c=c*j;
              j--;
            }

            /*Escritura*/
            close(fd4[0]);
            write(fd4[1],&c,sizeof(float));
            break;
          }else if (i==2){
            /*Lectura*/
            printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
            close(fd5[1]);
            read(fd5[0],b,2*sizeof(float));
            printf("He leido %f %f \n",b[0],b[1]);

            /*Operacion*/
            if ((b[1]==0)||(b[0]/b[1]<1)){
              c=0;
              close(fd4[0]);
              write(fd4[1],&c,sizeof(float));
              break;
            }
            j=b[0]/b[1];
            c=1;
            while (j!=1){
              c=c*j;
              j--;
            }

            /*Escritura*/
            close(fd6[0]);
            write(fd6[1],&c,sizeof(float));
            break;
          }else{
            /*Lectura*/
            printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
            close(fd7[1]);
            read(fd7[0],b,2*sizeof(float));
            printf("He leido %f %f \n",b[0],b[1]);

            /*Operacion*/
            if (b[0]<0){
              b[0]=-b[0];
            }
            if (b[1]<0){
              b[1]=-b[1];
            }
            c=b[0]+b[1];

            /*Escritura*/
            close(fd8[0]);
            write(fd8[1],&c,sizeof(float));
            break;
          }
      }else{
          if (i==0){
            /*Leo de teclado los valores*/
            printf("PADRE %d \n",getpid());
            printf("Introduzca los operandos \n");
            fscanf(stdin,"%f %f",&a[0],&a[1]);
            printf("%f %f \n",a[0],a[1]);

            /*Paso los datos al hijo*/
            close(fd1[0]);
            write(fd1[1],a,2*sizeof(float));
            
            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd2[1]);
            read(fd2[0],&d,sizeof(float));
            printf("La potencia de %f elevado a %f queda %f \n",a[0],a[1],d);
            
          }else if (i==1){
            /*NO HACE FALTA VOLVER A PEDIR LOS NUMEROS*/

            /*Paso los datos al hijo*/
            close(fd3[0]);
            write(fd3[1],a,2*sizeof(float));

            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd4[1]);
            read(fd4[0],&d,sizeof(float));
            printf("El factorial de %f entre %f queda %f \n",a[0],a[1],d);
          }else if (i==2){
            /*NO HACE FALTA VOLVER A PEDIR LOS NUMEROS*/

            /*Paso los datos al hijo*/
            close(fd5[0]);
            write(fd5[1],a,2*sizeof(float));

            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd6[1]);
            read(fd6[0],&d,sizeof(float));
            printf("El factorial de %f entre %f queda %f \n",a[0],a[1],d);
          }else{
            /*NO HACE FALTA VOLVER A PEDIR LOS NUMEROS*/

            /*Paso los datos al hijo*/
            close(fd7[0]);
            write(fd7[1],a,2*sizeof(float));

            /*Espero a que muera el hijo*/
            waitpid(pid,&status,0);

            /*Leo del hijo los valores e imprimo*/
            close(fd8[1]);
            read(fd8[0],&d,sizeof(float));
            printf("El valor absoluto de %f mas el de %f queda %f \n",a[0],a[1],d);
          }

      }
  }
	printf("Terminado: %d\n",getpid());
  exit(EXIT_SUCCESS);
}
