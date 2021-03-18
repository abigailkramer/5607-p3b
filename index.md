## Project Overview

This part of the project went pretty well, since I was able to achieve a decent bit of it for project 3a. Refraction gave me a little bit of trouble, in terms of getting the right ray. I was initially failing to take into account whether or not the ray was leaving or entering an object in the image. Once that was sorted out, I just needed to clean up my formula (a square root was forgotten), and I was all set. After that, the rayTriangleIntersection function was fairly simple, although it did take me a bit to figure out interpolating the normals. Parallelizing the raytracer took a little bit of work, mainly just a bit of research to figure out how OpenMP was actually used. It seems to work quite well. Rendering the images on my computer is *very* slow in general (especially for the new example text files), so the difference to me was drastic. 


## Additional Features
- Parallelize the raytracer
- Spot lights


## Examples

![outdoor](https://user-images.githubusercontent.com/59031606/111701404-ca4b9900-8808-11eb-9473-c7c116d83001.png)

![shadow](https://user-images.githubusercontent.com/59031606/111701440-d9cae200-8808-11eb-9305-ecbe9058d4a7.png)


**Parallelization**
- w/o parallelization - rendering never finished (was taking ridiculously long - had to Ctrl+C)
- schedule(dynamic,5) - rendering took 1464798.72 ms
- schedule(dynamic,8) - rendering took 

![plant](https://user-images.githubusercontent.com/59031606/111701410-cd468980-8808-11eb-9ed7-995c95e63169.png)
