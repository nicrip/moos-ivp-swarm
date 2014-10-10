/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: SwarmManager.cpp                                */
/*    DATE: August 2014                                     */
/************************************************************/

#include <iterator>
#include <ctime>
#include "MBUtils.h"
#include "SwarmManager.h"

using namespace std;

//---------------------------------------------------------
// Constructor

SwarmManager::SwarmManager()
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

SwarmManager::~SwarmManager()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SwarmManager::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  stringstream ss;
  double drift_x;  //MSEAS model velocity data is in cm/s
  double drift_y;
  int num_rows;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    for(unsigned int i=0; i<m_num_nodes; i++) {
      ss << "NAV_LONG_" << m_node_prefix << (m_start_node_idx + i);
      if (msg.GetKey() == ss.str()) {
        m_node_long = msg.GetDouble();
        m_model_pos_request(i, 0) = m_node_long;
        m_model_pos_request(i, 2) = 100;  //depth is artificially set for now
        m_model_pos_request(i+m_num_nodes, 0) = m_node_long;
        m_model_pos_request(i+m_num_nodes, 2) = 100;  //depth is artificially set for now
        m_nodes_long_set[i] = true;
      }
      ss.str("");
      ss.clear();
      ss << "NAV_LAT_" << m_node_prefix << (m_start_node_idx + i);
      if (msg.GetKey() == ss.str()) {
        m_node_lat = msg.GetDouble();
        m_model_pos_request(i, 1) = m_node_lat;
        m_model_pos_request(i+m_num_nodes, 1) = m_node_lat;
        m_nodes_lat_set[i] = true;
        //cout << ss.str() << endl;
      }
      ss.str("");
      ss.clear();
      //cout << m_nodes_lat_set << endl;
    }
    if (msg.GetKey() == m_model_return_moosvar) {
      num_rows = p->GetBinaryDataSize()/sizeof(double);
      Matrix a(num_rows, 1);
      double *a_data = a.fortran_vec();
      memcpy(a_data, p->GetBinaryData(), p->GetBinaryDataSize());
      m_model_values_return = a;
      for(unsigned int i=0; i<m_num_nodes; i++) {
        ss << "DRIFT_X_" << m_node_prefix << (m_start_node_idx + i);
        drift_x = m_model_values_return(i)/100;  //MSEAS model velocity data is in cm/s
        m_Comms.Notify(ss.str(), drift_x);
        ss.str("");
        ss.clear();
        ss << "DRIFT_Y_" << m_node_prefix << (m_start_node_idx + i);
        drift_y = m_model_values_return(i+m_num_nodes)/100;
        m_Comms.Notify(ss.str(), drift_y);
        ss.str("");
        ss.clear();
      }
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

bool SwarmManager::OnConnectToServer()
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

bool SwarmManager::Iterate()
{
  m_iterations++;

  bool all_long, all_lat;
  all_long = m_nodes_long_set.count() == m_num_nodes;
  all_lat = m_nodes_lat_set.count() == m_num_nodes;
  if (all_long && all_lat) {
    cout << "RECEIVED POSITION DATA FOR ALL NODES" << endl;
//    cout << m_model_pos_request << endl;
//    cout << m_model_varnames << endl;
    m_Comms.Notify(m_positions_moosvar, (unsigned char*) m_model_pos_request_data_ptr, (unsigned int) ((m_num_nodes*2*3)*sizeof(double)));
    m_Comms.Notify(m_varname_moosvar, m_model_varnames);
    m_Comms.Notify(m_filepath_moosvar, m_filepath);
    m_Comms.Notify(m_time_request_offset_moosvar, 0.0);
    m_end_time = clock();
    double elapsed_secs = double(m_end_time - m_start_time) / CLOCKS_PER_SEC;
    cout << "ELAPSED TIME TO RECEIVE DATA FOR ALL NODES: " << elapsed_secs << endl;
    m_nodes_long_set.reset();
    m_nodes_lat_set.reset();
    m_start_time = clock();
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SwarmManager::OnStartUp()
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

  if (!m_MissionReader.GetConfigurationParam("MSEAS_FILEPATH", m_filepath)) {
    cerr << "MSEAS model filepath MSEAS_FILEPATH not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("MSEAS_TIME_OFFSET", m_time_offset)) {
    cerr << "MSEAS model time offset MSEAS_TIME_OFFSET not specified! Assuming 0..." << endl;
    m_time_offset = 0;
  }

  if (!m_MissionReader.GetConfigurationParam("NODE_PREFIX", m_node_prefix)) {
    cerr << "Node prefix NODE_PREFIX not specified! Assuming no prefix..." << endl;
    m_node_prefix = "";
  }

  if (!m_MissionReader.GetConfigurationParam("SWARM_SUBSET_PREFIX", m_subset_prefix)) {
    cerr << "Swarm subset prefix SWARM_SUBSET_PREFIX not specified! Assuming no prefix..." << endl;
    m_subset_prefix = "";
  }

  if (!m_MissionReader.GetConfigurationParam("NODE_START_INDEX", m_start_node_idx)) {
    cerr << "Node start index NODE_START_INDEX not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("NODE_END_INDEX", m_end_node_idx)) {
    cerr << "Node end index NODE_END_INDEX not specified! Quitting..." << endl;
    return(false);
  }

  if (m_end_node_idx < m_start_node_idx) {
    cerr << "Node end index must be larger than node start index! Quitting..." << endl;
    return(false);
  } else {
    m_num_nodes = m_end_node_idx - m_start_node_idx + 1;
  }

  stringstream ss;
  ss << m_subset_prefix << "MSEASOCEANMODEL_FILEPATH";
  m_filepath_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_subset_prefix << "MSEASOCEANMODEL_POSITIONS";
  m_positions_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_subset_prefix << "MSEASOCEANMODEL_VARNAME";
  m_varname_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_subset_prefix << "MSEASOCEANMODEL_TIME_REQUEST_OFFSET";
  m_time_request_offset_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_subset_prefix << "MSEASOCEANMODEL_TIME_OFFSET_RESET";
  m_time_offset_reset_moosvar = ss.str();
  ss.str("");
  ss.clear();
  ss << m_subset_prefix << "MSEASOCEANMODEL_RETURN";
  m_model_return_moosvar = ss.str();
  ss.str("");
  ss.clear();

  Matrix a(m_num_nodes*2, 3);
  m_model_pos_request = a;
  m_model_pos_request_data_ptr = (unsigned char*)m_model_pos_request.fortran_vec();

  m_nodes_long_set.resize(m_num_nodes);
  m_nodes_lat_set.resize(m_num_nodes);
  m_nodes_long_set.reset();
  m_nodes_lat_set.reset();
  m_start_time = clock();

  m_timewarp = GetMOOSTimeWarp();

  stringstream varname_v_ss, varname_u_ss;
  for(unsigned int i=0; i<m_num_nodes; i++) {
    if (i != m_num_nodes-1) {
      varname_v_ss << "v,";
      varname_u_ss << "u,";
    } else {
      varname_v_ss << "v,";
      varname_u_ss << "u";
    }
  }
  cout << m_num_nodes << endl;
  cout << varname_u_ss.str() << endl;
  cout << varname_v_ss.str() << endl;
  varname_v_ss << varname_u_ss.str();
  m_model_varnames = varname_v_ss.str();
  cout << m_model_varnames << endl;
  cout << varname_v_ss.str() << endl;
  varname_v_ss.str("");
  varname_v_ss.clear();
  varname_u_ss.str("");
  varname_u_ss.clear();

  RegisterVariables();

  //stringstream ss;
  for(unsigned int i=0; i<m_num_nodes; i++) {
    ss << "NAV_LONG_" << m_node_prefix << (m_start_node_idx + i);
    m_Comms.Register(ss.str(), 0);
    ss.str("");
    ss.clear();
    ss << "NAV_LAT_" << m_node_prefix << (m_start_node_idx + i);
    m_Comms.Register(ss.str(), 0);
    ss.str("");
    ss.clear();
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void SwarmManager::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register(m_model_return_moosvar, 0);
}

