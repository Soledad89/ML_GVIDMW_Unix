



#ifndef __RADAR_SHOW__
#define __RADAR_SHOW__

#include <stdio.h>
#include "ppidef.h"

s_ppi_video_for_disp * dispbuf1;
s_ppi_video_for_disp * dispbuf2;

FILE * rpkgfp;
FILE * hpkgfp;
bool radarOpenState[2]={false,false};
bool TVopenState[2] = {false,false};


#endif