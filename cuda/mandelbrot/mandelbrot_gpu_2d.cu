#define NO_FREETYPE 
#include <cuda_runtime_api.h>
#include "Mandelbrot.h"
#include <math.h>
#include <chrono>
#include <iostream> 
#include <pngwriter.h>
using namespace std;
#include <sys/time.h>


int dimX, dimY;

int main(int argc, char **argv) {
    struct timeval T0, T1;
    time_t start, end;
    struct timeval t0, t1;

    gettimeofday(&T0,NULL);
    if (argc!=10) {
        printf("Wywołanie %s LD_Re, LD_Im, PG_Re, PG_Im, Poziom, Pion, Iteracje dimX(kernel block) dimY(kernel block)\n ",argv[0]);
	    exit(1);
    }
    //Ustaw obszar obliczeń {X0,Y0} - lewy dolny róg
    double X0=atof(argv[1]);    double Y0=atof(argv[2]);
    //{X1,Y1} - prawy górny róg
    double X1=atof(argv[3]);    double Y1=atof(argv[4]);
    //Ustal rozmiar w pikselach {POZ,PION} 
    int POZ=atoi(argv[5]); int PION=atoi(argv[6]);
    //Ustal liczbę iteracji próbkowania {ITER}
    int ITER=atoi(argv[7]);

    dimX = atoi(argv[8]);
    dimY = atoi(argv[9]);

    printf("Mandelbrot_gpu_2d: dim(%d, %d) ", dimX, dimY);

    //Zaalokuj tablicę do przechowywania wyniku
    int *Iters = (int*) malloc(sizeof(int)*POZ*PION);
    //Zaalokuj tablicę do przechowywania sum kontrolnych 
    unsigned int *RowSums  = (unsigned int*) malloc(sizeof(unsigned int)*PION);
   
    //printf("Computations for rectangle { (%lf %lf), (%lf %lf) }\n",X0,Y0,X1,Y1);
    start=clock();
    auto start2 = chrono::steady_clock::now(); 
    gettimeofday(&t0,NULL);
    int SUM = computeMandelbrot(X0,Y0,X1,Y1,POZ,PION,ITER,Iters);
    gettimeofday(&t1,NULL);
    auto stop = chrono::steady_clock::now();
    end=clock();
    auto diff = stop - start2;

    //cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    //cout << chrono::duration <double, micro> (diff).count() << " us" << endl;
    //cout << chrono::duration <double, nano> (diff).count() << " ns" << endl;
    printf("T - %lf s\n",1.0*(end-start)/CLOCKS_PER_SEC);
    //printf("Elapsed time %12.6lf s\n\n",(t1.tv_sec-t0.tv_sec)+1e-6*(t1.tv_usec-t0.tv_usec));

    start=clock();
    //makePicture(Iters, POZ, PION, ITER);
    end=clock();
    //printf("Saving PPM took %lf s\n\n",1.0*(end-start)/CLOCKS_PER_SEC);
    start=clock();
    //makePicturePNG(Iters, POZ, PION, ITER);    end=clock();
    //printf("Saving PNG took %lf s\n\n",1.0*(end-start)/CLOCKS_PER_SEC);    
    gettimeofday(&T1,NULL);
    //printf("Total time %12.6lf s\n\n",(T1.tv_sec-T0.tv_sec)+1e-6*(T1.tv_usec-T0.tv_usec));

}

__global__ void kernel(real X0, real Y0, real X1, real Y1, int POZ, int PION, int ITER,int *Mandel){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    if(i >= PION || j >= POZ)
        return;
    double dX=(X1-X0)/(POZ-1);
    double dY=(Y1-Y0)/(PION-1);
    double x,y,Zx,Zy, temp;
    int k;
    Zx = Zy = 0;
    x = X0 + i * dX;
    y = Y0 + j * dY;
    k = 0;
    for(k = 0; k < ITER; k++){
        temp = Zx*Zx - Zy*Zy + x;
        Zy = 2*Zx*Zy + y;
        Zx = temp;

        if(Zy*Zy + Zx*Zx >= 2)
            break;
    }
    Mandel[j * PION + i] = k;
}

