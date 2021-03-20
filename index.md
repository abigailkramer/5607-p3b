## Project Overview

This part of the project went pretty well, since I was able to achieve a decent bit of it for project 3a. Refraction gave me a little bit of trouble, in terms of getting the right ray. I was initially failing to take into account whether or not the ray was leaving or entering an object in the image. Once that was sorted out, I just needed to clean up my formula (a square root was forgotten), and I was all set. After that, the rayTriangleIntersection function was fairly simple, although it did take me a bit to figure out interpolating the normals. Parallelizing the raytracer took a little bit of work, mainly just a bit of research to figure out how OpenMP was actually used. It seems to work quite well. Rendering the images on my computer is *very* slow in general (especially for the new example text files), so the difference to me was drastic. 


## Additional Features
- Parallelize the raytracer
- Spot lights


## Examples

![outdoor](https://user-images.githubusercontent.com/59031606/111701404-ca4b9900-8808-11eb-9473-c7c116d83001.png)

**Parallelization**
- w/o parallelization - 21246.26 ms
- schedule(dynamic,4) - 5575.60 ms
- schedule(dynamic) - 6830.64 ms
- schedule(dynamic,8) - 5815.12 ms

![shadow](https://user-images.githubusercontent.com/59031606/111701440-d9cae200-8808-11eb-9305-ecbe9058d4a7.png)


**Parallelization**
- w/o parallelization - rendering never finished (was taking ridiculously long - had to Ctrl+C)
- schedule(dynamic,4) - rendering took 1533105.28 ms
- schedule(dynamic,5) - rendering took 1464798.72 ms

![plant](https://user-images.githubusercontent.com/59031606/111701410-cd468980-8808-11eb-9ed7-995c95e63169.png)

![gear](https://user-images.githubusercontent.com/59031606/111848459-0f3f0080-88d9-11eb-9062-52492f79a16e.png)

**Artistic Image**

![test](https://user-images.githubusercontent.com/59031606/111883922-2f7bc780-898c-11eb-82be-3f0fadb6a5d3.png)

![foo](https://user-images.githubusercontent.com/59031606/111848461-1239f100-88d9-11eb-8420-20e21038606a.png)

![bottle](https://user-images.githubusercontent.com/59031606/111848465-149c4b00-88d9-11eb-9ac6-ba65f54446c7.png)
