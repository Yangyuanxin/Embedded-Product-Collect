#include "menu_filebrowse.h"
#include "menucontrol.h"
#include "menu_setting.h"
#include "cardreader.h"

UINT bww; //测试用
char buf[100]; //测试用

//在当前版本中并用不到下面这个函数，因为这个函数改了好几个版本比较经典，故保留下来供后面参考。
//后面特定文件的浏览，都可从下面这个函数的基础上修改得到。
uint8_t File_Perview(void)
{			
	//******************************初始化函数内部参数******************************//	 	   
	u8 res_touch;   //触摸屏的按键
	u8 res_key;     //按键的按键
	u8 rval=0;			//返回值	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
 	_btn_obj* rbtn;		//返回按钮控件
  _filelistbox_obj * flistbox;  //文件夹
	_filelistbox_list * filelistx; 	//文件 
	
  //******************************创建文件列表******************************//
	app_filebrower((u8*)"磁盘",0X07);//创建文件浏览窗框
  flistbox = filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		//配置显示那种扩展名的文件，可在filelistbox.h文件中查看，若配置为0xff则可显示所有类型的文件，类似于配置GPIO引脚
		flistbox->fliter=0xff;//FLBOX_FLT_GCODE;//|FLBOX_FLT_TEXT|FLBOX_FLT_LRC;	//查找文本文件
		filelistbox_add_disk(flistbox);					//添加磁盘路径(在filelist启动前,必须执行一次该函数)
 		filelistbox_draw_listbox(flistbox);     //绘制文件列表
	} 	
	//******************************创建返回按钮******************************//
	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮	
	if(rbtn==NULL)rval=1;	//没有足够内存够分配
	else
	{																				
	 	rbtn->caption=(u8*)"返回";//返回 
	 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=RED;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);//画按钮
	}
	//******************************按键消息处理******************************//
   	while(rval==0)
	{
		tp_dev.scan(0) ;    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5); //消抖
		filelistbox_check(flistbox,&in_obj);    //扫描文件，如果是文件夹的话则打开文件夹
		if(flistbox->dbclick==0X81)//双击文件了
		{
			flistbox->dbclick =0X00; //清除双击文件的状态
			//在这如何修改都不能实现，双击目标文件使之没有反应，暂时通过修改
			//GUI库filelistbox.c文件中的276行（即filelistbox_check(flistbox,&in_obj);函数）
			//在双击目标文件时，使之不做任何处理。后面加其它功能时记得加回来。
			//原行：if(filelistbox->dbclick==0X81&&filelistbox->list!=NULL)filelist_delete(filelistbox);//删除之前的条目,释放内存
      //修改后：//if(filelistbox->dbclick==0X81&&filelistbox->list!=NULL)filelist_delete(filelistbox);//删除之前的条目,释放内存		
		}	
		res_touch = btn_check(rbtn,&in_obj);		//扫描返回触摸屏的按键结果
		res_key = KEY_Scan(0);                  //返回按键扫描的结果
		if(res_touch==1)  //处理屏幕返回按键消息
		{
			if(((rbtn->sta&0X80)==0))//按钮状态改变了  //此处有个bug：理论上我按KEY0不应该进入该if语句，但实际会进入。不过达到了我要的效果
			{
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
				if(filelistx->type==FICO_DISK)//已经不能再往上了,返回主界面
				{				 
//					btn_delete(rbtn);				//删除按钮	 //不加此句功能也正常，加此句为了释放按钮占的内存
					//mui_init(1); 	//Main UI 初始化
					currentMenu = main_interface;
					break;
				}else filelistbox_back(flistbox);//退回上一层目录	 
			}				
		}
		if(res_key==KEY0_PRESS) //返回主菜单
		{
			res_key = 0;
			currentMenu = main_interface;break;
//			filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
//			do
//			{		
//				filelistbox_back(flistbox);//退回上一层目录
//				filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
//			}while(filelistx->type!=FICO_DISK); //只要当前目录的类型定义不是磁盘，就一直返回
//			btn_delete(rbtn);				//删除按钮	 //不加此句功能也正常，加此句为了释放按钮占的内存
//			mui_init(1); 	//Main UI 初始化返回主界面
		}
	}
	filelistbox_delete(flistbox);	//删除filelist，释放filelist占的内存
	//下面这条语句加在这会使返回主界面后，在按钮区域处显示之前按钮的背景色，所以不能加在此处，
	//但为了释放其所占取的内存将其放到前面实现。因为这个函数在删除按钮后会恢复背景色。
	btn_delete(rbtn);				//删除按钮，释放按钮占的内存 	 	   	
	return rval; 	
}