int computeMandelbrot(real X0, real Y0, real X1, real Y1, int POZ, int PION, int ITER,int *Mandel ){
    int SUM=0;
    int SIZE = PION * POZ;
    
    int* arr_gpu;
    cudaError_t status;
    status = cudaMalloc((void**)&arr_gpu, sizeof(int) * SIZE);
    if(status != cudaSuccess){
        cout << cudaGetErrorString(status) << endl;
        return SUM;
    }
    
    dim3 threadsPerBlock(dimX, dimY,1);
    dim3 numBlocks(POZ / dimX + 1, PION / dimY + 1, 1);

    kernel<<<numBlocks, threadsPerBlock, 0>>>(X0, Y0, X1, Y1, POZ, PION, ITER, arr_gpu);
    status = cudaMemcpy(Mandel, arr_gpu, sizeof(int) * SIZE, cudaMemcpyDeviceToHost);
    if(status != cudaSuccess){
        cout << cudaGetErrorString(status) << endl;
        return SUM;
    }

    return SUM;
}

void makePicture(int *Mandel,int width, int height, int MAX){
    
    int red_value, green_value, blue_value;
    
    float scale = 256.0/MAX;
    
    int MyPalette[41][3]={
        {255,255,255}, //0
        {255,255,255}, //1 not used
        {255,255,255}, //2 not used
        {255,255,255}, //3 not used
        {255,255,255}, //4 not used
        {255,180,255}, //5
        {255,180,255}, //6 not used
        {255,180,255}, //7 not used
        {248,128,240}, //8
        {248,128,240}, //9 not used
        {240,64,224}, //10
        {240,64,224}, //11 not used
        {232,32,208}, //12
        {224,16,192}, //13
        {216,8,176}, //14
        {208,4,160}, //15
        {200,2,144}, //16
        {192,1,128}, //17
        {184,0,112}, //18
        {176,0,96}, //19
        {168,0,80}, //20
        {160,0,64}, //21
        {152,0,48}, //22
        {144,0,32}, //23
        {136,0,16}, //24
        {128,0,0}, //25
        {120,16,0}, //26
        {112,32,0}, //27
        {104,48,0}, //28
        {96,64,0}, //29
        {88,80,0}, //30
        {80,96,0}, //31
        {72,112,0}, //32
        {64,128,0}, //33
        {56,144,0}, //34
        {48,160,0}, //35
        {40,176,0}, //36
        {32,192,0}, //37
        {16,224,0}, //38
        {8,240,0}, //39
        {0,0,0} //40
    };
    
    FILE *f = fopen("Mandel.ppm", "wb");
    fprintf(f, "P6\n%i %i 255\n", width, height);
    for (int j=height-1; j>=0; j--) {
        for (int i=0; i<width; i++) {
            // compute index to the palette
            int indx= (int) floor(5.0*scale*log2f(1.0f*Mandel[j*width+i]+1));
            red_value=MyPalette[indx][0];
            green_value=MyPalette[indx][2];
            blue_value=MyPalette[indx][1];
            
            fputc(red_value, f);   // 0 .. 255
            fputc(green_value, f); // 0 .. 255
            fputc(blue_value, f);  // 0 .. 255
        }
    }
    fclose(f);
    
}


