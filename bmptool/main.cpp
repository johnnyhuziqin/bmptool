//
//  main.cpp
//  bmptool
//
//  Created by huziqin on 15/8/18.
//  Copyright (c) 2015年 huziqin. All rights reserved.
//

#include<stdio.h>
#include "string.h"
#include<stdint.h>
//#include <malloc.h>
//#include <alloc.h>
#include <stdlib.h>
//#include "malloc.h"
//#include<windows.h>
typedef unsigned char BYTE ;
typedef int16_t WORD;
typedef uint32_t  DWORD;
typedef int32_t LONG;
typedef uint32_t UINT32;

typedef struct  tagBITMAPFILEHEADER
{
    char bfType[2] __attribute__ ((packed));//位图文件的类型，必须为BM(1-2字节）
    DWORD bfSize __attribute__ ((packed));//位图文件的大小，以字节为单位（3-6字节，低位在前）
    WORD bfReserved1 __attribute__ ((packed));//位图文件保留字，必须为0(7-8字节）
    WORD bfReserved2 __attribute__ ((packed));//位图文件保留字，必须为0(9-10字节）
    DWORD bfOffBits __attribute__ ((packed));//位图数据的起始位置，以相对于位图（11-14字节，低位在前）
    //文件头的偏移量表示，以字节为单位
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;//本结构所占用字节数（15-18字节）
    LONG biWidth;//位图的宽度，以像素为单位（19-22字节）
    LONG biHeight;//位图的高度，以像素为单位（23-26字节）
    WORD biPlanes;//目标设备的级别，必须为1(27-28字节）
    WORD biBitCount;//每个像素所需的位数，必须是1（双色），（29-30字节）
    //4(16色），8(256色）16(高彩色)或24（真彩色）之一
    DWORD biCompression;//位图压缩类型，必须是0（不压缩），（31-34字节）
    //1(BI_RLE8压缩类型）或2(BI_RLE4压缩类型）之一
    DWORD biSizeImage;//位图的大小(其中包含了为了补齐行数是4的倍数而添加的空字节)，以字节为单位（35-38字节）
    LONG biXPelsPerMeter;//位图水平分辨率，每米像素数（39-42字节）
    LONG biYPelsPerMeter;//位图垂直分辨率，每米像素数（43-46字节)
    DWORD biClrUsed;//位图实际使用的颜色表中的颜色数（47-50字节）
    DWORD biClrImportant;//位图显示过程中重要的颜色数（51-54字节）
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD{
    BYTE rgbBlue;//蓝色的亮度（值范围为0-255)
    BYTE rgbGreen;//绿色的亮度（值范围为0-255)
    BYTE rgbRed;//红色的亮度（值范围为0-255)
    BYTE rgbReserved;//保留，必须为0
}RGBQUAD;


typedef struct tagBITMAPINFO{
    BITMAPINFOHEADER bmiHeader;//位图信息头
    RGBQUAD bmiColors[16];//颜色表
}BITMAPINFO;

typedef struct
{
    BYTE b;
    BYTE g;
    BYTE r;
}RGB;

/*#define __palette4F(G,B,R,A) \
( ((UINT32)((16 + (((257*R) + (504*G) + (98*B))/1000))&0xff)<<24)|((UINT32)((128 + (((439*B) - (148*R) - (291*G))/1000))&0xff)<<16)|((UINT32)((128 + (((439*R) - (368*G) - (71*B))/1000))&0xff)<<8)|(A&0xff) ) 	//disp429
*/
#define __palette4F(G,B,R,A) \
( ((UINT32)((G)&0xff)<<24)|((UINT32)((B)&0xff)<<16)|((UINT32)((R)&0xff)<<8)|(A&0xff) )


#include "array.c"


#define BMP256

int main(void)
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    #ifdef BMP256
    RGBQUAD palete[256];
    #else
    RGBQUAD palete[16];
    #endif
    int i = 0,j=0;
    int total_col = 0,total_row = 0, bytes_in_a_row = 0;
    BYTE *img = NULL;
    BYTE *bitmap = NULL;
    int img_sz = 0;
    
    FILE*pfout=fopen("修改后的图像.bmp","wb");
    bitmap = (BYTE *)other_card_icon_Data;
    
    total_col = bitmap[0] << 8 | bitmap[1];
    total_row = bitmap[2] << 8 | bitmap[3];
    printf("w=%d,h=%d\n",total_col,total_row );

    int total_col_with_padding = (total_col + 3)*4/4;
    #ifdef BMP256
	img_sz = total_col_with_padding*total_row;
    #else
    img_sz = total_col_with_padding*total_row/2;
    #endif
    bitmap += 4;
    
    fileHeader.bfType[0] = 'B';
    fileHeader.bfType[1] = 'M';
    fileHeader.bfSize = sizeof(fileHeader)+sizeof(infoHeader)+sizeof(palete)+img_sz;
    fileHeader.bfOffBits = sizeof(fileHeader)+sizeof(infoHeader)+sizeof(palete);
    
    infoHeader.biSize = sizeof(infoHeader);
    infoHeader.biWidth = total_col;
    infoHeader.biHeight = total_row;
    infoHeader.biPlanes = 1;
    #ifdef BMP256
    infoHeader.biBitCount = 8;
    infoHeader.biClrUsed = 256;
    infoHeader.biClrImportant = 256;
    #else
    infoHeader.biBitCount = 4;
    infoHeader.biClrUsed = 16;
    infoHeader.biClrImportant = 16;
    #endif
    infoHeader.biCompression = 0;
    infoHeader.biSizeImage = img_sz;
    infoHeader.biXPelsPerMeter = 100;
    infoHeader.biYPelsPerMeter = 100;
    
    #ifdef BMP256
    int total_color = 0;
    total_color = sizeof(palette_16color)/4;
    printf("total_color = %d\n", total_color);
	for (i=0; i<total_color; i++) {
        //memcpy(&palete[i],&palette_16color[i],4);
        palete[i].rgbGreen = palette_16color[i] >> 24;
        palete[i].rgbBlue = (palette_16color[i] >> 16)&0xff;
        palete[i].rgbRed = (palette_16color[i]) >> 8 & 0xff;
        palete[i].rgbReserved = 0;
    }
    #else
    for (i=0; i<16; i++) {
        //memcpy(&palete[i],&palette_16color[i],4);
        palete[i].rgbGreen = palette_16color[i] >> 24;
        palete[i].rgbBlue = (palette_16color[i] >> 16)&0xff;
        palete[i].rgbRed = (palette_16color[i]) >> 8 & 0xff;
        palete[i].rgbReserved = 0;
    }
    #endif
    printf("file size is:%u,info header size:%u\n",fileHeader.bfSize,infoHeader.biSize);
    
    printf("header size is:%lu,icon data size:%d\n",sizeof(fileHeader),img_sz);
    
    img = (BYTE *)malloc(img_sz);
    #ifdef BMP256
    bytes_in_a_row = total_col_with_padding;
    #else
    bytes_in_a_row = total_col_with_padding/2;   //how many bytes in one row;
    #endif

    
    //turn upside down
    for (i = 0; i<total_row; i++) {
        for (j = 0; j < bytes_in_a_row; j++) {
        	if(j < total_col){
				*(img + j + i*(bytes_in_a_row)) = bitmap[j + total_col*(total_row-1-i)];

        	}else{
        		*(img + j + i*(bytes_in_a_row)) = 0;
        	}
        }
    }

    // memcpy(img,bitmap,img_sz);
    
    
    fwrite(&fileHeader,sizeof(fileHeader),1,pfout);
    fwrite(&infoHeader,sizeof(infoHeader),1,pfout);
    fwrite(palete,sizeof(palete),1,pfout);
    fwrite(img,img_sz,1,pfout);
    fclose(pfout);
}
