#include <QtGui/QApplication>
#include "frontend.h"

#include "backend.h"

//temp includes, remove them after testing done!
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    frontend w;
//    w.show();
//
//    return a.exec();
    backend* b=new backend();
    map<string, map<string, map<string, data> > > results;
    string error = b->get_error();
    char input;
    if(!error.empty()) {
        cout << error;
    } else {
        do {
            b->get_local_dev_info(results);
            for(map<string, map<string, map<string, data> > >::iterator it3 = results.begin(); it3 != results.end(); ++it3) {
                //cout << it3->first << endl;
                map<string, map<string, data> > results2 = it3->second;
                for(map<string, map<string, data> >::iterator it = results2.begin(); it != results2.end(); ++it) {
                    //cout << it->first;
                    map<string, data> results2 = it->second;
                    for(map<string, data>::iterator it2 = results2.begin(); it2 != results2.end(); ++it2) {
                        data second = it2->second;
                        vector<int> r = b->get_peak_values(it3->first, second.i);
                        for(unsigned int i = 0; i < r.size(); ++i) {
                            cout << r[i] << " ";
                        }
                        //cout << it2->first << ":";
                        //cout << " i: " << second.i;
                        //cout << " Maxvalue: " << second.maxvalue;
                        //cout << " Minvalue: " << second.minvalue;
                        //cout << " Mute: " << second.mute_value;
                        //cout << " Mode: ";
                        //for(unsigned int i = 0; i < second.mode_values.size(); ++i) {
                        //    cout << second.mode_values[i];
                        //}
                        //cout << " Current mode: " << second.current_mode;
                        //cout << " Values: ";
                        //for(unsigned int i = 0; i < second.values.size(); ++i) {
                        //    cout << second.values[i] << " ";
                        //}
                        //cout << endl;
                    }
                }
            }
            cin >> input;
        } while(input != 'd');
    }
}
