/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: NodeManager.h                                   */
/*    DATE: August 2014                                     */
/************************************************************/

#ifndef NodeManager_HEADER
#define NodeManager_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>
#include <octave/Cell.h>

class NodeManager : public CMOOSApp
{
 public:
   NodeManager();
   ~NodeManager();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
   unsigned int   m_iterations;
   double         m_timewarp;
   double         m_node_lat;
   bool           m_lat_set;
   double         m_node_lon;
   bool           m_lon_set;
   double         m_node_depth;
   std::string    m_filepath;
   Matrix         m_model_pos_request;
   unsigned char* m_model_pos_request_data_ptr;
   std::string    m_model_varnames;
   Matrix         m_model_values_return;
   double         m_speed_factor;
   double         m_start_time;
   double         m_elapsed_time;
};

#endif
