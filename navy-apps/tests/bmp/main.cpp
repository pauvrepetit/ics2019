#include <assert.h>
#include <stdlib.h>
#include <ndl.h>

#include <stdio.h>

int main() {
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("111\n");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  printf("222\n");
  assert(bmp->pixels);
  printf("333\n");
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("444\n");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("555\n");
  NDL_Render();
  printf("666\n");
  NDL_CloseDisplay();
  printf("777\n");
  while (1);
  return 0;
}
