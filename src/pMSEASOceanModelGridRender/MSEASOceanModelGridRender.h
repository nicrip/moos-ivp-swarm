/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: MSEASOceanModelGridRender.h                     */
/*    DATE: August 2014                                     */
/************************************************************/

#ifndef MSEASOceanModelGridRender_HEADER
#define MSEASOceanModelGridRender_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "XYConvexGrid.h"
#include "XYVector.h"
#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>

class MSEASOceanModelGridRender : public CMOOSApp
{
 public:
   MSEASOceanModelGridRender();
   ~MSEASOceanModelGridRender();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
  unsigned int    m_iterations;
  double          m_timewarp;
  CMOOSGeodesy    m_geodesy;
  double          m_origin_lon;
  double          m_origin_lat;
  double          m_depth;
  int             m_grid_num_x_els;
  int             m_grid_num_y_els;
  double          m_grid_min_value;
  double          m_grid_max_value;
  double          m_grid_width_space;
  double          m_grid_height_space;
  int             m_grid_num_els;
  Matrix          m_model_pos_request;
  unsigned char*  m_model_pos_request_data_ptr;
  Matrix          m_model_values_return;
  std::string     m_varname_request;
  std::string     m_filepath_request;
  double          m_time_offset_request;
  std::string     m_moosvar_prefix;
  std::string     m_filepath_moosvar;
  std::string     m_positions_moosvar;
  std::string     m_varname_moosvar;
  std::string     m_time_request_offset_moosvar;
  std::string     m_time_offset_reset_moosvar;
  std::string     m_model_return_moosvar;
  XYConvexGrid    m_grid;
  std::vector<XYVector> m_vector_field;
  double          m_global_lon_origin;
  double          m_global_lat_origin;
  int             m_varnames_size;
  double          m_vector_power;
  double          m_vector_scale;
};

#endif
