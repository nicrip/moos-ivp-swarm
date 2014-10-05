/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: NodeLCMComms.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef NodeLCMComms_HEADER
#define NodeLCMComms_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <lcm/lcm-cpp.hpp>
#include "node_in_t.hpp"
#include "node_out_t.hpp"

class NodeLCMComms : public CMOOSApp
{
 public:
   NodeLCMComms();
   ~NodeLCMComms();

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
