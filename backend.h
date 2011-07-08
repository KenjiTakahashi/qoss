/**
 * This is a part of qoss @ http://github.com/KenjiTakahashi/qoss/
 * Karol "Kenji Takahashi" Wozniak (C) 2010
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef BACKEND_H
#define BACKEND_H

#define STEREO 2
#define MONO 1
#define OTHER 0
#define NONE 0

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

typedef struct OSSDATA {
    int i;
    int mute_i;
    int mode_i;
    int minvalue;
    int maxvalue;
    int type;
    int peak;
    vector<string> modes;
    map<string, ext_data> ext; // do sth with this!
} ossdata;

class backend
{
public:
    backend();
    //readers
    vector<int> get_control_values(string dev, int i);
    int get_mute_value(string dev, int i);
    vector<string> get_mode_values(string dev, int i);
    int get_current_mode(string dev, int i);
    vector<int> get_peak_values(string dev, int i);
    //writers
    void set_mute_value(string dev, int i, int value);
    void set_mode_value(string dev, int i, string value);
    void set_control_values(string dev, int i, vector<int> values);
    //others
    void get_local_dev_info(map<string, map<string, map<string, ossdata> > >& results);
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
