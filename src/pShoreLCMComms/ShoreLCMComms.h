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

struct lcm_out
{
   lcm_out() : m_drift_x(0.0), m_drift_y(0.0), m_deploy("false"), m_return("false"), m_moos_manual_override("false") {}
   double       m_drift_x;
   double       m_drift_y;
   std::string  m_deploy;
   std::string  m_return;
   std::string  m_moos_manual_override;
};

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
   void LCMSubscribe();
   static void * LCMHandle(void* data);
   void LCMCallback_node_out_t(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const node_out_t* msg);

 private: // Configuration variables
   std::string  m_node_prefix;
   unsigned int m_node_start_idx;
   unsigned int m_node_end_idx;
   unsigned int m_num_nodes;
   lcm::LCM*    m_lcm;

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
   //LCM Shore Out Variables
   std::vector<lcm_out> m_lcm_out;
   //LCM Shore In Variables
   std::string  m_node_name;
   double       m_nav_long;
   double       m_nav_lat;
   double       m_nav_depth;
   std::string  m_node_report_local;
};

#endif
