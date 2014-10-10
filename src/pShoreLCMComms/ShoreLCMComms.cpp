/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: ShoreLCMComms.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ShoreLCMComms.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ShoreLCMComms::ShoreLCMComms()
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

ShoreLCMComms::~ShoreLCMComms()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ShoreLCMComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  stringstream ss;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == "DEPLOY_ALL") {
      for(unsigned int i=0; i<m_num_nodes; i++) {
        m_lcm_out[i].m_deploy = msg.GetString();
      }

    } else if (msg.GetKey() == "RETURN_ALL") {
      for(unsigned int i=0; i<m_num_nodes; i++) {
        m_lcm_out[i].m_return = msg.GetString();
      }
    } else if (msg.GetKey() == "MOOS_MANUAL_OVERRIDE_ALL") {
      for(unsigned int i=0; i<m_num_nodes; i++) {
        m_lcm_out[i].m_moos_manual_override = msg.GetString();
      }
    } else {
      for(unsigned int i=0; i<m_num_nodes; i++) {
        ss << "DRIFT_X_" << m_node_prefix << (m_node_start_idx + i);
        if (msg.GetKey() == ss.str()) {
          m_lcm_out[i].m_drift_x = msg.GetDouble();
        }
        ss.str("");
        ss.clear();
        ss << "DRIFT_Y_" << m_node_prefix << (m_node_start_idx + i);
        if (msg.GetKey() == ss.str()) {
          m_lcm_out[i].m_drift_y = msg.GetDouble();
        }
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

bool ShoreLCMComms::OnConnectToServer()
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

bool ShoreLCMComms::Iterate()
{
  m_iterations++;

  stringstream ss;
  for(unsigned int i=0; i<m_num_nodes; i++) {
    ss << "NODE_" << (m_node_start_idx + i) << "_TO_NODE";
    node_in_t out;
    out.drift_x = m_lcm_out[i].m_drift_x;
    out.drift_y = m_lcm_out[i].m_drift_y;
    out.deploy_str = m_lcm_out[i].m_deploy;
    out.return_str = m_lcm_out[i].m_return;
    out.moos_manual_override_str = m_lcm_out[i].m_moos_manual_override;
    m_lcm->publish(ss.str(), &out);
    ss.clear();
    ss.str("");
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool ShoreLCMComms::OnStartUp()
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

  if (!m_MissionReader.GetConfigurationParam("NODE_PREFIX", m_node_prefix)) {
    cerr << "Node prefix NODE_PREFIX not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("NODE_START_INDEX", m_node_start_idx)) {
    cerr << "Node start index NODE_START_IDX not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("NODE_END_INDEX", m_node_end_idx)) {
    cerr << "Node end index NODE_END_INDEX not specified! Quitting..." << endl;
    return(false);
  }

  if (m_node_end_idx < m_node_start_idx) {
    cerr << "Node end index must be larger than node start index! Quitting..." << endl;
    return(false);
  } else {
    m_num_nodes = m_node_end_idx - m_node_start_idx + 1;
  }

  m_lcm_out.resize(m_num_nodes);

  m_lcm = new lcm::LCM();
  LCMSubscribe();
  pthread_t lcm_thread;
  if(pthread_create(&lcm_thread, NULL, ShoreLCMComms::LCMHandle, this)) {
    cout << "Error creating LCM thread" << endl;
    return(false);
  }

  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();

  stringstream ss;
  for(unsigned int i=0; i<m_num_nodes; i++) {
    ss << "DRIFT_X_" << m_node_prefix << (m_node_start_idx + i);
    m_Comms.Register(ss.str(), 0);
    ss.clear();
    ss.str("");
    ss << "DRIFT_Y_" << m_node_prefix << (m_node_start_idx + i);
    m_Comms.Register(ss.str(), 0);
    ss.clear();
    ss.str("");
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void ShoreLCMComms::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("DEPLOY_ALL", 0);
  m_Comms.Register("RETURN_ALL", 0);
  m_Comms.Register("MOOS_MANUAL_OVERRIDE_ALL", 0);
}

//----------------------------------------------------------
// LCM FUNCTIONS

void* ShoreLCMComms::LCMHandle(void* data) {
  while(0==((ShoreLCMComms *) data)->m_lcm->handle());
}

void ShoreLCMComms::LCMSubscribe() {
  stringstream ss;
  for(unsigned int i=0; i<m_num_nodes; i++) {
    ss << m_node_prefix << (m_node_start_idx + i) << "_TO_SHORE";
    m_lcm->subscribe(string(ss.str()), &ShoreLCMComms::LCMCallback_node_out_t, this);
    ss.clear();
    ss.str("");
  }
}

void ShoreLCMComms::LCMCallback_node_out_t(const lcm::ReceiveBuffer* rbuf, const string& channel, const node_out_t* msg) {
  stringstream ss;
  m_node_name = msg->node_name;
  m_nav_long = msg->nav_long;
  ss << "NAV_LONG_" << m_node_name;
  m_Comms.Notify(ss.str(), m_nav_long);
  ss.clear();
  ss.str("");
  m_nav_lat = msg->nav_lat;
  ss << "NAV_LAT_" << m_node_name;
  m_Comms.Notify(ss.str(), m_nav_lat);
  ss.clear();
  ss.str("");
  m_node_report_local = msg->node_report_local;
  m_Comms.Notify("NODE_REPORT", m_node_report_local);
}
