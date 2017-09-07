#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char* jpg[]={"beck1.jpg","beck2.jpg","beck3.jpg","beck4.jpg"};
static struct fb_var_screeninfo var;
static unsigned char* fbmem = NULL;

static void show_pic(int x, int width, int height, char* buffer);
static void show_point(int x, int y, unsigned long color);
char keyfc();
static int printpic(char* a);

int main(int argc,char* argv[]){
	char key = 0;
	int i = 0;
	int fd = 0;
		fd = open("/dev/fb0", O_RDWR);
		if(fd < 0)
		{
			printf("open /dev/fb0 failed!\n");
			return -1;
		}

		ioctl(fd, FBIOGET_VSCREENINFO, &var);
	
		unsigned int fbsize = var.xres * var.yres * var.bits_per_pixel / 8;
		fbmem = (unsigned char*)mmap(NULL, fbsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
//	for(i=0;i<1;i++){

	for(i=0;i<4;i++){
		printf("i=%d\n",i);

		printpic(jpg[i]);
		if(i >= 3){i = 0;}
		sleep(3);
		
	}
	key = keyfc();
	while(1){
	if(key = 1)
		printpic(jpg[i]);
	else if(key = 2)
		sleep(77);
	else if(key = 3)
		printf("hello\n");
	else if(key = 4)
		printf("bye!\n");
	else
	printpic(jpg[i]);
	}


	munmap(fbmem, fbsize);
	close(fd);
	return 0;
}
char keyfc()
{
	int fd = 0;
	fd = open("/dev/xyd_button", O_RDONLY);

	char key = 0;
	int ret = 0;
	while(1)
	{
		ret = read(fd, &key, 4);
		if(ret > 0)
			{
				printf("key%d is pressed!\n", key);
			}
																				
	}

	close(fd);
	return ret;
}

static int printpic (char* a)
{
	int i = 0;
	int fd = 0;
//	fd = open("/dev/fb0", O_RDWR);
//	if(fd < 0)
//		{
//			printf("open /dev/fb0 failed!\n");
//			return -1;
//		}

										
	ioctl(fd, FBIOGET_VSCREENINFO, &var);

	unsigned int fbsize = var.xres * var.yres * var.bits_per_pixel / 8;
//	fbmem = (unsigned char*)mmap(NULL, fbsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

//1. 分配和初始化一个decompression结构体
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	
//2. 指定源文件
	FILE * infile;
//	argv[0] = "beck1.jpg";
//	argv[1] = "beck2.jpg";
//	argv[2] = "beck3.jpg";
//	argv[3] = "beck4.jpg";
//	for(i=0;i<4;i++){
	if ((infile = fopen(a, "rb")) == NULL)
	{
		    fprintf(stderr, "can't open %s\n", a);
			exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);

//3. 用jpeg_read_header获得jpg信息
	jpeg_read_header(&cinfo, TRUE);
	printf("width:  %d\n",cinfo.image_width);
	printf("height: %d\n",cinfo.image_height);
	printf("num_components: %d\n",cinfo.num_components);
//4. 设置解压参数,比如放大、缩小。设置默认参数
//	printf("input the rate <x/y>: ");
//	fflush(stdout);
//	scanf("%d/%d", &(cinfo.scale_num), &(cinfo.scale_denom));
	cinfo.scale_num = 1;
	cinfo.scale_denom = 1;

//5. 启动解压：jpeg_start_decompress
	jpeg_start_decompress(&cinfo);

//6. 输出的图象的信息
	printf("dwidth:	%d\n",cinfo.output_width);
	printf("dheight: %d\n",cinfo.output_height);
	printf("dnum_components: %d\n",cinfo.output_components);
	JSAMPARRAY buffer;
	int row_stride;	

	row_stride = cinfo.output_width * cinfo.output_components;
								  
	buffer = (*cinfo.mem->alloc_sarray)
//	buffer = malloc(row_stride);
	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

//7. 循环调用jpeg_read_scanlines来一行一行地获得解压的数据
	while (cinfo.output_scanline < cinfo.output_height)
		{
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);
			show_pic(0, cinfo.output_width, cinfo.output_scanline, *buffer);
		}
	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	return 0;
}

/*static void show_pic(int x, int width, int height, char* buffer)
{
	int i = 0;
	int sd = 0;
	
	if(width > 799){width = 799;}
	if(height > 479){height = 479;}
	x = (799 - width)/2;
	int sd = x +width;
	if (sd > 799)
	{
		sd = 799;
	}
	printf("%d%d\n",width,height);
	unsigned long color;
	for(i = x; i < sd; i++)
		{
			color = ((*buffer << 16) | (*(buffer + 1) << 8) | *(buffer + 2));
			show_point(i, height, color);
			buffer = buffer + 3;
		}
}*/

static void show_pic(int x, int width, int height, char* buffer)
{
	int i = 0;
	unsigned long color;

	int sd = 0; 

	if(height > 479)
		{
			height = 479;
		}
	if(width > 799)
		{
			width = 799;
		}
	x = (799 - width)/2;

	sd = x + width; 
	if(sd > 799)
		{
			sd = 799;
		}

	for(i = x; i < sd; i++)
		{
			color = ((*buffer << 16) | (*(buffer + 1) << 8) | *(buffer + 2));
			show_point(i, height, color);
			buffer = buffer + 3;
		}
}
static void show_point(int x, int y, unsigned long color)
{	
	unsigned long* pen_32 = (unsigned long*)(fbmem + x * var.bits_per_pixel/8 + y*var.xres*var.bits_per_pixel/8);
	*pen_32 = color;
}
