/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: MSEASOceanModel.cpp                             */
/*    DATE: August 2014                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "MSEASOceanModel.h"
#include <ctime>
#include <sstream>

using namespace std;

//---------------------------------------------------------
// Constructor

MSEASOceanModel::MSEASOceanModel() : m_request_flag(false), m_reset_time_flag(false), m_model_pos_setflag(false), m_model_varname_setflag(false), m_model_time_request_offset_setflag(false), m_appcasting_ready(false)
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

MSEASOceanModel::~MSEASOceanModel()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MSEASOceanModel::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == m_positions_moosvar) {
      int num_values = p->GetBinaryDataSize()/sizeof(double);
      int num_rows = num_values/3;
      Matrix a(num_rows, 3);
      double *a_data = a.fortran_vec();
      memcpy(a_data, p->GetBinaryData(), p->GetBinaryDataSize());
      m_model_pos_request = a;
      //cout << m_model_pos_request << endl;
      //cout << m_model_pos_request.rows() << ", " << m_model_pos_request.cols() << endl;
      m_model_pos_setflag = true;
    } else if (msg.GetKey() == m_varname_moosvar) {
      //cout << msg.GetString() << endl;
      m_model_varnames_request = msg.GetString();
      m_model_varname_vec_request = parseString(m_model_varnames_request, ',');
      int varnames_size = m_model_varname_vec_request.size();
      Cell a(varnames_size, 1);
      for(unsigned int i=0; i<varnames_size; i++)
        a(i) = m_model_varname_vec_request[i];
      m_model_varname_cell_request = a;
      m_model_varname_setflag = true;
    } else if (msg.GetKey() == m_time_request_offset_moosvar) {
      //cout << msg.GetDouble() << endl;
      m_model_time_request_offset = msg.GetDouble();
      m_model_time_request_offset_setflag = true;
    } else if (msg.GetKey() == m_time_offset_reset_moosvar) {
      //cout << msg.GetDouble() << endl;
      m_model_time_request_offset = msg.GetDouble();
      m_reset_time_flag = true;
    } else if (msg.GetKey() == m_filepath_moosvar) {
      //cout << msg.GetString() << endl;
      m_model_filepath = msg.GetString();
      m_request_flag = true;
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

bool MSEASOceanModel::OnConnectToServer()
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

bool MSEASOceanModel::Iterate()
{
  AppCastingMOOSApp::Iterate();
  m_iterations++;

  if(m_reset_time_flag) {
    m_model_time_offset = MOOSTime();
  }

  if(m_request_flag && m_model_pos_setflag && m_model_varname_setflag && m_model_time_request_offset_setflag) {
    m_model_time_request = MOOSTime() + m_model_time_request_offset;
    cout << "filepath requested: " << m_model_filepath << endl;
    cout << "varname requested: " << m_model_varnames_request << endl;
    cout << "utc model time requested: " << m_model_time_request << endl;
    cout << "utc begin time: " << m_model_time_offset << endl;
    cout << "seconds since start (delta): " << (m_model_time_request - m_model_time_offset) << endl;
    m_batch_request(0) = m_model_pos_request;
    m_batch_request(1) = m_model_varname_cell_request;
    //--------------------------------------------------------------------//
    //Matrix a(m_model_pos_request.rows(), 1);
    //octave_value_list a;
    //a(0) = "meridional velocity";
    //a(1) = "zonal velocity";
    //for (int i = 2; i < m_model_pos_request.rows(); i++) {
    //  a(i) = "meridional velocity";
    //}
    //m_batch_request(1) = a;
    //--------------------------------------------------------------------//
    m_batch_request(2) = m_model_time_request;
    m_batch_request(3) = m_model_filepath;
    m_batch_request(4) = m_model_time_offset;
    m_batch_return = feval("mseas_model_batch_request", m_batch_request, 1);
    cout << "TIME TO PROCESS " << m_model_pos_request.rows() << " REQUESTS: " << m_batch_return(2).scalar_value() << m_batch_return(2).scalar_value() << " seconds." << endl;
    Matrix model_values_return = m_batch_return(0).matrix_value();
    m_model_values_return = model_values_return;
    cout << "FIRST VALUE RETURNED: " << m_model_values_return(0) << endl;
    m_model_values_return_data_ptr = (unsigned char*) m_model_values_return.fortran_vec();
    cout << "NUMBER OF RETURNS: " << m_model_values_return.rows() << endl;
    m_Comms.Notify(m_model_return_moosvar, (unsigned char*) m_model_values_return_data_ptr, (unsigned int) (m_model_values_return.rows()*sizeof(double)));
    m_request_flag = false;
    m_appcasting_ready = true;
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MSEASOceanModel::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
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

  // get path to octave files -> if unspecified, exit
  if (!m_MissionReader.GetConfigurationParam("OCTAVE_PATH", m_octave_path)) {
    cerr << "Path to required octave files OCTAVE_PATH not specified! Quitting..." << endl;
    return(false);
  }

  // get MOOS variable prefix -> if unspecified, assume no prefix
  if (!m_MissionReader.GetConfigurationParam("MOOSVAR_PREFIX", m_moosvar_prefix)) {
    cerr << "MOOS variable prefix MOOSVAR_PREFIX not specified! Assuming no prefix..." << endl;
    m_moosvar_prefix = "";
  }

  // initialize instance of octave
  string_vector argv (4);
  argv(0) = "embedded";
  argv(1) = "-q";
  argv(2) = "--path";
  argv(3) = m_octave_path;
  octave_main (4, argv.c_str_vec (), 1);
  m_batch_request(5);
  m_batch_return(3);

  // set time offset
  m_model_time_offset = MOOSTime();

//  /* EXAMPLE GENERIC OCTAVE USAGE */
//  Matrix inMatrix (2, 3);
//  inMatrix (0, 0) = 10;
//  inMatrix (0, 1) = 9;
//  inMatrix (0, 2) = 8;
//  inMatrix (1, 0) = 7;
//  inMatrix (1, 1) = 6;
//  octave_value_list in(4);
//  in(0) = inMatrix;
//  in(1) = 10;
//  in(2) = "TEST STRING IN";
//  std::cout << "MATRIX SIZE " << in(0).size() << std::endl;
//  Cell b(1, 2);
//  b(0) = "test string 1";
//  b(1) = "test string again";
//  Array<string> c = b.cellstr_value();
//  cout << c(0) << endl;
//  cout << c(1) << endl;
//  in(3) = c;
//  unsigned char* d = (unsigned char*)c.fortran_vec();
//  octave_value_list out;
//  out = feval("test_octave", in, 1);
//  std::cout << "RETURNED" << std::endl;
//  std::cout << "OUTPUT LENGTH " << out.length() << std::endl;
//  std::cout << "MATRIX: " << out(0).matrix_value() << std::endl;
//  std::cout << "DOUBLE: " << out(1).scalar_value() << std::endl;
//  std::cout << "STRING: " << out(2).string_value() << std::endl;

  // example usage of mseas_model_batch_request.m
  int numel = 4;
  Matrix model_pos_request(numel,3);
  for (int i = 0; i < numel; i++) {
    model_pos_request(i, 0) = -73.0;
    model_pos_request(i, 1) = 39.0;
    model_pos_request(i, 2) = 100.0;
  }
  string model_varname_request = "meridional velocity";
  Cell a(numel, 1);
  for(unsigned int i=0; i<numel; i++)
    a(i) = model_varname_request;
  Cell model_varname_cell_request = a;
  double model_time_request = MOOSTime();
  string model_filepath = "/home/rypkema/Workspace/mseas_data/pe_out_vrot.nc";
  double model_time_offset = model_time_request;
  cout <<fixed << "CURRENT TIME: " << model_time_request << endl;
  octave_value_list batch_request(5);
  batch_request(0) = model_pos_request;
  batch_request(1) = model_varname_cell_request;
  batch_request(2) = model_time_request;
  batch_request(3) = model_filepath;
  batch_request(4) = model_time_offset;
  octave_value_list batch_return;
  batch_return = feval("mseas_model_batch_request", batch_request, 1);
  cout << "TIME TO PROCESS " << numel << " REQUESTS: " << batch_return(2).scalar_value() << " seconds." << endl;
  Matrix model_values = batch_return(0).matrix_value();
  cout << "FIRST VALUE RETURNED: " << model_values(0) << endl;

  stringstream ss;
  ss << m_moosvar_prefix << "MSEASOCEANMODEL_RETURN";
  m_model_return_moosvar = ss.str();
  ss.str("");
  ss.clear();

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void MSEASOceanModel::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // m_Comms.Register("FOOBAR", 0);
  stringstream ss;

  ss << m_moosvar_prefix << "MSEASOCEANMODEL_FILEPATH";
  m_filepath_moosvar = ss.str();
  m_Comms.Register(m_filepath_moosvar, 0);            // filepath of MSEAS model file - when msg received, request is performed
  ss.str("");
  ss.clear();

  ss << m_moosvar_prefix << "MSEASOCEANMODEL_POSITIONS";
  m_positions_moosvar = ss.str();
  m_Comms.Register(m_positions_moosvar, 0);           // Matrix of lon, lat, depth positions
  ss.str("");
  ss.clear();

  ss << m_moosvar_prefix << "MSEASOCEANMODEL_VARNAME";
  m_varname_moosvar = ss.str();
  m_Comms.Register(m_varname_moosvar, 0);             // name of desired model variable
  ss.str("");
  ss.clear();

  ss << m_moosvar_prefix << "MSEASOCEANMODEL_TIME_REQUEST_OFFSET";
  m_time_request_offset_moosvar = ss.str();
  m_Comms.Register(m_time_request_offset_moosvar, 0); // offset in seconds from start of MSEAS model file
  ss.str("");
  ss.clear();

  ss << m_moosvar_prefix << "MSEASOCEANMODEL_TIME_OFFSET_RESET";
  m_time_offset_reset_moosvar = ss.str();
  m_Comms.Register(m_time_offset_reset_moosvar, 0);   // resets the offset time of this program
  ss.str("");
  ss.clear();

  m_Comms.Register("MATRIX_TEST", 0);
}


