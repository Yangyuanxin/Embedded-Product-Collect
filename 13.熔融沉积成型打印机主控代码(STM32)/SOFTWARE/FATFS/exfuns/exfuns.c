#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "malloc.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 

//文件学习心得 By xiaoyuan
//1.  下面的“文件类型列表” 要和filelistbox.h函数中的“文件过滤器个位功能定义”要对应一致。
//如：BIN文件排在第一位 对应的在filelistbox.h “#define FLBOX_FLT_BIN 	1<<0”
//2.FILE_TYPE_TBL[8][13]该二维数组的一级目录指的是文件大类，二级目录指的是文件小类
// exfun.h函数中的“f_typetell返回的类型定义”就是根据该二维数组生成的，生成法则便是f_typetell(u8 *fname)
// 函数中的(i<<4)|j语句，i指的是大类号（存储在高四位），j指的是小类号（存储在底四位）。 
// 但是exfun.h函数中的“f_typetell返回的类型定义”不知道有什么用途，好像在后面并没有用到。
//3.filelistbox.c文件中的读取并检查文件类型的位置目前为595-607。
//4.
//////////////////////////////////////////////////////////////////////////////////	 

 //文件类型列表
u8*const FILE_TYPE_TBL[8][13]=
{
{"BIN"},			//BIN文件   //1<<0
{"LRC"},			//LRC文件   //1<<1
{"NES"},			//NES文件   //1<<2
{"TXT","C","H"},	//文本文件  //1<<3
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","ACC","WMA","WAV","MID","FLAC"},//音乐文件  //1<<4
{"BMP","JPG","JPEG","GIF",},//图片文件  //1<<5
{""},  //视频文件 //1<<6
{"GCODE"},  //GCODE文件//1<<7
};
///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs[2];  		//逻辑磁盘工作区.	 
FIL *file;	  		//文件1
FIL *ftemp;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录

u8 *fatbuf;			//SD卡数据缓存区
///////////////////////////////////////////////////////////////////////////////////////
//为exfuns申请内存
//返回值:0,成功
//1,失败
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)mymalloc(sizeof(FATFS));	//为磁盘0工作区申请内存	
	fs[1]=(FATFS*)mymalloc(sizeof(FATFS));	//为磁盘1工作区申请内存
	file=(FIL*)mymalloc(sizeof(FIL));		//为file申请内存
	ftemp=(FIL*)mymalloc(sizeof(FIL));		//为ftemp申请内存
	fatbuf=(u8*)mymalloc(512);				//为fatbuf申请内存
	if(fs[0]&&fs[1]&&file&&ftemp&&fatbuf)return 0;  //申请有一个失败,即失败.
	else return 1;	
}

//将小写字母转为大写字母,如果是数字,则保持不变.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}	      
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//		 其他,高四位表示所属大类,低四位表示所属小类.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[6];
	u8 *attr='\0';//后缀名
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==250)return 0XFF;//错误的字符串.
 	for(i=0;i<6;i++)//得到后缀名
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<5;i++)tbuf[i]=char_upper(tbuf[i]);//全部变为大写 
	for(i=0;i<8;i++)
	{
		for(j=0;j<13;j++)
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//没找到		 			   
}	 

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}		   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//文件复制
//注意文件大小不要超过4GB.
//将psrc文件,copy到pdst.
//fcpymsg,函数指针,用于实现拷贝时的信息显示
//        pname:文件/文件夹名
//        pct:百分比
//        mode:
//			[0]:更新文件名
//			[1]:更新百分比pct
//			[2]:更新文件夹
//			[3~7]:保留
//psrc,pdst:源文件和目标文件
//totsize:总大小(当totsize为0的时候,表示仅仅为单个文件拷贝)
//cpdsize:已复制了的大小.
//fwmode:文件写入模式
//0:不覆盖原有的文件
//1:覆盖原有的文件
//返回值:0,正常
//    其他,错误,0XFF,强制退出
u8 exf_copy(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8 *psrc,u8 *pdst,u32 totsize,u32 cpdsize,u8 fwmode)
{
	u8 res;
    u16 br=0;
	u16 bw=0;
	FIL *fsrc=0;
	FIL *fdst=0;
	u8 *fbuf=0;
	u8 curpct=0;
 	fsrc=(FIL*)mymalloc(sizeof(FIL));//申请内存
 	fdst=(FIL*)mymalloc(sizeof(FIL));
	fbuf=(u8*)mymalloc(8192);
  	if(fsrc==NULL||fdst==NULL||fbuf==NULL)res=100;//前面的值留给fatfs
	else
	{   
		if(fwmode==0)fwmode=FA_CREATE_NEW;//不覆盖
		else fwmode=FA_CREATE_ALWAYS;	  //覆盖存在的文件
		 
	 	res=f_open(fsrc,(const TCHAR*)psrc,FA_READ|FA_OPEN_EXISTING);	//打开只读文件
	 	if(res==0)res=f_open(fdst,(const TCHAR*)pdst,FA_WRITE|fwmode); 	//第一个打开成功,才开始打开第二个
		if(res==0)//两个都打开成功了
		{
			if(totsize==0)//仅仅是单个文件复制
			{
				totsize=fsrc->fsize;
				cpdsize=0;
				curpct=0;
		 	}else curpct=(cpdsize*100)/totsize;	//得到新百分比
			fcpymsg(psrc,curpct,0X02);			//更新百分比
			while(res==0)//开始复制
			{
				res=f_read(fsrc,fbuf,8192,(UINT*)&br);	//源头读出512字节
				if(res||br==0)break;
				res=f_write(fdst,fbuf,(UINT)br,(UINT*)&bw);	//写入目的文件
				cpdsize+=bw;
				if(curpct!=(cpdsize*100)/totsize)//是否需要更新百分比
				{
					curpct=(cpdsize*100)/totsize;
					if(fcpymsg(psrc,curpct,0X02))//更新百分比
					{
						res=0XFF;//强制退出
						break;
					}
				}			    
				if(res||bw<br)break;       
			}
		    f_close(fsrc);
		    f_close(fdst);
		}
	}
	myfree(fsrc);//释放内存
	myfree(fdst);
	myfree(fbuf);
	return res;
}

