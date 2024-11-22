#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"  // This already defines Pnm_rgb

double compute_rms_difference(Pnm_ppm image1, Pnm_ppm image2) {
    int w = (image1->width < image2->width) ? image1->width : image2->width;
    int h = (image1->height < image2->height) ? image1->height : image2->height;
    
    double sum = 0.0;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            Pnm_rgb pixel1 = image1->methods->at(image1->pixels,i,j);
            Pnm_rgb pixel2 =  image2->methods->at(image1->pixels,i,j);
            
            double r_diff = pixel1->red - pixel2->red;
            double g_diff = pixel1->green - pixel2->green;
            double b_diff = pixel1->blue - pixel2->blue;
            
            sum += (r_diff * r_diff + g_diff * g_diff + b_diff * b_diff);
        }
    }
    
    return sqrt(sum / (3 * w * h));
}

int main(int argc, char *argv[]) {
    assert(argc == 3);
    
    FILE* file1 = fopen(argv[1],"r");
    FILE* file2 = fopen(argv[2],"r");

    Pnm_ppm image1 = Pnm_ppmread(file1,uarray2_methods_plain);
    Pnm_ppm image2 = Pnm_ppmread(file2,uarray2_methods_plain);

    double rms = compute_rms_difference(image1, image2);
    printf("%.4f\n", rms);

    Pnm_ppmfree(&image1);
    Pnm_ppmfree(&image2);

    return 0;
}