//---------------------------------------------------------
// Procedure: buildReport

bool MSEASOceanModel::buildReport()
{
  if (m_appcasting_ready) {
    m_msgs << std::fixed;
    m_msgs << "REQUEST:" << endl;
    m_msgs << "   Requested MSEAS Filepath: " << m_model_filepath << endl;
    m_msgs << "   Requested Position (First): LON: " << m_model_pos_request(0,0) << ", LAT: " << m_model_pos_request(0,1) << ", DEP: " << m_model_pos_request(0,2) << endl;
    m_msgs << "   Requested Position (Last): LON: " << m_model_pos_request(m_model_pos_request.rows()-1,0) << ", LAT: " << m_model_pos_request(m_model_pos_request.rows()-1,1) << ", DEP: " << m_model_pos_request(m_model_pos_request.rows()-1,2) << endl;
    m_msgs << "   Requested Variables: " << m_model_varnames_request << endl;
    m_msgs << "   Requested Time Offset (seconds): " << m_model_time_request_offset << endl;
    m_msgs << "   Current Model Time (utc): " << m_model_time_request << endl;
    m_msgs << "   Model Start Time (utc): " << m_model_time_offset << endl;
    m_msgs << "   Requested Model Time (seconds): " << (m_model_time_request - m_model_time_offset) << endl;
    m_msgs << endl << "RETURN:" << endl;
    m_msgs << "   Processing Time for " << m_model_pos_request.rows() << " Requests (seconds): " << m_batch_return(2).scalar_value() << endl;
    m_msgs << "   Number of Returns: " << m_model_values_return.rows() << endl;
    m_msgs << "   Returned Value (First): " << m_model_values_return(0) << endl;
    m_msgs << "   Returned Value (Last): " << m_model_values_return(m_model_values_return.rows()-1) << endl;
    return(true);
  } else {
    m_msgs << "REQUEST:" << endl;
    m_msgs << "   No Request!" << endl;
    m_msgs << endl << "RETURN:" << endl;
    m_msgs << "   No Return!" << endl;
    return(true);
  }
}
