# Fractals
Parallel graphical fractal generation

Fisierul tema1_par.c contine varianta paralelizata a calculului multimilor
Mandelbrot si Julia si a afisarii lor sub forma de imagini greyscale.

Intrucat algoritmul secvential a fost folosit drept schelet, in acest README
va fi vorba doar despre logica paralelizarii acestuia folosind Pthreads.

In cadrul functiei main(), dupa operatiile de citire a inputului si alocarea
spatiului necesar pentru matricile rezultatelor, se creeaza, printr-o bucla
de tip for(), un numar de threaduri egal cu P (primit ca parametru la rulare).

Fiecare dintre aceste threaduri se va ocupa de calculul unei zone specifce a
matricelor rezultat, astfel incat nu este necesara iterarea prin intregul
spatiu de valori pe un singur fir de executie. Logica impartirii acestor zone
este urmatoarea: Fiecare thread calculeaza, pentru matricea cu parametrii
width si height, o portiune a inaltimii in care va calcula valorile complexe.

Start-ul, in fiecare thread, este reprezentat de id-ul theardului ori lungimea
portiunii de inaltime a unui thread. Aceasta lungime de portiune este calculata
drept inaltimea totala (height) impartita la numarul de thearduri (P). Astfel,
ne asiguram ca fiecare thread va avea o portiune egala (sau cat mai egala) pe
care sa lucreze, maximizand eficienta si scalabilitatea programului. Endul unui
theard (ultima linie din inaltime procesata) este inaintea startului threadului
urmator.

Calculele parametrilor se realizeaza in cadrul celor doua bucle for() preluate
din varianta secventiala a algoritmului, cu precizarea ca in forul destinat
iterarii pe spatiul de inaltime, fiecare thread va itera doar pe portiunea lui.
Astfel, el va completa o portiune specifica din matricea rezultat.

La finalul acestor calcule este pusa o bariera de tipul pthread_barrier_wait(),
prin care se asigura faptul ca matricea rezultat este completa inaintea calcu-
lului coordonatelor ecran. Transformarea in coordonate ecran este, de asemena,
paralelizata, dar intrucat conversia reprezinta o operatie de mirror pe axa OX,
este necesara iterarea doar pe jumatate din spatiul inaltimii. Deci, ultimul
for() va functiona pe jumatate din intervalele de start si end initiale, adica
pe start/2 si end/2. Aceleeasi logica de operatii de aplica atat asupra calcu-
lului multimii Julia cat si a calculului multimii Mandelbrot.

Dupa incheierea trasformarii coordonatelor Mandelbrot, se iese din theraduri si
se revine in main(), unde are loc procesul de scriere in fisere a rezultatelor.

Timp alocat: In jur de 4-5 ore paralelizare si teste + 1 ora comenatrii/README.
Multumesc pentru timpul aocrdat parcurgerii acestui README.
