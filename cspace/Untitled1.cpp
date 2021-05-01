
#include <stdio.h>
#include <malloc.h>
#include "BmpFormat.h"

BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;

void showBmpHead(BITMAPFILEHEADER pBmpHead)
{  //������ʾ��Ϣ�ĺ����������ļ�ͷ�ṹ��
    printf("BMP�ļ���С��%dkb\n", fileHeader.bfSize/1024);
    printf("�����ֱ���Ϊ0��%d\n",  fileHeader.bfReserved1);
    printf("�����ֱ���Ϊ0��%d\n",  fileHeader.bfReserved2);
    printf("ʵ��λͼ���ݵ�ƫ���ֽ���: %d\n",  fileHeader.bfOffBits);
}
void showBmpInfoHead(BITMAPINFOHEADER pBmpinfoHead)
{//������ʾ��Ϣ�ĺ��������������Ϣͷ�ṹ��
   printf("λͼ��Ϣͷ:\n" );
   printf("��Ϣͷ�Ĵ�С:%d\n" ,infoHeader.biSize);
   printf("λͼ���:%d\n" ,infoHeader.biWidth);
   printf("λͼ�߶�:%d\n" ,infoHeader.biHeight);
   printf("ͼ���λ����(λ�����ǵ�ɫ�������,Ĭ��Ϊ1����ɫ��):%d\n" ,infoHeader.biPlanes);
   printf("ÿ�����ص�λ��:%d\n" ,infoHeader.biBitCount);
   printf("ѹ����ʽ:%d\n" ,infoHeader.biCompression);
   printf("ͼ��Ĵ�С:%d\n" ,infoHeader.biSizeImage);
   printf("ˮƽ����ֱ���:%d\n" ,infoHeader.biXPelsPerMeter);
   printf("��ֱ����ֱ���:%d\n" ,infoHeader.biYPelsPerMeter);
   printf("ʹ�õ���ɫ��:%d\n" ,infoHeader.biClrUsed);
   printf("��Ҫ��ɫ��:%d\n" ,infoHeader.biClrImportant);
}

int main()
{
    FILE* fp;
    fp = fopen("cat-sol.bmp", "rb");//��ȡͬĿ¼�µ�image.bmp�ļ���
    if(fp == NULL)
    {
        printf("��'image.bmp'ʧ�ܣ�\n");
        return -1;
    }
    //������ȶ�ȡbifType������C���Խṹ��Sizeof������򡪡�������ڲ���֮�ͣ��Ӷ����¶��ļ���λ
    unsigned short  fileType;
    fread(&fileType,1,sizeof (unsigned short), fp);  
    if (fileType = 0x4d42)
    {
        printf("�ļ����ͱ�ʶ��ȷ!" );  
        printf("\n�ļ���ʶ����%d\n", fileType);
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fp);
        showBmpInfoHead(infoHeader);
        fclose(fp);
    }
}
