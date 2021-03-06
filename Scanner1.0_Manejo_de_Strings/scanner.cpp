//el analizador lexicogr�fico de pl0
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "pl0.h"
#include "lexico.h"
#include "auxiliares.h"

char linea[MAXLINEA];     //buffer de l�neas 
int ll;                   //contador de caracteres
int offset;               //corrimiento en la lectura de los caracteres del programa fuente
int fin_de_archivo;       //bandera de fin de archivo (obtch)   
int ch;                   //�ltimo caracter le�do
char lex[MAXID+1];        //�ltimo lexeme le�do ( +1 para colocar "\0")
long int valor ;          //valor num�rico de una lexeme correspondiene a un n�mero

int obtch(),getline(char s[],int lim); //funciones internas a scanner.cpp
char charTemp[2];
int k = 0;

//tabla que guarda las cadena y sus identificadores de variables en cada fila.
char palabras[MAXSTRING][MAXSTRING];
char palabrasVar[MAXSTRING][MAXSTRING];
char strTemp[MAXSTRING];

//obtoken: obtiene el siguiente token del programa fuente                   
void obtoken()
{
 char lexid[MAXID+1]; //+1 para colocar el marcador "\0"
 int i,j;
 int ok=0;

 //quitar blancos, caracter de cambio de l�nea y tabuladores
 while (ch==' ' || ch=='\n' || ch=='\t') ch=obtch() ;
	
	//extern char palabras
	//extern char palabrasVar
	

	//Si comienza con comillas dobles (34 en ascii), es String hasta donde terminen las comillas
	if(ch==34){
		lexid[0] = ch;
		i = 1;
		do{
			ch = obtch();
			if(ch=='%'){
				charTemp[0]=ch;
				ch = obtch();
				if(ch=='d' || ch == 'f' || ch=='s' || ch =='c'){
					charTemp[1] = ch;
					lexid[i] = charTemp[0];
					i++;
					lexid[i] = charTemp[1];
					i++;
					strcat(strTemp,charTemp);
					strcat(strTemp,",");
				}				
				else{
					lexid[i] = ch;
					i++;	
				}
			}else{
				lexid[i] = ch;
				i++;
			}
		}while(ch != 34);
		ch = obtch();
		lexid[i-1]='\0';
		printf("\n%s",lexid);
		token = tok_cadena;

		strTemp[strlen(strTemp)-1] = '\0';
		strcpy(palabrasVar[k],strTemp);
		strcpy(palabras[k],lexid);
		k++;
		strcpy(strTemp,"");
	}
	//Si comienza con comillas simples (39 en ascii), es String hasta donde terminen las comillas
	else if(ch==39){
		lexid[0] = ch;
		i = 1;
		do{
			ch = obtch();
			if(ch=='%'){
				charTemp[0]=ch;
				ch = obtch();
				if(ch=='d' || ch == 'f' || ch=='s' || ch =='c'){
					charTemp[1] = ch;
					lexid[i] = charTemp[0];
					i++;
					lexid[i] = charTemp[1];
					i++;
					strcat(strTemp,charTemp);
					strcat(strTemp,",");
				}				
				else{
					lexid[i] = ch;
					i++;	
				}
			}else{
				lexid[i] = ch;
				i++;
			}
		}while(ch != 39);
		ch = obtch();
		lexid[i-1]='\0';
		printf("\n%s",lexid);
		token = tok_cadena;

		strTemp[strlen(strTemp)-1] = '\0';
		strcpy(palabrasVar[k],strTemp);
		strcpy(palabras[k],lexid);
		k++;
		strcpy(strTemp,"");
	}
	//Si comienza con # (35 en ascii), es un comentario.
	else if(ch==35){
		lexid[0] = ch;
		i = 1;
		do{
			ch = obtch();
			i++;
		}while(ch != '\n');
		ch = obtch();
		lexid[i-1]='\0';
		int l = 0;
		do{
			printf("\b");
			l++;
		}while(l<i+7);
	}
	
else
 //si la lexeme comienza con una letra, es identificador o palabra reservada
 if (isalpha(ch)) {
    lexid[0]=ch;
    i=1;
    while ( isalpha( (ch=obtch()) ) ||  isdigit(ch) || ch=='.'   ) 
      if (i<MAXID) lexid[i++]=ch;
    lexid[i]='\0';
  
    //�es identificador o palabra reservada?.buscar en la tabla de palabras reservadas
	//una b�squeda lineal que tendr� que ser sustitu�da por otro tipo de b�squeda m�s efectiva. 
	//...en esa nueva b�squeda desaparecer� el "break"
    for (j=0;j<MAXPAL;++j) 
        if (strcmp(lexid,lexpal[j])==0) {
	       ok=1;
	       break;
        }

	
    if (ok==1) 
    
       token=tokpal[j]; //es palabra reservada
    else
       token=tok_id; //es identificador
 	
 	printf("\n%s",lexid);
 	
    strcpy(lex,lexid); //copiar en lex
 }
 else //si comienza con un d�gito...
 		
    if (isdigit(ch)) {
       int es_flotante=0; // ****** cambia a 1 si encuentra un .
       lexid[0]=ch;
       i=j=1;
       while ( isdigit( (ch=obtch())) || (es_flotante == 0 and ch=='.')) {
	         if(ch=='.') es_flotante= 1;
	         if (i<MAXDIGIT) lexid[i++]=ch;
	         j++;
       }
       lexid[i]='\0';
       if (j>MAXDIGIT)
          error(30); //este n�mero es demasiado grande
       
       if(es_flotante==1) token=tok_flotante;
       else token=tok_numero;
       valor=atol(lexid); //valor num�rico de una lexeme correspondiene a un n�mero	  
       printf("\n%s",lexid);      
    }
    else //reconocimiento de s�mbolos especiales, incluyendo pares de simbolos (aplicamos "lookahead symbol technique")
       if (ch=='<') {
          ch=obtch();
          if (ch=='=') {
          	 printf("\n<=");
             token=tok_menorigual;
             ch=obtch();
          }
          else{
          	printf("\n<");
            token=tok_menor;
          }
             /*if (ch=='>') {
             	printf("\n<>");
                token=tok_null;
                ch=obtch();
             }
             else{
             	printf("\n<");
                token=tok_menor;
             }*/
       }
       else
          if (ch=='>') {
             ch=obtch();
             if (ch=='=') {
             	printf("\n>=");
                token=tok_mayorigual;
                ch=obtch();
             }
             else{ 
             	printf("\n>");
                token=tok_mayor;
             }
          }
          else 
             if (ch==':') {
                ch=obtch();
                if (ch=='=') {
                   printf("\n:=");
	               token=tok_asignar;
	               ch=obtch();
                }
               else{
               	   printf("\n%c",ch);
	               token=tok_null;
	           }
             } else{
				if(ch=='+'){
					ch=obtch();
					if(ch=='='){
						printf("\n+=");
						token=tok_sumasign;
						ch=obtch();
					}else{
						printf("\n%c",ch);
						token=tok_sum;
					}
				}else{
					if(ch=='='){
						ch=obtch();
						if(ch=='='){
							printf("\n==");
							token=tok_igual;
							ch=obtch();
						}else{
							printf("\n%c",ch);
							token=tok_null;
						}
					
					}else{
						if(ch=='!'){
							ch=obtch();
							if(ch=='='){
								printf("\n!=");
								token=tok_negacion;
								ch=obtch();
							}else{
								printf("\n%c",ch);
								token=tok_null;
							}
						}else{
							if(ch=='&'){
								ch=obtch();
								if(ch=='&'){
									printf("\n&&");
									token=tok_and;
									ch=obtch();
								}else{
									printf("\n%c",ch);
									token=tok_null;
								}
							}else{
								if(ch=='|'){
									ch=obtch();
									if(ch=='|'){
										printf("\n||");
										token=tok_or;
										ch=obtch();
									}else{
										printf("\n%c",ch);
										token=tok_null;
									}
								}else{
									printf("\n%c",ch);
									token=espec[ch]; //hashing directo en la tabla de tokens de s�mbolos especiales del lenguaje
									ch=obtch();
								}
							}
						}
					}
				}             
             }
            /* else {
             	printf("\n%c",ch);
                token=espec[ch]; //hashing directo en la tabla de tokens de s�mbolos especiales del lenguaje
                ch=obtch();
             }*/
}

