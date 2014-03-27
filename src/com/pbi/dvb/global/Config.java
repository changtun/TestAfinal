package com.pbi.dvb.global;

import android.view.KeyEvent;

public class Config
{
    // test hardware specification
    private static final boolean IS_TEST_SPECIFICATION = false;
//    public static final String PVR_SERVER="http://61.58.63.16/PVR";��������
 //   public static final String PVR_SERVER="http://192.168.30.162/PVR/";//pbi test address
    public static final String PVR_SERVER="http://192.168.30.162/ppp";//pbi test address
    
    // update url
    public static final int CONNECTION_SUCCESS = 200;
    
    public static final int NO_PROGRAMME = 400;
    
    // default password
    public static final String PROGRAMME_LOCK = "3333";
    
    public static final String SUPER_PASSWORD = "9637";
    
    // ca mail,ippv,finger print message.
    public static final int CA_MAIL = 411;
    
    public static final int CA_NOTICE = 412;
    
    public static final int CA_IPP_NOTICE = 413;
    
    public static final int CA_IPP_UPDATE = 414;
    
    public static final int CA_FP = 415;
    
    public static final int CA_FORCE_SWITCH = 416;
    
    public static final int CA_OSD_SHOW = 417;
    
    public static final int CA_PARENTAL_REPLY = 418;
    
    public static final int CA_MSG_PIN_CHECK = 419;
    
    public static final int CA_MSG_PIN_CHANGE = 420;
    
    public static final int CA_MSG_STATE_INFO = 421;
    
    public static final int CA_MSG_PPID_LIST = 422;
    
    public static final int CA_MSG_PPID_DETAIL = 423;
    
    public static final int CA_MSG_PPID_NOTIFY = 424;
    
    public static final int CA_MSG_PPID_DETAIL_NOTIFY = 425;
    
    public static final int CA_MSG_PLAY_CONTROL_MODIFY = 426;
    
    public static final int CA_MSG_PLAY_CONTROL_QUERY = 427;
    
    public static final int CA_IPPV_QUERY_REPLY = 428;
    
    public static final int CA_MSG_CARD_Status = 429;
    
    public static final int CA_MSG_PPV_LIST = 430;
    
    public static final int CA_MSG_IPPV_LIST = 431;
    
    public static final int CA_OSD_HIGH_SPEED = 9;
    
    public static final int CA_OSD_INTERMEDIATE_SPEED = 3;
    
    public static final int CA_OSD_LOW_SPEED = 1;
    
    // pvr message
    public static final int PVR_TMS_PROGRESS = 500;
    
    public static final int PVR_REC_PROGRESS = 501;
    
    public static final int PVR_PLAY_EOF = 502;
    
    public static final int PVR_PLAY_SOF = 503;
    
    public static final int PVR_PLAY_ERROR = 504;
    
    public static final int PVR_PLAY_REACH_REC = 505;
    
    public static final int PVR_REC_DISKFULL = 506;
    
    public static final int PVR_REC_ERROR = 507;
    
    public static final int PVR_REC_OVER_FIX = 508;
    
    public static final int PVR_REC_REACH_PLAY = 509;
    
    public static final int PVR_REC_DISK_SLOW = 510;
    
    //switch service message 
    public static final int RED_KEY_MESSAGE = 900;
    public static final int UP_SWITCH_SERVICE = 901;
    public static final int DOWN_SWITCH_SERVICE = 902;
    public static final int NUMBER_SWITCH_SERVICE = 903;
    public static final int HIDE_BANNER_MESSAGE = 904;
    public static final int SHOW_BANNER_MESSAGE = 905;
    
    /*
     * 对于 installation 来说自动搜索, 快速搜索, 网络搜索没有什么区别. 都是先搜主频点 NIT, 再根据主频点 NIT 中描述的频点列表进行搜索. 若搜索 NIT 失败则退出搜索.
     * 唯一的区别就是指定的主频点可能不同, 主频点数目也不相同.但这个区别对于 installation 来说处理方式是相同的. 之所以这里还是添加了这三种类型完全是为了上层使用方便
     */
    
    public static final int Install_Search_Type_eAUTO_SEARCH = 0;/* 自动搜索 */
    
    public static final int Install_Search_Type_eQUICK_SEARCH = 1;/* 快速搜索 */
    
    public static final int Install_Search_Type_eNIT_SEARCH = 2;/* 网络搜索 */
    
    public static final int Install_Search_Type_eALL_SEARCH = 3;/* 全频搜索 */
    
    public static final int Install_Search_Type_eTP_SEARCH = 4;/* 单频点搜索 */
    
