# Fractals
Parallel graphical fractal generation

The file main.c contains the parallel version of the set calculation
Mandelbrot and Julia and their display in the form of greyscale images.

Since the sequential algorithm was used as the skeleton in this README
it will only be about the logic of parallelizing it using Pthreads.

In the main () function, after the input reading and allocation operations
the space required for the result matrices is created by a loop
for (), a number of threads equal to P (received as a parameter at run).

Each of these threads will deal with the calculation of a specific area
resulting matrices, so that iteration through the integer is not required
value space on a single thread. The logic of dividing these areas
is as follows: Each thread calculates for the parameter matrix
width and height, a portion of the height in which it will calculate complex values.

The start, in each thread, is represented by the thread id or length
the height portion of a thread. This portion length is calculated
as the total height divided by the number of thears (P). So,
we make sure that each thread will have an equal (or as equal) portion as possible
to work, maximizing the efficiency and scalability of the program. The end of a
theard (last line of processed height) is before the start of the thread
following.

The parameter calculations are performed within the two for () loops taken over
from the sequential version of the algorithm, specifying that in the intended forum
iterations on the height space, each thread will only iterate on its portion.
Thus, it will complete a specific portion of the resulting matrix.

At the end of these calculations a barrier of type pthread_barrier_wait () is placed,
which ensures that the resulting matrix is ​​complete before calculating
screen coordinates. Transforming into screen coordinates is also
parallelized, but since the conversion is a mirror operation on the OX axis,
iteration is required for only half of the height space. So, the last one
for () will work on half of the initial start and end intervals, ie
on start / 2 and end / 2. The same logic of operations applies to both
of the Julia set as well as the calculation of the Mandelbrot set.

After completing the transformation of the Mandelbrot coordinates, it exits the therads and
it returns to main (), where the process of writing the results in files takes place.

Allocated time: Around 4-5 hours of parallelization and tests + 1 hour of command / README.
Thank you for taking the time to complete this README.
