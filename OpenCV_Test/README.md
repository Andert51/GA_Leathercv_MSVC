# Analizador de Piel de Carnero con Algoritmos Genéticos y Visión por Computador (OpenCV)

Autor: Andert51

Repositorio: https://github.com/Andert51/GA_Leathercv_MSVC


Resumen (IMRaD)

Introducción
- Contexto: En la fabricación de bolsos y marroquinería, es común optimizar el aprovechamiento de pieles naturales mediante recortes con formas específicas. Planteamos el problema de anidar (packing) recortes circulares dentro del contorno utilizable de una piel, extraído desde una imagen.
- Objetivo: Determinar, a partir de una imagen `piel_carnero.png`, (1) cuántos recortes circulares completos de 20 cm de diámetro (radio = 10 cm) pueden obtenerse y (2) en qué coordenadas (centros) deben ubicarse. Adicionalmente, (3) reportar una figura con los recortes y una tabla con sus centros, (4) documentar la implementación y (5) discutir el criterio de adecuación (fitness) usado. Finalmente, (6) repetir el análisis para 25 cm de diámetro y comparar resultados.
- Enfoque: Usamos visión por computador (OpenCV) para segmentar/limpiar el área de piel y un algoritmo genético (AG) para optimizar la colocación de círculos no superpuestos y completamente contenidos dentro del área aprovechable.


Métodos

Datos, supuestos y unidades
- Entrada principal: imagen en color `piel_carnero.png` ubicada en el directorio raíz del proyecto. La imagen debe mostrar la pieza de piel sobre un fondo contrastante.
- Calibración espacial: se asume una relación de `5 píxeles = 1 cm` (constante `PIXELS_PER_CM = 5.0`).
- Escenarios de corte: diámetro de 20 cm (radio 10 cm) y, de forma comparativa, diámetro de 25 cm (radio 12.5 cm).

Pipeline de visión por computador
1) Conversión a escala de grises.
2) Umbralización binaria inversa con `threshold(gray, 200, 255, THRESH_BINARY_INV)` para separar piel (valor 255) de fondo (valor 0).
3) Limpieza morfológica con elemento elíptico 5×5:
   - Cierre morfológico para rellenar huecos pequeños.
   - Apertura morfológica para eliminar ruido pequeño.
4) Exportación intermedia:
   - `piel_carnero_bin.png`: imagen binaria depurada.
   - `piel_carnero_bin.csv`: matriz binaria (0/255) como CSV para análisis externo.

Representación y restricciones
- Individuo (solución candidata): vector de círculos, cada círculo definido por su centro `(x, y)` en píxeles y radio fijo `CIRCLE_RADIUS_PIXELS` según el escenario.
- Restricciones duras:
  - Contención: cada círculo debe quedar completamente dentro del área utilizable (región blanca en la binaria tras la morfología) y dentro de los límites de la imagen.
  - No superposición: dos círculos no deben solaparse.

Función de aptitud (fitness)
- Para cada círculo en el individuo:
  - Si el círculo no está completamente dentro de la piel, se ignora en el conteo (no suma, no resta), evitando recompensar candidatos inválidos.
  - Si está dentro, pero se superpone con otro círculo del mismo individuo, se penaliza con `-0.5`.
  - Si está dentro y sin superposición, aporta `+1.0` a la aptitud.
- La aptitud final es la suma sobre los círculos. Maximizar esta función impulsa soluciones con más cortes válidos y sin solapamientos.

Algoritmo genético (AG)
- Parámetros por defecto:
  - `POPULATION_SIZE = 100`
  - `MAX_GENERATIONS = 500`
  - `MUTATION_RATE = 0.05`
  - `CROSSOVER_RATE = 0.8`
