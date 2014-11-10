temp = 'temp';
salt = 'salt';
vel = 'meridional velocity';
lon = -73;
lat = 39;
depth = 40;
time = [2006, 8, 30, 3, 0, 0];

tic()
readmseaspe_octave_steph('/home/rypkema/Workspace/mseas_data/pe_out_vrot.nc');
for i = 1:400
  readmseaspe_octave_steph(vel,lon,lat,depth,time);
end
readmseaspe_octave_steph();
time_ser = toc()
time_ser
