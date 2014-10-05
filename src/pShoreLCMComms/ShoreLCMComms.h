/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ShoreLCMComms.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef ShoreLCMComms_HEADER
#define ShoreLCMComms_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <lcm/lcm-cpp.hpp>
#include "node_in_t.hpp"
#include "node_out_t.hpp"

class ShoreLCMComms : public CMOOSApp
{
 public:
   ShoreLCMComms();
   ~ShoreLCMComms();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
};

#endif
