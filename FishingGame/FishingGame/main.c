#include "myLib.h"
#include "boat.h"
#include "waterBG.h"
#include "wave.h"
#include "BassHoriz.h"
#include "TroutHoriz.h"
#include "PerchHoriz.h"
#include "PikeHoriz.h"
#include "SalmonHoriz.h"
#include "Bass_Vert.h"
#include "Perch_Vert.h"
#include "Pike_Vert.h"
#include "Trout_Vert.h"
#include "Salmon_Vert.h"
#include "Sin_Lut.h"
#include "hook.h"
#include "startscreen.h"
#include "gameover.h"
#include "inst.h"
#include "text.h"
#include <debugging.h>
#include <stdlib.h>
#include <stdio.h>

void drawChar(int row, int col, char ch, unsigned short color)
{
    int r,c;
    for(r=0; r<8; r++)
    {
        for(c=0; c< 6; c++)
        {
            if(fontdata_6x8[OFFSET(r, c, 6)+48*ch])
            {
                setPixel(c+col, r+row, color);
            }
        }
    }
    
}

void drawString(int row, int col, char *str, unsigned short color)
{
    while(*str)
    {
        drawChar(row, col, *str++, color);
        col += 6;
    }
}     

int lu_sin(u16 theta){
	return (short)sin_lut[(theta>>7)&0x1FF];
}

int lu_cos(u16 theta){   
	return (short)sin_lut[((theta>>7)+128)&0x1FF];     
}

void vid_vsync(){
	while(REG_VCOUNT >= 160){};
	while(REG_VCOUNT < 160){};	
}

void dma_cpy(const volatile void *dst, const volatile void *src, u32 count, u16 ch){
	REG_DMA[ch].cnt = 0;
	REG_DMA[ch].src = (u32*)src;
	REG_DMA[ch].dst = (u32*)dst;
	REG_DMA[ch].cnt = count | (1<<0x1f);
}

int clamp(int a, int min, int max){
	return (a > min)? min : ((a < max) ? max : a);
}

int wrap(int a, int min, int max){
	while(a > max){
		a -= max;
	}
	return a-1;
}

int abs(int a){
	return a<0 ? -a : a;
}

typedef struct{
	short x;
	short y;
	char visible;
	short dir;
	short speed;
	short sinLoc;
} fish;

typedef struct {
	short x;
	short y;
	char visible;
} fishhook;

//I love you, too. More than you know.

void buildHook(fishhook hook){
	obj_attr_mem[100] = hook.y | (hook.visible ? SPRITE_RENDER_NORMAL : SPRITE_RENDER_HIDDEN) | SPRITE_SHAPE_TALL;
	obj_attr_mem[101] = hook.x;
}

void buildFish(fish someFish, u16 fishIndex, u16 fishType){
	obj_attr_mem[(60 + fishType*20) + (fishIndex*4)] = (0xFF & someFish.y) | (someFish.visible ? SPRITE_RENDER_NORMAL : SPRITE_RENDER_HIDDEN) | ((fishType == 0) ? SPRITE_SHAPE_WIDE : SPRITE_SHAPE_TALL);
	obj_attr_mem[(61 + fishType*20) + (fishIndex*4)] = (0x1FF & someFish.x) | SPRITE_SIZE_MEDIUM | ((someFish.dir > 0) ? 0 : (1<<12));
}

void doGame();

int main(void){
	REG_DISPCNT = BG2_ENABLE | MODE3 | OBJ_ENABLE | SPR_MAP_1D;
	while(1){
		for(int i = 0; i < 108; i++){
			obj_attr_mem[i] = 0;
		}
		doGame();
	}
}

