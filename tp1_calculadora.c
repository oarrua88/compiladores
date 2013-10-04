/****************************************************************************************
COMPILADORES
TRABAJO NRO 1

TEMA: 
* Implementar una calculadora de expresiones aritméticas, con manejo de números reales, enteros o en notación científica.

OBJETIVOS:
          
* Implementar un Analizador Léxico que reconozca números, comentarios y operadores.
* Implementar un evaluador de expresiones aritméticas.
* Complementar el aprendizaje de la materia mediante la práctica. Analizador Léxico:
* Números reales.
* Operadores ( ( ) = + - * / ).
* Comentarios de una línea (//).

ALUMNO:

* Oscar Arrúa


/*********** LIbrerias utilizadas **************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/***************** MACROS **********************/

//Codigos
#define PROGRAM		256
#define TYPE		257
#define VAR			258
#define ARRAY		259
#define BEGIN		260
#define END			261
#define PR_DO		262
#define TO			263
#define DOWNTO		264
#define THEN		265
#define OF			266
#define FUNCTION	267
#define PROCEDURE	268
#define PR_INTEGER	269
#define PR_REAL		270
#define PR_BOOLEAN	271
#define PR_CHAR		272
#define PR_FOR		273
#define PR_IF		274
#define PR_ELSE		275
#define PR_WHILE	276
#define REPEAT		277
#define UNTIL		278
#define PR_CASE		279
#define RECORD		280
#define WRITELN		281
#define WRITE		282
#define CONST		283
#define NUM			284
#define ID			285
#define BOOL		286
#define CAR			287
#define LITERAL		288
#define NOT			289
#define OPREL		290
#define OPSUMA		291
#define OPMULT		292
#define OPASIGNA	293
#define USER_TYPE	294

// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

/***************CONSTANTES VARIAS***************/
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101
#define TAM_RES 15
#define TAM_EXPR 50
#define SEPA '/0'
#define TAM_PILA 60

/************* DEFINICIONES ********************/

typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; // null puede representar variable no declarada	
	// aqui irian mas atributos para funciones y procedimientos...

} entrada;

typedef struct {
	int compLex;
	entrada *pe;
} token;

typedef struct {
     char aritlex[TAMLEX];
     int nrolin;
     int cantlex;
} string;


typedef char TipoDato;

typedef struct {
 int posac;
 int cant_element;
 TipoDato lista_s[TAM_PILA];
}pila_tipo_dato;


/************* VARIABLES GLOBALES **************/

int consumir;			     /* 1 indica al analizador lexico que debe devolver
						     el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		    // string utilizado para cargar mensajes de error

token _token;				// token global para recibir componentes del Analizador Lexico


//variables para la conversion postfija
char lineacompl[30];
char resultado[10];
char exprposf [50];
pila_tipo_dato pila_operators;
pila_tipo_dato pila_posf;


///////////////// INICIO POST-FIJO //////////
/***FUNCIONES CON LOS TIPOS DE DATOS***/
void pila_init(pila_tipo_dato *pila);
int pila_apilar(pila_tipo_dato *pila, TipoDato *a);
TipoDato pila_getfrente(pila_tipo_dato *pila,char *dest);
int arrayToPila(pila_tipo_dato * pila, char *b);
int esOperador(char *a);
int precedencia(char *a );
void operar(char * op1, char * op2,char operador, char * resultado);
void ejecutar(pila_tipo_dato *pilaPos, char *res );
void posfijo(char *colaE,pila_tipo_dato * pilaOP, char *k, int fin_linea);

void pila_init(pila_tipo_dato *pila){
   // INICIAMOS PILA CON '\0'
   pila->posac=TAM_PILA;
   pila->cant_element=0;
   int i=0;
   for (i=0;i<=TAM_PILA;i++){
        pila->lista_s[i]='\0';
   }
}

