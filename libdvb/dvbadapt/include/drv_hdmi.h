#ifndef __DRV_HDMI_H_
#define __DRV_HDMI_H_
#define HI_HDMI 0
#if HI_HDMI
#include "hi_unf_hdmi.h"

HI_S32 HIADP_HDMI_Init(HI_UNF_HDMI_ID_E enHDMIId, HI_UNF_ENC_FMT_E enWantFmt);
HI_S32 HIADP_HDMI_DeInit(HI_UNF_HDMI_ID_E enHDMIId);
HI_S32 HIADP_HDMI_SetAdecAttr(HI_UNF_SND_INTERFACE_E enInterface, HI_UNF_SAMPLE_RATE_E enRate);
#endif
#endif /* __DRV_HDMI_H_ */


