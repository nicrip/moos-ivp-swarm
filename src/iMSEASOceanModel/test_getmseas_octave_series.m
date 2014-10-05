temp = 'temp';
salt = 'salt';
vel = 'meridional velocity';
lon = -73;
lat = 39;
depth = 40;
time = [2006, 8, 30, 3, 0, 0];

readmseaspe_octave({'/home/rypkema/Workspace/mseas_data/pe_out_ts.nc', '/home/rypkema/Workspace/mseas_data/pe_out_vrot.nc'});
tic()
for i = 1:400
  readmseaspe_octave(vel,lon,lat,depth,time);
end
time_ser = toc()
readmseaspe_octave();
time_ser
