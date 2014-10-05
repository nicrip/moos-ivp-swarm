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

 private: // State variables
   unsigned int             m_iterations;
   double                   m_timewarp;
   std::string              m_octave_path;
   Matrix                   m_model_pos_request;
   std::string              m_model_varnames_request;
   std::vector<std::string> m_model_varname_vec_request;
   Cell                     m_model_varname_cell_request;
   double                   m_model_time_request;
   std::string              m_model_filepath;
   double                   m_model_time_offset;
   double                   m_model_time_request_offset;
   octave_value_list        m_batch_request;
   octave_value_list        m_batch_return;
   Matrix                   m_model_values_return;
   unsigned char*           m_model_values_return_data_ptr;
   bool                     m_request_flag;
   bool                     m_reset_time_flag;
   bool                     m_model_pos_setflag;
   bool                     m_model_varname_setflag;
   bool                     m_model_time_request_offset_setflag;
   std::string              m_moosvar_prefix;
   std::string              m_filepath_moosvar;
   std::string              m_positions_moosvar;
   std::string              m_varname_moosvar;
   std::string              m_time_request_offset_moosvar;
   std::string              m_time_offset_reset_moosvar;
   std::string              m_model_return_moosvar;
   bool                     m_appcasting_ready;
};

#endif
