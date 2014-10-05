/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: SwarmManager.h                                  */
/*    DATE: August 2014                                     */
/************************************************************/

#ifndef SwarmManager_HEADER
#define SwarmManager_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>
#include <octave/Cell.h>
#include <boost/dynamic_bitset.hpp>

class SwarmManager : public CMOOSApp
{
 public:
   SwarmManager();
   ~SwarmManager();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

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

 private: // State variables
   double         m_node_lat;
   double         m_node_long;
   double         m_node_depth;
   boost::dynamic_bitset<> m_nodes_long_set;
   boost::dynamic_bitset<> m_nodes_lat_set;
   unsigned int   m_iterations;
   double         m_timewarp;
   unsigned int   m_num_nodes;
   Matrix         m_model_pos_request;
   unsigned char* m_model_pos_request_data_ptr;
   std::string    m_model_varnames;
   Matrix         m_model_values_return;
   clock_t        m_start_time;
   clock_t        m_end_time;
};

#endif
