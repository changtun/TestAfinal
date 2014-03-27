Filterģ�齨����stpti�������ϣ���stb����section��ʽ��������ݵĽ�����
filterģ�����4���ļ����ֱ�Ϊ��
			filter.c sys_filter.h ��filterģ��ʵ�ֵ�C�ļ���Hͷ�ļ�
			test_filter.c test_sys_filter.h ��filterģ��Ĳ���C�ļ���H�ļ�

filterģ���ʵ�ֿ��Է�Ϊ�������object��ʵ�֣�channel & filter
channel ��Ӧstpti�������ϵ�slot������ʵ��һ���ض�PID��ͨ����filter���Լ򵥵Ķ�ӦΪstpti�������ϵ�filter,ʵ���ض�pid���棬��ͬtable��ͨ��������Ĺ���ԭ�����ͨ�����÷ֱ�Ϊ8��BYTE����match��mask��ʵ�֣���Ȼ��stpti�������ϵ�filter��һ���𣬼�filterģ�����ʵ����filter��������ʵ�ʵ�һ���򵥲���ʵ����˵��filterģ���ʹ�á��������ľ��������������ֵ˵����ο�sys_filter.hԴ��ĺ���˵����

1).��ʹ��ģ��ǰӦ���ó�ʼ��������
	S8 SFILTER_Init (void)��
2).����һ��channel��ʵ���ض�pid��ͨ����
	tSFILTER_SlotId SFILTER_Allocate_Channel (tSFILTER_SlotMode	sbufferMode,S16 task_priority, U32 millisecond )��
	����task_priority ��ֵ�����ڸ��ֲ�ͬ��ʵ�֣�Ӧ����ʵ�ʵĵ������ȷ����
3).ע���channel��Ӧ�Ļص���������һ��Ҳ�����ڲ���4֮�󣬲����Ƽ��� ����4֮ǰʵ�֡�
  void SFILTER_RegisterCallBack_Function (tSFILTER_SlotId SlotID, SFilterCallback_t *pCallbacks)��
  ����Դ˺����Ĳ���SFilterCallback_t *pCallbacks ����˵����
  typedef struct {
	tSFILTER_GetBufferFct		pfGetBuffer;  
	tSFILTER_FreeBufferFct		pfFreeBuffer;
	tSFILTER_SlotCallbackFct	pfCallBack;
}SFilterCallback_t;
//��channel���˵�һ��section֮�󽫵���pfGetBuffer����ȡ��һ���ڴ棨��̬���Ǿ�̬�ɴ˺���ʵ�־�����,�ѹ��˵õ���                                             section���ݿ�����ȡ�õ��ڴ��С�Ȼ����pfCallBack��������pfCallBack����ֱ�ӽ���������һ��Ϊ�˼ӿ���˺ͽ�������ٶȣ�pfCallBack����һ��ֻ����Ӧ�Ľ���task����һ����Ϣ������Ϣ������֮ǰ����������Ϣ�а�����������Ҫ�����ݣ�����4�����ݱز����٣�pid,lenth��buffer address,tSFILTER_FreeBufferFct pfFreeBuffer;����tSFILTER_FreeBufferFct pfFreeBuffer�ڽ���������֮���ɽ����������ã��ͷ���pfGetBufferȡ�õ��ڴ档��������������Ϊ����ָ������ݳ�Ա������ΪNULL��

4).���ض���channel����һ�����߶��filter,���ú���ΪtSFILTER_FilterId SFILTER_Allocate_Filter (tSFILTER_SlotId Slot,tSFILTER_FliterMode sfilterMode)��
  ����tSFILTER_SlotId SlotΪSFILTER_Allocate_Channel()�����ĳɹ�����ֵ( >=0 )������tSFILTER_FliterMode sfilterModeΪfilter����ģʽ��
  eSFILTER_MODE_HW����eSFILTER_MODE_SW��eSFILTER_MODE_HWģʽʱƥ���ֽڵļ����stpti��Ӳ��������ȥ��ɣ�eSFILTER_MODE_SW��filter�����ȥ��ɣ�Ϊ�˼ӿ��ѱ���ٶȣ�����Ƽ�����eSFILTER_MODE_HWģʽ��eSFILTER_MODE_SWģʽֻ��Ϊ����չ����Ҫ������tSFILTER_FliterMode sfilterMode ������Ӧÿһ��channelӦͳһ������ĳһ��channel�����filter��ΪeSFILTER_MODE_HWģʽ���߶�ΪeSFILTER_MODE_SWģʽ��
