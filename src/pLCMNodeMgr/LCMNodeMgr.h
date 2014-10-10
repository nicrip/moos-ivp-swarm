/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LCMNodeMgr.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef LCMNodeMgr_HEADER
#define LCMNodeMgr_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <lcm/lcm-cpp.hpp>
#include "node_nav_t.hpp"
#include "node_drift_t.hpp"

class LCMNodeMgr : public CMOOSApp
{
 public:
   LCMNodeMgr();
   ~LCMNodeMgr();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();
   void LCMSubscribe();
   static void * LCMHandle(void* data);
   void LCMCallback_node_drift_t(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const node_drift_t* msg);

 private: // Configuration variables
   std::string  m_node_name;
   lcm::LCM*    m_lcm;

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
   double       m_nav_long;
   double       m_nav_lat;
   double       m_nav_depth;
   double       m_nav_x;
   double       m_nav_y;
   bool         m_lat_set;
   bool         m_long_set;
   bool         m_depth_set;
};

#endif
