#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TODO_OK 0
#define ERROR_ARG 1
#define ERROR_ARCH 2
#define ERROR_LINEA_LARGA 3

#define ARG_TXT 1
#define ARG_TIPO_TXT 2
#define ARG_BIN 3

#define TAM_LINEA 501
#define TAM_DESCR 51

typedef struct
{
    int dia;
    int mes;
    int anio;
}Fecha;


typedef struct
{
    int codigo;
    char descripcion[TAM_DESCR];
    int cantidad;
    float precioUnitario;
    Fecha fechaPreUni;
}Producto;

typedef int (*TxtABin)(const char* Linea, Producto* prod);

//int generarArchivos(const char* nomProds, const char* nomMovs);
int actualizarProductos(const char* nomProds, const char* nomMovs);
void acumularDuplicados(FILE* movsA, Producto* mov, FILE* prodsTmpA);
int mostrarProductos(const char* nombProds);
int txtVarABin(const char* Linea, Producto* prod);
int txtFijoABin(const char* Linea, Producto* prod);


///Clase_8-05-2020_txt_a_bin nombre_txt [v/f] nombre_bin
int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        puts("Cantidad incorrecta de argumentos.");
        return ERROR_ARG;
    }

    FILE* productosTxtA = fopen(argv[ARG_TXT],"rt");

    if(!productosTxtA)
    {
        puts("Error abriendo archivo.");
        return ERROR_ARCH;
    }

    FILE* productosBinA = fopen(argv[ARG_BIN],"wb");

    if(!productosBinA)
    {
        puts("Error abriendo archivo.");
        return ERROR_ARCH;
    }

    FILE* errores = fopen(argv[ARG_TXT], "wb"); //Grabo en este archivo los errores
        if(!errores)
        {
            puts("Error abriendo archivo.");
            return ERROR_ARCH;
        }

    Producto prod;

///FORMA FACIL DE CONVERTIR DE TXT A BIN (NO CONVIENE)
/*    while(fscanf(
                 productosTxtA,"%d|%[^|]|%d|%f|%d|%d/%d/%d",
                 &prod.codigo,
                 prod.descripcion,
                 &prod.cantidad,
                 &prod.precioUnitario,
                 &prod.fechaPreUni.dia, &prod.fechaPreUni.mes, &prod.fechaPreUni.anio) != EOF);

    {
        fwrite(&prod, sizeof(Producto), 1, productosBinA);
    }
*/
///FORMA LABORIOSA DE CONVERTIR TXT A BIN (RECOMENDADA)

    char Linea[TAM_LINEA];
    int codRet = TODO_OK;
    char tipoTxt = argv[ARG_TIPO_TXT][0];

   TxtABin txtABin = tipoTxt == 'v' ? txtVarABin : txtFijoABin; ///WARNING txtABin no se usa.

    while(fgets(Linea, TAM_LINEA, productosTxtA) && codRet != ERROR_LINEA_LARGA)
    {
       codRet = txtFijoABin(Linea, &prod);

        if(codRet == TODO_OK)
            fwrite(&prod, sizeof(Producto), 1, productosBinA);
        else
            fwrite(&prod, sizeof(Producto), 1, errores); //GRABO LA LINEA EN EL ARCHIVO ERRORES.TXT
    }

    fclose(productosTxtA);
    fclose(productosBinA);

    if(codRet == ERROR_LINEA_LARGA)
        return codRet;

    mostrarProductos(argv[ARG_BIN]);

    system("pause");
    return 0;
}

/*int generarArchivos(const char* nomProds, const char* nomMovs)
{
    static Producto prodsV[]= //NO ACLARO EL TAMAÑO YA QUE EL MISMO PROGRAMA SABRA DE CUANTO SERA
    {
        {2, "Mandarina", 40}, {4, "Banana", 100}, {7, "Pera", 50}, {9, "Manzana", 70}, {10, "Naranja", 90}
    };

        FILE* prodsA = fopen(nomProds, "wb");

            if(!prodsA)
            {
                puts("ERROR ABRIENDO ARCHIVO.");
                return ERROR_ARCH;
            }

            fwrite(prodsV, sizeof(Producto), 5, prodsA);
            fclose(prodsA);

    return TODO_OK;
}
*/