- Inicialización: población de individuos con entre 1 y 20 círculos, ubicados aleatoriamente dentro de los límites admisibles de la imagen.
- Selección: torneo (k=3), eligiendo al mejor entre muestras aleatorias.
- Cruzamiento: combinación por submuestreo (cada círculo de los padres se hereda con prob. 0.5), generando un descendiente que reúne subconjuntos de ambos padres.
- Mutación:
  - Desplazamiento aleatorio de centros con prob. `MUTATION_RATE` por círculo.
  - Inserción de un círculo con prob. `MUTATION_RATE` (si tamaño < 50).
  - Eliminación aleatoria de un círculo con prob. `MUTATION_RATE` (si tamaño > 1).
- Elitismo: se preserva el 20% superior de la población en cada generación.
- Criterio de parada: número fijo de generaciones (`MAX_GENERATIONS`). Se muestra progreso resumido en consola.

Implementación
- Lenguaje y librerías: C++17, OpenCV 4.x, consola de Windows con salida coloreada.
- Archivo principal: `OpenCV_Test/GA_ImCv.cpp`.
- Clases y funciones clave:
  - `CyberInterface`: impresión UI (barras de progreso, tablas, mensajes) y animaciones ligeras.
  - `SkinAnalyzer`:
    - `loadImage()`: carga imagen y reporta dimensiones y tamaño real (cm).
    - `performThresholding()`: aplica el pipeline de visión descrito arriba.
    - `saveBinaryResults()`: escribe `PNG` y `CSV` de la máscara binaria.
    - `isCircleValid()`: verifica contención completa del círculo dentro de la piel.
    - `circlesOverlap()`: verifica solapamiento entre círculos.
    - `calculateFitness()`: implementa el criterio de aptitud descrito.
    - `generateInitialPopulation()`, `selectByTournament()`, `crossover()`, `mutate()`, `runGeneticAlgorithm()`: ciclo del AG.
    - `showResults()`: filtra círculos válidos y no solapados del mejor individuo, imprime estadísticas y tabla de centros, y llama a `createResultImage()`.
    - `createResultImage()`: dibuja los círculos y guarda la figura final como `piel_carnero_resultado.png`.
    - `analyzeWithDifferentRadius(newRadiusCm)`: repite todo el proceso para un nuevo radio en centímetros (ej. 12.5 cm para diámetro 25 cm).
- Flujo en `main()`:
  1) Carga de imagen `piel_carnero.png`.
  2) Umbralización y limpieza.
  3) Exportación binaria (`PNG` y `CSV`).
  4) AG para diámetro 20 cm (radio 10 cm): imprime tabla y genera figura.
  5) AG para diámetro 25 cm (radio 12.5 cm): imprime tabla y genera figura nuevamente.
  6) Visualización en ventanas (opcional, requiere interacción).

Complejidad y consideraciones de desempeño
- Evaluar validez de un círculo verifica píxeles dentro del radio, por lo que su coste es O(r^2) por círculo. La aptitud recorre círculos y pares potencialmente solapados, con coste aproximado O(n^2 + n·r^2) por individuo (n=círculos). El AG escala con población y generaciones; use imágenes de tamaño razonable o ajuste parámetros en equipos modestos.


Resultados

Cómo compilar (Windows + MSVC + OpenCV)
1) Requisitos:
   - Windows 10/11 (consola con soporte VT habilitada; el programa la activa automáticamente).
   - Visual Studio 2019/2022 (Desktop development with C++).
   - OpenCV 4.x (precompilado para MSVC o compilado localmente).
2) Configuración de proyecto (sugerida):
   - Agregar rutas de inclusión de OpenCV a `C/C++ > Additional Include Directories`.
   - Agregar rutas de librerías a `Linker > Additional Library Directories`.
   - Vincular `opencv_world4xx.lib` (o libs equivalentes) en `Linker > Input > Additional Dependencies`.
   - Asegurar `PATH` (o `Debugging > Environment`) apunta a la carpeta de `opencv_world4xx.dll` para tiempo de ejecución.
3) Colocar `piel_carnero.png` en el directorio del proyecto (raíz de la solución en este repositorio).
4) Compilar y ejecutar.

