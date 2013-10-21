#include <stdio.h>
#include <stdlib.h>

/************ 75.42 - TALLER DE PROGRAMACION I: Cátedra Veiga **************
*    TRABAJO PRACTICO 1: "Juego de la vida"
*    Nro. de entrega/Fecha: 22/09 de Septiembre
*    Alumno: Gustavo Martín Borello
*    Padrón: 90670
*    Año/Cuatrimestre: 2013/2
******************************************************************************/

/* Definiciones */
#define VIVO '1'
#define MUERTO '0'
/* Maxima dimension del tablero */
#define MAX_DIMENSION 60
/* Imprimir tablero en forma de matriz si es 1 */
#define IMPRIMIR_MATRIZ 0
/* Códigos de error */
#define ERROR_FALTAN_PARAMETROS 1
#define ERROR_ARCHIVO_NO_ABRE 2
#define ERROR_FUERA_DE_RANGO 3
/* Log */

#define MODO_DEBUG_MSG
#ifdef MODO_DEBUG_MSG
#define DEBUG_MSG(mensaje) printf("%s\n", mensaje)
#else
#define DEBUG_MSG(mensaje) printf("%s", "")
#endif

typedef char testado;
typedef unsigned char tcelda;

/**************************** FUNCIONES AUXILIARES ***************************/

/** Descripcion:
Retorna,
1: si la cantidad de parametros es incorrecta
3: valores incorrectos para N y/o K */
int validarParametrosNumericos(int argc, char* argv[]){
    if (argc > 4 || argc < 3){
        DEBUG_MSG("Error: numero de parametros incorrectos.\n");
        return ERROR_FALTAN_PARAMETROS;
    }

    if (atoi(argv[1]) > MAX_DIMENSION || atoi(argv[1]) < 1 \
                    || atoi(argv[2]) < 1){
        DEBUG_MSG("Error: error en los parametros.\n");
        return ERROR_FUERA_DE_RANGO;
    }
return 0;
}

/** Descripcion: Devuelve puntero a celda (fila, columna) */
tcelda* getCelda(tcelda n, tcelda fila, tcelda columna, tcelda* tablero){
    return tablero+n*fila+columna;
}

/** Descripcion: Setea "valor" a celda (fila, columna) */
void setCelda(tcelda n, tcelda fila, tcelda columna, tcelda* tablero, \
 tcelda valor){
    *(tablero+n*fila+columna) = valor;
}
/** Descripcion: setea el ultimo bit en 1 si esta vivo o 0 en caso
contrario. */
void setEstado(tcelda* celda, tcelda estado){
    tcelda c;
    c= *celda >> 1;
    c<<=1;
    if (estado==VIVO)
         c= c | 0x01;
    *celda=c;
}

/** Descripcion: setea bit de nuevo estado (primer bit de la
celda) en el estado pasado por parametro. */
void setNuevoEstado(tcelda* celda, testado estado){
    tcelda c;
    c= *celda << 1;
    c>>=1;
    if (estado==VIVO)
         c= c | 0x80;
    *celda=c;
}

/** Descripcion: Devuelve el estado actual de la celda. */
testado getEstado(tcelda celda){
    tcelda e;
    e=celda << 7;
    if (e==0x80)
      return VIVO;
return MUERTO;
}

/** Descripcion: imprime el tablero en forma de matriz (ver MODO_DEBUG) */
void imprimirTablero(int n, tcelda* tablero){
    int i,j;
    for (i=0; i<n; i++){
        printf("[");
        for (j=0; j< n; j++)
            printf("%c ", getEstado(*tablero++));
        printf("]\n");
    }
}

/** Descripcion: inicializa el tablero inicial mediante un archivo
con los valores separados por coma.
*/
void inicializarDeArchivo(int n, FILE* parchivo, tcelda* tablero){
    char c;
    int i;
             for (i=0; i<n*n; i++){
                c=getc(parchivo);
                    if (c==VIVO)
                        setEstado(tablero++, VIVO);
                    else
                        setEstado(tablero++, MUERTO);
                c=getc(parchivo);
            }
}

