/***    propname.h - property names for language translation
 *
 *      VKBD Property Names
 */

#define PROP_NAME_POLL          "IDLE_SENSITIVITY"
#define PROP_NAME_DELAY         "IDLE_SECONDS"
#define PROP_NAME_FAST_PASTE    "VIDEO_FASTPASTE"
#define PROP_NAME_EXTRA_KEYS    "KBD_BUFFER_EXTEND"
#define PROP_NAME_KBD_RESTRICT  "KBD_RATE_LOCK"
#define PROP_NAME_KBD_CTRL_BP   "KBD_CTRL_BYPASS"
#define PROP_NAME_KBD_ALTENT_BP "KBD_ALTENT_BYPASS"

/*** WARNING - Do not change the order of the following ***/
#define HOT_KEY_LIST    \
 "NONE\0ALT_ESC\0CTRL_ESC\0"

/*
 *      VTIMER Property Names
 */

#define PROP_NAME_DIRECT_ACCESS         "HW_TIMER"
#define PROP_NAME_SOUND_INHIBIT         "HW_NOSOUND"


/*
 *      VLPT Property Names
 */

#define PROP_NAME_LPT1TIMEOUT "PRINT_TIMEOUT"


/*
 *      VBIOS Property Names
 */

#define PROP_NAME_BREAK             "DOS_BREAK"
#define PROP_NAME_SHELL             "DOS_SHELL"
#define PROP_NAME_RMSIZE            "DOS_RMSIZE"          /*BC002*/
#define PROP_NAME_FCB               "DOS_FCBS"
#define PROP_NAME_FCB2              "DOS_FCBS_KEEP"
#define PROP_NAME_FILES             "DOS_FILES"           /*BN011*/
#define PROP_NAME_DOSDD             "DOS_DEVICE"
#define PROP_NAME_DOS_OWN_UMB       "DOS_UMB"             /*BC002*/
#define PROP_NAME_DOS_LOADED_HIGH   "DOS_HIGH"            /*BN002*/
#define PROP_NAME_COPYROM           "HW_ROM_TO_RAM"
#define PROP_NAME_IOPL3             "HW_IOPL_LOCK"
#define PROP_NAME_INCLUDE_REGIONS   "MEM_INCLUDE_REGIONS"
#define PROP_NAME_EXCLUDE_REGIONS   "MEM_EXCLUDE_REGIONS"
#define PROP_NAME_LASTDRIVE         "DOS_LASTDRIVE"                /*BN003*/
#define PROP_NAME_VM_BOOT           "DOS_STARTUP_DRIVE"

#define VIDEO_DRIVER_NAME           "MDOS Video Device Driver"


/*
 *      VVIDEO Property Names
 */

#define PROP_NAME_INT10EMULATE  "VIDEO_ROM_EMULATION"
#define PROP_NAME_RTRCEMULATE   "VIDEO_RETRACE_EMULATION"
#define PROP_NAME_MODERESTRICT  "VIDEO_MODE_RESTRICTION"
#define PROP_NAME_NORESTRICT    "NONE        "
#define PROP_NAME_CGARESTRICT   "CGA            "
#define PROP_NAME_MONORESTRICT  "MONO           "   /* length must match CGARESTRICT */
#define PROP_NAME_ONDEMAND      "VIDEO_ONDEMAND_MEMORY"
#define PROP_NAME_INT2F         "VIDEO_SWITCH_NOTIFICATION"
#define PROP_NAME_UPDATEWINDOW  "VIDEO_WINDOW_REFRESH"
#define PROP_NAME_SYNCOUTPUT    "Video_Window_scroll_synchronization"
#define PROP_NAME_8514A_IOTRAP  "VIDEO_8514A_XGA_IOTRAP"     /*DCR1321*/


/*
 *      VXMS Property Names
 */

/* VXMS Property Names
 *
 * Giving VXMS_PER_VDM_LIMIT as 0 will stop XMS operation in that
 * VDM. This field is restricted to 0-65535KB with default value as
 * 1024K. VXMS_MIN_HMA_USAGE are restricted to be in 0-63k with default
 * as 0. VXMS_NUM_HANDLES are restricted to 0-128 with default as 32.
 * VXMS_UMB_STATE is bool and defaults to deactive.
 */

#define PROP_NAME_VXMS_PER_VDM_LIMIT    "XMS_MEMORY_LIMIT"
#define PROP_NAME_VXMS_MIN_HMA          "XMS_MINIMUM_HMA"
#define PROP_NAME_VXMS_NUM_HANDLES      "XMS_HANDLES"


/*
 *      VEMM Property Names
 */

#define PROP_VEMM_LIMIT   "EMS_MEMORY_LIMIT"
#define PROP_VEMM_HIGH    "EMS_HIGH_OS_MAP_REGION"
#define PROP_VEMM_LOW     "EMS_LOW_OS_MAP_REGION"
#define PROP_VEMM_FRAME   "EMS_FRAME_LOCATION"

/*** WARNING - Do not change the order of the following ***/
#define PROP_VEMM_LIST    \
 "AUTO\0NONE\0C000\0C400\0C800\0CC00\0D000\0D400\0D800\0DC00\0" \
 "8000\08400\08800\08C00\09000\0"


/*
 *      VMOUSE Property Names
 */

#define PROP_NAME_MOUSECAPTURE  "MOUSE_EXCLUSIVE_ACCESS"

/***LT+ FAKEVERINFO defines a fake version entry for a DOS app.
 *
 *      MVDMR.LIB -> OS2KRNL
 *
 *      DOSEM fakes out the specified version for a DOS app which
 *      listed on a FAKEVERINFO structure.
 */

#define PROP_NAME_FAKE_VERSION  "DOS_VERSION"

/*** DPMI Property Name
 *
 *      This property enables or disables DPMI support for this VDM.
 */

#define PROP_NAME_DPMI          "DPMI_MEMORY_LIMIT"          /* BN004 */

/*** VCOM Property Name
 *
 *      This hold resource in this VDM.
 */

#define PROP_NAME_HOLD_RES      "COM_HOLD"

/*** VTOUCH Property Name
 *
 *
 */

#define PROP_NAME_EXCLUSIVE_TOUCH       "TOUCH_EXCLUSIVE_ACCESS"

/*** DPMI  ***/

#define PROP_NAME_DPX_ENABLE     "DPMI_DOS_API"

#define PROP_DPX_TRANS_ALWAYS    "ENABLED"
#define PROP_DPX_TRANS_APP_SEL   "AUTO"
#define PROP_DPX_TRANS_NEVER     "DISABLED"

#define PROP_DPX_TRANS_LIST \
   PROP_DPX_TRANS_APP_SEL "\0" \
   PROP_DPX_TRANS_ALWAYS "\0" \
   PROP_DPX_TRANS_NEVER "\0"

/*** Windows properties ***/

#define PROP_NAME_WIN_RUNMODE    "WIN_RUNMODE"

#define PROP_WMODE_STD           "STANDARD"
#define PROP_WMODE_REAL          "REAL"
#define PROP_WMODE_AUTO          "AUTO"
