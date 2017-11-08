/*
 ============================================================================
 Name        : histogram_equalization.c
 Author      : M.h.sepanj
 Version     : 1.0
 Copyright   : Open
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct pixel {

	unsigned char blue ;
	unsigned char green ;
	unsigned char red ;
	int position ;
};

typedef struct pixel pixel;

#define MAX_PIXELS 20000000


pixel* all_pixels [MAX_PIXELS] ;
unsigned char all_gray_pixels[MAX_PIXELS] ;

void make_pixel(FILE *p,int i){
    unsigned char s[3];
    pixel* newpix=(pixel*)malloc(sizeof(pixel));
    newpix->position=ftell(p);
    fread(s,1,3,p);
    newpix->blue=s[0];
    newpix->green=s[1];
    newpix->red=s[2];
    all_pixels[i]=newpix;
}

int basechannel(unsigned char s[],int b,int j){
    unsigned char c;
    int i;
    unsigned int n=0;
    for(i=0;i<j;i++){
        c = s[i];
        n += c*pow((float)b,i);
    }
    return n;
}


int main() {

	int height , bpp , width , data_start , header ,i,j ,  pixels_size = 0 ;
	int scale[256]  , histogram[256] , cumulative_histogram[256] , final_result[256];
//	char *file_name = argv[1];
	char *file_name = "e:\\1.bmp" , c[54] ;
	unsigned char zero ;
	unsigned char s[4];
	float alpha , intensity_probability[256] , equlized_histogram[256] ; 
	
	FILE* o = fopen("e:\\2.bmp","wb");
	FILE* f = fopen(file_name,"rb");
	
	if (f==NULL)	return -1 ;
		
	
	fread(s,1,2,f);
	if(s[0]!='B' || s[1]!='M'){
		printf("This is not bitmap file!\n");
	    return -1;
	}
	fseek(f,8,SEEK_CUR);
	fread(s,1,4,f);
	data_start = basechannel(s,256,4);
	fread(s,1,4,f);
	header = basechannel(s,256,4);
	if(header!=40){
		printf("Unknown header!\n");
		return -1;
	}
	fread(s,1,4,f);
	width = basechannel(s,256,4);
	fread(s,1,4,f);
	height=basechannel(s,256,4);
	if(width*height<11){
		printf("There is no data in this picture!\n");
		return -1;
	}
	fseek(f,2,SEEK_CUR);
	fread(s,1,2,f);
	bpp =basechannel(s,256,2);
	if(bpp!=24){
		printf("This is not a 24 bpp bitmap!\n");
		return -1;
	}
	fseek(f,data_start,SEEK_SET);
	for(i=0;i<width*height;i++){
		if(i%width==0 && i!=0){
			j=(width*3)%4;
			if(j==0)
				j=4;
	            j=4-j;
	            fseek(f,j,SEEK_CUR);
	        }
	        make_pixel(f,i);
			pixels_size ++ ;
	}
	/*
	for (i = 0; i < height*width; ++i) {
		printf("%d ",all_pixels[i]->blue);
	}
	*/
/*
	 fclose(f);  

	 f = fopen(file_name,"rb");

	 fread(c,1,54,f);
	 fwrite(c,1,54,o);
	 */
	  /*

	  for (i = 0; i < pixels_size ; i++)
	  {
		  fwrite(&all_pixels[i]->blue,1,1,o);
		  fwrite(&all_pixels[i]->green,1,1,o);
		  fwrite(&all_pixels[i]->red,1,1,o);
	  }
	  */
	  for (i = 0; i < pixels_size  ; i++){
		  all_gray_pixels[i] = ((double) 0.2989 * all_pixels[i]->red + (double)0.5870 * all_pixels[i]->green + (double)0.1140 * all_pixels[i]->blue );
		//  fwrite(&all_gray_pixels[i],1,1,o);fwrite(&all_gray_pixels[i],1,1,o);fwrite(&all_gray_pixels[i],1,1,o);
		  //printf("%.2f ",all_gray_pixels[i]);
	  }
	  
	  
	 for(i=0 ; i< 256 ; ++i)
			  histogram[i] = 0 ;

	 for ( i = 0;  i < pixels_size ;  i++){
		 histogram[(int)all_gray_pixels[i]]++ ;
	 }
	 /*
	 for(i=0 ; i<256 ; i++){
		 printf("%d: %d -",i,histogram[i]) ;
	 }
	 */
	 alpha = 255.0 / (float)pixels_size ;
	 //printf("alpha %.5f -",alpha) ;
	
	 for(i = 0; i < 256; i++)
		intensity_probability[i] = (double)histogram[i] / pixels_size;
    
	 cumulative_histogram[0] = histogram[0];
	 printf("%d \n",histogram[0]) ;
    for(i = 1; i < 256; i++){
		cumulative_histogram[i] = histogram[i] + cumulative_histogram[i-1];
		printf("%d -",cumulative_histogram[i]) ;
	}
	printf("\n") ;
	 for(i = 0; i < 256; i++){
		 scale[i] = (double)cumulative_histogram[i] * alpha;
		//printf("%.5f ",scale[i]);
	}
	 getchar();
	 for(i = 0; i < 256; i++)
		 equlized_histogram[i] = 0;


    for(i = 0; i < 256; i++)
		equlized_histogram[scale[i]] += intensity_probability[i];
    
	printf("___________________________________\n") ;
	
    for(i = 0; i < 256; i++){
		final_result[i] = floor(equlized_histogram[i]*255);
	}
	
	fclose(f);  

	 f = fopen(file_name,"rb");

	 fread(c,1,54,f);
	 fwrite(c,1,54,o);

	  /*

	  for (i = 0; i < pixels_size ; i++)
	  {
		  fwrite(&all_pixels[i]->blue,1,1,o);
		  fwrite(&all_pixels[i]->green,1,1,o);
		  fwrite(&all_pixels[i]->red,1,1,o);
	  }
	  */
	 for (i = 0; i < pixels_size  ; i++){
		 
		 fwrite(&scale[all_gray_pixels[i]],1,1,o);fwrite(&scale[all_gray_pixels[i]],1,1,o);fwrite(&scale[all_gray_pixels[i]],1,1,o);
		  
	  }

	return 0;
}