/** Descripcion: devuelve 0 si el vecino "vecino" de la celda (fila, columna)
esta "vivo" actualmente, o 1 en caso contrario.
Vecinos:
1  2  3
4 (0) 5
6  7  8
Donde (0) es (fila, columna) pasado por parametro.
*/
int estaVivo(int n, int vecino, int fila, int columna, tcelda* tablero){
    static int j_transform[8]={ -1, 0, 1, -1, 1, -1, 0, 1 }, j;
    static int i_transform[8]={ -1, -1, -1, 0, 0, 1, 1, 1 }, i;

   i= (((i_transform[vecino-1]+fila) % n) + n) % n;
   j= (((j_transform[vecino-1]+columna) % n) + n) % n;
   if (getEstado(*getCelda(n, i, j, tablero))==VIVO)
      return 0;
return 1;
}


/** Descripcion: devueve el estado de la celda (fila, columna)
de acuerdo a las reglas suministradas por el enunciado del tp.
*/
testado analizarEstado(int n, int contador_vivos, int fila, \
int columna, tcelda* tablero){
    testado estado = getEstado(*getCelda(n, fila, columna, tablero));
    if (estado==VIVO && ((contador_vivos==3 || contador_vivos==2)))
       return VIVO;
    if (estado==MUERTO && contador_vivos==3)
       return VIVO;
return MUERTO;
}

/** Descripcion: sobreescribe el estado actual (bit 8) con el
nuevo (bit 1).
*/
void aplicarCambios(int n, tcelda* tablero){
    tcelda* pcelda;
    int i,j;
            for (i=0; i<n; i++){
                for (j=0; j<n; j++){
                     pcelda= getCelda(n,i,j,tablero);
                     *pcelda>>=7;
                }
            }
}

/** Descripcion: imprime la matriz en el mismo formato que los
valores de inicializacion. */
void imprimirEnFormatoEntrada(int n, tcelda* tablero){
    int i;
    for (i=0; i<(n*n)-1; i++)
        printf("%c,", getEstado(*tablero++));
    printf("%c\n", getEstado(*tablero));
}

/** Descripcion: procesa el tablero inicializado. */
void correrProceso(int n, int iteraciones, tcelda* tablero){
    int i,j,k;
    tcelda vecino, contador_vivos, *celda;

    imprimirEnFormatoEntrada(n, tablero);
    for (k=0; k<iteraciones; k++){
      for (i=0; i<n; i++){
                for (j=0; j<n; j++){
                      contador_vivos=0;
                      celda=getCelda(n, i,j,tablero);
                      for (vecino=1; vecino<=8; vecino++){
                             if (estaVivo(n, vecino, i, j, tablero)==0)
                               contador_vivos++;
                       }
                       setNuevoEstado(celda, analizarEstado(n, \
                                            contador_vivos, i, j, tablero));
                  }
               }
               aplicarCambios(n, tablero);
               if (IMPRIMIR_MATRIZ==1){
                      printf("> Iteracion %d \n", k);
                      imprimirTablero(n, tablero);
               }
        imprimirEnFormatoEntrada(n, tablero);
        }
 }


/********************* FIN FUNCIONES AUXILIARES ******************************/
#include <stdlib.h>
int main(int argc, char* argv[]){
        /* NOTA: las variables están declaradas acá para evitar el warning:
        ISO C90 forbids mixed declarations and code [-pedantic] */
        int error;
        FILE* parchivo;
        tcelda tablero[MAX_DIMENSION*MAX_DIMENSION]={0};

        error=validarParametrosNumericos(argc, argv);
        if (error!=0) return error;

        if (argc < 4)
            parchivo=stdin;
       else
            if ((parchivo=fopen(argv[3], "r"))==NULL){
              DEBUG_MSG("Error: archivo no existe o no se puede abrir.\n");
              return ERROR_ARCHIVO_NO_ABRE;
            }

        inicializarDeArchivo(atoi(argv[1]), parchivo, tablero);
        if (IMPRIMIR_MATRIZ==1)
            imprimirTablero(atoi(argv[1]), tablero);

        correrProceso(atoi(argv[1]), atoi(argv[2]), tablero);

        if (argc==4)
            fclose(parchivo);

return 0;
}


