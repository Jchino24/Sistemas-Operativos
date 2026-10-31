# Sistemas Operativos

Prácticas y ejercicios de la materia. Cada carpeta es un trabajo independiente.

## Algoritmo de Peterson

Implementación en C del **algoritmo de Peterson** para exclusión mutua entre dos
procesos, usando hilos (`pthread`) y variables compartidas. No necesita locks ni
instrucciones especiales: solo dos variables.

### Variables compartidas

| Variable | Tipo | Para qué |
|---|---|---|
| `interesado[2]` | `atomic_bool` | Cada hilo avisa de que quiere entrar |
| `turno` | `atomic_int` | A quién le toca cuando los dos quieren entrar |

Son `atomic_*` porque en una CPU multinúcleo las escrituras normales pueden
quedar en caché local o reordenarse. Sin atomicidad el algoritmo puede fallar de
forma intermitente, que es lo peor que puede pasar en una práctica.

### Cómo funciona

Cada hilo ejecuta siempre la misma secuencia:

1. **Entrada** — `interesado[id] = true` y después `turno = otro`. El hilo
   declara su interés y *cede* el turno al otro.
2. **Espera activa** — `while (interesado[otro] && turno == otro)`. Solo espera si
   el otro quiere entrar **y** es su turno. En cuanto el otro sale, la condición
   se rompe y entra.
3. **Sección crítica** — aquí se imprime la entrada y la salida, con un `sleep(1)`
   para que se vea el turno.
4. **Salida** — `interesado[id] = false`.

### Propiedades que cumple

- **Exclusión mutua**: no pueden estar los dos dentro a la vez. Si ambos ponen
  `interesado = true`, el último que escribió `turno` es el que espera.
- **Progreso**: si uno no quiere entrar, el otro no se queda bloqueado.
- **Espera limitada**: el que espera entra como mucho tras una salida del otro.

### Compilar y ejecutar

```bash
gcc -O2 -pthread "Algoritmo De Peterson/peterson.c" -o peterson
./peterson
```

### Salida esperada

El orden puede variar, pero nunca deben solaparse las secciones críticas:

```
--> Hilo 0 ENTRO a la sección crítica
<-- Hilo 0 SALIÓ de la sección crítica
--> Hilo 1 ENTRO a la sección crítica
<-- Hilo 1 SALIÓ de la sección crítica
Fin del programa. Exclusión mutua exitosa.
```

> Si se quitan los `atomic_` y se compila con `-O2`, el orden puede romperse:
> es la mejor demostración de por qué el algoritmo necesita memoria atómica en
> un procesador real.