Ejecución y artefactos generados
- Al ejecutar se muestran mensajes de progreso y resultados en la consola.
- Se crean archivos:
  - `piel_carnero_bin.png`: máscara binaria (piel=blanco, fondo=negro).
  - `piel_carnero_bin.csv`: valores 0/255 por píxel.
  - `piel_carnero_resultado.png`: figura con círculos numerados y centros marcados. Nota: el segundo análisis (25 cm) sobrescribe este nombre; renómbrelo tras el primer análisis si desea conservar ambas figuras.

Resultados para diámetro 20 cm (radio = 10 cm)
- El programa reporta:
  - `Circulos optimos encontrados: N` (N es el número de recortes completos obtenidos).
  - `Radio de cada circulo: 10 cm (50 pixeles)` conforme a `PIXELS_PER_CM=5`.
  - `Area por circulo` y `Area total aprovechada`.
- Tabla de centros (se imprime en consola):
  - Columnas: `ID`, `X(px)`, `Y(px)`, `X(cm)`, `Y(cm)`.
  - Esta tabla es la respuesta inequívoca de “¿en dónde ubicar el centro de cada área circular?”. Puede copiarse/guardarse desde la consola.
- Figura:
  - `piel_carnero_resultado.png` contiene los círculos trazados y numerados, respondiendo a la solicitud de acompañar la respuesta con una figura.

Resultados para diámetro 25 cm (radio = 12.5 cm)
- El programa ejecuta automáticamente un segundo análisis con `analyzeWithDifferentRadius(12.5)`, reportando su propia tabla y figura (mismo nombre de archivo; renombre si desea mantener ambas).
- Expectativa cualitativa: al aumentar el radio de 10 cm a 12.5 cm, el área por círculo crece por un factor de (12.5/10)^2 = 1.5625. Por lo tanto, el número de círculos tiende a disminuir (aprox. en esa proporción, sujeto a la geometría y bordes de la piel). La tabla/figura resultante refleja la nueva colocación óptima.

Respuestas a las preguntas del enunciado
1) Diámetro de 20 cm (radio = 10 cm):
   - ¿Cuántos recortes circulares completos? El número exacto es el `N` que el programa imprime como `Circulos optimos encontrados` y coincide con el número de filas en la tabla mostrada.
   - ¿Dónde ubicar los centros? Se provee la tabla en consola con columnas en píxeles y centímetros; la figura `piel_carnero_resultado.png` traza cada círculo con su índice.
   - Documentación de la solución: ver las secciones “Métodos” e “Implementación”.
   - Criterio de adecuación (fitness): ver la sección “Función de aptitud (fitness)” y la discusión más abajo.
2) Cambio a diámetro de 25 cm:
   - El resultado (número de recortes y sus centros) lo reporta el segundo análisis automático. Se espera una reducción en la cantidad de recortes respecto de 20 cm; ver “Resultados para diámetro 25 cm”.


Discusión

Adecuación del criterio de fitness
- Sumar `+1` por cada círculo válido y no solapado alinea directamente la función objetivo con el propósito de maximizar el número de recortes completos.
- Penalizar solapamientos con `-0.5` desalienta configuraciones inválidas, pero aún permite la exploración guiada (no es una penalización tan severa como excluir por completo al individuo), mejorando la diversidad y la convergencia del AG.
- Ignorar círculos fuera de la piel (no sumar ni restar) evita recompensar intentos inválidos sin destruir por completo individuos prometedores con pocos elementos desajustados.

Fortalezas
- El pipeline de visión (umbral + morfología) otorga máscaras robustas a ruido y huecos pequeños.
- El AG con elitismo mantiene soluciones de alta calidad, mientras que selección por torneo y mutación conservan diversidad.
- La representación simple (centros de círculos) facilita operadores y chequeos.