    public static final int Install_Search_Type_eOTHER_SEARCH = 5;/* 未定义 */
    
    /* QAM 调制类型 */
    public static final int DVBCore_Modulation_eDVBCORE_MOD_16QAM = 0;
    
    public static final int DVBCore_Modulation_eDVBCORE_MOD_32QAM = 1;
    
    public static final int DVBCore_Modulation_eDVBCORE_MOD_64QAM = 2;
    
    public static final int DVBCore_Modulation_eDVBCORE_MOD_128QAM = 3;
    
    public static final int DVBCore_Modulation_eDVBCORE_MOD_256QAM = 4;
    
    public static final int DVBCore_Modulation_eDVBCORE_MOD_512QAM = 5;
    
    public static final int DVBCore_Modulation_eDVBCORE_MOD_QPSK = 6;
    
    public static final int DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBS = 0;
    
    public static final int DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBS2 = 1;
    
    public static final int DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBC = 2;
    
    public static final int DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBT = 3;
    
    public static final int DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DMBT = 4;
    
    public static final int DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_ANALOG = 5;
    
    public static final int DVBPlayer_Prog_Type_SERVICE = 0;
    
    public static final int DVBPlayer_Prog_Type_AVPID = 1;
    
    public static final int DVBPlayer_Prog_Type_AUDIO = 2;
    
    // service type
    public static final char SERVICE_TYPE_NOTYPE = 0xff;
    
    public static final char SERVICE_TYPE_UNKNOWN = 0x00;
    
    public static final char SERVICE_TYPE_TV = 0x01;
    
    public static final char SERVICE_TYPE_RADIO = 0x02;
    
    public static final char SERVICE_TYPE_TELETEXT = 0x03;
    
    public static final char SERVICE_TYPE_NVOD_REFER = 0x04;
    
    public static final char SERVICE_TYPE_NVOD_TIMESHIFT = 0x05;
    
    public static final char SERVICE_TYPE_MOSAIC = 0x06;
    
    public static final char SERVICE_TYPE_DATA_BRODCAST = 0x0C;
    
    // launcher uri
    public static final String DVB_TV_PROGRAMME = "television";
    
    public static final String DVB_RADIO_PROGRAMME = "broadcast";
    
    public static final String DVB_EPG = "epg";
    
    public static final String DVB_EIT = "eit";
    
    public static final String DVB_CHANNEL_SEARCH = "search";
    
    public static final String DVB_DIRECT_SEARCH = "direct_search";
    
    public static final String DVB_ALL_SEARCH = "all_search";
    
    public static final String DVB_FREQ_SETTING = "frequency_setting";
    public static final String DVB_BOOT_DEFAULT_CHANNEL = "boot_default_channel";
    
    public static final String DVB_CHANNEL_EDIT = "edit";
    
    public static final String DVB_CA = "ca";
    
    public static final String DVB_PVR_LIST = "pvr_list";
    
    public static final String DVB_PVR_PLAY_LIST = "pvr_play_list";
    
    public static final String DVB_CA_STATUS = "ca_status";
    
    public static final String DVB_CA_PWD_SETTINGS = "ca_password_settings";
    
    public static final String DVB_CA_GRADE = "ca_adult_grade";
    
    public static final String DVB_CA_AUTH = "ca_auth";
    
    public static final String DVB_CA_EMAIL = "ca_email";
    
    public static final String DVB_SYSTEM_INFO = "sys_info";
    
    // public static final String DVB_CA_ZIMU_CARD_FEEDING = "ca_zimu_card_feeding";
    public static final String DVB_CA_USER_VIEW = "ca_userview";
    
    public static final String DVB_FACTORY_TEST = "factory_test";
    
    // key code
    /** Key code constant: Play/Pause media key. */
    public static final int KEY_PLAY_PAUSE = KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE;// 85
    
    /** Key code constant: Sound track/Language key. */
    public static final int KEY_MUL_LANGUAGE = KeyEvent.KEYCODE_DEL;// 67
    
    /** Key code constant: Menu key. */
    public static final int KEY_EPG = KeyEvent.KEYCODE_MENU;// 82
    
    /** Key code constant: Search key. */
    public static final int KEY_SEARCH = 84;
    
    /* Start LinKang, 2013-12-20, The DELETE variable is quoted by other place */
    public static final int DELETE = -1;
    
    /* End LinKang, 2013-12-20, The DELETE variable is quoted by other place */
    
    /** Key code constant: Page Up key. */
    public static final int PAGE_UP = KeyEvent.KEYCODE_MEDIA_REWIND;
    