int pila_apilar(pila_tipo_dato *pila, char *a){
    int i=0;
    int _posicion_inicial=(pila->posac)-strlen(a)-2;//nos posicionamos para
    if (_posicion_inicial>0){
        //cargar en la pila
        pila->lista_s[_posicion_inicial]='\0';

        while (a[i]!='\0'){
           pila->lista_s[_posicion_inicial+(i+1)]=a[i];
           i++;
        }
        pila->posac=_posicion_inicial+1;//nos situamos sobre la primera letra desps de '\0'
        pila->cant_element=pila->cant_element+1;
      return 1;
    }
    
    else{
       printf("error, pila llena");
       return -1;
    }

 }
 
TipoDato pila_getfrente(pila_tipo_dato *pila,char *dest){
    int i=0;
    if (pila->posac != TAM_PILA){
        while (pila->lista_s[pila->posac+i] != '\0'){
           dest[i]=pila->lista_s[pila->posac+i];
           i++;
        }
    }
}

void pila_desapilar(pila_tipo_dato *pila, char *dest){
    int i=0;
    if (pila->posac != TAM_PILA){
        while (pila->lista_s[pila->posac+i] != '\0'){
           dest[i]=pila->lista_s[pila->posac+i];
           pila->lista_s[pila->posac+i]='\0';
           i++;
        }
        //avanzamos hasta la sigte pos
        pila->posac=(pila->posac+i)+1;
        pila->cant_element=pila->cant_element-1;
    }
}

void init_array(char *a,int dim){
   int i=0;
   for(i=0;i<=dim;i++){
      a[i]='\0';
   }
}

//convierte el array en una pila
int arrayToPila(pila_tipo_dato *pila, char *b)
{
   int i=strlen(b)-1;
   int e=TAM_PILA;
   int cantele=0;

   for(i;i>=0;i--){
        pila->lista_s[e]=b[i];

       if(b[i]=='#'){
           cantele++;
           pila->lista_s[e]='\0';
      }
       e--;
   }
    pila->posac=e+1;
    pila->cant_element=cantele;
 }

//funciones complementarias de operadores
int esOperador(char *a){
    if (a[0]=='+' || a[0]=='-'||a[0]=='*'||a[0]=='/'){
       return 1;
    }
    else{
         return 0;
    }
}
int precedencia(char *a ){
     int ret=0;
     if(a[0]=='+' || a[0]=='-'){
        ret=1;
     }
     if(a[0]=='*' || a[0]=='/'){
        ret= 2;
     }
   return ret;
}

//vaciar el vector
void limpiar(char *vec,int tamanho){
  int k=0;
  for(k=0;k<=tamanho;k++){
      vec[k]='\0';
  }
}
void posfijo(char *colaE,pila_tipo_dato *pilaOP, char *numactual,int fin_linea ){
     char oper='\0'; //auxiliar
      int Boolinsert=0;
     char sep='#';
    if (esOperador(numactual)==0){ //quiere decir que es operando o numero
        if (fin_linea==1){ //simbolo terminal
           while(pilaOP->cant_element!=0){
             char quitar;
             pila_desapilar(pilaOP,&quitar);
             strcat(colaE,&quitar);
           }
        }
        else{
             strcat(colaE,numactual);
             strcat(colaE,&sep);
             }
    }
    if(esOperador(numactual)==1)
	{
          //3.1) Si la pila esta vacía o el operador tiene mayor prioridad
            //que el que está en la cima de la pila (o bien es la máxima prioridad),
            //se mete en la pila.
       if (pilaOP->cant_element>0){
           pila_getfrente(pilaOP,&oper);
              if (precedencia(numactual)>precedencia(&oper)){
                 pila_apilar(pilaOP,numactual);
                Boolinsert=1;
              }
       }
       if (pilaOP->cant_element==0){
          pila_apilar(pilaOP,numactual);
          Boolinsert=1;
       }
      //3.2) Si la prioridad es menor o igual, el operador se saca de la pila,
      //se mete en la expresión postfija y se vuelve a hacer la comparación con
      //el nuevo elemento de la cima de la pila.
       if(Boolinsert==0 && pilaOP->cant_element>0)
       {
          pila_getfrente(pilaOP,&oper);
          while(precedencia(numactual)<=precedencia(&oper)&& pilaOP->cant_element>0 ){
            char quitar;
            pila_desapilar(pilaOP,&quitar);

            strcat(colaE,&quitar);

            pila_getfrente(pilaOP,&oper);
          }
        pila_apilar(pilaOP,numactual);
       }
    }

}

