#include<stdio.h>
#include<iostream>
#include<stdlib.h>
using namespace std; 

typedef struct {
//	short bfType;
    unsigned int bfSize,bfReserved;
    unsigned int bfOffBits; //偏移
}BmpFH;

typedef struct {
	unsigned int biSize;
    unsigned int biWidth,biHeight;//宽高
    unsigned short biPlanes;
    unsigned short biBitCount;//位数
    unsigned int biCompression, /**/biSizeImage,  biXPelsPerMeter,biYPelsPerMeter;
	unsigned int biClrUsed,biClrImportant;
}BmpIH;

typedef struct{
	unsigned char rgbBlue,rgbGreen,rgbRed,rgbReserved;
}PixelInfo;

typedef struct{
	unsigned char y,u,v;
}PixelYuv;

// Y = 0.299 R + 0.587 G + 0.114 B
// U = - 0.1687 R - 0.3313 G + 0.5 B + 128
// V = 0.5 R - 0.4187 G - 0.0813 B + 128

void rgb_yuv(PixelInfo rgb,PixelYuv *yuv){
    // yuv->y = 0.299*rgb.rgbRed + 0.587*rgb.rgbGreen + 0.114*rgb.rgbBlue;
    // yuv->u= - 0.1687*rgb.rgbRed - 0.3313*rgb.rgbGreen + 0.5*rgb.rgbBlue + 128;
    // yuv->v = 0.5*rgb.rgbRed - 0.4187*rgb.rgbGreen - 0.0813*rgb.rgbBlue + 128;

    yuv->y=0.257*rgb.rgbRed+0.504*rgb.rgbGreen+0.098*rgb.rgbBlue+16;
    yuv->u=0.439*rgb.rgbRed-0.368*rgb.rgbGreen-0.071*rgb.rgbBlue+128;
    yuv->v=0.0-0.148*rgb.rgbRed-0.291*rgb.rgbGreen+0.439*rgb.rgbBlue+128;
}

int main(){
    FILE *fbmp;
    FILE *frgb,*fyuv,*fnv21;
    if( (fbmp=fopen("cat-sol-big.bmp","rb")) == NULL ){
        puts("Fail to open file bmp!");
        exit(0);
    }
    if( (frgb=fopen("ElCapitain.rgb","wb+")) == NULL ){
       puts("Fail to open file Rgb!");
       exit(0);
   }
    if( (fyuv=fopen("ElCapitain.yuv","wb+")) == NULL ){
        puts("Fail to open file yuv!");
        exit(0);
    }
    if( (fnv21=fopen("ElCapitai.nv21","wb+")) == NULL ){
        puts("Fail to open file nv21!");
        exit(0);
    }


    BmpFH bfh;BmpIH bih; PixelInfo pif;
    char ftype[2];
    fread(&ftype,sizeof(ftype),1,fbmp);
    fread(&bfh,sizeof(bfh),1,fbmp);
    fread(&bih,sizeof(bih),1,fbmp);
    if(24>bih.biBitCount){
        fread(&pif,sizeof(pif),1,fbmp);
    }
    printf("bfSize:%d\n offset:%d\n wid:%d height:%d \n Bitcount:%d \n imageSize:%d \n %d %d \n",bfh.bfSize,bfh.bfOffBits,bih.biWidth,bih.biHeight,bih.biBitCount,bih.biSizeImage,bih.biClrUsed,bih.biClrImportant);
	int high=bih.biHeight;
    /*
        offset文件指针位置调节+
        别的不同位数的bmp处理
    
    */
    //int span = bih.biWidth+ (4-bih.biWidth%4);//填充字节后跨度
    /*
        奇数宽度处理 舍弃最后一个
    */
    PixelInfo * image = (PixelInfo *)malloc(bih.biWidth*bih.biHeight*sizeof(PixelInfo));
    unsigned char pb,pg,pr;
    int i,j; int fillers = (4-bih.biWidth%4)%4;
    cout<<fillers<<endl;
    //读取BMP中的RGB
    for(i=high-1;i>=0;i--){
        for(j=0;j<bih.biWidth;j++){
            fread(&pb,sizeof(pb),1,fbmp);
            fread(&pg,sizeof(pg),1,fbmp);
            fread(&pr,sizeof(pr),1,fbmp);

            image[i*bih.biWidth+j].rgbBlue=pb;
            image[i*bih.biWidth+j].rgbGreen=pg;
            image[i*bih.biWidth+j].rgbRed=pr;
        }
        //最后补齐的空读
        unsigned int c[4];
        fread(c,sizeof(pb)*(fillers)*3,1,fbmp);
    }
    PixelYuv * yuvImage = (PixelYuv * )malloc(bih.biWidth*bih.biHeight*sizeof(PixelYuv));
    //去除几列 
    
    int wid=bih.biWidth-bih.biWidth%4;
	for(i=0;i<high;i++){
        for(j=0;j<wid;j++){
            pb=image[i*bih.biWidth+j].rgbBlue;
            pg=image[i*bih.biWidth+j].rgbGreen;
            pr=image[i*bih.biWidth+j].rgbRed;
            //写rgb
            fwrite(&pb,sizeof(pb),1,frgb);
            fwrite(&pg,sizeof(pg),1,frgb);
            fwrite(&pr,sizeof(pr),1,frgb);

            //rgb转yuv
            rgb_yuv(image[i*bih.biWidth+j],yuvImage+i*bih.biWidth+j);

            //写yuv
            fwrite(&yuvImage[i*bih.biWidth+j].y,sizeof(yuvImage[i*bih.biWidth+j].y),1,fyuv);
            fwrite(&yuvImage[i*bih.biWidth+j].u,sizeof(yuvImage[i*bih.biWidth+j].u),1,fyuv);
            fwrite(&yuvImage[i*bih.biWidth+j].v,sizeof(yuvImage[i*bih.biWidth+j].v),1,fyuv);            
        }
        // unsigned int c =0;
        // fwrite(&c,sizeof(pb)*(fillers)*3,1,frgb);
        // fwrite(&c,sizeof(pb)*(fillers)*3,1,fyuv);
    }

    if(high%2==1)high--; 
    //写nv21-y
    for(i=0;i<high;i++){
        for(j=0;j<wid;j++){
            fwrite(&yuvImage[i*bih.biWidth+j].y,sizeof(pb),1,fnv21);
        }
    }
    // write uv
    for(i=0;i<high;i++){
        for(j=0;j<wid;j++){ 
                if(i%2==0&&0==j%2){
                    
                    fwrite(&(yuvImage[i*bih.biWidth+j].v),sizeof(pb),1,fnv21);
                    fwrite(&(yuvImage[i*bih.biWidth+j].u),sizeof(pb),1,fnv21);
                }        
        }
    }
    cout<<high<<"   "<<wid<<endl;
        
	fclose(fbmp);fclose(frgb);fclose(fyuv);fclose(fnv21);
    return 0;
}
