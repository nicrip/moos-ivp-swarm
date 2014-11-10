/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: MSEASOceanModelGridRender.cpp                   */
/*    DATE: August 2014                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "MSEASOceanModelGridRender.h"
#include "XYFormatUtilsConvexGrid.h"
#include <math.h>

using namespace std;

//---------------------------------------------------------
// Constructor

MSEASOceanModelGridRender::MSEASOceanModelGridRender()
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

MSEASOceanModelGridRender::~MSEASOceanModelGridRender()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MSEASOceanModelGridRender::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == m_model_return_moosvar) {
      cout << "RECEIVED RETURN DATA" << endl;
      int num_values = p->GetBinaryDataSize()/sizeof(double);
      int num_cols = num_values/m_varnames_size;
      Matrix a(m_varnames_size, num_cols);
      double *a_data = a.fortran_vec();
      memcpy(a_data, p->GetBinaryData(), p->GetBinaryDataSize());
      m_model_values_return = a;
      cout << m_model_values_return << endl;
      // update grid values
      unsigned int i, gsize = m_grid.size();
      stringstream iss;
      for(i=0; i<gsize; i++) {
        double cx = m_grid.getElement(i).getCenterX();
        double cy = m_grid.getElement(i).getCenterY();
        m_vector_field[i].setPosition(cx,cy);
        double dx = m_model_values_return(0,i);
        double dy = m_model_values_return(1,i);
        if (dx < 0) {
          dx = pow(dx, m_vector_power)*m_vector_scale;
          if (dx > 0) dx = -dx;
        } else {
          dx = pow(dx, m_vector_power)*m_vector_scale;
        }
        if (dy < 0) {
          dy = -pow(dy, m_vector_power)*m_vector_scale;
          if (dy > 0) dy = -dy;
        } else {
          dy = pow(dy, m_vector_power)*m_vector_scale;
        }
        m_vector_field[i].setVectorXY(dx, dy);
        iss << m_moosvar_prefix << i;
        m_vector_field[i].set_label(iss.str());
        iss.clear();
        iss.str("");
        string spec = m_vector_field[i].get_spec();
        m_Comms.Notify("VIEW_VECTOR", spec);
        m_grid.setVal(i, m_model_values_return(0,i));
        //cout << cx << " " << cy << " " << m_grid.getVal(i) << endl;
      }
      //post grid for viewing
      stringstream ss;
      ss << m_moosvar_prefix << "GRID";
      m_grid.set_label(ss.str());
      m_grid.set_active(true);
      string spec = m_grid.get_spec();
      m_Comms.Notify("VIEW_GRID", spec);
//      cout << m_model_values_return << endl;
//      cout << m_model_values_return.rows() << ", " << m_model_values_return.cols() << endl;
    }

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MSEASOceanModelGridRender::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);

   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MSEASOceanModelGridRender::Iterate()
{
  m_iterations++;

  //cout << "REQUESTING GRID DATA" << endl;
  m_Comms.Notify(m_positions_moosvar, (unsigned char*) m_model_pos_request_data_ptr, (unsigned int) ((m_grid_num_els*3)*sizeof(double))); // Matrix of lon, lat, depth positions
  //m_Comms.Notify(m_varname_moosvar, m_varname_request);                                                                                   // name of desired model variable
  //m_Comms.Notify(m_time_request_offset_moosvar, m_time_offset_request);                                                                   // offset in seconds from start of MSEAS model file
  //m_Comms.Notify(m_filepath_moosvar, m_filepath_request);                                                                                 // filepath of MSEAS model file - when msg received, request is performed

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MSEASOceanModelGridRender::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);

      if(param == "FOO") {
        //handled
      }
      else if(param == "BAR") {
        //handled
      }
    }
  }

  m_timewarp = GetMOOSTimeWarp();

  /* EXAMPLE MATRIX BINARY DATA MOOS TRANSFER */