//////////////FUNCIONES DE LA EJECUCION//////////////////
void operar(char *op1, char *op2,char operador, char *resultado){
     float dat1= atof(op1),dat2= atof(op2),datres;
     if (operador=='+'){datres=atof(op2)+atof(op1);}
     if (operador=='-'){datres=atof(op2)-atof(op1);}
     if (operador=='*'){datres=atof(op2)*atof(op1);}
     if (operador=='/'){datres=atof(op2)/atof(op1);}
     sprintf(resultado, "%f", datres);
}

void ejecutar(pila_tipo_dato *pilaPos, char *res ){
     pila_tipo_dato  auxresult;
     pila_init(&auxresult);
     int error=0;
     char elemento[TAM_RES],result1[TAM_RES],result2[TAM_RES],resultOp[TAM_RES];

     while(pilaPos->cant_element!=0)
     {
        //ceramos los vectores
        init_array(result1,TAM_RES);
        init_array(result2,TAM_RES);
        init_array(elemento,TAM_RES);
        init_array(resultOp,TAM_RES);

        pila_desapilar(pilaPos,elemento);

        //vemos si es operador
        if (esOperador(elemento)==1)
        {

            if ((auxresult.cant_element)<2){
               res[0]=elemento[0];
               strcat(res,"Error!!");
               pilaPos->cant_element=0;
               error=1;

               //ocurrio un error
               }
          pila_desapilar(&auxresult,result1);
          pila_desapilar(&auxresult,result2);
          operar(result1, result2,elemento[0],resultOp);
          pila_apilar(&auxresult,resultOp);
          if(error==0){pila_getfrente(&auxresult,res);}

        }
        else{
           pila_apilar(&auxresult,elemento);

        }
    }
}

////////////////// FIN POST-FIJO /////////////

// variables para el analizador lexico
FILE *archivo;			// Fuente pascal
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int _numero_linea = 1;	// Numero de Linea
int _errores;            // Cantidad de errores
int _cantidad_lexema = 0; // Cantidad de lexemas
string _expresion[1024]; // Guarda la expresion completa a analizar
int _linea_1 = 1;       // Sirve para controlar el cambio de línea

//variables para la conversion postfija
//char lineacompl[30];
//char resultado[10];
//char exprposf [50];
//pila_Td pila_operators;
//pila_Td pila_posf;

/************** ANALIZADOR LÉXICO *********************/

void sigLex();		// Del analizador Lexico

/**************** Funciones **********************/

/*********************HASH************************/
entrada *tabla;				//declarar la tabla de simbolos
int tamTabla=TAMHASH;		//utilizado para cuando se debe hacer rehash
int elems=0;				//utilizado para cuando se debe hacer rehash

int h(const char* k, int m)
{
	unsigned h=0,g;
	int i;
	for (i=0;i<strlen(k);i++)
	{
		h=(h << 4) + k[i];
		if (g=h&0xf0000000){
			h=h^(g>>24);
			h=h^g;
		}
	}
	return h%m;
}
void insertar(entrada e);

void initTabla()
{	
	int i=0;

	tabla=(entrada*)malloc(tamTabla*sizeof(entrada));
	for(i=0;i<tamTabla;i++)
	{
		tabla[i].compLex=-1;
	}
}

int esprimo(int n)
{
	int i;
	for(i=3;i*i<=n;i+=2)
		if (n%i==0)
			return 0;
	return 1;
}

int siguiente_primo(int n)
{
	if (n%2==0)
		n++;
	for (;!esprimo(n);n+=2);

	return n;
}

