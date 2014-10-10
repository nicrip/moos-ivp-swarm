/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LCMOceanModelMgr.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef LCMOceanModelMgr_HEADER
#define LCMOceanModelMgr_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>
#include <octave/Cell.h>
#include <boost/dynamic_bitset.hpp>
#include <lcm/lcm-cpp.hpp>
#include "node_nav_t.hpp"
#include "node_drift_t.hpp"

class LCMOceanModelMgr : public CMOOSApp
{
 public:
   LCMOceanModelMgr();
   ~LCMOceanModelMgr();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();
   void LCMSubscribe();
   static void * LCMHandle(void* data);
   void LCMCallback_node_nav_t(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const node_nav_t* msg);
   void OceanModelRequest();

 private: // Configuration variables
   unsigned int   m_start_node_idx;
   unsigned int   m_end_node_idx;
   double         m_time_offset;
   std::string    m_filepath;
   std::string    m_node_prefix;
   std::string    m_subset_prefix;
   std::string    m_filepath_moosvar;
   std::string    m_positions_moosvar;
   std::string    m_varname_moosvar;
   std::string    m_time_request_offset_moosvar;
   std::string    m_time_offset_reset_moosvar;
   std::string    m_model_return_moosvar;
   lcm::LCM*      m_lcm;

 private: // State variables
   unsigned int             m_iterations;
   double                   m_timewarp;
   boost::dynamic_bitset<>  m_nodes_nav_set;
   bool                     m_all_set;
   bool                     m_first_request;
   unsigned int             m_num_nodes;
   Matrix                   m_model_pos_request;
   unsigned char*           m_model_pos_request_data_ptr;
   std::string              m_model_varnames;
   Matrix                   m_model_values_return;
};

#endif
