#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

//位图文件头结构
typedef struct tagBITMAPFILEHEADER 
{
	//WORD   bfType;		  //指定文件类型，必须是（0x4D42）
	DWORD  bfSize;           //文件大小
	WORD   bfReserved1;     //保留字，必须为0
	WORD   bfReserved2;     //保留字，同上
	DWORD  bfOffBits;      //指定从实际图像数据到文件头起始的偏移量
} BMPHeader;

//位图信息头结构
typedef struct tagBITMAPINFOHEADER
 {
	DWORD   biSize;             //信息头大小，为40
	LONG    biWidth;            //位图宽
	LONG    biHeight;           //位图高
	WORD    biPlanes;           //平面数，为1
	WORD    biBitCount;         //每像素位数，可以是1，2，4，8，16，24，32
	DWORD   biCompression;      //压缩方式，可以是0，1，2，其中0表示不压缩
	DWORD   biSizeImage;        //实际位图数据占用的字节数
	LONG    biXPelsPerMeter;    //X方向分辨率
	LONG    biYPelsPerMeter;    //Y方向分辨率
	DWORD   biClrUsed;          //使用的颜色数，如果为0，则表示默认值(2^颜色位数)
	DWORD   biClrImportant;     //重要颜色数，如果为0，则表示所有颜色都是重要的
} BMPInformation;

//位图调色板结构
typedef struct tagRGBQUAD 
{
	BYTE   rgbBlue; //该颜色的蓝色分量
	BYTE   rgbGreen; //该颜色的绿色分量
	BYTE   rgbRed; //该颜色的红色分量
	BYTE   rgbReserved; //保留值
} RGBQuad;

int convolution(BYTE *d,int i,int j,int a[],int width);
void sobel(BYTE*dataOfBmp,int width,int height);
void sobelimpro(BYTE*dataOfBmp,int width,int height);
void quick_sort(int ary[],int left,int right); 