//  int numel = 10000;
//  Matrix test(numel,3);
//  test.fill(0.0);
//  for (int i = 0; i < numel; i++) {
//    test(i, 0) = -73.0;
//    test(i, 1) = 39.0;
//    test(i, 2) = 100.0;
//  }
//  cout << test << endl;
//  unsigned char* test_pointer = (unsigned char*) test.fortran_vec();
//  m_Comms.Notify("MATRIX_TEST", (unsigned char*) test_pointer, (unsigned int) (test.rows()*test.cols()*sizeof(double)));

  if (!m_MissionReader.GetConfigurationParam("ORIGIN_LAT", m_origin_lat)) {
    cerr << "Origin latitude ORIGIN_LAT not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("ORIGIN_LON", m_origin_lon)) {
    cerr << "Origin longitude ORIGIN_LON not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("DEPTH", m_depth)) {
    cerr << "Depth DEPTH not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("GRID_NUM_X", m_grid_num_x_els)) {
    cerr << "Grid number of x elements GRID_NUM_X not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("GRID_NUM_Y", m_grid_num_y_els)) {
    cerr << "Grid number of y elements GRID_NUM_Y not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("GRID_MIN_VALUE", m_grid_min_value)) {
    cerr << "Grid minimum value GRID_MIN_VALUE not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("GRID_MAX_VALUE", m_grid_max_value)) {
    cerr << "Grid maximum value GRID_MAX_VALUE not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("GRID_WIDTH_SPACE", m_grid_width_space)) {
    cerr << "Grid width spacing GRID_WIDTH_SPACE not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("GRID_HEIGHT_SPACE", m_grid_height_space)) {
    cerr << "Grid height spacing GRID_HEIGHT_SPACE not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("VARNAME_REQUEST", m_varname_request)) {
    cerr << "Request variable name VARNAME_REQUEST not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("FILEPATH_REQUEST", m_filepath_request)) {
    cerr << "Request filepath FILEPATH_REQUEST not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("TIME_OFFSET_REQUEST", m_time_offset_request)) {
    cerr << "Request time offset TIME_OFFSET_REQUEST not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("MOOSVAR_PREFIX", m_moosvar_prefix)) {
    cerr << "MOOS variable prefix MOOSVAR_PREFIX not specified! Assuming no prefix..." << endl;
    m_moosvar_prefix = "";
  }

  if (!m_MissionReader.GetConfigurationParam("VECTOR_POWER", m_vector_power)) {
    cerr << "Vector power VECTOR_POWER not specified! Assuming power 2.0..." << endl;
    m_vector_power = 2.0;
  }

  if (!m_MissionReader.GetConfigurationParam("VECTOR_SCALE", m_vector_scale)) {
    cerr << "Vector scale VECTOR_SCALE not specified! Assuming scale 1.0..." << endl;
    m_vector_scale = 1.0;
  }

  if (!m_MissionReader.GetValue("LatOrigin", m_global_lat_origin)) {
    cerr << "MOOS GLOBAL variable latitude origin LatOrigin not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetValue("LongOrigin", m_global_lon_origin)) {
    cerr << "MOOS GLOBAL variable longitude origin LongOrigin not specified! Quitting..." << endl;
    return(false);
  }

  m_geodesy.Initialise(m_origin_lat, m_origin_lon);
  m_grid_num_els = m_grid_num_x_els*m_grid_num_y_els;
  Matrix a(m_grid_num_els, 3);
  m_model_pos_request = a;
  double cur_lon, cur_lat;
  double cur_height_space = 0;
  int cur_idx = 0;
  for(int i = 0; i < m_grid_num_y_els; i++) {
    double cur_width_space = 0;
    for(int j = 0; j < m_grid_num_x_els; j++) {
      m_geodesy.UTM2LatLong(cur_width_space, cur_height_space, cur_lat, cur_lon);
      m_model_pos_request(cur_idx, 0) = cur_lon;
      m_model_pos_request(cur_idx, 1) = cur_lat;
      m_model_pos_request(cur_idx, 2) = m_depth;
//      cout << cur_idx << endl;
//      cout << cur_width_space << " " << cur_height_space << endl;
//      cout << cur_lon << " " << cur_lat << endl;
      cur_width_space += m_grid_width_space;
      cur_idx++;
    }
    cur_height_space += m_grid_height_space;
  }
//  cout << m_model_pos_request << endl;
//  cout << m_model_pos_request.rows() << endl;
  m_model_pos_request_data_ptr = (unsigned char*) m_model_pos_request.fortran_vec();

  stringstream ss;
  ss << m_moosvar_prefix << "MSEASOCEANMODEL_FILEPATH";
  m_filepath_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_moosvar_prefix << "MSEASOCEANMODEL_POSITIONS";
  m_positions_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_moosvar_prefix << "MSEASOCEANMODEL_VARNAME";
  m_varname_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_moosvar_prefix << "MSEASOCEANMODEL_TIME_REQUEST_OFFSET";
  m_time_request_offset_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_moosvar_prefix << "MSEASOCEANMODEL_TIME_OFFSET_RESET";
  m_time_offset_reset_moosvar = ss.str();
  ss.str("");
  ss.clear();

  cout << "INITIAL REQUEST OF GRID DATA" << endl;
  m_Comms.Notify(m_positions_moosvar, (unsigned char*) m_model_pos_request_data_ptr, (unsigned int) ((m_grid_num_els*3)*sizeof(double)));
  m_Comms.Notify(m_varname_moosvar, m_varname_request);
  m_Comms.Notify(m_time_request_offset_moosvar, m_time_offset_request);
  vector<string> model_varname_vec_request = parseString(m_varname_request, ',');
  m_varnames_size = model_varname_vec_request.size();

  // set up grid for rendering
  double width = m_grid_num_x_els*m_grid_width_space;
  double height = m_grid_num_y_els*m_grid_height_space;
  m_vector_field.resize(m_grid_num_x_els*m_grid_num_y_els);
  double tl_x, tl_y;
  m_geodesy.LatLong2LocalUTM(m_global_lat_origin, m_global_lon_origin, tl_y, tl_x);
  tl_x = -tl_x;
  tl_y = -tl_y;
  cout << m_origin_lat << " " << m_origin_lon << endl;
  cout << tl_y << " " << tl_x << endl;
  pair<double, double> top_left(tl_x, tl_y);
  pair<double, double> top_right(tl_x+width, tl_y);
  pair<double, double> bot_right(tl_x+width, tl_y+height);
  pair<double, double> bot_left(tl_x, tl_y+height);
  double cell_size = m_grid_width_space;
  ss << "pts={" << top_left.first << "," << top_left.second << ": " << top_right.first << "," << top_right.second << ": " << bot_right.first << "," << bot_right.second << ": " << bot_left.first << "," << bot_left.second << "},cell_size=" << cell_size << ",cell_vars=x:0";
  std::string grid_config = ss.str();
  cout << grid_config << endl;
  m_grid = string2ConvexGrid(grid_config);
  ss.str("");
  ss.clear();

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void MSEASOceanModelGridRender::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  stringstream ss;

  ss << m_moosvar_prefix << "MSEASOCEANMODEL_RETURN";
  m_model_return_moosvar = ss.str();
  m_Comms.Register(m_model_return_moosvar, 0);            // data returned by iMSEASOceanModel
  ss.str("");
  ss.clear();
}

