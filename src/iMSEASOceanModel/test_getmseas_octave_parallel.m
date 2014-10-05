1;

function ret = test_getmseas(a,b,c,d,e)
	readmseaspe_octave('/home/rypkema/Workspace/mseas_data/pe_out_vrot.nc');
	ret = readmseaspe_octave(a,b,c,d,e);
	readmseaspe_octave();
endfunction

function ret = test_1
	temp = 'temp';
	salt = 'salt';
	vel = 'meridional velocity';
	lon = -73;
	lat = 39;
	depth = 100;
	time = [2006, 8, 30, 3, 0, 0];
	%readmseaspe_octave(vel,lon,lat,depth,time);
	a = {vel}
	b = {lon}
	c = {lat}
	d = {depth}
	e = {time}
	tic
%	for i = 1:10000
%		a = [a, vel];
%		b = [b, lon];
%		c = [c, lat];
%		d = [d, depth];
%		e = [e, time];
%	end
  len = 1000;
  a = repmat(a,[len,1]);
  b = repmat(b,[len,1]);
  c = repmat(c,[len,1]);
  d = repmat(d,[len,1]);
  e = repmat(e,[len,1]);
	toc()
	tic()
	asdf = parcellfun(100,@test_getmseas,(a),(b),(c),(d),(e));
	disp(asdf);
	time_par = toc()
	time_par
endfunction
