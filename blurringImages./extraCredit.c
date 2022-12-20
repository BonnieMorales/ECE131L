#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// separate in three different arrays (channels RGB)
void getRGBarr(unsigned char *arr, unsigned char *arrR, unsigned char *arrG, unsigned char *arrB, int size)
{
	for (int i = 0; i < size; i+=3, arrR++, arrG++, arrB++)
	{
		*arrR = arr[i];
		*arrG = arr[i+1];
		*arrB = arr[i+2];
	}
}

void reshape1Dto2D(unsigned char ** arr2D, unsigned char * arr1D, int h, int w)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++, arr1D++)
			arr2D[i][j] = *arr1D;	
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

void getRGBfinalArr(unsigned char * from1, unsigned char * from2, unsigned char * from3, unsigned char * to, int size)
{
	for (int i = 0; i < size; i+=3, from1++, from2++, from3++)
	{
		to[i] = *from1;
		to[i+1] = *from2;
		to[i+2] = *from3;
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

	// 1. Separate RGB channels in three separate arrays
	unsigned char *arrR = malloc(channelSize * sizeof(unsigned char));
	unsigned char *arrG = malloc(channelSize * sizeof(unsigned char));
	unsigned char *arrB = malloc(channelSize * sizeof(unsigned char));
	getRGBarr(arr, arrR, arrG, arrB, size);
	
	// 2. Allocate 2D dynamic array for each channel RGB
	unsigned char **arr2DR = malloc(h * sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		arr2DR[i] = malloc(w * sizeof(unsigned char));

	unsigned char **arr2DG = malloc(h * sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		arr2DG[i] = malloc(w * sizeof(unsigned char));

	unsigned char **arr2DB = malloc(h * sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		arr2DB[i] = malloc(w * sizeof(unsigned char));

	// 3. Reshape 1D to 2D for each channel RGB
	reshape1Dto2D(arr2DR, arrR, h, w);
	reshape1Dto2D(arr2DG, arrG, h, w);
	reshape1Dto2D(arr2DB, arrB, h, w);

	// 4. zero-pad the image for each channel RGB
	int paddedHeigth, paddedWidth;

		// Allocate memory for array with padded size for each channel
	getZeroPadDimesion(&paddedHeigth, &paddedWidth, h, w , filter_size);
	
	unsigned char **paddedArr2DR = calloc(paddedHeigth, sizeof(unsigned char*));
	for (int i = 0; i < paddedHeigth; i++)
		paddedArr2DR[i] = calloc(paddedWidth, sizeof(unsigned char));

	unsigned char **paddedArr2DG = calloc(paddedHeigth, sizeof(unsigned char*));
	for (int i = 0; i < paddedHeigth; i++)
		paddedArr2DG[i] = calloc(paddedWidth, sizeof(unsigned char));

	unsigned char **paddedArr2DB = calloc(paddedHeigth, sizeof(unsigned char*));
	for (int i = 0; i < paddedHeigth; i++)
		paddedArr2DB[i] = calloc(paddedWidth, sizeof(unsigned char));

		// Copy over the original image
	int index = ((filter_size + 1)/2)-1; // index is the radius of the filter

	copyToPaddedArr(index, h, w, paddedArr2DR, arr2DR);
	copyToPaddedArr(index, h, w, paddedArr2DG, arr2DG);
	copyToPaddedArr(index, h, w, paddedArr2DB, arr2DB);


	// 5. Perform convolution in each channel RGB
	unsigned char **convolutionArrR = calloc(h, sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		convolutionArrR[i] = calloc(w, sizeof(int));

	unsigned char **convolutionArrG = calloc(h, sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		convolutionArrG[i] = calloc(w, sizeof(int));

	unsigned char **convolutionArrB = calloc(h, sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		convolutionArrB[i] = calloc(w, sizeof(int));

	convolution(index, h, w, paddedArr2DR, convolutionArrR, filter_size);
	convolution(index, h, w, paddedArr2DG, convolutionArrG, filter_size);
	convolution(index, h, w, paddedArr2DB, convolutionArrB, filter_size);

	// 6. Reshape back to 1D each channel
	unsigned char *arr1DR = malloc(channelSize * sizeof(unsigned char));
	reshape2Dto1D(convolutionArrR, arr1DR, h, w);

	unsigned char *arr1DG = malloc(channelSize * sizeof(unsigned char));
	reshape2Dto1D(convolutionArrG, arr1DG, h, w);

	unsigned char *arr1DB = malloc(channelSize * sizeof(unsigned char));
	reshape2Dto1D(convolutionArrB, arr1DB, h, w);

	// 7. get final array RGB
	unsigned char *blurArrRGB = malloc(size * sizeof(unsigned char));
	getRGBfinalArr(arr1DR, arr1DG, arr1DB, blurArrRGB, size);


	char outFile [] = "C:/cygwin64/home/Daira/ECE131L/Challenge/tree_blur.jpg"; //Replace ?? with the path where you want to save your output 
					// Ex: char outFile[] = "/Users/phuongtran/Documents/.../outImg.jpg";
	
	stbi_write_jpg(outFile, w, h, c, blurArrRGB, w * c);

	// de-allocate memory
	free(arr);
	free(arrR);
	free(arrG);
	free(arrB);
	free(arr2DR);
	free(arr2DG);
	free(arr2DB);
	free(paddedArr2DR);
	free(paddedArr2DG);
	free(paddedArr2DB);
	free(convolutionArrR);
	free(convolutionArrG);
	free(convolutionArrB);
	free(arr1DR);
	free(arr1DG);
	free(arr1DB);
	free(blurArrRGB);
	return 0;
}