Limitaciones y oportunidades de mejora
- Reproducibilidad: el uso de `random_device` implica resultados no deterministas. Para auditoría, conviene fijar una semilla (`mt19937 gen(seed)`), al menos en los operadores del AG y en la inicialización.
- Eficiencia: el chequeo de contención por píxel es costoso; podría acelerarse con máscaras precomputadas (plantillas) y convolución binaria o distancias firmadas.
- Estrategia de cruce/mutación: podrían explorarse operadores geométricos más informados (p. ej., heurísticas tipo Poisson disk sampling, relajación por fuerzas, búsqueda local híbrida).
- Multiobjetivo: maximizar número y, a la vez, minimizar recortes en bordes, maximizar regularidad de patrón o robustez a tolerancias de corte.
- Gestión de archivos de salida: actualmente la figura del segundo escenario sobrescribe el nombre anterior; podría parametrizarse el sufijo del archivo (ej., `piel_carnero_resultado_20cm.png` y `..._25cm.png`).


Conclusiones
- Se implementó un sistema completo que, a partir de una imagen de piel y una calibración simple (5 px/cm), binariza el área aprovechable y aplica un AG para ubicar recortes circulares sin solapamientos y contenidos en la máscara.
- El sistema responde las preguntas del enunciado entregando: (a) el número de recortes, (b) la tabla con coordenadas en píxeles y centímetros y (c) una figura con la solución, para 20 cm y 25 cm de diámetro.
- La metodología es extensible a otros diámetros y, con ajustes menores, a otras formas (elipses, plantillas) y criterios multiobjetivo.


Guía práctica (paso a paso)
1) Coloque `piel_carnero.png` en la raíz del proyecto.
2) Compile y ejecute el proyecto en Visual Studio con OpenCV correctamente configurado.
3) Revise en la consola:
   - “Circulos optimos encontrados: ...”
   - La “TABLA DE COORDENADAS DE CENTROS”.
4) Recupere los archivos generados:
   - `piel_carnero_bin.png`, `piel_carnero_bin.csv`, `piel_carnero_resultado.png`.
5) Para conservar ambas figuras (20 cm y 25 cm):
   - Tras el primer bloque de resultados (20 cm), copie/renombre `piel_carnero_resultado.png` a `piel_carnero_resultado_20cm.png`.
   - Al finalizar el análisis de 25 cm, renombre la nueva figura a `piel_carnero_resultado_25cm.png`.


Anexos

A) Pseudocódigo simplificado de la evaluación de aptitud

- Para cada individuo (conjunto de círculos):
  - fitness = 0
  - Para i en círculos:
    - Si círculo i NO está completamente dentro de la piel ? continuar
    - overlaps = existe j > i tal que dist(i,j) < 2·radio
    - Si overlaps ? fitness -= 0.5
      Si no ? fitness += 1.0

B) Interpretación de la tabla (consola)
- `ID`: índice del círculo (1..N) que coincide con el número dibujado en la figura.
- `X(px)`, `Y(px)`: coordenadas del centro en píxeles.
- `X(cm)`, `Y(cm)`: coordenadas convertidas a centímetros usando `PIXELS_PER_CM`.

C) Estructura del repositorio (relevante)
- `OpenCV_Test/GA_ImCv.cpp`: implementación completa de UI, visión y AG.
- `OpenCV_Test/README.md`: este documento.
- Archivos de salida generados en la raíz (junto a la imagen de entrada):
  - `piel_carnero_bin.png`, `piel_carnero_bin.csv`, `piel_carnero_resultado.png`.

D) Solución de problemas
- No se ve color en consola: asegúrese de ejecutar en consola de Windows moderna; el programa habilita `ENABLE_VIRTUAL_TERMINAL_PROCESSING`.
- No carga la imagen: verifique que `piel_carnero.png` esté en el directorio indicado por el proyecto en tiempo de ejecución.
- Error al cargar DLL de OpenCV: agregue la carpeta `bin` de OpenCV a `PATH` o configure `Debugging > Environment` en Visual Studio.
- Resultados diferentes en cada ejecución: inicialización aleatoria; si desea estabilidad, considere fijar semillas en los generadores.


Referencias
- OpenCV: https://opencv.org/
- Goldberg, D.E. Genetic Algorithms in Search, Optimization and Machine Learning.
- Eiben, A.E., Smith, J.E. Introduction to Evolutionary Computing.
