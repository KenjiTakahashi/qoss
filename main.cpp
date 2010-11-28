#include <QtGui/QApplication>
#include "frontend.h"

#include "backend.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    frontend w;
//    w.show();
//
//    return a.exec();
    backend* b=new backend();
    map<string, map<string, data> > results;
    b->get_local_dev_info(results, 0);
    string error = b->get_error();
    if(!error.empty()) {
        cout << error;
    } else {
        if(results.empty()) {
            cout << "LOL";
        }
        for(map<string, map<string, data> >::iterator it = results.begin(); it != results.end(); ++it) {
            cout << (*it).first;
            map<string, data> results2 = (*it).second;
            for(map<string, data>::iterator it2 = results2.begin(); it2 != results2.end(); ++it2) {
                cout << (*it2).first << ": ";
                data second = (*it2).second;
                cout << " " << second.ctrl << " ";
                cout << "Maxvalue: " << second.maxvalue;
                cout << " Minvalue: " << second.minvalue;
                cout << " Mute: " << second.mute_value;
                cout << " Mode: ";
                for(unsigned int i = 0; i < second.mode_values.size(); ++i) {
                    cout << second.mode_values[i];
                }
                cout << " Values: ";
                for(unsigned int i = 0; i < second.values.size(); ++i) {
                    cout << second.values[i] << " ";
                }
                cout << endl;
            }
        }
    }
}