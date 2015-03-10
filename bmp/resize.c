/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Resizes a BMP and saves it in a new BMP file based on the user's input.
 */
       
#include <stdio.h>
#include <stdlib.h>
 
#include "bmp.h"
 
int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
 
    else if (atoi(argv[1]) < 1 || atoi(argv[1]) > 100)
    {
        printf("n, the resize factor must satisfy 0 < n <= 100.\n");
        return 2;
    }
    
    // remember filenames
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
 
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }
 
    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
 
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
 
    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
 
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
 
    // determine padding for original BMP file
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // edit biWidth of resized BMP file based on user's n (resize factor)
    BITMAPINFOHEADER bi_resized = bi;
    bi_resized.biWidth = bi.biWidth * n;
 
    // determine padding for scanlines in resized BMP file using biWidth after accounting for n (resize factor)
    int padding_aft_resize =  (4 - (bi_resized.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // edit remaining of BITMAPINFOHEADER and BITMAPFILEHEADER of resized BMP file based on user's n (resize factor)
    bi_resized.biHeight = bi.biHeight * n;
    bi_resized.biSizeImage = (bi_resized.biWidth * sizeof(RGBTRIPLE) + padding_aft_resize) * abs(bi.biHeight) * n;
    BITMAPFILEHEADER bf_resized = bf;
    bf_resized.bfSize = bi_resized.biSizeImage + 54;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_resized, sizeof(BITMAPFILEHEADER), 1, outptr);
 
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_resized, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        fpos_t pos;
        fgetpos(inptr, &pos);
        
        // loop resizeFactor times to print vertically
        for (int m = 0; m < n; m++)
        {
        
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                // resize horizontally
                for (int l = 0; l < n; l++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < padding_aft_resize; k++)
            {
                fputc(0x00, outptr);
            }
            
            if (m != n - 1)
            {
                fsetpos(inptr, &pos);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
