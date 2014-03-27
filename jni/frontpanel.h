#ifndef __FRONT_PANEL_H__
#define __FRONT_PANEL_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


bool _register_front_panel_Class(JNIEnv *env );

/*LogNum >= 0  => "CXxXX"   LogNum < 0   => "----"  */
int Front_Panel_Dispaly(JNIEnv *env, int LogNum );


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __FRONT_PANEL_H__ */