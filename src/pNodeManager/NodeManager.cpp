/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: NodeManager.cpp                                 */
/*    DATE: August 2014                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "NodeManager.h"

using namespace std;

//---------------------------------------------------------
// Constructor

NodeManager::NodeManager() : m_lat_set(false), m_lon_set(false)
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

NodeManager::~NodeManager()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool NodeManager::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == "NAV_LAT") {
      m_node_lat = msg.GetDouble();
      m_lat_set = true;
    } else if (msg.GetKey() == "NAV_LONG") {
      m_node_lon = msg.GetDouble();
      m_lon_set = true;
    } else if (msg.GetKey() == "MSEASOCEANMODEL_RETURN") {
      int num_values = p->GetBinaryDataSize()/sizeof(double);
      int num_rows = num_values;
      Matrix a(num_rows, 1);
      double *a_data = a.fortran_vec();
      memcpy(a_data, p->GetBinaryData(), p->GetBinaryDataSize());
      m_model_values_return = a;
      double drift_x = m_model_values_return(0)/100;  //MSEAS model velocity data is in cm/s
      double drift_y = m_model_values_return(1)/100;
      m_Comms.Notify("DRIFT_X", drift_x);
      m_Comms.Notify("DRIFT_Y", drift_y);
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

bool NodeManager::OnConnectToServer()
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

bool NodeManager::Iterate()
{
  m_iterations++;

  if (m_lat_set && m_lon_set) {
    Matrix a(2, 3);
    m_model_pos_request = a;
    m_model_pos_request(0, 0) = m_node_lon;
    m_model_pos_request(0, 1) = m_node_lat;
    m_model_pos_request(0, 2) = 100;
    m_model_pos_request(1, 0) = m_node_lon;
    m_model_pos_request(1, 1) = m_node_lat;
    m_model_pos_request(1, 2) = 100;
    m_model_pos_request_data_ptr = (unsigned char*) m_model_pos_request.fortran_vec();
    m_Comms.Notify("MSEASOCEANMODEL_POSITIONS", (unsigned char*) m_model_pos_request_data_ptr, (unsigned int) ((2*3)*sizeof(double)));
    m_model_varnames = "meridional velocity,zonal velocity";
    m_Comms.Notify("MSEASOCEANMODEL_VARNAME", m_model_varnames);
    m_Comms.Notify("MSEASOCEANMODEL_FILEPATH", m_filepath);
    m_Comms.Notify("MSEASOCEANMODEL_TIME_REQUEST_OFFSET", 0.0);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool NodeManager::OnStartUp()
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

  if (!m_MissionReader.GetConfigurationParam("FILEPATH", m_filepath)) {
    cerr << "MSEAS filepath FILEPATH not specified! Quitting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("SPEED_FACTOR", m_speed_factor)) {
    cerr << "MSEAS speedup factor not specified! Setting to default of 1..." << endl;
    m_speed_factor = 1;
  }

  m_start_time = MOOSTime();

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void NodeManager::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
  m_Comms.Register("MSEASOCEANMODEL_RETURN", 0);  // data returned by iMSEASOceanModel
  m_Comms.Register("NAV_LAT", 0);
  m_Comms.Register("NAV_LONG", 0);
}

