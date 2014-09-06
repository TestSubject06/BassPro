#include "myLib.h"

u16* videoBuffer = (u16*)0x6000000;
u16* obj_attr_mem = (u16*)0x7000000;
u16* sprite_palette_bank_address = (u16*)0x5000200;

void setPixel(int x, int y, u16 color){
	videoBuffer[(y*240)+x] = color;
}

void drawRect(int x, int y, int width, int height, u16 color){
	//Draw horizontal lines until it's done
	for(int i = 0; i <= height; i++){
		for(int j = 0; j <= width; j++){
			videoBuffer[((y+i)*240)+(x+j)] = color;
		}
	}	
}

void drawHollowRect(int x, int y, int width, int height, u16 color){
	for(int i = 0; i <= width; i++){
		videoBuffer[(y*240) + (x+i)] = color;
		videoBuffer[((y+height)*240) + (x+i)] = color;	
	}
	for(int i = 0; i <= height; i++){
		videoBuffer[((y+i)*240) + (x)] = color;
		videoBuffer[((y+i)*240) + (x+width)] = color;	
	}
}