int main()
 {
	BMPHeader fileHead;
	BMPInformation bitHead;
	RGBQuad *pRgb;
	FILE *pFile;
	WORD fileType;      //bmp位图标志位
	LONG nPlantNum;     //调色板大小
	BYTE strFile[50];

	pFile=fopen("lena.bmp","rb");
	if(pFile!=NULL) 
{
		fread(&fileType,1,sizeof(WORD),pFile);
		if(fileType!=0x4d42) 
{             //判断是否为bmp图
			printf("File is not bmp file!\n");
			return 0;
		}
		fread(&fileHead,1,sizeof(BMPHeader),pFile);   //读位图文件头
		fread(&bitHead,1,sizeof(BMPInformation),pFile);    //读位图信息头
	} else
 {
		printf("File open fail!\n");
		return 0;
	}
	nPlantNum=(long)(pow(2,(double)bitHead.biBitCount));
	pRgb=(RGBQuad*)malloc(nPlantNum*sizeof(RGBQuad));
	memset(pRgb,0,nPlantNum*sizeof(RGBQuad));
	fread(pRgb,sizeof(RGBQuad),nPlantNum,pFile);    //读调色板数据

	int width=bitHead.biWidth;
	printf("%d\n",width);
	int height=bitHead.biHeight;
	printf("%d\n",height);
	BYTE *pColorData=(BYTE*)malloc(height*width);
	memset(pColorData,0,height*width);
	LONG nData=height*width;
	fread(pColorData,1,nData,pFile);           //读像素数据
	
//	sobel(pColorData,width,height);			//sobel算子边缘边缘检测 
	sobelimpro(pColorData,width,height);  //改进的Sobel算子边缘检测 
	 
	FILE *pWrite=NULL;
	pWrite=fopen("out.bmp","wb");        //图像写操作，建立图像文件
	if(pWrite==NULL) 
{
		printf("Create new file fail!\n");
	}
	fwrite(&fileType,sizeof(WORD),1,pWrite);              //写标志位
	fwrite(&fileHead,sizeof(BMPHeader),1,pWrite);         //写文件头
	fwrite(&bitHead,sizeof(BMPInformation),1,pWrite);     //写信息头
	fwrite(pRgb,sizeof(RGBQuad),nPlantNum,pWrite);     //写调色板
	fwrite(pColorData,1,nData,pWrite);                 //写图像数据
	fclose(pWrite);
	system("start out.bmp");
	
	return 0;
}
传统的Sobel算子边缘检测程序：
////Sobel算子边缘检测
void sobel(BYTE *dataOfBmp,int width,int height)
{
	int i,j,temp1,temp2,temp;
	int *tempM=(int*)malloc(height*width*sizeof(int));
	memset(tempM,0,height*width*sizeof(int));
	for(i=0; i<width; i++)
	{
		for(j=0; j<height; j++)
		{
			if(i==0||j==0||i==width-1||j==height-1)    //去除边界 
			{
				dataOfBmp[j*width+i]=0;
			}
			else
			{			
					//竖直方向卷积
temp1=abs(dataOfBmp[(j+1)*width+(i-1)]+
2*dataOfBmp[(j+1)*width+i]+dataOfBmp[(j+1)*width+(i+1)]-dataOfBmp[(j-1)*width+(i-1)]-2*dataOfBmp[(j-1)*width+i]-dataOfBmp[(j-1)*width+(i+1)])/3;
							//水平方向卷积
temp2=abs(dataOfBmp[(j-1)*width+(i+1)]+
2*dataOfBmp[(j)*width+(i+1)]+dataOfBmp[+
1)*width+(i+1)]-dataOfBmp[(j-1)*width+(i-1)]-2*dataOfBmp[j*width+(i-1)]-dataOfBmp[(+1)*width+(i-1)])/3;      
			}
			if(temp1>temp2)   //取两方向中较大者
				temp=temp1;
			else
				temp=temp2;
			if(temp>=30)     //阈值
			{
				temp=0;
			}
			else
			{
				temp=0;
			}
			tempM[j*width+i]=(BYTE)temp;
		}
	}
	for(i=0; i<height*width; i++)
	{
		dataOfBmp[i]=tempM[i]; //图像数据回传
	}
	free(tempM);//关闭指针
	printf("EdgeDection is successful!");
}
改进的Sobel算子边缘检测程序：
//卷积计算函数 
int convolution(BYTE *d,int i,int j,int a[],int width)
{
		int l=0,w,h,s=0;
		for(w=-2;w<3;w++)
		{ 
			for(h=-2;h<3;h++)
			{
				s+=d[(j+w)*width+i]*a[l];
				l++;
			}
		}
		return s;
}
//快速排序函数 
void quick_sort(int a[],int left,int right)
{
    int i=left,j=right,k=a[left],t; 
    if(i>j)
        return ;
    while(i!=j)
    {
        while(i<j&&a[j]>=k)//先从右边比较            
j--;
        while(i<j&&a[i]<=k)//再从左边比较
            i++;
        if(i<j)
        {
            t=ary[i];a[i]=a[j];a[j]=t;//交换
        }
    }
    a[left]=a[i];
    a[i]=k;
    quick_sort(a,left,i-1);
    quick_sort(a,i+1,right);
}
//八个方向的模板 
int templet[8][25]= {{1,4,6,4,1,2,8,12,8,2,0,0,0,0,0,-2,-8,-12,-8,-2,-1,-4,-6,-4,-1},
	{4,6,4,1,2,1,12,12,8,0,2,8,0,-8,-2,0,-8,-12,-12,-1,-2,-1,-4,-6,-4},
	{6,4,1,2,0,4,12,8,0,-2,1,8,0,-8,-1,2,0,-8,-12,-4,0,-2,-1,-4,-6},
	{4,1,2,0,-2,6,12,8,-8,-1,4,12,0,-12,-4,1,8,-8,-12,-6,2,0,-2,-1,-4},
	{1,2,0,-2,-1,4,8,0,-8,-4,6,12,0,-12,-6,4,8,0,-8,-4,1,2,0,-2,-1},
	{2,0,-2,-1,-4,1,8,-8,-12,-6,4,12,0,-12,-4,6,12,8,-8,-1,4,1,2,0,-2},
	{0,-2,-1,-4,-6,2,0,-8,-12,-4,1,8,0,-8,-1,4,12,8,0,-2,6,4,1,2,0},
	{-2,-1,-4,-6,-4,0,-8,-12,-12,-1,2,8,0,-8,-2,1,12,12,8,0,4,6,4,1,2}
};
//改进的Sobel算子边缘检测
void sobelimpro(BYTE *dataOfBmp,int width,int height)
{
	int i,j,m,temp[8],maxtemp,a[width][height];
	int *tempM=(int*)malloc(height*width*sizeof(int));
	memset(tempM,0,height*width*sizeof(int));
	for(i=0; i<width; i++)
	{
		for(j=0; j<height; j++)
		{		
if(i==0||j==0||i==2||j==2||i==1||j==1||i==width-2||j==height-2
||i==width-1||j==height-1)//去除边界
			{
				tempM[j*width+i]=0;
			}
			else
			{
				for(m=0; m<8; m++)
				{
				//求梯度
			            temp[m]=abs(convolution(dataOfBmp,i,j,
templet[m],width))/5;
				}
				quick_sort(temp,0,7);//快速排序
				maxtemp=temp[7];
				a[i][j]=maxtemp;
if(maxtemp>115&&((maxtemp>a[i][j-1]&&maxtemp>a[i][j+1]||(maxtemp>a[i+1][j]&&maxtemp>a[i-1][j])))//阈值判断
				{
						maxtemp=255;		
				}else
				{
					maxtemp=0;
				}
				tempM[j*width+i]=(BYTE)maxtemp;
			}
		}
	}
	for(i=0; i<height*width; i++)
	{
		dataOfBmp[i]=tempM[i];    //图像数据回传
	}
	free(tempM);
	printf("EdgeDection is successful!");
}

