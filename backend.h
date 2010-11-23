#ifndef BACKEND_H
#define BACKEND_H

#define MAX_DEVS 16
#define MONO 1
#define STEREO 2
#define MUTE 0

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

typedef struct DATA {
    string name;
    int minvalue;
    int maxvalue;
    int type; // MONO || STEREO || MUTE
    vector<int> values;
} data;

class backend
{
public:
    backend();
    void get_local_dev_info(map<string, map<string, map<string, vector<data> > > >& results, int dev);
    string get_error();

private:
    int find_default_mixer();
    void init_local_fd(int dev);
    void read_control_values(int dev, oss_mixext& extinfo, data& d);
    void split(const string& s, char delimiter, vector<string>& result);

    int local_fd[MAX_DEVS];
    int global_fd;
    int dev;
    int mix_num;
    string error;
};

#endif // BACKEND_H