//得到路径下的文件夹
//返回值:0,路径就是个卷标号.
//    其他,文件夹名字首地址
u8* exf_get_src_dname(u8* dpfn)
{
	u16 temp=0;
 	while(*dpfn!=0)
	{
		dpfn++;
		temp++;	
	}
	if(temp<4)return 0; 
	while((*dpfn!=0x5c)&&(*dpfn!=0x2f))dpfn--;	//追述到倒数第一个"\"或者"/"处 
	return ++dpfn;
}
//得到文件夹大小
//注意文件夹大小不要超过4GB.
//返回值:0,文件夹大小为0,或者读取过程中发生了错误.
//    其他,文件夹大小.
u32 exf_fdsize(u8 *fdname)
{
#define MAX_PATHNAME_DEPTH	512+1	//最大目标文件路径+文件名深度
	u8 res=0;	  
    DIR *fddir=0;		//目录
	FILINFO *finfo=0;	//文件信息
	u8 *fn=0;   		//长文件名
	u8 * pathname=0;	//目标文件夹路径+文件名
 	u16 pathlen=0;		//目标路径长度
	u32 fdsize=0;

	fddir=(DIR*)mymalloc(sizeof(DIR));//申请内存
 	finfo=(FILINFO*)mymalloc(sizeof(FILINFO));
   	if(fddir==NULL||finfo==NULL)res=100;
	if(res==0)
	{
	   	finfo->lfsize=_MAX_LFN*2+1;
		finfo->lfname=mymalloc(finfo->lfsize);//申请内存
 		pathname=mymalloc(MAX_PATHNAME_DEPTH);	    
 		if(finfo->lfname==NULL||pathname==NULL)res=101;	   
 		if(res==0)
		{
			pathname[0]=0;	    
			strcat((char*)pathname,(const char*)fdname); //复制路径	
		    res=f_opendir(fddir,(const TCHAR*)fdname); 		//打开源目录
		    if(res==0)//打开目录成功 
			{														   
				while(res==0)//开始复制文件夹里面的东东
				{
			        res=f_readdir(fddir,finfo);						//读取目录下的一个文件
			        if(res!=FR_OK||finfo->fname[0]==0)break;		//错误了/到末尾了,退出
			        if(finfo->fname[0]=='.')continue;     			//忽略上级目录
					if(finfo->fattrib&0X10)//是子目录(文件属性,0X20,归档文件;0X10,子目录;)
					{
   						fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname);//得到文件名
						pathlen=strlen((const char*)pathname);		//得到当前路径的长度
						strcat((char*)pathname,(const char*)"/");	//加斜杠
						strcat((char*)pathname,(const char*)fn);	//源路径加上子目录名字
 						//printf("\r\nsub folder:%s\r\n",pathname);	//打印子目录名
						fdsize+=exf_fdsize(pathname);				//得到子目录大小
						pathname[pathlen]=0;						//加入结束符
					}else fdsize+=finfo->fsize;						//非目录,直接加上文件的大小
						
				} 
		    }	  
  			myfree(pathname);	    
			myfree(finfo->lfname);
		}
 	}
	myfree(fddir);    
	myfree(finfo);
	if(res)return 0;
	else return fdsize;
}	  
//文件夹复制
//注意文件夹大小不要超过4GB.
//将psrc文件夹,copy到pdst文件夹.
//pdst:必须形如"X:"/"X:XX"/"X:XX/XX"之类的.而且要实现确认上一级文件夹存在
//fcpymsg,函数指针,用于实现拷贝时的信息显示
//        pname:文件/文件夹名
//        pct:百分比
//        mode:
//			[0]:更新文件名
//			[1]:更新百分比pct
//			[2]:更新文件夹
//			[3~7]:保留
//psrc,pdst:源文件夹和目标文件夹
//totsize:总大小(当totsize为0的时候,表示仅仅为单个文件拷贝)
//cpdsize:已复制了的大小.
//fwmode:文件写入模式
//0:不覆盖原有的文件
//1:覆盖原有的文件
//返回值:0,成功
//    其他,错误代码;0XFF,强制退出
u8 exf_fdcopy(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8 *psrc,u8 *pdst,u32 *totsize,u32 *cpdsize,u8 fwmode)
{
#define MAX_PATHNAME_DEPTH	512+1	//最大目标文件路径+文件名深度
	u8 res=0;	  
    DIR *srcdir=0;		//源目录
	DIR *dstdir=0;		//源目录
	FILINFO *finfo=0;	//文件信息
	u8 *fn=0;   		//长文件名

	u8 * dstpathname=0;	//目标文件夹路径+文件名
	u8 * srcpathname=0;	//源文件夹路径+文件名
	
 	u16 dstpathlen=0;	//目标路径长度
 	u16 srcpathlen=0;	//源路径长度

  
	srcdir=(DIR*)mymalloc(sizeof(DIR));//申请内存
 	dstdir=(DIR*)mymalloc(sizeof(DIR));
	finfo=(FILINFO*)mymalloc(sizeof(FILINFO));

   	if(srcdir==NULL||dstdir==NULL||finfo==NULL)res=100;
	if(res==0)
	{
	   	finfo->lfsize=_MAX_LFN*2+1;
		finfo->lfname=mymalloc(finfo->lfsize);//申请内存
 		dstpathname=mymalloc(MAX_PATHNAME_DEPTH);
		srcpathname=mymalloc(MAX_PATHNAME_DEPTH);
 		if(finfo->lfname==NULL||dstpathname==NULL||srcpathname==NULL)res=101;	   
 		if(res==0)
		{
			dstpathname[0]=0;
			srcpathname[0]=0;
			strcat((char*)srcpathname,(const char*)psrc); 	//复制原始源文件路径	
			strcat((char*)dstpathname,(const char*)pdst); 	//复制原始目标文件路径	
		    res=f_opendir(srcdir,(const TCHAR*)psrc); 		//打开源目录
		    if(res==0)//打开目录成功 
			{
  				strcat((char*)dstpathname,(const char*)"/");//加入斜杠
 				fn=exf_get_src_dname(psrc);
				if(fn==0)//卷标拷贝
				{
					dstpathlen=strlen((const char*)dstpathname);
					dstpathname[dstpathlen]=psrc[0];	//记录卷标
					dstpathname[dstpathlen+1]=0;		//结束符 
				}else strcat((char*)dstpathname,(const char*)fn);//加文件名		
 				fcpymsg(fn,0,0X04);//更新文件夹名
				res=f_mkdir((const TCHAR*)dstpathname);//如果文件夹已经存在,就不创建.如果不存在就创建新的文件夹.
				if(res==FR_EXIST)res=0;
				while(res==0)//开始复制文件夹里面的东东
				{
			        res=f_readdir(srcdir,finfo);					//读取目录下的一个文件
			        if(res!=FR_OK||finfo->fname[0]==0)break;		//错误了/到末尾了,退出
			        if(finfo->fname[0]=='.')continue;     			//忽略上级目录
					fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname); 	//得到文件名
					dstpathlen=strlen((const char*)dstpathname);	//得到当前目标路径的长度
					srcpathlen=strlen((const char*)srcpathname);	//得到源路径长度

					strcat((char*)srcpathname,(const char*)"/");//源路径加斜杠
 					if(finfo->fattrib&0X10)//是子目录(文件属性,0X20,归档文件;0X10,子目录;)
					{
						strcat((char*)srcpathname,(const char*)fn);		//源路径加上子目录名字
						res=exf_fdcopy(fcpymsg,srcpathname,dstpathname,totsize,cpdsize,fwmode);	//拷贝文件夹
					}else //非目录
					{
						strcat((char*)dstpathname,(const char*)"/");//目标路径加斜杠
						strcat((char*)dstpathname,(const char*)fn);	//目标路径加文件名
						strcat((char*)srcpathname,(const char*)fn);	//源路径加文件名
 						fcpymsg(fn,0,0X01);//更新文件名
						res=exf_copy(fcpymsg,srcpathname,dstpathname,*totsize,*cpdsize,fwmode);//复制文件
						*cpdsize+=finfo->fsize;//增加一个文件大小
					}
					srcpathname[srcpathlen]=0;//加入结束符
					dstpathname[dstpathlen]=0;//加入结束符	    
				} 
		    }	  
  			myfree(dstpathname);
 			myfree(srcpathname);
			myfree(finfo->lfname);
		}
 	}
	myfree(srcdir);
	myfree(dstdir);
	myfree(finfo);
    return res;	  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////




















