#define _GNU_SOURCE 1
#include "strutil.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *substr(const char *str, size_t n){
    char* str_recortada = calloc(n+1, sizeof(char));
    if (!str_recortada) return NULL;
    for (int i = 0; i < n && str[i] != '\0'; i++) str_recortada[i] = str[i];
    return str_recortada;
}

int contador_aparaciones(const char *str, char sep){
    int contador = 0;
    for (int i = 0; str[i]; i++) if (str[i] == sep) contador ++;
    return contador;
}

char **split(const char *str, char sep){
    int i = 0, contador = contador_aparaciones(str, sep), pos_vec = 0, cant_caracteres = 0;
    char** vec_split = malloc(sizeof(char*) * (contador+2));
    int pos_ultimo_sep = i;
    while (pos_vec < contador+1){
        if (str[i] == sep || str[i] == '\0'){
            vec_split[pos_vec] = substr(str + pos_ultimo_sep, cant_caracteres);
            pos_vec++;
            cant_caracteres = 0;
            if (str[i] != '\0') pos_ultimo_sep = ++i;
        }
        else {cant_caracteres++; i++;}
    }
    vec_split[pos_vec] = NULL;
    return vec_split;
}

size_t largo_total_join(char **strv){
    size_t contador = 0;
    for (int i = 0; strv[i]; i++) contador += strlen(strv[i]) + 1;
    return contador;
}

char *join(char **strv, char sep){
    char* vec_join = malloc((largo_total_join(strv) + 1) * sizeof(char));
    if (!vec_join) return NULL;
    size_t pos_actual = 0;
    for (int i = 0; strv[i]; i++){
        vec_join[pos_actual] = '\0';
        strcat(vec_join + pos_actual, strv[i]);
        pos_actual += strlen(strv[i]);
        if (strv[i+1] && sep != '\0'){
            vec_join[pos_actual] = sep;
            pos_actual++;
        }
    }
    vec_join[pos_actual] = '\0';
    return vec_join;
}

void free_strv(char *strv[]){
    for (int i = 0; strv[i] ;i++) free(strv[i]);
    free(strv);
}