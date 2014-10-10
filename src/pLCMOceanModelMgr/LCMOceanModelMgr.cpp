/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LCMOceanModelMgr.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "LCMOceanModelMgr.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LCMOceanModelMgr::LCMOceanModelMgr() : m_all_set(false), m_first_request(true)
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

LCMOceanModelMgr::~LCMOceanModelMgr()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool LCMOceanModelMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  stringstream ss;
  int num_rows;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == m_model_return_moosvar) {
      num_rows = p->GetBinaryDataSize()/sizeof(double);
      Matrix a(num_rows, 1);
      double *a_data = a.fortran_vec();
      memcpy(a_data, p->GetBinaryData(), p->GetBinaryDataSize());
      m_model_values_return = a;
      for(unsigned int i=0; i<m_num_nodes; i++) {
        ss << m_node_prefix << (m_start_node_idx + i) << "_DRIFT";
        node_drift_t out;
        out.drift_x = m_model_values_return(i)/100;
        out.drift_y = m_model_values_return(i+m_num_nodes)/100;
        m_lcm->publish(ss.str(), &out);
        cout << ss.str() << " X: " << out.drift_x << " Y: " << out.drift_y << endl;
        ss.str("");
        ss.clear();
      }
      OceanModelRequest();
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

bool LCMOceanModelMgr::OnConnectToServer()
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

bool LCMOceanModelMgr::Iterate()
{
  m_iterations++;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool LCMOceanModelMgr::OnStartUp()
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

  m_Comms.Register(m_model_return_moosvar, 0);

  Matrix a(m_num_nodes*2, 3);
  m_model_pos_request = a;
  m_model_pos_request_data_ptr = (unsigned char*)m_model_pos_request.fortran_vec();

  m_nodes_nav_set.resize(m_num_nodes);
  m_nodes_nav_set.reset();

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

  m_lcm = new lcm::LCM();
  LCMSubscribe();
  pthread_t lcm_thread;
  if(pthread_create(&lcm_thread, NULL, LCMOceanModelMgr::LCMHandle, this)) {
    cout << "Error creating LCM thread" << endl;
    return(false);
  }

  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void LCMOceanModelMgr::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
}

void LCMOceanModelMgr::OceanModelRequest()
{
  m_Comms.Notify(m_positions_moosvar, (unsigned char*) m_model_pos_request_data_ptr, (unsigned int) ((m_num_nodes*2*3)*sizeof(double)));
  m_Comms.Notify(m_varname_moosvar, m_model_varnames);
  m_Comms.Notify(m_filepath_moosvar, m_filepath);
  m_Comms.Notify(m_time_request_offset_moosvar, 0.0);
}

//----------------------------------------------------------
// LCM FUNCTIONS

void* LCMOceanModelMgr::LCMHandle(void* data) {
  while(0==((LCMOceanModelMgr *) data)->m_lcm->handle());
}

void LCMOceanModelMgr::LCMSubscribe() {
  stringstream ss;
  for(unsigned int i=0; i<m_num_nodes; i++) {
    ss << m_node_prefix << (m_start_node_idx + i) << "_NAV";
    m_lcm->subscribe(string(ss.str()), &LCMOceanModelMgr::LCMCallback_node_nav_t, this);
    ss.clear();
    ss.str("");
  }
}

void LCMOceanModelMgr::LCMCallback_node_nav_t(const lcm::ReceiveBuffer* rbuf, const string& channel, const node_nav_t* msg) {
  std::string node_name = channel;
  double node_long = msg->nav_long;
  double node_lat = msg->nav_lat;
  double node_depth = msg->nav_depth;
  stringstream ss;
  for(unsigned int i=0; i<m_num_nodes; i++) {
    ss << m_node_prefix << (m_start_node_idx + i) << "_NAV";
    if (ss.str() == node_name) {
      m_model_pos_request(i, 0) = node_long;
      m_model_pos_request(i, 1) = node_lat;
      m_model_pos_request(i, 2) = node_depth;
      m_model_pos_request(i+m_num_nodes, 0) = node_long;
      m_model_pos_request(i+m_num_nodes, 1) = node_lat;
      m_model_pos_request(i+m_num_nodes, 2) = node_depth;
      if (!m_all_set) {
        m_nodes_nav_set[i] = true;
        m_all_set = m_nodes_nav_set.count() == m_num_nodes;
      }
      if (m_all_set && m_first_request) {
        OceanModelRequest();
        m_first_request = false;
      }
      ss.str("");
      ss.clear();
      break;
    }
    ss.str("");
    ss.clear();
  }
}

