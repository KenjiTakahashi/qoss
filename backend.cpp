#include "backend.h"

backend::backend() {
    const char* devmixer;
    global_fd = -1;
    for(int i = 0; i < MAX_DEVS; ++i) {
        local_fd[i] = -1;
    }
    if((devmixer = getenv("OSS_MIXERDEV")) == NULL) {
        devmixer = "/dev/mixer";
    }
    if((global_fd = open(devmixer, O_RDWR, 0)) == -1) {
        error = "No mixer found";
    }
    int mix_num;
    if(ioctl(global_fd, SNDCTL_MIX_NRMIX, &mix_num) == -1) {
        if(errno == EINVAL) {
            error = "OSS version 4.0 or later is needed!";
        } else {
            error = "Could not get number of mixer devices";
        }
    } else {
        for(int i = 0; i < mix_num; ++i) {
            init_local_fd(i);
            init_extinfo(i);
        }
    }
}
void backend::init_extinfo(int dev) {
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
                g_extinfo[mi.name].push_back(extinfo);
            }
        }
    }
}
void backend::get_local_dev_info(map<string, map<string, map<string, data> > >& results) {
    int dev = -1;
    for(map<string, vector<oss_mixext> >::iterator it = g_extinfo.begin(); it != g_extinfo.end(); ++it) {
        dev++;
        string name = it->first;
        vector<oss_mixext> extinfo = it->second;
        for(unsigned int i = 0; i < extinfo.size(); ++i) {
            unsigned int type = extinfo[i].type;
            if(type != 0 && type != 1 && type != 12) {
                vector<string> chain;
                split(extinfo[i].extname, '.', chain);
                if(chain.size() == 3) {
                    if(chain[2] == "mute") {
                        results[name][chain[0]][chain[1]].mute_value = get_mute_value(dev, extinfo[i]);
                    } else if(chain[2] == "mode") {
                        results[name][chain[0]][chain[1]].mode_values = get_mode_values(dev, extinfo[i]);
                        results[name][chain[0]][chain[1]].current_mode = get_current_mode(dev, extinfo[i]);
                    } else {
                        vector<string> chain2;
                        split(chain[2], '-', chain2);
                        if(chain2.size() == 1) {
                            results[name][chain[0]][chain[1]].ext[chain2[0]].values = get_control_values(dev, extinfo[i]);
                        } else {
                            results[name][chain[0]][chain[1]].ext[chain2[0]].mute_value = get_mute_value(dev, extinfo[i]);
                        }
                    }
                } else {
                    if(chain.size() == 1) {
                        chain.push_back("");
                    }
                    vector<string> chain2;
                    split(chain[1], '-', chain2);
                    if(chain2.size() == 2 && chain2[1] == "mute") {
                        results[name][chain[0]][chain2[0]].mute_value = get_mute_value(dev, extinfo[i]);
                    } else {
                        results[name][chain[0]][chain[1]].minvalue = extinfo[i].minvalue;
                        results[name][chain[0]][chain[1]].maxvalue = extinfo[i].maxvalue;
                        if(extinfo[i].type == MIXT_ENUM) {
                            results[name][chain[0]][chain[1]].mode_values = get_mode_values(dev, extinfo[i]);
                            results[name][chain[0]][chain[1]].current_mode = get_current_mode(dev, extinfo[i]);
                        } else {
                            results[name][chain[0]][chain[1]].values = get_control_values(dev, extinfo[i]);
                        }
                    }
                }
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
int backend::get_current_mode(int dev, oss_mixext &extinfo) {
    oss_mixer_value v;
    v.dev = extinfo.dev;
    v.ctrl = extinfo.ctrl;
    v.timestamp = extinfo.timestamp;
    int value = -1;
    if(ioctl(local_fd[dev], SNDCTL_MIX_READ, &v) == -1) {
        if(errno == EIDRM) {
            //reload
        }
    } else {
        value = v.value;
    }
    return value;
}
vector<int> backend::get_control_values(int dev, oss_mixext& extinfo) {
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
