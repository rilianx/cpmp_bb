# CPMP Solver

## Compilation (en carpeta src)
````
g++ Greedy.cpp Layout.cpp Bsg.cpp lbatman.cpp main_cpmp.cpp -o feg -std=c++11
````

## Execution (en carpeta src)
````
# ./feg H instance
./feg 5 ../Instancias/CVS/3-7/data3-7-9.dat
````

### Output
````
initial Layout:
[18 2 17 ]
[20 5 19 ]
[12 10 3 ]
[13 4 16 ]
[6 7 15 ]
[1 14 21 ]
[9 11 8 ]

greedy cost:16

tree search:
L==U
optimal cost:13
#lb_counter:68926
4.195
````

---

## Funciones de interés

* `main` en `main_cpmp.cpp`
* `search2` en `main_cpmp.cpp` implementa algoritmo **A***
* `lb2` en `lbatman.cpp` implementa heurística admisible para **A***
* `greedy_solve` en `Greedy.cpp` implementa algoritmo greedy del paper
* `Layout.h` y `Layout.cpp` implementa la lógica del problema CPMP, los movimientos y funciones de utilidad.