void makePictureInt(int *Mandel,int width, int height, int MAX){
    
    float scale = 255.0/MAX;
    
    int red_value, green_value, blue_value;

    
    int MyPalette[35][3]={
        {255,0,255},
        {248,0,240},
        {240,0,224},
        {232,0,208},
        {224,0,192},
        {216,0,176},
        {208,0,160},
        {200,0,144},
        {192,0,128},
        {184,0,112},
        {176,0,96},
        {168,0,80},
        {160,0,64},
        {152,0,48},
        {144,0,32},
        {136,0,16},
        {128,0,0},
        {120,16,0},
        {112,32,0},
        {104,48,0},
        {96,64,0},
        {88,80,0},
        {80,96,0},
        {72,112,0},
        {64,128,0},
        {56,144,0},
        {48,160,0},
        {40,176,0},
        {32,192,0},
        {16,224,0},
        {8,240,0},
        {0,0,0}
    };
    
    FILE *f = fopen("Mandel.ppm", "wb");
    
    fprintf(f, "P3\n%i %i 255\n", width, height);
    printf("MAX = %d, scale %lf\n",MAX,scale);
    for (int j=height-1; j>=0; j--) {
        for (int i=0; i<width; i++)
        {
            //if ( ((i%4)==0) && ((j%4)==0) ) printf("%d ",Mandel[j*width+i]);
            //red_value = (int) round(scale*(Mandel[j*width+i])/16);
            //green_value = (int) round(scale*(Mandel[j*width+i])/16);
            //blue_value = (int) round(scale*(Mandel[j*width+i])/16);
            int indx= (int) round(4*scale*log2(Mandel[j*width+i]+1));
            red_value=MyPalette[indx][0];
            green_value=MyPalette[indx][2];
            blue_value=MyPalette[indx][1];
            
            fprintf(f,"%d ",red_value);   // 0 .. 255
            fprintf(f,"%d ",green_value); // 0 .. 255
            fprintf(f,"%d ",blue_value);  // 0 .. 255
        }
        fprintf(f,"\n");
        //if ( (j%4)==0)  printf("\n");

    }
    fclose(f);    
}

void makePicturePNG(int *Mandel,int width, int height, int MAX){
    double red_value, green_value, blue_value;
    float scale = 256.0/MAX;
    double MyPalette[41][3]={
        {1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0},// 0, 1, 2, 3, 
        {1.0,1.0,1.0},{1.0,0.7,1.0},{1.0,0.7,1.0},{1.0,0.7,1.0},// 4, 5, 6, 7,
        {0.97,0.5,0.94},{0.97,0.5,0.94},{0.94,0.25,0.88},{0.94,0.25,0.88},//8, 9, 10, 11,
        {0.91,0.12,0.81},{0.88,0.06,0.75},{0.85,0.03,0.69},{0.82,0.015,0.63},//12, 13, 14, 15, 
        {0.78,0.008,0.56},{0.75,0.004,0.50},{0.72,0.0,0.44},{0.69,0.0,0.37},//16, 17, 18, 19,
        {0.66,0.0,0.31},{0.63,0.0,0.25},{0.60,0.0,0.19},{0.56,0.0,0.13},//20, 21, 22, 23,
        {0.53,0.0,0.06},{0.5,0.0,0.0},{0.47,0.06,0.0},{0.44,0.12,0},//24, 25, 26, 27, 
        {0.41,0.18,0.0},{0.38,0.25,0.0},{0.35,0.31,0.0},{0.31,0.38,0.0},//28, 29, 30, 31,
        {0.28,0.44,0.0},{0.25,0.50,0.0},{0.22,0.56,0.0},{0.19,0.63,0.0},//32, 33, 34, 35,
        {0.16,0.69,0.0},{0.13,0.75,0.0},{0.06,0.88,0.0},{0.03,0.94,0.0},//36, 37, 38, 39,
        {0.0,0.0,0.0}//40
        };

    pngwriter png(width,height,1.0,"Mandelbrot.png");   
    for (int j=height-1; j>=0; j--) {
        for (int i=0; i<width; i++) {
            // compute index to the palette
            int indx= (int) floor(5.0*scale*log2f(1.0f*Mandel[j*width+i]+1));
            red_value=MyPalette[indx][0];
            green_value=MyPalette[indx][2];
            blue_value=MyPalette[indx][1];
            png.plot(i,j, red_value, green_value, blue_value);            
        }
    }
    png.close();
}