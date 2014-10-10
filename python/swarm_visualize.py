from ddapp.consoleapp import ConsoleApp
import ddapp.visualization as vis
from ddapp.timercallback import TimerCallback
from ddapp.shallowCopy import shallowCopy
from ddapp.debugVis import DebugData
from ddapp import lcmUtils
import ddapp.vtkNumpy as vtknp
import numpy as np
from node_nav_t import node_nav_t

def startSwarmVisualization():
    global timerCallback, nav_data, nav_cloud
    nav_cloud = vtknp.getVtkPolyDataFromNumpyPoints(nav_data)
    nav_cloud_obj = vis.showPolyData(shallowCopy(nav_cloud), 'nav data')

    nav_cloud_obj.initialized = False

    def updateSwarm():
      global nav_cloud

      if not nav_cloud_obj.initialized:
         nav_cloud_obj.mapper.SetColorModeToMapScalars()
         nav_cloud_obj.initialized = True

      #print nav_data.shape[0], nav_cloud.GetNumberOfPoints()
      nav_cloud = vtknp.getVtkPolyDataFromNumpyPoints(nav_data)
      nav_cloud_obj.setPolyData(shallowCopy(nav_cloud))
      #print nav_cloud_obj.polyData.GetNumberOfPoints()

    timerCallback = TimerCallback(targetFps=30)
    timerCallback.callback = updateSwarm
    timerCallback.start()

def handleNavData(msg):
    global nav_data
    #print channel
    nav_data = np.append(nav_data, [[msg.nav_x, msg.nav_y, 0]], axis=0)
    #print nav_data[-1]


def main():
    global app, view, nav_data

    nav_data = np.array([[0, 0, 0]])

    lcmUtils.addSubscriber(".*_NAV$", node_nav_t, handleNavData)

    app = ConsoleApp()

    app.setupGlobals(globals())
    app.showPythonConsole()

    view = app.createView()
    view.show()

    global d
    d = DebugData()
    d.addLine([0,0,0], [1,0,0], color=[0,1,0])
    d.addSphere((0,0,0), radius=0.02, color=[1,0,0])

    #vis.showPolyData(d.getPolyData(), 'my debug geometry', colorByName='RGB255')

    startSwarmVisualization()

    app.start()

if __name__ == '__main__':
    main()
