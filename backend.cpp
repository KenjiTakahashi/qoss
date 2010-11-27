#include "backend.h"

backend::backend() {
    const char* devmixer;
    global_fd = -1, dev = -1;
    for(int i = 0; i < MAX_DEVS; ++i) {
        local_fd[i] = -1;
    }
    if((devmixer = getenv("OSS_MIXERDEV")) == NULL) {
        devmixer = "/dev/mixer";
    }
    if((global_fd = open(devmixer, O_RDWR, 0)) == -1) {
        error = "No mixer found";
    } else if(dev == -1) {
        dev = find_default_mixer();
    }
    for(int i = 0; i < mix_num; ++i) {
        init_local_fd(i);
    }
}
int backend::find_default_mixer() {
    oss_mixerinfo mi;
    int best = -1, bestpri = 0;
    if(ioctl(global_fd, SNDCTL_MIX_NRMIX, &mix_num) == -1) {
        if(errno == EINVAL) {
            error = "OSS version 4.0 or later is needed!";
        } else {
            error = "Could not get number of mixer devices";
        }
    } else if(mix_num == 0) {
        error ="No mixers available";
    } else {
        for(int i = 0; i < mix_num; ++i) {
            mi.dev = i;
            ioctl(global_fd, SNDCTL_MIXERINFO, &mi);
            if(mi.enabled) {
                if(best == -1) {
                    best = i;
                }
                if(mi.priority > bestpri) {
                    best = i;
                    bestpri = mi.priority;
                }
            }
        }
        if(best == -1) {
            error = "No default mixers available";
        }
    }
    return best;
}
void backend::get_local_dev_info(map<string, map<string, data> >& results, int dev) {
    oss_mixerinfo mi;
    mi.dev = dev;
    if(ioctl(global_fd, SNDCTL_MIXERINFO, &mi) == -1) {
        error = "Error getting mixer info";
    } else {
        for(int i = 0; i < mi.nrext; ++i) {
            oss_mixext extinfo;
            extinfo.dev = dev;
            extinfo.ctrl = i;
            if(ioctl(local_fd[dev], SNDCTL_MIX_EXTINFO, &extinfo) == -1) {
                error = "Error getting extended mixer info";
            } else {
                unsigned int type = extinfo.type;
                if(type != 0 && type != 1 && type != 12) {
                    vector<string> chain;
                    split(extinfo.extname, '.', chain);
                    if(chain.size() == 3) {
                        if(chain[2] == "mute") {
                            results[chain[0]][chain[1]].mute_value = get_mute_value(dev, extinfo);
                        } else if(chain[2] == "mode") {
                            results[chain[0]][chain[1]].mode_values = get_mode_values(dev, extinfo);
                        } else {
                            vector<string> chain2;
                            split(chain[2], '-', chain2);
                            if(chain2.size() == 1) {
                                results[chain[0]][chain[1]].ext[chain2[0]].values = read_control_values(dev, extinfo);
                            } else {
                                results[chain[0]][chain[1]].ext[chain2[0]].mute_value = get_mute_value(dev, extinfo);
                            }
                        }
                    } else {
                        if(chain.size() == 1) {
                            chain.push_back("");
                        }
                        vector<string> chain2;
                        split(chain[1], '-', chain2);
                        if(chain2.size() == 2 && chain2[1] == "mute") {
                            cout << chain2[0] << endl;
                            results[chain[0]][chain2[0]].mute_value = get_mute_value(dev, extinfo);
                        } else {
                            results[chain[0]][chain[1]].minvalue = extinfo.minvalue;
                            results[chain[0]][chain[1]].maxvalue = extinfo.maxvalue;
                            if(extinfo.type == MIXT_ENUM) {
                                results[chain[0]][chain[1]].mode_values = get_mode_values(dev, extinfo);
                            } else {
                                results[chain[0]][chain[1]].values = read_control_values(dev, extinfo);
                            }
                        }
                    }
                }
//                oss_audioinfo ai;
//                ai.dev = i;
//                if(ioctl(local_fd[dev], SNDCTL_ENGINEINFO, &ai) != -1 && *ai.label != '\0') {
//                    printf("%s\n",ai.label);
//                }
            }
        }
    }
}
void backend::init_local_fd(int dev) {
    int fd;
    oss_mixerinfo mi;
    if(dev < 0 || dev >= MAX_DEVS) {
        error = "Bad mixer device handler received!";
    } else if(!(local_fd[dev] != -1)) {
        mi.dev = dev;
        if((ioctl(global_fd, SNDCTL_MIXERINFO, &mi)) == -1) {
            error = "Error getting mixer info!";
        } else if((fd = open(mi.devnode, O_RDWR, 0)) == -1) {
            error = mi.devnode;
        } else {
            local_fd[dev] = fd;
        }
    }
}
int backend::get_mute_value(int dev, oss_mixext &extinfo) {
    oss_mixer_value v;
    v.dev = extinfo.dev;
    v.ctrl = extinfo.ctrl;
    v.timestamp = extinfo.timestamp;
    if(ioctl(local_fd[dev], SNDCTL_MIX_READ, &v) == -1) {
        if(errno == EIDRM) {
            //reload
        }
    } else {
        return v.value;
    }
    return -1;
}
vector<string> backend::get_mode_values(int dev, oss_mixext &extinfo) {
    oss_mixer_enuminfo ei;
    ei.dev = extinfo.dev;
    ei.ctrl = extinfo.ctrl;
    vector<string> results;
    if(ioctl(local_fd[dev], SNDCTL_MIX_ENUMINFO, &ei) == -1) {
        //errors
    } else {
        for(int i = 0; i < ei.nvalues; ++i) {
            string str;
            int j = ei.strindex[i];
            while(char c = ei.strings[j]) {
                str += c;
                j++;
            }
            results.push_back(str);
        }
    }
    return results;
}
vector<int> backend::read_control_values(int dev, oss_mixext& extinfo) {
    oss_mixer_value v;
    v.dev = extinfo.dev;
    v.ctrl = extinfo.ctrl;
    v.timestamp = extinfo.timestamp;
    vector<int> values;
    if(ioctl(local_fd[dev], SNDCTL_MIX_READ, &v) == -1) {
        if(errno == EIDRM) {
            //reload
        }
    } else {
        switch(extinfo.type) {
            case MIXT_STEREOSLIDER16:
            values.push_back(v.value & 0xffff);
            values.push_back((v.value >> 16) & 0xffff);
            break;
            case MIXT_STEREOSLIDER:
            values.push_back(v.value & 0xff);
            values.push_back((v.value >> 8) & 0xff);
            break;
            case MIXT_MONOSLIDER16:
            values.push_back(v.value & 0xffff);
            break;
            case MIXT_MONOSLIDER:
            values.push_back(v.value & 0xff);
            break;
            default:
            break;
        }
    }
    return values;
}
string backend::get_error() {
    return error;
}
void backend::split(const string& s, char delimiter, vector<string>& result) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delimiter)) {
        result.push_back(item);
    }
}
