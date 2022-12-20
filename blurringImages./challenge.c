#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void getGrayArr(unsigned char * arr, unsigned char *grayArr, int size)
{
	for (int i = 0, j = 0; i < size; i+=3, j++)
	{
		int intSum = (int) ((arr[i] + arr[i+1] + arr[i+2])/3);
		*(grayArr + j) = (unsigned char) intSum;
	}

}

void reshape1Dto2D(unsigned char ** arr2D, unsigned char * grayArr, int h, int w)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++, grayArr++)
			arr2D[i][j] = *grayArr;	
	}
}

// get dimmesions of zero-pad array
void getZeroPadDimesion(int * paddedRows, int * paddedCols, int h, int w, int filter_size)
{
	*paddedRows = h + filter_size - 1;
	*paddedCols = w + filter_size -1;
}

// copy original array to zero padded array
void copyToPaddedArr(int index, int h, int w, unsigned char ** paddedArr2D, unsigned char ** arr2D)
{
	for (int i = index, k = 0; k < h; i++, k++)
	{
		for(int j = index, l = 0; l < w; j++, l++)
			paddedArr2D[i][j] = arr2D[k][l];
	}
}

int getSum (unsigned char ** arr2D, int r, int c, int filter_size, int index)
{
	/*
	c1, r1 = [r-1][c-1]
	c2, r1 = [r-1][c]
	c3, r1 = [r-1][c+1]

	c1, r2 = [r][c-1]
	c2, r2 = [r][c]
	c3, r2 = [r][c+1]

	c1, r3 = [r+1][c-1]
	c2, r3 = [r+1][c]
	c3, r3 = [r+1][c+1]
	*/
	int limRow = r + index; 
	int limCol = c + index;
	int sum = 0;
	for (int i = r-index; i <= limRow; i++)
	{
		for (int j = c-index; j <= limCol; j++)
		{
			sum += (int)(arr2D[i][j]);
		}
	}

	int tot = sum/(filter_size*filter_size);

	return ((unsigned char) (tot));
}

void convolution(int index, int rows, int cols, unsigned char ** arr2D, unsigned char ** convolutionArr, int filter_size)
{
	int limRow = rows+index;
	int limCol = cols+index;
	for (int i = index; i < limRow; i++)
	{
		for (int j = index; j < limCol; j++)
		{
			convolutionArr[i-index][j-index] = getSum(arr2D, i, j, filter_size, index);
		}
	}
}

void reshape2Dto1D(unsigned char ** arr2D, unsigned char *arr1D, int h, int w)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			arr1D[i * w + j] = arr2D[i][j];
			//printf("%i %i %i\n", arr1D[i * w + j], arr1D[i * w + j + 1], arr1D[i * w + j + 2]);
			
		}

	}
}

void repeat3Times(unsigned char * from, unsigned char * to, int channelSize)
{
	for (int i = 0; i < channelSize; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			to[i*3 +j] = from[i];
		}
	}

}


int main(){
	int w, h, c;
	char filename[200]; 
	int filter_size;
	
	printf("Insert file name: ");
	/*fill in the path to where you save the input image WHEN running your code.
	For mac You can get it by using right click->option and look for Copy image as pathname */
	
	//C:\cygwin64\home\Daira\ECE131L\Challenge\tree.jpg
	scanf("%s", filename); 
	
	printf("Insert size of the filter: ");
	scanf("\n%i", &filter_size);

	unsigned char *arr = stbi_load(filename, &w, &h, &c, 0);
	
	int size = w*h*c;
	int channelSize = w*h; //this will be the size for your 1D grayscale and 3 channels
						   // your 2D array would have h rows and w columns

	// see the width, heigth and channels
	printf("w:%d h:%d c:%d\n", w, h, c);

	// 1. Allocate 1D array to save grayscale values
	unsigned char *grayArr = malloc(channelSize * sizeof(unsigned char));

		// Calculate grayscale values and save them
	getGrayArr(arr, grayArr, size);
	
	// 2. Allocate 2D dynamic array
	unsigned char **arr2D = malloc(h * sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		arr2D[i] = malloc(w * sizeof(unsigned char));

	// 3. Reshape 1D to 2D grayscale
	reshape1Dto2D(arr2D, grayArr, h, w);

	// 4. zero-pad the image
	int paddedHeigth, paddedWidth;

		// Allocate memory for array with padded size
	getZeroPadDimesion(&paddedHeigth, &paddedWidth, h, w , filter_size);
	unsigned char **paddedArr2D = calloc(paddedHeigth, sizeof(unsigned char*));
	for (int i = 0; i < paddedHeigth; i++)
		paddedArr2D[i] = calloc(paddedWidth, sizeof(unsigned char));

		// Copy over the original image
	int index = ((filter_size + 1)/2)-1;
	copyToPaddedArr(index, h, w, paddedArr2D, arr2D);

	// 5. Perform convolution
	unsigned char **convolutionArr = calloc(h, sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		convolutionArr[i] = calloc(w, sizeof(int));

	convolution(index, h, w, paddedArr2D, convolutionArr, filter_size);

	// 6. Reshape back to 1D
	unsigned char *arr1D = malloc(channelSize * sizeof(unsigned char));
	reshape2Dto1D(convolutionArr, arr1D, h, w);

		// copy 3 times value from 1D
	unsigned char *blurArr = malloc(size * sizeof(unsigned char));
	repeat3Times(arr1D, blurArr, channelSize);


	char outFile [] = "C:/cygwin64/home/Daira/ECE131L/Challenge/tree_blur_gray.jpg"; //Replace ?? with the path where you want to save your output 
					// Ex: char outFile[] = "/Users/phuongtran/Documents/.../outImg.jpg";
	
	stbi_write_jpg(outFile, w, h, c, blurArr, w * c);

	// de-allocate memory
	free(arr);
	free(grayArr);
	free(arr2D);
	free(paddedArr2D);
	free(convolutionArr);
	free(arr1D);
	free(blurArr);
	return 0;
}