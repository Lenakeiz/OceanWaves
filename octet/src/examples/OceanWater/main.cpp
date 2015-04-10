////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
//

#define OCTET_BULLET 0

#include "../../octet.h"

#include "OceanWaterGLSL.h"
#include "OceanWater.h"


int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
  octet::OceanWaterGLSL app(argc, argv);
  //octet::OceanWater app(argc, argv);
  app.init();

  // open windows
  octet::app::run_all_apps();
}


