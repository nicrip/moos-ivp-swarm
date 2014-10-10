#!/bin/bash

WARP=10
MULTICAST=multicast_1
SWARM_NUM_NODES_X=10
SWARM_NUM_NODES_Y=5
SWARM_DEPTH=200
SWARM_PREFIX=NODE_
SWARM_NUM_NODES=$(($SWARM_NUM_NODES_X*$SWARM_NUM_NODES_Y))
SWARM_NUM_MANAGERS=4
SHORE_SHARE_PORT="9001"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$WARP \
    SNAME="shoreside"  SHARE_LISTEN=$SHORE_SHARE_PORT        \
    SPORT="9000" MULTICAST=$MULTICAST      

echo "Built shoreside .moos file here."

pAntler targ_shoreside.moos >& /dev/null &
echo "Launched shoreside."

uMAC targ_shoreside.moos

echo "Killing all processes..."
kill %1
echo "Done killing processes."