void doGame(){
	//Draw title screen
	dma_cpy(videoBuffer, &startscreen, 38400, 3);
	int frameCounter = 0;
	while((~(REG_BUTTONS) & BTN_STRT) == 0){
		frameCounter++;
	}

	dma_cpy(videoBuffer, &inst, 38400, 3);
	while((~(REG_BUTTONS) & BTN_STRT) > 0){
		frameCounter++;
		if((~(REG_BUTTONS) & BTN_SELC) > 0){
			return;
		}
	}

	while((~(REG_BUTTONS) & BTN_STRT) == 0){
		frameCounter++;
		if((~(REG_BUTTONS) & BTN_SELC) > 0){
			return;
		}
	}
	
	srand(frameCounter);
	frameCounter = 0;

	dma_cpy(videoBuffer, &waterBG, 38400, 3);
	dma_cpy(sprite_palette_bank_address, &boat_palette, 11, 3);
	dma_cpy(&sprite_palette_bank_address[11], &wave_palette[1], 2, 3);
	dma_cpy(&sprite_palette_bank_address[16], &BassHoriz_palette, 12, 3);
	dma_cpy(&sprite_palette_bank_address[32], &TroutHoriz_palette, 9, 3);
	dma_cpy(&sprite_palette_bank_address[48], &SalmonHoriz_palette, 8, 3);
	dma_cpy(&sprite_palette_bank_address[64], &PikeHoriz_palette, 9, 3);
	dma_cpy(&sprite_palette_bank_address[80], &PerchHoriz_palette, 9, 3);
	//Whatever, the compiler will optimize all this math for me.
	dma_cpy(OVRAM_BNK2, boat, 1024, 3);
	dma_cpy(OVRAM_BNK2+512, wave, 64, 3);
	dma_cpy(OVRAM_BNK2+512+32, BassHoriz, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192, TroutHoriz, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192, SalmonHoriz, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192, PikeHoriz, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192+192, PerchHoriz, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192+192+192, Bass_Vert, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192+192+192+192, Trout_Vert, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192+192+192+192+192, Salmon_Vert, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192+192+192+192+192+192, Pike_Vert, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192+192+192+192+192+192+192, Perch_Vert, 384, 3);
	dma_cpy(OVRAM_BNK2+512+32+192+192+192+192+192+192+192+192+192+192, hook, 32, 3);

	//Waves Last to be drawn. These are on top of everything
	short wave_y[15] = {23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23};
	u16 w_ATTR_0;
	u16 w_ATTR_1;
	u16 w_ATTR_2;
	for(int i = 0; i < 15; i++){
		//Y position
		w_ATTR_0 = wave_y[i] | (1<<13) | (1<<14) | ((i%2==0)?(1<<9):0);
		//X position
		w_ATTR_1 = (i*16) | (0<<14);
		//Tile Index
		w_ATTR_2 = 512+64;
		obj_attr_mem[((4*i))] = w_ATTR_0;
		obj_attr_mem[((4*i)+1)] = w_ATTR_1;
		obj_attr_mem[((4*i)+2)] = w_ATTR_2;
	}
	//Obj_attr_mem is filled from 0 - 59  :: (15*4)

	//Test Fishes
	fish fishes[10];
	for(int i = 0; i<5; i++){		
		fishes[i].x = rand()%240;
		fishes[i].y = rand()%100+40;
		fishes[i].visible = 1;
		fishes[i].dir = (i%2>0 ? 1 : -1);
		fishes[i].speed = rand()%3+1;
		fishes[i].sinLoc = 0;
		//Y position 0, 256 color mode, wide sprite
		u16 f_ATTR_0 = fishes[i].y | (1<<14);
		//X position of 25, Largest sprite (for a 64x32 sprite)
		u16 f_ATTR_1 = fishes[i].x | (1<<15);
		//Tile index of 512(The beginning of charblock 2), no other flags to set
		u16 f_ATTR_2 = (580 + (24*i)) | ((i+1)<<12);
		obj_attr_mem[60 + (i*4)] = f_ATTR_0;
		obj_attr_mem[61 + (i*4)] = f_ATTR_1;
		obj_attr_mem[62 + (i*4)] = f_ATTR_2;
	}
	for(int i = 0; i<5; i++){
		//Y position 0, 256 color mode, tall sprite
		u16 f_ATTR_0 = 0 | (2<<14);
		//X position of 25, Largest sprite (for a 64x32 sprite)
		u16 f_ATTR_1 = 0 | (1<<15);
		//Tile index of 512(The beginning of charblock 2), no other flags to set
		u16 f_ATTR_2 = (700 + (24*i)) | ((i+1)<<12);
		obj_attr_mem[80 + (i*4)] = f_ATTR_0;
		obj_attr_mem[81 + (i*4)] = f_ATTR_1;
		obj_attr_mem[82 + (i*4)] = f_ATTR_2;
		fishes[i+5].x = -32;
		fishes[i+5].y = 40+(i*20);
		fishes[i+5].visible = 0;
	}
	const char fishAnim[16] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1};
	//OAM is filled from 60 - 99 :: ((4 * 5) *2)	


	//Hook - Behind everything but the boat
	u16 h_ATTR_0 = 0 | SPRITE_SHAPE_TALL | SPRITE_RENDER_HIDDEN;
	u16 h_ATTR_1 = 0;
	u16 h_ATTR_2 = 820;
	obj_attr_mem[100] = h_ATTR_0;
	obj_attr_mem[101] = h_ATTR_1;
	obj_attr_mem[102] = h_ATTR_2;
	fishhook hook = {0, 0, 0};

	//Boat - The first thing to be drawn, thus at the bottom of the stack
	short boatY = -16;
	short boatX = -32;
	//Y position 0, 256 color mode, wide sprite
	u16 ATTR_0 = 0 | (3<<8) | (1<<13) | (1<<14);
	//X position of 25, Largest sprite (for a 64x32 sprite)
	u16 ATTR_1 = 25 | (0<<9) |(3<<14);
	//Tile index of 512(The beginning of charblock 2), no other flags to set
	u16 ATTR_2 = 512;
	obj_attr_mem[104] = ATTR_0;
	obj_attr_mem[105] = ATTR_1;
	obj_attr_mem[106] = ATTR_2;

	//obj_attr_mem[3] = (lu_cos(0xFFFF - 3000)>>4);
	//obj_attr_mem[7] = -(lu_sin(0xFFFF - 3000)>>4);
	//obj_attr_mem[11] = (lu_sin(0xFFFF - 3000)>>4);
	//obj_attr_mem[15] = (lu_cos(0xFFFF - 3000)>>4);

	//Render order
	//Waves
	//Fish
	//Hook 
	//Boat 

	int i = 0;
	int offset = 0;
	const int offsets[40] = {0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -3, -3, -4, -4, -4, -4, -4,-3, -3, -3, -2, -2, -1, -1};
	int aPressed = 0;
	int hookFalling = 0;
	int hasFish = 0;
	int cFishIndex = -1;
	int caughtFish = 0;
	int timer = 7196;
	int timeUp = 1;
	while(timeUp){
		if((~(REG_BUTTONS) & BTN_A) > 0 && aPressed == 0){
			aPressed = 1;
			if(hookFalling == 0){
				hook.x = boatX + 60;
				hook.y = boatY + 40;
				hook.visible = 1;
				hookFalling = 1;
			}else if(hookFalling == 1){	
				hookFalling = 2;
			}
		}
		if(aPressed == 1 && (~(REG_BUTTONS) & BTN_A) == 0){
			aPressed = 0;
		}
		if(hookFalling == 1){
			hook.y+=2;
			if(hook.y > 144){
				hookFalling = 2;
			}
		}else if(hookFalling == 2){
			hook.y -= 2;
			if(hook.y < 20){
				hookFalling = 0;
				hook.visible = 0;
				if(hasFish){
					fishes[cFishIndex].visible = 0;
					//TODO:: Randomize new fish spawn.
					fishes[cFishIndex-5].visible = 1;					
					fishes[cFishIndex-5].dir = frameCounter%2?-1:1;
					fishes[cFishIndex-5].x = fishes[cFishIndex-5].dir>0? -50 : 290;
					fishes[cFishIndex-5].y = rand()%100+40;
					fishes[cFishIndex-5].speed = rand()%3+1;
					hasFish = 0;
					caughtFish++;
				}				
			}
		}

		for(int i = 0; i < 5; i++){
			if(fishes[i].visible){
				fishes[i].x += fishes[i].speed*fishes[i].dir;
				if(fishes[i].x > 260 && fishes[i].dir == 1){
					fishes[i].x = -32;
					fishes[i].y = rand()%100+40;
					fishes[i].sinLoc = 0;
				}else if(fishes[i].x < -32 && fishes[i].dir == -1){
					fishes[i].x = 260;
					fishes[i].y = rand()%100+40;
					fishes[i].sinLoc = 0;
				}
				fishes[i].y += offsets[fishes[i].sinLoc];
				fishes[i].sinLoc++;
				if(fishes[i].sinLoc >= 40){
					fishes[i].sinLoc = 0;
				}
			}
		}

		//Check collision with hook against fish mouth
		if(hookFalling == 2 && !hasFish){
			for(int i = 0; i < 5; i++){
				if(fishes[i].visible){
					if(fishes[i].dir == 1){
						if(hook.x < (fishes[i].x+32) && hook.x > (fishes[i].x+20)){
							if((hook.y+8) > (fishes[i].y) && (hook.y+8) < (fishes[i].y + 16)){
								//Collision occured with right-facing fish
								hasFish = 1;
								fishes[i].visible = 0;
								fishes[i+5].x = hook.x-4;
								fishes[i+5].y = hook.y+8;
								fishes[i+5].visible = 1;
								cFishIndex = i+5;
								break;
							}
						}
					}else{
						if((hook.x-8) < (fishes[i].x+2) && hook.x > (fishes[i].x-8)){
							if((hook.y+8) > (fishes[i].y+4) && (hook.y+8) < (fishes[i].y + 12)){
								//Collision occured with left-facing fish
								hasFish = 1;
								fishes[i].visible = 0;	
								fishes[i+5].x = hook.x-4;
								fishes[i+5].y = hook.y+8;						
								fishes[i+5].visible = 1;
								cFishIndex = i+5;
								break;
							}
						}
					}
				}
			}
		}

		if(hasFish){
			fishes[cFishIndex].y = hook.y+8;
		}

		if((~(REG_BUTTONS) & BTN_SELC) > 0){
			return;
		}

		//Wait for VBLANK
		vid_vsync();
		for(int i = 0; i < 5; i++){
			obj_attr_mem[62 + (4*i)] = (0xFE00 & obj_attr_mem[62 + (4*i)]) | ((580 + (24*i)) + fishAnim[wrap(frameCounter, 0, 16)]*8);
			obj_attr_mem[82 + (4*i)] = (0xFE00 & obj_attr_mem[82 + (4*i)]) | ((700 + (24*i)) + fishAnim[wrap(frameCounter, 0, 16)]*8);
			buildFish(fishes[i], i, 0);
			buildFish(fishes[i+5], i, 1);
		}	
		buildHook(hook);
		for(i = (frameCounter%2)+29; i < 160; i+=2){
			offset = offsets[wrap(frameCounter+i, 0, 40)];
			dma_cpy(&videoBuffer[i*240 + (offset>=0? offset : 0)], &waterBG[i*240 + (offset>=0 ? 0 : -offset)], 240 - abs(offset), 3);
		}
		frameCounter++;
		if(frameCounter > 80){
			frameCounter = 0;
		}
		if(hookFalling){
			drawRect(boatX+63, boatY+49, 0, (hook.y - boatY)-46, 0);
		}
		//Flicker and move the water cusps on the surface
		for(i=0; i<15; i++){
			obj_attr_mem[(4*i)] ^= (1<<9);
			wave_y[i] = 21 + (offsets[wrap(frameCounter+i, 0, 40)]>>1);
			obj_attr_mem[(4*i)] = (obj_attr_mem[(4*i)]&0xFF00) | (0xFF & wave_y[i]);
		}
		//Basic movement code
		if((~(REG_BUTTONS) & BTN_LEFT) > 0 && !hookFalling){
			boatX-=2;
			if(boatX < -48) boatX = -48;
		}
		if((~(REG_BUTTONS) & BTN_RGHT) > 0 && !hookFalling){
			boatX+=2;
			if(boatX > 240-80) boatX = 240-80;
		}
		//divide boat X by 16 to get 'tile index' of the wave it's centered on
		boatY = wave_y[(boatX+64)>>4] - 39;

		obj_attr_mem[3] = lu_cos(300*offsets[wrap((frameCounter>>1)+((boatX+64)>>4), 0, 40)])>>4;
		obj_attr_mem[7] = -(lu_sin(300*offsets[wrap((frameCounter>>1)+((boatX+64)>>4), 0, 40)])>>4);
		obj_attr_mem[11] = lu_sin(300*offsets[wrap((frameCounter>>1)+((boatX+64)>>4), 0, 40)])>>4;
		obj_attr_mem[15] = lu_cos(300*offsets[wrap((frameCounter>>1)+((boatX+64)>>4), 0, 40)])>>4;

		obj_attr_mem[105] = (0x01FF & boatX) | (3<<14);
		obj_attr_mem[104] = (0xFF & boatY) | (0xFF00 & obj_attr_mem[104]);
		//obj_attr_mem[62] = ATTR_2 + 4 + (offsets[wrap(frameCounter, 0, 40)]*2);

		timer--;
		if(timer <= 0){
			timeUp = 0;
		}
	}

	for(int i = 0; i < 108; i++){
		obj_attr_mem[i] = 0;
	}

	//Display End Screen
	dma_cpy(videoBuffer, &gameover, 38400, 3);
	//Fill the location with the number of fish caught in the 2 minutes.
	char str[15];
	sprintf(str, "%d", caughtFish);
	drawString(126, 206, &str[0], RGB(31, 31, 31));
	while(1){
		if((~(REG_BUTTONS) & BTN_SELC) > 0){
			return;
		}
	}
}
