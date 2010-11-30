#ifndef BACKEND_H
#define BACKEND_H

#define MAX_DEVS 16

using namespace std;

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include </usr/lib/oss/include/sys/soundcard.h>
#include <sys/ioctl.h>

typedef struct EXT_DATA {
    vector<int> values;
    int mute_value;
} ext_data;

typedef struct DATA {
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
    string get_error();
    map<string, vector<oss_mixext> > g_extinfo; // make it private!

private:
    void init_local_fd(int dev);
    void init_extinfo(int dev);
    vector<int> get_control_values(int dev, oss_mixext& extinfo);
    int get_mute_value(int dev, oss_mixext& extinfo);
    vector<string> get_mode_values(int dev, oss_mixext& extinfo);
    int get_current_mode(int dev, oss_mixext& extinfo);
    void split(const string& s, char delimiter, vector<string>& result);

    int local_fd[MAX_DEVS];
    int global_fd;
    string error;
};

#endif // BACKEND_H
