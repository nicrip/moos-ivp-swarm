#!/bin/bash 

WARP=15
MULTICAST=multicast_1
VTYPE=UUV
SWARM_NUM_NODES_X=1
SWARM_NUM_NODES_Y=1
SWARM_OFFSET_X=200
SWARM_OFFSET_Y=-200
SWARM_ORIGIN_X=0
SWARM_ORIGIN_Y=0
SWARM_DEPTH=200
SWARM_PREFIX=NODE_
SWARM_NUM_NODES=$(($SWARM_NUM_NODES_X*$SWARM_NUM_NODES_Y))
SWARM_START_PORT=9100
SWARM_START_SHARE_PORT=10100
SHORE_SHARE_PORT="9001"
DEPLOY_X=$((( RANDOM % 2000 ) - 1000))
DEPLOY_Y=$(echo "sqrt (1000*1000 - $DEPLOY_X*$DEPLOY_X)" | bc -l)
INVERT=$(($RANDOM%2))
echo $INVERT
if [ $INVERT -eq 0 ]; then
   echo "INVERT!"
   DEPLOY_Y=-$DEPLOY_Y
fi
echo "sqrt ($DEPLOY_Y*$DEPLOY_Y + $DEPLOY_X*$DEPLOY_X)" | bc -l

mkdir -p node_moos
mkdir -p node_bhv
mkdir -p node_output

COUNT=0
for ((i = 0; i < $SWARM_NUM_NODES_X; i++))
do
  for ((j = 0; j < $SWARM_NUM_NODES_Y; j++))
    do
      NODE_X=$(($i*$SWARM_OFFSET_X+$SWARM_ORIGIN_X))
      NODE_Y=$(($j*$SWARM_OFFSET_Y+$SWARM_ORIGIN_Y))
      NODE_POS="$NODE_X,$NODE_Y"
      NODE_PORT="$(($SWARM_START_PORT+$COUNT))"
      NODE_SHARE_PORT="$(($SWARM_START_SHARE_PORT+$COUNT))"
      NODE_NAME="$SWARM_PREFIX$COUNT"
      nsplug meta_vehicle.moos ./node_moos/targ_$NODE_NAME.moos -f	\
      NODE_NAME=$NODE_NAME						\
      NODE_PORT=$NODE_PORT						\
      WARP=$WARP							\
      SHARE_LISTEN=$NODE_SHARE_PORT					\
      SHORE_LIST=$SHORE_SHARE_PORT					\
      MULTICAST=$MULTICAST						\
      VTYPE=$VTYPE							\
      START_POS=$NODE_POS
      nsplug meta_vehicle.bhv ./node_bhv/targ_$NODE_NAME.bhv -f         \
      NODE_NAME=$NODE_NAME                                     		\
      PULSESPEED=$PULSESPEED                          			\
      START_POS=$NODE_POS                             			\
      DEPLOY_POS="$DEPLOY_X,$DEPLOY_Y"
      COUNT=$((COUNT+1))
    done
done

echo "Built $SWARM_NUM_NODES node .moos and .bhv files under node_moos and node_bhv."

for ((i = 0; i < $SWARM_NUM_NODES; i++))
do
  NODE_NAME="$SWARM_PREFIX$i"
  pAntler ./node_moos/targ_$NODE_NAME.moos >& ./node_output/$NODE_NAME.out &
  sleep 0.1
done

echo "Launched $SWARM_NUM_NODES nodes."

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
  echo "(1) Exit script (2) Exit and kill simulation"
  printf "> "
  read ANSWER
done

# %1 matches the PID of the first job in the active jobs list, 
# namely the pAntler job launched in Part 4.
if [ "${ANSWER}" = "2" ]; then
  echo "Killing all processes..."
  for ((i = 1; i <= $SWARM_NUM_NODES; i++))
  do
    kill %$i
  done
  echo "Done killing processes."
fi
