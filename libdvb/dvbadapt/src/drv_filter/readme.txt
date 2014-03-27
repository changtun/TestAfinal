Filter模块建立在stpti驱动层上，供stb中以section格式传输的数据的解析。
filter模块包含4个文件，分别为：
			filter.c sys_filter.h ：filter模块实现的C文件及H头文件
			test_filter.c test_sys_filter.h ：filter模块的测试C文件及H文件

filter模块的实现可以分为针对两个object的实现：channel & filter
channel 对应stpti驱动层上的slot，可以实现一个特定PID的通道。filter可以简单的对应为stpti驱动层上的filter,实现特定pid下面，不同table的通道，具体的过滤原则可以通过设置分别为8个BYTE长的match和mask来实现，当然和stpti驱动层上的filter有一点差别，即filter模块可以实现软filter。下面以实际的一个简单操作实例来说明filter模块的使用。（函数的具体参数，及返回值说明请参考sys_filter.h源码的函数说明）

1).在使用模块前应调用初始化函数：
	S8 SFILTER_Init (void)；
2).申请一个channel来实现特定pid的通道。
	tSFILTER_SlotId SFILTER_Allocate_Channel (tSFILTER_SlotMode	sbufferMode,S16 task_priority, U32 millisecond )；
	参数task_priority 的值依赖于各种不同的实现，应根据实际的的情况来确定。
3).注册此channel相应的回调函数，这一步也可以在步骤4之后，不过推荐在 步骤4之前实现。
  void SFILTER_RegisterCallBack_Function (tSFILTER_SlotId SlotID, SFilterCallback_t *pCallbacks)；
  下面对此函数的参数SFilterCallback_t *pCallbacks 单独说明：
  typedef struct {
	tSFILTER_GetBufferFct		pfGetBuffer;  
	tSFILTER_FreeBufferFct		pfFreeBuffer;
	tSFILTER_SlotCallbackFct	pfCallBack;
}SFilterCallback_t;
//此channel过滤到一个section之后将调用pfGetBuffer函数取得一块内存（动态还是静态由此函数实现决定）,把过滤得到的                                             section数据拷贝到取得的内存中。然后交由pfCallBack函数处理，pfCallBack可以直接解析，不过一般为了加快过滤和解析表的速度，pfCallBack函数一般只给对应的解析task发送一个消息（此消息队列在之前创建），消息中包含解析所需要的数据，其中4个数据必不可少，pid,lenth，buffer address,tSFILTER_FreeBufferFct pfFreeBuffer;其中tSFILTER_FreeBufferFct pfFreeBuffer在解析完数据之后，由解析函数调用，释放由pfGetBuffer取得的内存。所以这三个类型为函数指针的数据成员都不能为NULL。

4).给特定的channel分配一个或者多个filter,调用函数为tSFILTER_FilterId SFILTER_Allocate_Filter (tSFILTER_SlotId Slot,tSFILTER_FliterMode sfilterMode)；
  参数tSFILTER_SlotId Slot为SFILTER_Allocate_Channel()函数的成功返回值( >=0 )。参数tSFILTER_FliterMode sfilterMode为filter过滤模式：
  eSFILTER_MODE_HW或者eSFILTER_MODE_SW。eSFILTER_MODE_HW模式时匹配字节的检查由stpti的硬件过滤器去完成，eSFILTER_MODE_SW由filter的软件去完成，为了加快搜表的速度，这个推荐都用eSFILTER_MODE_HW模式，eSFILTER_MODE_SW模式只是为了扩展的需要。另外tSFILTER_FliterMode sfilterMode 参数对应每一个channel应统一，即对某一个channel分配的filter都为eSFILTER_MODE_HW模式或者都为eSFILTER_MODE_SW模式。
5)S8 SFILTER_SetFilter (tSFILTER_SlotId        Slot,
							 tSFILTER_FilterId      Filter,							
							 U8               *FilterData,
							 U8                *FilterMask,
							 U8 			not_equal_byte_index);
	参数Slot应与函数SFILTER_Allocate_Filter (tSFILTER_SlotId Slot,tSFILTER_FliterMode sfilterMode)；中的参数Slot一致。
	参数Filter应为函数SFILTER_Allocate_Filter (tSFILTER_SlotId Slot,tSFILTER_FliterMode sfilterMode)的成功返回值( >=0).
	参数not_equal_byte_index表示要过滤的section的匹配字节与匹配字的关系是相同(not_equal_byte_index = FALSE)还是相反(not_equal_byte_index =TRUE)
6).调用S8 SFILTER_SetPid (tSFILTER_SlotId Slot,unsigned int ReqPid)；设置对应channel的PID,同时启动此channel开始过滤数据。


7).调用S8 SFILTER_Control_Channel (tSFILTER_SlotId Slot,tSFILTER_Ctrl Ctrl)函数，控制channel状态。
  参数tSFILTER_Ctrl Ctrl可为：eSFILTER_CTRL_DISABLE  暂定channel的数据过滤
							  eSFILTER_CTRL_ENABLE   重新开始channel的过滤
							  eSFILTER_CTRL_RESET    清空channel的数据缓冲区，并重新开始过滤数据。
8).调用S8 SFILTER_Control_Filter (tSFILTER_FilterId Filter,tSFILTER_Ctrl Ctrl)函数，控制filter状态。
   参数tSFILTER_Ctrl Ctrl可为：eSFILTER_CTRL_DISABLE  暂停filter数据过滤
							  eSFILTER_CTRL_ENABLE   重新开始filter的过滤
	由于filter没有数据缓冲区的概念，所以参数tSFILTER_Ctrl Ctrl不可以为 eSFILTER_CTRL_RESET。
9).当一个filter完成过滤任务后，调用S8 SFILTER_Free_Filter (tSFILTER_FilterId Filter)；函数删除此filter.在调用这个函数时，此filter的状态可以为able，也可以为  disable。
10).当一个channel完成过滤任务后，并且与这个channel相连接的filter都被成功free之后，可以调用S8 SFILTER_Free_Channel (tSFILTER_SlotId Slot)；函数删除这个     channel。在调用这个函数时，此channel的状态可以为able，也可以为  disable。
11).在不需要filter模块时(在实际的stb系统中这种情况基本不会发生,除非待机)，可以调用函数S8 SFILTER_Terminate (void)；关闭filter模块。

 除了在上面操作例程中列出的函数外，filter模块还对用户提供的几个函数，这几个函数的用法请查看头文件源码的函数说明，分别为：
 tSFILTER_SlotId SFILTER_Check_DataPID(unsigned int ReqPid)；
 U8 SFILTER_NumFilterAssociate_Slot(tSFILTER_SlotId SlotID)；
 S8 SFILTER_Tasks_SetPriority(S16 NewPriority)；
 S8 SFILTER_Task_SetPriority(tSFILTER_SlotId Slot, S16 NewPriority)；
 U8  SFILTER_Tasks_Resume(void)；


