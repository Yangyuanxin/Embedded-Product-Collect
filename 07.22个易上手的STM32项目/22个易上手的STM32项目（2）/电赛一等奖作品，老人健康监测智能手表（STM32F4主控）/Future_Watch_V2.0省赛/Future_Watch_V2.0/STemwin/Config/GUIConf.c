#include "GUI.h"

#define GUI_NUMBYTES  (80*1024) 
#define GUI_BLOCKSIZE 0x80  		//每块内存的大小

void GUI_X_Config(void) {
  //
  // 32 bit aligned memory area
  //
  static U32 aMemory[GUI_NUMBYTES / 4];
  //
  // Assign memory to emWin
  //
  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
  GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //设置存储快的平均尺寸,该区越大,可用的存储快数量越少
  GUI_SetDefaultFont(GUI_FONT_6X8);
}
