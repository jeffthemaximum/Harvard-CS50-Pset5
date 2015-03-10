/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// function to test if first four bytes match pattern for jpeg 
bool isJpeg(uint8_t buffer[])
{
      if((buffer[0] == 255 && buffer[1] == 216 && buffer[2] == 255 && buffer[3] == 224)||
    (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && buffer[3] == 0xe1))
    {
        return true;
    }
    return false;
}

int main(int argc, char* argv[])
{

    // open cf card
    FILE* infile = fopen ("card.raw", "r");

    // define buffer
    uint8_t buffer[512];
    
    // keep track of number of files written
    int number = 0;
    
    // initiate filenames
    char filename[8];
    
    //detect eof
    while (fread(&buffer, 512, 1, infile) != 0)
    {

        //detect start of new jpg
        while(isJpeg(buffer))
        {
            //create filename
            sprintf(filename, "%.3d.jpg", number);
            
            // open output file
            FILE* outfile = fopen(filename, "w");

            fwrite(&buffer, 512, 1, outfile);
            
            if(fread(&buffer, 512, 1, infile) == 0)
                break;
            
            // write new file until newer file is found
            while (!isJpeg(buffer))
            {
                //fread(&buffer, 512, 1, infile);
                fwrite(&buffer, 512, 1, outfile);
                if(fread(&buffer, 512, 1, infile) == 0)
                 break;
            }
            
            //increase file
            number++; 
            
            //close outfile
            fclose(outfile);
        }
    }
    //close infole
    fclose(infile);
}
