/* tp 2 */
/* 
Interfaz comandos:

agregar_archivo <nombre_archivo>: 
procesa de forma completa un archivo de .csv que contiene datos de vuelos.

ver_tablero <K cantidad vuelos> <modo: asc/desc> <desde> <hasta>: 
muestra los K vuelos ordenados por fecha de forma ascendente (asc) o 
descendente (desc), cuya fecha de despegue esté dentro de el intervalo 
<desde> <hasta> (inclusive).

info_vuelo <código vuelo>: 
muestra toda la información posible en sobre el vuelo que tiene el código 
pasado por parámetro.

prioridad_vuelos <K cantidad vuelos>: 
muestra los códigos de los K vuelos que tienen mayor prioridad.

borrar <desde> <hasta>: 
borra todos los vuelos cuya fecha de despegue estén dentro del intervalo 
<desde> <hasta> (inclusive).

Si un comando es válido deberá imprimir OK por salida estándar después 
de ser ejecutado. Si un comando no pertenece a los listados previamente o 
tiene un error, se imprime Error en comando <comando> por stderr y 
continua la ejecución.
*/
