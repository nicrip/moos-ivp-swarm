/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: NodeLCMComms.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "NodeLCMComms.h"

using namespace std;

//---------------------------------------------------------
// Constructor

NodeLCMComms::NodeLCMComms() : m_lat_set(false), m_long_set(false), m_report_set(false)
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

NodeLCMComms::~NodeLCMComms()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool NodeLCMComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == "NAV_LAT") {
      m_nav_lat = msg.GetDouble();
      m_lat_set = true;
    } else if (msg.GetKey() == "NAV_LONG") {
      m_nav_long = msg.GetDouble();
      m_long_set = true;
    } else if (msg.GetKey() == "NODE_REPORT_LOCAL") {
      m_node_report_local = msg.GetString();
      m_report_set = true;
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

bool NodeLCMComms::OnConnectToServer()
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

bool NodeLCMComms::Iterate()
{
  m_iterations++;

  if (m_lat_set && m_long_set && m_report_set) {
    stringstream ss;
    ss << m_node_name << "_TO_SHORE";
    node_out_t out;
    out.node_name = m_node_name;
    out.nav_long = m_nav_long;
    out.nav_lat = m_nav_lat;
    out.nav_depth = 100;
    out.node_report_local = m_node_report_local;
    m_lcm->publish(ss.str(), &out);
    ss.clear();
    ss.str("");
    m_lat_set = false;
    m_long_set = false;
    m_report_set = false;
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool NodeLCMComms::OnStartUp()
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

  //SetIterateMode(COMMS_DRIVEN_ITERATE_AND_MAIL);

  if (!m_MissionReader.GetConfigurationParam("NODE_NAME", m_node_name)) {
    cerr << "Node name NODE_NAME not specified! Quitting..." << endl;
    return(false);
  }

  m_lcm = new lcm::LCM();
  LCMSubscribe();
  pthread_t lcm_thread;
  if(pthread_create(&lcm_thread, NULL, NodeLCMComms::LCMHandle, this)) {
    cout << "Error creating LCM thread" << endl;
    return(false);
  }

  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void NodeLCMComms::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("NAV_LAT", 0);
  m_Comms.Register("NAV_LONG", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
}

//----------------------------------------------------------
// LCM FUNCTIONS

void* NodeLCMComms::LCMHandle(void* data) {
  while(0==((NodeLCMComms *) data)->m_lcm->handle());
}

void NodeLCMComms::LCMSubscribe() {
  stringstream ss;
  ss << m_node_name << "_TO_NODE";
  m_lcm->subscribe(string(ss.str()), &NodeLCMComms::LCMCallback_node_in_t, this);
  ss.clear();
  ss.str("");
}

void NodeLCMComms::LCMCallback_node_in_t(const lcm::ReceiveBuffer* rbuf, const string& channel, const node_in_t* msg) {
  m_drift_x = msg->drift_x;
  m_Comms.Notify("DRIFT_X", m_drift_x);
  m_drift_y = msg->drift_y;
  m_Comms.Notify("DRIFT_Y", m_drift_y);
  m_deploy = msg->deploy_str;
  m_Comms.Notify("DEPLOY", m_deploy);
  m_return = msg->return_str;
  m_Comms.Notify("RETURN", m_return);
  m_moos_manual_override = msg->moos_manual_override_str;
  m_Comms.Notify("MOOS_MANUAL_OVERRIDE", m_moos_manual_override);
}
