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
#include <QtGui/QApplication>
#include "frontend.h"

//temp includes, remove them after testing done!
#include "middleend.h"
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    map<string, map<string, map<string, ossdata> > > results;
    string error = backend_handler->get_error();
    char input;
    if(!error.empty()) {
        cout << error;
    } else {
        do {
            backend_handler->get_local_dev_info(results);
            for(map<string, map<string, map<string, ossdata> > >::iterator it3 = results.begin(); it3 != results.end(); ++it3) {
                cout << it3->first << endl;
                map<string, map<string, ossdata> > results2 = it3->second;
                for(map<string, map<string, ossdata> >::iterator it = results2.begin(); it != results2.end(); ++it) {
                    cout << it->first;
                    map<string, ossdata> results2 = it->second;
                    for(map<string, ossdata>::iterator it2 = results2.begin(); it2 != results2.end(); ++it2) {
                        ossdata second = it2->second;
                        cout << it2->first << ":";
                        cout << " i: " << second.i;
                        //cout << " Maxvalue: " << second.maxvalue;
                        //cout << " Minvalue: " << second.minvalue;
                        cout << " Mute i: " << second.mute_i;
                        //cout << " Mode: ";
                        //for(unsigned int i = 0; i < second.mode_values.size(); ++i) {
                        //    cout << second.mode_values[i];
                        //}
                        cout << " Mode i: " << second.mode_i;
                        cout << " Type: " << second.type;
                        cout << " Peak: " << second.peak;
                        cout << endl;
                    }
                }
            }
            cin >> input;
        } while(input != 'd');
    }
    QApplication a(argc, argv);
//    frontend w;
    QMainWindow w;
    QOSSConfig *structure = new QOSSConfig();
    w.setCentralWidget(structure);
    w.show();

    return a.exec();
}