//en caso de que la tabla llegue al limite, duplicar el tamaño
void rehash()
{
	entrada *vieja;
	int i;
	vieja=tabla;
	tamTabla=siguiente_primo(2*tamTabla);
	initTabla();
	for (i=0;i<tamTabla/2;i++)
	{
		if(vieja[i].compLex!=-1)
			insertar(vieja[i]);
	}		
	free(vieja);
}

//insertar una entrada en la tabla
void insertar(entrada e)
{
	int pos;
	if (++elems>=tamTabla/2)
		rehash();
	pos=h(e.lexema,tamTabla);
	while (tabla[pos].compLex!=-1)
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	tabla[pos]=e;

}
//busca una clave en la tabla, si no existe devuelve NULL, posicion en caso contrario
entrada* buscar(const char *clave)
{
	int pos;
	entrada *e;
	pos=h(clave,tamTabla);
	while(tabla[pos].compLex!=-1 && strcmp(tabla[pos].lexema,clave)!=0 )
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	return &tabla[pos];
}


void insertTablaSimbolos(const char *s, int n)
{
	entrada e;
	sprintf(e.lexema,s);
	e.compLex=n;
	insertar(e);
}

void initTablaSimbolos()
{
   // SOLO SE CARGAN LOS QUE SE VA A UTILIZAR
	entrada pr,*e;
	insertTablaSimbolos(".",'.');
	insertTablaSimbolos("(",'(');
	insertTablaSimbolos(")",')');
	insertTablaSimbolos("+",OPSUMA);
	insertTablaSimbolos("-",OPSUMA);
	insertTablaSimbolos("*",OPMULT);
	insertTablaSimbolos("/",OPMULT);
}

//  INDICAR ERROR
void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n" , _numero_linea , mensaje);	
}



