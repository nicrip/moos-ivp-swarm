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
   void LCMSubscribe();
   static void * LCMHandle(void* data);
   void LCMCallback_node_in_t(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const node_in_t* msg);

 private: // Configuration variables
   std::string  m_node_name;
   lcm::LCM*    m_lcm;

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
   //LCM Node Out Variables
   double       m_nav_long;
   double       m_nav_lat;
   double       m_nav_depth;
   std::string  m_node_report_local;
   bool         m_lat_set;
   bool         m_long_set;
   bool         m_report_set;
   //LCM Node In Variables
   double       m_drift_x;
   double       m_drift_y;
   std::string  m_deploy;
   std::string  m_return;
   std::string  m_moos_manual_override;
};

#endif
