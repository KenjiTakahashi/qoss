cdef extern from "sys/ioctl.h":
    int ioctl(int device, int request, void *value)

cdef extern from "/usr/lib/oss/include/sys/soundcard.h":
    enum: SNDCTL_MIX_NRMIX
    enum: SNDCTL_MIXERINFO
    enum: SNDCTL_MIX_EXTINFO
    enum: MIXT_STEREOSLIDER16
    enum: MIXT_MONOSLIDER16
    enum: MIXT_STEREOSLIDER
    enum: MIXT_MONOSLIDER
    enum: MIXT_STEREOPEAK
    enum: MIXT_MONOPEAK
    enum: MIXT_ENUM
    enum: MIXT_MUTE
    enum: MIXT_VALUE
    enum: SNDCTL_MIX_READ
    enum: SNDCTL_MIX_ENUMINFO
    enum: OSS_HANDLE_SIZE
    enum: OSS_DEVNODE_SIZE
    enum: OSS_ID_SIZE
    enum: OSS_ENUM_MAXVALUE
    enum: OSS_ENUM_STRINGSIZE
    ctypedef char oss_handle_t[OSS_HANDLE_SIZE]
    ctypedef char oss_devnode_t[OSS_DEVNODE_SIZE]
    ctypedef char oss_id_t[OSS_ID_SIZE]
    ctypedef struct oss_mixerinfo:
        int dev
        oss_id_t id
        char name[32]
        int modify_counter
        int card_number
        int port_number
        oss_handle_t handle
        int magic
        int enabled
        int caps
        int flags
        int nrext
        int priority
        oss_devnode_t devnode
        int legacy_device
        int filler[245]
    ctypedef struct oss_mixext:
        int dev
        int ctrl
        int type
        int maxvalue
        int minvalue
        int flags
        oss_id_t id
        int parent
        int dummy
        int timestamp
        char data[64]
        unsigned char enum_present[32]
        int control_no
        unsigned int desc
        char extname[32]
        int update_counter
        int rgbcolor
        int filler[6]
    ctypedef struct oss_mixer_value:
        int dev
        int ctrl
        int value
        int flags
        int timestamp
        int filler[8]
    ctypedef struct oss_mixer_enuminfo:
        int dev
        int ctrl
        int nvalues
        int version
        short strindex[OSS_ENUM_MAXVALUE]
        char strings[OSS_ENUM_STRINGSIZE]