//  ANALIZADOR LÉXICO
void sigLex()
{
	int i=0, longid=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{
        // ESPACIO EN BLANCO (ELIMINAR)
		if (c==' ' || c=='\t')
			continue;
        // INCREMENTAR SALTO DE LÍNEA 
		else if(c=='\n')
		{
			_numero_linea++;
			continue;
		}
		// ANALIZADOR NÚMERICO
		else if (isdigit(c))
		{
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;

				while(!acepto)
				{
					switch(estado){
					//una secuencia netamente de digitos, puede ocurrir . o e
                    case 0: 
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;

                    //un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
					case 1:
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
                    //la fraccion decimal, pueden seguir los digitos o e
   					case 2:
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					//una e, puede seguir +, - o una secuencia de digitos
                    case 3:
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					//necesariamente debe venir por lo menos un digito
                    case 4:
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					//una secuencia de digitos correspondiente al exponente
                    case 5:
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					//estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
                    case 6:
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						_token.pe=buscar(id);
						if (_token.pe->compLex==-1)
						{
							sprintf(e.lexema,id);
							e.compLex=NUM;
							insertar(e);
							_token.pe=buscar(id);
						}
						_token.compLex=NUM;
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		// OPERADOR SUMA
		else if (c=='+')
		{
			_token.compLex=OPSUMA;
			_token.pe=buscar("+");
			break;
		}
        // OPERADOR RESTA
		else if (c=='-')
		{
			_token.compLex=OPSUMA;
			_token.pe=buscar("-");
			break;
		}
		// OPERADOR MULTIPLICACIÓN
		else if (c=='*')
		{
			_token.compLex=OPMULT;
			_token.pe=buscar("*");
			break;
		}
        // OPERADOR DIVISIÓN
		else if (c=='/')
		{
			// VERIFICAR QUE SEA UN COMENTARIO
			if ((c=fgetc(archivo))=='/')
			{//es un comentario
				while(c!='\n')
				{	
                    if ((c=fgetc(archivo))=='\n')
				    {                         
				        break;
                    } 
                    else if ((c=fgetc(archivo))==EOF)
                   	{
		                 _token.compLex=EOF;
                         sprintf(e.lexema,"EOF");
                         _token.pe=&e;
                         break;
                    }
			        //ungetc(c,archivo);
				}
				// SI ES PRIMERA LINEA NO SUMAR
				if(_numero_linea > 1){
                   _numero_linea++;
                }                 
				continue;
			}
			else
			{
				ungetc(c,archivo);
                _token.compLex=OPMULT;
                _token.pe=buscar("/");
			}
			break;
		}
        // PUNTO
		else if (c=='.')
		{
			_token.compLex='.';
			_token.pe=buscar(".");
			break;
		}
        // PARENTESIS
		else if (c=='(')
		{
			_token.compLex='(';
			_token.pe=buscar("(");
			break;
		}
        // PARENTESIS
		else if (c==')')
		{
			_token.compLex=')';
			_token.pe=buscar(")");
			break;
		}
		else if (c!=EOF)
		{
            error("No se encontro una expresion aritmetica valida");
            _errores++;
            sprintf(msg,"%c no esperado",c);
			error(msg);
		}
		}
	if (c==EOF)
	{
		_token.compLex=EOF;
		sprintf(e.lexema,"EOF");
		_token.pe=&e;
	}

}

//  CALCULAR RESULTADOS
void calcular_resultados(){
    int linActual=1; //controla el fin de una linea para su ejecucion
    int i=0;
    char cadLinea[30]; //auxiliar para impresion de la expresion
    init_array(cadLinea,30);
    init_array(lineacompl,30);
    init_array(resultado,10);
    init_array(exprposf,50);
    pila_init(&pila_operators);
    pila_init(&pila_posf);

    while (i<_cantidad_lexema){ //no debe tomar el ultimo caracter por que es EOF
          if (i<_cantidad_lexema-1){
              if(linActual==_expresion[i].nrolin){
                   strcat(cadLinea,_expresion[i].aritlex);
                   posfijo(exprposf,&pila_operators,_expresion[i].aritlex,0);
                   i++;
              }else{
                    posfijo(exprposf,&pila_operators,_expresion[i].aritlex,1);
                    arrayToPila(&pila_posf,exprposf);
                    ejecutar(&pila_posf,resultado); //se calcula el resultado de la expresion
                    printf("\nLin%d:%s = %s",linActual,cadLinea,resultado);
                    //se reestablecen las variables
                    linActual=_expresion[i].nrolin;
                    init_array(lineacompl,30);
                    init_array(resultado,10);
                    init_array(exprposf,50);
                    pila_init(&pila_operators);
                    pila_init(&pila_posf);
                    strcpy(cadLinea,"");}

          }else{
                //Si se llega a EOF, realizar el ultimo calculo
                posfijo(exprposf,&pila_operators,_expresion[i-1].aritlex,1);
                arrayToPila(&pila_posf,exprposf);
                ejecutar(&pila_posf,resultado);
                printf("\nLin%d: %s = %s",linActual,cadLinea,resultado);
                i++;

          }

     }

}

int main(int argc,char* args[])
{
	// inicializar analizador lexico
	int complex=0;

    // indica el número de línea
    int _linea = 0;
    
    initTabla();
	initTablaSimbolos();
	
	if(argc > 1)
	{
	    if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (_token.compLex!=EOF){
			sigLex();
            // printf("Lin %d: %s -> %d\n",_numero_linea,_token.pe->lexema,_token.compLex);
            // acumulamos los lexemas de la expresión en una estructura llamada string mientras no se cambie de línea
			if (_numero_linea==_linea){
              	sprintf(_expresion[_cantidad_lexema].aritlex,_token.pe->lexema);
                _expresion[_cantidad_lexema].nrolin=_numero_linea;
                _expresion[_cantidad_lexema].cantlex=_cantidad_lexema;
             	_cantidad_lexema++;
            }else{
               // SALTO DE LINEA   
               _linea=_numero_linea;
           	   sprintf(_expresion[_cantidad_lexema].aritlex,_token.pe->lexema);
               _expresion[_cantidad_lexema].nrolin=_numero_linea;
               _expresion[_cantidad_lexema].cantlex=_cantidad_lexema;
           	   _cantidad_lexema++;
            }
		}
        // CALCULAR LOS RESULTADOS DE LAS EXPRESIONES
        calcular_resultados();
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
    system("PAUSE");
	return 0;
}