    /** Key code constant: Page Down key. */
    public static final int PAGE_DOWN = KeyEvent.KEYCODE_MEDIA_FAST_FORWARD;
    
    /** Key code constant: Red key/F1. */
    public static final int KEY_RED = KeyEvent.KEYCODE_MINUS;
    
    /** Key code constant: Green key/F2. */
    public static final int KEY_GREEN = KeyEvent.KEYCODE_EQUALS;
    
    /** Key code constant: Yellow key/F3. */
    public static final int KEY_YELLOW = KeyEvent.KEYCODE_ALT_LEFT;
    
    /** Key code constant: Blue key/F4. */
    public static final int KEY_BLUE = KeyEvent.KEYCODE_ALT_RIGHT;
    
    /** Key code constant: Favourite key. */
    public static final int KEY_FAV = 1177;
    
    /** Key code constant: Switch video/audio key. */
    public static final int KEY_SWITCH = KeyEvent.KEYCODE_CAMERA;
    
    public static final int KEY_REC = 9999;
    
    public static final int KEY_VOLUME_UP = 24;
    
    public static final int KEY_VOLUME_DOWN = 25;
    
    public static final int KEY_MUTE = 91;
    
    /** Key code constant: Fast change key. */
    public static final int KEY_FAST_CHANGE = KeyEvent.KEYCODE_POUND;
    
    /** Key code constant: Show key. */
    public static final int KEY_SHOW = KeyEvent.KEYCODE_STAR;
    
    // search progress message
    public static final int SIGNAL_INTERRUPT = 100;
    
    public static final int UPDATE_SEARCH_PROGRESS = 101;
    
    public static final int UPDATE_STRENGTH_QUALITY = 102;
    
    public static final int SIGLE_TP_SEARCH_COMPLETE = 110;
    
    public static final int SIGLE_TP_SEARCH_FAILED = 111;
    
    public static final int AUTO_SEARCH_COMPLETE = 120;
    
    public static final int AUTO_SEARCH_FAILED = 121;
    
    public static final int NIT_UPDATE = 122;
    
    // player message
    public static final int NOTIFICATION_CHILD_THREAD = 200;
    
    public static final int PLAYER_PROGRAM_INTERRUPT = 201;
    
    public static final int PLAYER_LOCK_TUNER_SUCCESS = 202;
    
    public static final int PLAYER_PMT_TIME_OUT = 203;
    
    public static final int PLAYER_START = 204;
    
    public static final int PLAYER_LANGUAGE = 205;
    
    public static final int AV_FRAME_UNNORMAL = 206;
    
    public static final int UPDATE_CA_MODE = 207;
    
    public static final int RECOVER_INITIAL_GRADE = 208;
    
    public static final int UPDATE_EPG_INFO = 209;
    
    public static final int UPDATE_PMT_PID = 210;
    
    // ca mode
    public static final char CA_NO_SCRAMBLING = 0x00;
    
    public static final char CA_SCRAMBLING = 0x01;
    
    // ca Message
    public static final int CTI_CA_E00 = 300;
    
    public static final int CTI_CA_E01 = 301;
    
    public static final int CTI_CA_E02 = 302;
    
    public static final int CTI_CA_E03 = 303;
    
    public static final int CTI_CA_E04 = 304;
    
    public static final int CTI_CA_E05 = 305;
    
    public static final int CTI_CA_E06 = 306;
    
    public static final int CTI_CA_E07 = 307;
    
    public static final int CTI_CA_E08 = 308;
    
    public static final int CTI_CA_E10 = 310;
    
    public static final int CTI_CA_E11 = 311;
    
    public static final int CTI_CA_E12 = 312;
    
    public static final int CTI_CA_E13 = 313;
    
    public static final int CTI_CA_E14 = 314;
    
    public static final int CTI_CA_E15 = 315;
    
    public static final int CTI_CA_E16 = 316;
    
    public static final int CTI_CA_E17 = 317;
    
    public static final int CTI_CA_E18 = 318;
    
    public static final int CTI_CA_E19 = 319;
    
    public static final int CTI_CA_E20 = 320;
    
    public static final int CTI_CA_E21 = 321;
    
    public static final int CTI_CA_E22 = 322;
    
    public static final int CTI_CA_E23 = 323;
    
    public static final int CTI_CA_E24 = 324;
    
    public static final int CTI_CA_E25 = 325;
    
    public static final int CTI_CA_E26 = 326;
    
    public static final int CTI_CA_E27 = 327;
    
    public static final int CTI_CA_E28 = 328;
    
    public static final int CTI_CA_E29 = 329;
    
    public static final int CTI_CA_E30 = 330;
    