int txtVarABin(const char* Linea, Producto* prod)
{
    char LineaCopy[TAM_LINEA];
    //int codRet = TODO_OK;

    strcpy(LineaCopy, Linea);

    char* iniCampo = strchr(LineaCopy, '\n'); ///BUSCO EL ENTER

    if(!iniCampo)
        return ERROR_LINEA_LARGA;

    ///Fecha Pre Uni
    *iniCampo = '\0';
    iniCampo = strrchr(LineaCopy,'|'); ///GUARDA LA DIRECCION DONDE ESTA "|"
    //TODO: VALIDAR TEXTO
    sscanf(iniCampo + 1, "%d/%d/%d", &prod->fechaPreUni.dia, &prod->fechaPreUni.mes, &prod->fechaPreUni.anio);

    ///Precio Unitario
    *iniCampo = '\0';
    iniCampo = strrchr(LineaCopy,'|');
    //TODO: VALIDAR TEXTO
    sscanf(iniCampo + 1, "%f", &prod->precioUnitario);

    ///Cantidad
    *iniCampo = '\0';
    iniCampo = strrchr(LineaCopy,'|');
    //TODO: VALIDAR TEXTO
    sscanf(iniCampo + 1, "%d", &prod->cantidad);

    ///Descripcion
    *iniCampo = '\0';
    iniCampo = strrchr(LineaCopy,'|');
    //TODO: VALIDAR TEXTO
    strncpy(prod->descripcion, iniCampo + 1, TAM_DESCR - 1); ///STRNCPY RECIBE COMO 3ER PARAMETRO LA CANTIDAD MAXIMA DE TAMAÑO ///NO HAY QUE CONTAR EL /0, POR ESO EL -1

    ///Codigo
    *iniCampo = '\0';
    //TODO: VALIDAR TEXTO
    sscanf(LineaCopy, "%d", &prod->codigo);

    return TODO_OK;
}

int txtFijoABin(const char* Linea, Producto* prod)
{
    char LineaCopy[TAM_LINEA];
    //int codRet = TODO_OK;

    strcpy(LineaCopy, Linea);

    char* iniCampo = strchr(LineaCopy, '\n'); ///BUSCO EL ENTER

    if(!iniCampo)
        return ERROR_LINEA_LARGA;

    ///Fecha Pre Uni
    *iniCampo = '\0';
    iniCampo -= 8;
    //TODO: VALIDAR TEXTO
    sscanf(iniCampo + 1, "%d/%d/%d", &prod->fechaPreUni.dia, &prod->fechaPreUni.mes, &prod->fechaPreUni.anio);

    ///Precio Unitario
    *iniCampo = '\0';
    iniCampo -= 8;
    //TODO: VALIDAR TEXTO
    sscanf(iniCampo + 1, "%f", &prod->precioUnitario);

    ///Cantidad
    *iniCampo = '\0';
    iniCampo -= 3;
    //TODO: VALIDAR TEXTO
    sscanf(iniCampo + 1, "%d", &prod->cantidad);

    ///Descripcion
    *iniCampo = '\0';
    iniCampo -= 50;
    //TODO: VALIDAR TEXTO
    strncpy(prod->descripcion, iniCampo + 1, TAM_DESCR - 1); ///STRNCPY RECIBE COMO 3ER PARAMETRO LA CANTIDAD MAXIMA DE TAMAÑO ///NO HAY QUE CONTAR EL /0, POR ESO EL -1

    ///Codigo
    *iniCampo = '\0';
    //TODO: VALIDAR TEXTO
    sscanf(LineaCopy, "%d", &prod->codigo);

    return TODO_OK;
}

int mostrarProductos(const char* nomProds)
{
    FILE* prodsA = fopen(nomProds, "rb");

        if(!prodsA)
        {
            puts("ERROR ABRIENDO ARCHIVO.");
            return ERROR_ARCH;
        }
            Producto prod;
            fread(&prod, sizeof(Producto), 1, prodsA);

                while(!feof(prodsA))
                {
                    printf("[%d, %s, %d, %.2f, %d/%d/%d]\n", prod.codigo, prod.descripcion, prod.cantidad, prod.precioUnitario, prod.fechaPreUni.dia, prod.fechaPreUni.mes, prod.fechaPreUni.anio);
                    fread(&prod, sizeof(Producto), 1, prodsA);
                }

    fclose(prodsA);

    return TODO_OK;
}