//浏览磁盘目录，并显示其中的.gcode文件。
//返回值是否双击了gcode文件：0，双击了；其他值，未双击。
uint8_t GcodeFile_Browse(void)
{			
	//******************************初始化函数内部参数******************************//	 	   
	u8 res_touch;   //触摸屏的按键
	u8 res_key;     //按键的按键
	u8 res;         //记录窗体中按下的按键：1，确认键；2，返回键
	u8 rval=0;			//
 	_btn_obj* rbtn;		//返回按钮控件
  _filelistbox_obj * flistbox;  //文件夹
	_filelistbox_list * filelistx; 	//文件 
	
//	u8 selx=0XFF;	
	u8 *pname=0;
// 	_filelistbox_list * filelistx;	
	
  //******************************创建文件列表******************************//
	app_filebrower((u8*)"磁盘",0X07);//创建文件浏览窗框
  flistbox = filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		//配置显示那种扩展名的文件，可在filelistbox.h文件中查看，若配置为0xff则可显示所有类型的文件，类似于配置GPIO引脚
		flistbox->fliter=FLBOX_FLT_GCODE;     	//查找gcode文件
		filelistbox_add_disk(flistbox);					//添加磁盘路径(在filelist启动前,必须执行一次该函数)
 		filelistbox_draw_listbox(flistbox);     //绘制文件列表
	} 	
	//******************************创建返回按钮******************************//
	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮	
	if(rbtn==NULL)rval=1;	//没有足够内存够分配
	else
	{																				
	 	rbtn->caption=(u8*)"返回";//返回 
	 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=RED;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);//画按钮
	}
	//******************************按键消息处理******************************//
   	while(rval==0)
	{
		tp_dev.scan(0) ;    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5); //消抖
		filelistbox_check(flistbox,&in_obj);    //扫描文件，如果是文件夹的话则打开文件夹
		if(flistbox->dbclick==0X81)//双击文件了
		{
			flistbox->dbclick =0X00; //清除双击文件的状态
			//currentMenu = main_interface;
			//return 0; //返回值，表示双击文件了。
			res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)"是否打印",12,0,0X03,0);
			if(res==1) 
			{
//				rval=f_opendir(&ebookdir,(const TCHAR*)flistbox->path); //打开选中的目录
//				if(rval)break;
//				dir_sdi(&ebookdir,flistbox->findextbl[flistbox->selindex-flistbox->foldercnt]);
//				rval=f_readdir(&ebookdir,&ebookinfo);//读取文件信息
//				if(rval)break;//打开成功    
//				fn=(u8*)(*ebookinfo.lfname?ebookinfo.lfname:ebookinfo.fname);
//				pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//申请内存
				//currentMenu = sysset_play;
				
				filelistx = filelist_search(flistbox->list,flistbox->selindex);
				strcpy((char *)card.filename,(const char*)filelistx->name);
				pname=gui_memin_malloc(strlen((const char*)filelistx->name)+strlen((const char*)flistbox->path)+2);
				if(pname)  
				{	
					pname=gui_path_name(pname,flistbox->path,filelistx->name);
					rval=f_open(&card.fgcode,(const TCHAR*)pname,FA_READ);
					f_read(&card.fgcode, buf,100,&bww); //读取gcode文件内容测试，成功在lcd屏幕上显示了gcode的文件内容。
						
					gui_memin_free(pname);
					if (rval==0)
					{
						currentMenu = print_interface;
						//redraw_screen=true;
						//windows_flag=false;	
						filelistbox_delete(flistbox);												
						card_startFileprint();
						//starttime=millis();
					}
				}	
				break;
			}
			else
			{
					filelistbox_draw_listbox(flistbox); //重画下文件列表，否则界面中间判断是否打印的窗体不会消失。现在只知道这样可以解决，后面有必要在做优化
          //break;	//在这不能用break语句，若用break语句而不用上面那条语句，界面会直接跳到磁盘界面，这样会感觉很奇葩			
			}
		}	
		res_touch = btn_check(rbtn,&in_obj);		//扫描返回触摸屏的按键结果
		res_key = KEY_Scan(0);                  //返回按键扫描的结果
		if(res_touch==1)  //处理屏幕返回按键消息
		{
			if(((rbtn->sta&0X80)==0))//按钮状态改变了 
			{
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
				if(filelistx->type==FICO_DISK)//已经不能再往上了,返回主界面
				{				 
					currentMenu = main_interface; //将当前界面指定到主界面
					break;
				}else filelistbox_back(flistbox);//退回上一层目录	 
			}				
		}
		if(res_key==KEY0_PRESS) //返回主菜单
		{
			res_key = 0;
			currentMenu = main_interface; //这里直接将当前界面指定到主界面即可实现返回主界面的功能
			break;
		}
	}
	filelistbox_delete(flistbox);	//删除filelist，释放filelist占的内存
	btn_delete(rbtn);				//删除按钮，释放按钮占的内存 	 	   	
	return rval; 	
}	


