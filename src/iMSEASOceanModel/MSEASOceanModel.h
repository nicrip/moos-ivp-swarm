/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: MSEASOceanModel.h                               */
/*    DATE: August 2014                                     */
/************************************************************/

#ifndef MSEASOceanModel_HEADER
#define MSEASOceanModel_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class MSEASOceanModel : public AppCastingMOOSApp
{
 public:
   MSEASOceanModel();
   ~MSEASOceanModel();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();
   bool buildReport();

 private: // Configuration variables
   std::string              m_octave_path;
   std::string              m_model_filepath;
   std::string              m_moosvar_prefix;
   double                   m_model_time_warp;

 private: // State variables
   unsigned int             m_iterations;
   double                   m_timewarp;
   Matrix                   m_model_pos_request;
   ColumnVector             m_model_lon_request;
   ColumnVector             m_model_lat_request;
   ColumnVector             m_model_depth_request;
   std::string              m_model_varnames_request;
   std::vector<std::string> m_model_varname_vec_request;
   Cell                     m_model_varname_cell_request;
   double                   m_model_time_request;
   double                   m_model_time_offset;
   double                   m_model_time_request_offset;
   double                   m_model_start_time;
   double                   m_model_end_time;
   double                   m_model_duration;
   octave_value_list        m_batch_request;
   octave_value_list        m_batch_return;
   Cell                     m_batch_return_cell;
   octave_value_list        m_time_request;
   octave_value_list        m_time_return;
   Matrix                   m_model_times_request;
   Matrix                   m_model_values_return;
   unsigned char*           m_model_values_return_data_ptr;
   bool                     m_reset_time_flag;
   bool                     m_model_pos_setflag;
   bool                     m_model_varname_setflag;
   bool                     m_model_time_request_offset_setflag;
   std::string              m_filepath_moosvar;
   std::string              m_positions_moosvar;
   std::string              m_varname_moosvar;
   std::string              m_time_request_offset_moosvar;
   std::string              m_time_offset_reset_moosvar;
   std::string              m_model_return_moosvar;
   bool                     m_appcasting_ready;
   clock_t                  m_start;
   clock_t                  m_end;
   double                   m_request_time;
};

#endif