5)S8 SFILTER_SetFilter (tSFILTER_SlotId        Slot,
							 tSFILTER_FilterId      Filter,							
							 U8               *FilterData,
							 U8                *FilterMask,
							 U8 			not_equal_byte_index);
	����SlotӦ�뺯��SFILTER_Allocate_Filter (tSFILTER_SlotId Slot,tSFILTER_FliterMode sfilterMode)���еĲ���Slotһ�¡�
	����FilterӦΪ����SFILTER_Allocate_Filter (tSFILTER_SlotId Slot,tSFILTER_FliterMode sfilterMode)�ĳɹ�����ֵ( >=0).
	����not_equal_byte_index��ʾҪ���˵�section��ƥ���ֽ���ƥ���ֵĹ�ϵ����ͬ(not_equal_byte_index = FALSE)�����෴(not_equal_byte_index =TRUE)
6).����S8 SFILTER_SetPid (tSFILTER_SlotId Slot,unsigned int ReqPid)�����ö�Ӧchannel��PID,ͬʱ������channel��ʼ�������ݡ�


7).����S8 SFILTER_Control_Channel (tSFILTER_SlotId Slot,tSFILTER_Ctrl Ctrl)����������channel״̬��
  ����tSFILTER_Ctrl Ctrl��Ϊ��eSFILTER_CTRL_DISABLE  �ݶ�channel�����ݹ���
							  eSFILTER_CTRL_ENABLE   ���¿�ʼchannel�Ĺ���
							  eSFILTER_CTRL_RESET    ���channel�����ݻ������������¿�ʼ�������ݡ�
8).����S8 SFILTER_Control_Filter (tSFILTER_FilterId Filter,tSFILTER_Ctrl Ctrl)����������filter״̬��
   ����tSFILTER_Ctrl Ctrl��Ϊ��eSFILTER_CTRL_DISABLE  ��ͣfilter���ݹ���
							  eSFILTER_CTRL_ENABLE   ���¿�ʼfilter�Ĺ���
	����filterû�����ݻ������ĸ�����Բ���tSFILTER_Ctrl Ctrl������Ϊ eSFILTER_CTRL_RESET��
9).��һ��filter��ɹ�������󣬵���S8 SFILTER_Free_Filter (tSFILTER_FilterId Filter)������ɾ����filter.�ڵ����������ʱ����filter��״̬����Ϊable��Ҳ����Ϊ  disable��
10).��һ��channel��ɹ�������󣬲��������channel�����ӵ�filter�����ɹ�free֮�󣬿��Ե���S8 SFILTER_Free_Channel (tSFILTER_SlotId Slot)������ɾ�����     channel���ڵ����������ʱ����channel��״̬����Ϊable��Ҳ����Ϊ  disable��
11).�ڲ���Ҫfilterģ��ʱ(��ʵ�ʵ�stbϵͳ����������������ᷢ��,���Ǵ���)�����Ե��ú���S8 SFILTER_Terminate (void)���ر�filterģ�顣

 ��������������������г��ĺ����⣬filterģ�黹���û��ṩ�ļ����������⼸���������÷���鿴ͷ�ļ�Դ��ĺ���˵�����ֱ�Ϊ��
 tSFILTER_SlotId SFILTER_Check_DataPID(unsigned int ReqPid)��
 U8 SFILTER_NumFilterAssociate_Slot(tSFILTER_SlotId SlotID)��
 S8 SFILTER_Tasks_SetPriority(S16 NewPriority)��
 S8 SFILTER_Task_SetPriority(tSFILTER_SlotId Slot, S16 NewPriority)��
 U8  SFILTER_Tasks_Resume(void)��


