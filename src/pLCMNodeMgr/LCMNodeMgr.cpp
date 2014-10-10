/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: LCMNodeMgr.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "LCMNodeMgr.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LCMNodeMgr::LCMNodeMgr() : m_nav_long(0.0), m_nav_lat(0.0), m_nav_depth(100.0), m_long_set(false), m_lat_set(false), m_depth_set(true), m_nav_x(0.0), m_nav_y(0.0)
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

LCMNodeMgr::~LCMNodeMgr()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool LCMNodeMgr::OnNewMail(MOOSMSG_LIST &NewMail)
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
    } else if (msg.GetKey() == "NAV_X") {
      m_nav_x = msg.GetDouble();
      m_long_set = true;
    } else if (msg.GetKey() == "NAV_Y") {
      m_nav_y = msg.GetDouble();
      m_long_set = true;
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

   if (m_long_set && m_lat_set && m_depth_set) {
     stringstream ss;
     ss << m_node_name << "_NAV";
     node_nav_t out;
     out.nav_long = m_nav_long;
     out.nav_lat = m_nav_lat;
     out.nav_depth = m_nav_depth;
     out.nav_x = m_nav_x;
     out.nav_y = m_nav_y;
     m_lcm->publish(ss.str(), &out);
     ss.clear();
     ss.str("");
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool LCMNodeMgr::OnConnectToServer()
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

bool LCMNodeMgr::Iterate()
{
  m_iterations++;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool LCMNodeMgr::OnStartUp()
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

  if (!m_MissionReader.GetConfigurationParam("NODE_NAME", m_node_name)) {
    cerr << "Node name NODE_NAME not specified! Quitting..." << endl;
    return(false);
  }

  m_lcm = new lcm::LCM();
  LCMSubscribe();
  pthread_t lcm_thread;
  if(pthread_create(&lcm_thread, NULL, LCMNodeMgr::LCMHandle, this)) {
    cout << "Error creating LCM thread" << endl;
    return(false);
  }

  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void LCMNodeMgr::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("NAV_LAT", 0);
  m_Comms.Register("NAV_LONG", 0);
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
}

//----------------------------------------------------------
// LCM FUNCTIONS

void* LCMNodeMgr::LCMHandle(void* data) {
  while(0==((LCMNodeMgr *) data)->m_lcm->handle());
}

void LCMNodeMgr::LCMSubscribe() {
  stringstream ss;
  ss << m_node_name << "_DRIFT";
  m_lcm->subscribe(string(ss.str()), &LCMNodeMgr::LCMCallback_node_drift_t, this);
  ss.clear();
  ss.str("");
}

void LCMNodeMgr::LCMCallback_node_drift_t(const lcm::ReceiveBuffer* rbuf, const string& channel, const node_drift_t* msg) {
  m_Comms.Notify("DRIFT_X", msg->drift_x);
  m_Comms.Notify("DRIFT_Y", msg->drift_y);
}
