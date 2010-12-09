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
#include "backend.h"
#include "middleend.h"
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    backend* b=new backend();
    map<string, map<string, map<string, ossdata> > > results;
    string error = b->get_error();
    char input;
    if(!error.empty()) {
        cout << error;
    } else {
        do {
            b->get_local_dev_info(results);
            for(map<string, map<string, map<string, ossdata> > >::iterator it3 = results.begin(); it3 != results.end(); ++it3) {
                cout << it3->first << endl;
                map<string, map<string, ossdata> > results2 = it3->second;
                for(map<string, map<string, ossdata> >::iterator it = results2.begin(); it != results2.end(); ++it) {
                    cout << it->first;
                    map<string, ossdata> results2 = it->second;
                    for(map<string, ossdata>::iterator it2 = results2.begin(); it2 != results2.end(); ++it2) {
                        ossdata second = it2->second;
                        //vector<int> r = b->get_peak_values(it3->first, second.i);
                        //for(unsigned int i = 0; i < r.size(); ++i) {
                        //    cout << r[i] << " ";
                        //}
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
                        //cout << " Current mode: " << second.current_mode;
                        //cout << " Values: ";
                        //for(unsigned int i = 0; i < second.values.size(); ++i) {
                        //    cout << second.values[i] << " ";
                        //}
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
    //QStringList v;
    //v << "LOL" << "OLDW";
    //QWidget *widget = new QWidget();
    //QHBoxLayout *layout = new QHBoxLayout();
    //widget->setLayout(layout);
    //QOSSSlider *qossslider = new QOSSSlider(STEREO, true, 0, 100, "test1", v);
    //layout->addWidget(qossslider);
    //QOSSSlider *slider2 = new QOSSSlider(STEREO, false, 0, 100, "test2", v);
    //layout->addWidget(slider2);
    //QOSSSlider *slider3 = new QOSSSlider(STEREO, false, 0, 100);
    //layout->addWidget(slider3);
    //w.setCentralWidget(widget);
    QOSSStructure *structure = new QOSSStructure();
    w.setCentralWidget(structure);
    w.show();

    return a.exec();
}
