////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Text overlay
//

<<<<<<< HEAD
#define OCTET_BULLET 0
=======
#define OCTET_BULLET 1
>>>>>>> 3a75384a2323b944c30740ef917b4e639f16fe4b

#include "../../octet.h"

#include "OceanWater.h"
<<<<<<< HEAD
#include "OceanWaterGLSL.h"
=======
>>>>>>> 3a75384a2323b944c30740ef917b4e639f16fe4b

/// Create a box with octet
int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
<<<<<<< HEAD
  octet::OceanWaterGLSL app(argc, argv);
=======
  octet::OceanWater app(argc, argv);
>>>>>>> 3a75384a2323b944c30740ef917b4e639f16fe4b
  app.init();

  // open windows
  octet::app::run_all_apps();
}