//obtch: obtiene el siguiente caracter del programa fuente
int obtch()
{ 

 if (fin_de_archivo==1) {
	fclose(fp);//cerrar el programa fuente
    printf("\nAnalisis lexicografico finalizado.");
    exit(1); //salir...
 }
  
 if (offset==ll-1) {
    ll=getline(linea,MAXLINEA); //trae una l�nea del programa fuente al buffer de l�neas
    if (ll==0) 	   
       fin_de_archivo=1; //se retrasa en un blanco la deteccion de EOF, porque obtoken lleva un caracter adelantado. si no, en 
						 //algunos casos tendr�amos problemas, por ejemplo: no se reconoceria el punto final del programa (...end.)

    //printf("\n%s",linea);
    offset=-1;
 }

 ++offset;

 if ( (linea[offset]=='\0') || (fin_de_archivo==1) )   
    return(' '); 
 else  
   // return(toupper(linea[offset])); //de esto depende si el lenguaje es sensitivo de may�sculas o no.
   return(linea[offset]);

}

//getline: lee la siguiente l�nea del fuente y regresa su tama�o (incluyendo '\n') o 0 si EOF. (por ejemplo: para VAR regresa 4)
//es probablemente la rutina de m�s bajo nivel del compilador
//tomada de "El Lenguaje de programaci�n C" - Kernighan & Ritchie - pag 28                        
int getline(char s[],int lim)
{
 int c,i;

 for (i=0;i<lim-1 && (c=getc(fp))!=EOF && c!='\n';++i)
     s[i]=c;

 if (c=='\n') {
    s[i]=c;
    ++i;
 }

 s[i]='\0';
 return (i);
}


