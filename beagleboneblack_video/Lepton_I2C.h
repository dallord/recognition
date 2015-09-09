#ifndef LEPTON_I2C
#define LEPTON_I2C

#include <QDebug>

void lepton_perform_ffc();
int lepton_scene_statistic(int type);
void lepton_set_roi(int s_c, int e_c, int s_r, int e_r);
int lepton_get_roi(int type);
int lepton_get_temperature();

void test();

#endif