//触摸了屏幕任意键
//返回值：1，碰触了屏幕任何区域；0，未触碰屏幕
uint8_t tp_touched(void)
{
	u8 res=0;
	tp_dev.scan(0);    
	if(tp_dev.sta&TP_PRES_DOWN)//有按键按下
	{ 
     res = 1;
	} 
	while(res)//等待按键松开
	{
		tp_dev.scan(0);  
		if((tp_dev.sta&TP_PRES_DOWN)==0)break;
		delay_ms(5);
	} 
	return res;
}

//浏览磁盘目录，并显示其中的gcode文件和图片文件。
//双击了gcode文件，进入打印处理；双击了图片文件，显示图片内容，并按屏幕任意键返回。
//创建该函数目的：用户在打印模型时只有打印文件，而不知道模型是什么样的，不确定要不要打印。
//有了该函数用户可以将打印文件和模型图片放在一起，因此在后面打印模型时就不会那么茫然。
//返回值：未用到
uint8_t GcoPicFile_Browse(void)
{			
	//******************************初始化函数内部参数******************************//	 	   
	u8 res_touch;   //触摸屏的按键
	u8 res_key;     //按键的按键
	u8 res;         //记录窗体中按下的按键：1，确认键；2，返回键
	u8 rval=0;			//
 	_btn_obj* rbtn;		//返回按钮控件
  _filelistbox_obj * flistbox;  //文件夹
	_filelistbox_list * filelistx; 	//文件 
	
//	u8 selx=0XFF;	
	u8 *pname=0;
	u8 filetype=0;	 //文件类型
	u8 gcode_flag=0;   //gcode文件标志
	u8 picture_flag=0; //图片文件标志
// 	_filelistbox_list * filelistx;	
	
  //******************************创建文件列表******************************//
	app_filebrower((u8*)"磁盘",0X07);//创建文件浏览窗框
  flistbox = filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
	if(flistbox==NULL)rval=1;							//申请内存失败.
	else  
	{
		//配置显示那种扩展名的文件，可在filelistbox.h文件中查看，若配置为0xff则可显示所有类型的文件，类似于配置GPIO引脚
		flistbox->fliter=FLBOX_FLT_GCODE|FLBOX_FLT_PICTURE;     	//查找图片文件
		filelistbox_add_disk(flistbox);					//添加磁盘路径(在filelist启动前,必须执行一次该函数)
 		filelistbox_draw_listbox(flistbox);     //绘制文件列表
	} 	
	//******************************创建返回按钮******************************//
	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮	
	if(rbtn==NULL)rval=1;	//没有足够内存够分配
	else
	{																				
	 	rbtn->caption=(u8*)"返回";//返回 
	 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=RED;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);//画按钮
	}
	//******************************按键消息处理******************************//
   	while(rval==0)
	{
		tp_dev.scan(0) ;    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5); //消抖
		filelistbox_check(flistbox,&in_obj);    //扫描文件，如果是文件夹的话则打开文件夹
		if(flistbox->dbclick==0X81)//双击文件了
		{
				flistbox->dbclick =0X00; //清除双击文件的状态//设置非文件浏览状态
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
				//strcpy((char *)card.filename,(const char*)filelistx->name);
				pname=gui_memin_malloc(strlen((const char*)filelistx->name)+strlen((const char*)flistbox->path)+2); //为带路径文件名申请内存
				if(pname==NULL)break;	//申请失败
				pname=gui_path_name(pname,flistbox->path,filelistx->name); //获取带路径文件名
				filetype = f_typetell(pname); //获取文件类型
				if(filetype==T_GCODE)//gcode文件
				{
					gcode_flag = 1;    //gcode标志位置位
			  }
				else if(filetype==T_GIF|T_JPEG|T_JPG|T_BMP)  //图片格式文件
				{
					picture_flag = 1;  //图片标志位置位
				}
				else break;
       if(gcode_flag == 1)   //对gcode文件的处理
			 {
				 	gcode_flag = 0; //清除gcode标志位
				  res = window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)"是否打印",12,0,0X03,0); //弹出“是否打印”窗框
					if(res==1) //按下了弹窗的确认键
					{
						if(pname)  //带路径的文件名pname非空
						{	
							//pname = gui_path_name(pname,flistbox->path,filelistx->name); //获取带路径的文件名
							rval=f_open(&card.fgcode,(const TCHAR*)pname,FA_READ); //打开当前文件								
							gui_memin_free(pname); //释放pname的内存
							if (rval==FR_OK) //文件打开成功
							{
								currentMenu = print_interface; //将当前菜单指向到打印界面
								//filelistbox_delete(flistbox);  //删除文件列表，释放内存	//break跳出后会在最后执行该语句，因此这可以省略											
								card_startFileprint(); //开启SD卡文件打印
								//starttime=millis();
							}
						}	
						break; //跳出循环
					}
					else
					{
							filelistbox_draw_listbox(flistbox); //重画下文件列表，否则界面中间判断是否打印的窗体不会消失。现在只知道这样可以解决，后面有必要在做优化
							//break;	//在这不能用break语句，若用break语句而不用上面那条语句，界面会直接跳到磁盘界面，这样会感觉很奇葩			
					}
			 }
       if(picture_flag==1)	//对图片文件进行处理		 
			 {
				 picture_flag=0;//清除图片标志位 
				 LCD_Clear(0x00);//黑屏 
					if(filetype==T_GIF) //图片文件为GIF格式
						gui_show_string(pname,5,5,lcddev.width-5,gui_phy.tbfsize,gui_phy.tbfsize,RED);	//显示GIF名字
					ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);			//播放这个图片
					if(filetype!=T_GIF)  //其它格式的图片文件
						gui_show_string(pname,5,5,lcddev.width-5,gui_phy.tbfsize,gui_phy.tbfsize,RED);	//显示图片名字						  
					//gui_memin_free(pname);			//释放内存 
					//gui_show_string("按任意键返回。。。",5,lcddev.height-40,lcddev.width-5,gui_phy.tbfsize,gui_phy.tbfsize,RED);	//显示GIF名字
					gui_show_strmid(0,100,lcddev.width,lcddev.height,RED,16,(u8 *)"按任意键返回。。。");	//显示按任意键退出
					while(1)//等待任意触摸屏按键按下
					{
						if(tp_touched())//触碰屏幕任意位置
						{ 
							//flistbox->dbclick =0X00;
							app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[1][gui_phy.language],0X07);//选择目标文件,并得到目标数量
							btn_draw(rbtn);			//画按钮
							filelistbox_rebuild_filelist(flistbox);//重建flistbox
							break; //跳出循环
						}
					}				
			 }
		 }	
		res_touch = btn_check(rbtn,&in_obj);		//扫描返回触摸屏按扭检测结果
		res_key = KEY_Scan(0);                  //返回按键扫描的结果
		if(res_touch==1)  //处理屏幕返回按键消息
		{
			if(((rbtn->sta&0X80)==0))//按钮状态改变了 
			{
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
				if(filelistx->type==FICO_DISK)//已经不能再往上了,返回主界面
				{				 
					currentMenu = main_interface; //将当前界面指定到主界面
					break;
				}else filelistbox_back(flistbox);//退回上一层目录	 
			}				
		}
		if(res_key==KEY0_PRESS) //返回主菜单
		{
			res_key = 0;
			currentMenu = main_interface; //这里直接将当前界面指定到主界面即可实现返回主界面的功能
			break;
		}
	}
	gui_memin_free(pname);			//释放内存
	filelistbox_delete(flistbox);	//删除filelist，释放filelist占的内存
	btn_delete(rbtn);				//删除按钮，释放按钮占的内存 	 	   	
	return rval; 	
}	