    public static final int CTI_CA_E40 = 340;
    
    public static final int CTI_CA_E41 = 341;
    
    public static final int CTI_CA_E42 = 342;
    
    public static final int CTI_CA_E43 = 343;
    
    public static final int CTI_CA_E44 = 344;
    
    public static final int CTI_CA_E45 = 345;
    
    public static final int CTI_CA_E46 = 346;
    
    public static final int CTI_CA_E47 = 347;
    
    public static final int CTI_CA_E50 = 350;
    
    public static final int CTI_CA_E51 = 351;
    
    public static final int CTI_CA_E52 = 352;
    
    public static final int CTI_CA_E53 = 353;
    
    public static final int CTI_CA_E54 = 354;
    
    public static final int CTI_CA_E55 = 355;
    
    public static final int CTI_CA_E56 = 356;
    
    public static final int CTI_CA_E57 = 357;
    
    // epg progress
    public static final char EPG_PLAYING_INFO = 10;
    
    public static final char EPG_EVENT_LIST = 11;
    
    public static final char EPG_LAYOUT_TIMER = 12;
    
    public static final int UPDAE_EVENT_PVR_STATUS = 13;
    
    // For PVR
    public static final String PVR_TASK_COMPLETE = "com.pbi.dvb.pvrRecordTaskFinished";
    
    public static final String PVR_TASK_START_RECORD = "com.pbi.dvb.pvrRecordTaskStart";
    
    public static final String ACTION_ALERT_CURRENT_TIME = "com.pbi.dvb.alertCurrentTime";
    
    // Add For User Behavior Survey BEGIN
    public static final String USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME = "project";
    
    public static final String USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME = "action";
    
    public static final String USER_BEHAVIOR_SURVEY_GLOBAL_TAG_NAME_LOGICAL_NUMBER = "logical_number";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_PROJECT_VALUE = "zapp";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_VIEW_TIME = "view_time";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_VIEW_LENGTH = "view_length";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_CHANGE_CHANNEL = "change_channel";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_SIGNAL_QUALITY = "signal_quality";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_SIGNAL_LEVEL = "signal_level";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_FREQUENCY = "signal_frequency";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_SYMBOL = "signal_symbol";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_QUALITY = "signal_quality";
    
    public static final String USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_LEVEL = "signal_level";
    
    public static final String USER_BEHAVIOR_UNKONW_VALUE = "unkonw";
    
    public static final String USER_BEHAVIOR_SURVEY_SHIFT_PROJECT_VALUE = "shift";
    
    public static final String USER_BEHAVIOR_SURVEY_SHIFT_ACTION_NAME_TIME_SHIFT_SPEED = "time_shift_speed";
    
    public static final String USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_START = "dvb_time_shift_start";
    
    public static final String USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_PAUSE = "dvb_time_shift_pause";
    
    public static final String USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_START_PLAY = "dvb_time_shift_start_play";
    
    public static final String USER_BEHAVIOR_SURVEY_PVR_PROJECT_VALUE = "pvr";
    
    public static final String USER_BEHAVIOR_SURVEY_PVR_ACTION_VALUE_START_PLAYBACK = "pvr_playback_start";
    
    // Add For User Behavior Survey END
    
    // Add For OTA Download BEGIN
    public static final String CLOSE_OTA_ACTIVITY = "com.pbi.dvb.closeOTAActivity";
    
    // Add For OTA Download END
    
    /* Start LinKang, 2013-12-19, Add the value for switch Left&Right key */
    public static final int SWITCH_TO_LEFT_RIGHT_CONTROL = 1;
    
    public static final int SWITCH_TO_VOICE_CONTROL = 0;
    
    /* End LinKang, 2013-12-19, Add the value for switch Left&Right key */
    
    /*
     * Start LinKang, 2013-12-19, Add the name of SharedPreferences for storing the information about last played
     * program...
     */
    // public static final String DVB_BEFORE_LAST_PROGRAM = "dvb_last_program";
    // public static final String DVB_LAST_PROGRAM = "dvb_channel";
    
    public static final String DVB_LOOKBACK_SERVICE = "dvb_lookback_service";
    
    public static final String DVB_LASTPLAY_SERVICE = "dvb_lastplay_service";
    
    /*
     * End LinKang, 2013-12-19, Add the name of SharedPreferences for storing the information about last played
     * program...
     */
    
    public static final String DVB_TAG = "DVB_LOG_TRACE";
    
    /* Start LinKang, 2014-01-21, Add some key define. */
    public static final String DVB_SERVICE_TYPE = "ServiceType";
    /* End LinKang, 2014-01-21, Add some key define. */
}
