/*
*********************************************************************************************************
*
*    模块名称 : 显示子函数
*    文件名称 : disp_lib.h
*
*********************************************************************************************************
*/

#ifndef _DISP_LIB_H_
#define _DISP_LIB_H_

void DispHeader(char *_str);
void DispHeaderSn(uint8_t _idx);
void DispMeasBar(uint8_t _ucLine, char *_pName, char *_pValue, char *_pUnit);
void DispMeasBarEx(uint8_t _ucLine, char *_pName, char *_pValue, char *_pUnit, uint16_t _usFillColor);
void DispParamBar(uint8_t _ucLine, char *_pName, char *_pValue, uint8_t _ucActive);
void DispHelpBar(char *_str1, char *_str2);
void DispHeader2(uint8_t _idx, char *_str);
void DispHeaderStr(char *_str);
void DispInfoBar16(uint8_t _ucLine, char *_pName, char *_pValue);
void DispInfoBar16Ex(uint8_t _ucLine, char *_pName, char *_pValue, uint16_t _ucColor);
void DispBox(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void DispLabel(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
    uint16_t _usColor, char *_pStr, FONT_T *_tFont);
void DispLabelRound(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
    uint16_t _usColor, char *_pStr, FONT_T *_tFont);
void DispProgressBar(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
    char *_str1, float _Percent, char *_str2, FONT_T *_tFont);
void ProgressBarSetColor(uint16_t _Color);
void DispMsgBox(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, char *_str);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
