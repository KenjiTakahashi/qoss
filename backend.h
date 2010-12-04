#ifndef BACKEND_H
#define BACKEND_H

using namespace std;

#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iostream>

#include <fcntl.h>
#include <errno.h>
#include </usr/lib/oss/include/sys/soundcard.h>
#include <sys/ioctl.h>

typedef struct EXT_DATA {
    vector<int> values;
    int mute_value;
} ext_data;

typedef struct DATA {
    int i;
    int minvalue;
    int maxvalue;
    vector<int> values;
    int mute_value;
    vector<string> mode_values;
    int current_mode;
    map<string, ext_data> ext;
} data;

class backend
{
public:
    backend();
    void get_local_dev_info(map<string, map<string, map<string, data> > >& results);
    vector<int> get_control_values(string dev, int i);
    int get_mute_value(string dev, int i);
    vector<string> get_mode_values(string dev, int i);
    int get_current_mode(string dev, int i);
    vector<int> get_peak_values(string dev, int i);
    string get_error();

private:
    void init_local_fd(int dev);
    void init_extinfo(int dev);
    vector<int> read_control_values(int dev, oss_mixext& extinfo);
    int read_mute_value(int dev, oss_mixext& extinfo);
    vector<string> read_mode_values(int dev, oss_mixext& extinfo);
    int read_current_mode(int dev, oss_mixext& extinfo);
    void split(const string& s, char delimiter, vector<string>& result);

    vector<int> local_fd;
    int global_fd;
    string error;
    map<string, vector<oss_mixext> > g_extinfo;
};

#endif // BACKEND_H
