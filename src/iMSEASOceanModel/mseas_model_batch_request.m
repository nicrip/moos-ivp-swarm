%% EXAMPLE USAGE %%
%file = '/home/rypkema/Workspace/mseas_data/pe_out_vrot.nc';
%vel = 'meridional velocity';
%lon = -73;
%lat = 39;
%depth = 100;
%timereq = time(); %current date/time from epoch in seconds
%lon_b = [lon];
%lon_b = repmat(lon_b, [10, 1]);
%lat_b = [lat];
%lat_b = repmat(lat_b, [10, 1]);
%depth_b = [depth];
%depth_b = repmat(depth_b, [10, 1]);
%pos_b = [lon_b, lat_b, depth_b];
%[a,b,c] = mseas_model_batch_request(pos_b, vel, time+1, file, time);
%a
%b
%c


function [MODEL_VALUES, MODEL_TIME, REQUEST_TIME_ELAPSED] = mseas_model_batch_request(MODEL_POS_REQUEST, MODEL_VARNAME_REQUEST, MODEL_TIME_REQUEST, MODEL_FILEPATH, MODEL_TIME_OFFSET)
  %% INPUT FORMATS %%
  %% MODEL_POS_REQUEST - nx3 matrix: [lon1 lat1 depth1; lon2 lat2 depth2; lon3 lat3 depth3; ...]
  %% MODEL_VARNAME_REQUEST - string: 'temp' or 'salt' or 'sound speed' or 'vertical velocity' or 'zonal velocity' or 'meridional velocity'
  %% MODEL_TIME_REQUEST - double: MODEL_TIME_OFFSET + offset seconds into model data
  %% MODEL_FILEPATH - string: path to MSEAS model file, e.g. '/home/rypkema/Workspace/mseas_data/pe_out_vrot.nc'
  %% MODEL_TIME_OFFSET - double: usually number of seconds since epoch representing when MOOSApp MSEASOceanModel has begun
  %% OUTPUT FORMATS %%
  %% MODEL_VALUES - nx1 octave_value_list of model values retrieved
  %% MODEL_TIME - double: unused, -1
  %% REQUEST_TIME_ELAPSED - double: number of seconds batch request has taken to retrieve all model values
  %time calculations
  %disp("+++++++++++++++++++++++++++++++++ OCTAVE +++++++++++++++++++++++++++++++++");
  %disp(MODEL_POS_REQUEST);
  %disp(MODEL_VARNAME_REQUEST);
  %disp(MODEL_TIME_REQUEST);
  %disp(MODEL_FILEPATH);
  %disp(MODEL_TIME_OFFSET);
  ncids = readmseaspe_octave(MODEL_FILEPATH);
  ncid = ncids{1}{1};
  model_start_time_vec = get_petim0(ncid);                                        %get MSEAS model start date/time
  model_duration_sec = ncid{'time'}(end) - ncid{'time'}(1);                       %get MSEAS model duration
  readmseaspe_octave();
  epoch = [1970 1 1 0 0 0];
  model_start_time_unix = 86400*(datenum(model_start_time_vec) - datenum(epoch)); %convert MSEAS model start date/time to unix seconds
  model_end_time_unix = model_start_time_unix + model_duration_sec;               %calculate MSEAS model end date/time in unix seconds
  offset_time = MODEL_TIME_OFFSET - model_start_time_unix;                        %MODEL_TIME_OFFSET is always current date/time - calculate difference between time now and MSEAS model start time
  adjusted_sample_time = MODEL_TIME_REQUEST - offset_time;                        %MODEL_TIME_REQUEST is usually the elapsed MOOSTime+(specified offset in seconds)
  if(adjusted_sample_time < model_start_time_unix)
    adjusted_sample_time = adjusted_sample_time + model_duration_sec;             %if given (specified offset in seconds) is negative, the first request will wrap from end of MSEAS model
  end
  if(adjusted_sample_time > model_end_time_unix)
    adjusted_sample_time = adjusted_sample_time - model_duration_sec;             %if request has passed end of MSEAS model time, the request will wrap again from start of MSEAS model
  end
  SAMPLE_TIME_VEC = datevec(datenum(epoch+[0 0 0 0 0 adjusted_sample_time]));     %convert unix time back to date/time vector required by readmseaspe_octave

  %batching
  numel = size(MODEL_POS_REQUEST)(1);
  filepath_batch = {MODEL_FILEPATH};
  filepath_batch = repmat(filepath_batch, [numel, 1]);
  varname_batch = MODEL_VARNAME_REQUEST;
  time_batch = {SAMPLE_TIME_VEC};
  time_batch = repmat(time_batch, [numel, 1]);
  lon_batch = mat2cell(MODEL_POS_REQUEST(:, 1), ones(1, numel));
  lat_batch = mat2cell(MODEL_POS_REQUEST(:, 2), ones(1, numel));
  depth_batch = mat2cell(MODEL_POS_REQUEST(:, 3), ones(1, numel));
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  %varname_batch = mat2cell(MODEL_VARNAME_REQUEST(:, 1), ones(1, numel));
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  %model requests
  tic();
  %MODEL_VALUES = cell2mat(parcellfun(2, @mseas_model_value_request, (filepath_batch), (varname_batch), (lon_batch), (lat_batch), (depth_batch), (time_batch)));
  MODEL_VALUES = cell2mat(cellfun(@mseas_model_value_request, (filepath_batch), (varname_batch), (lon_batch), (lat_batch), (depth_batch), (time_batch)));
  REQUEST_TIME_ELAPSED = toc();
  MODEL_TIME = -1;
  %disp("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
endfunction

function [val] = mseas_model_value_request(filepath, varname, lon, lat, depth, time)
	%parallelized function single model request
	readmseaspe_octave(filepath);
	val = readmseaspe_octave(varname, lon, lat, depth, time);
	readmseaspe_octave();
endfunction

function pe_tim0 = get_petim0(ncid);
  %% EXTRACTED FROM radmseaspe_octave.m %%
  %model start time read
  pe_tim0 = [];
  tim0str = ncid{'time'}.units(:);
  if isempty(tim0str)
     disp (' ');
     disp ('***Error:  GET_PETIM0 - unable to read time units');
     disp (' ');
     return;
  end;
  sind = findstr (upper(tim0str),'SINCE');
  if isempty(sind)
     pe_tim0 = try_pejdate0 (ncid);
     return;
  end;
  if (length(tim0str)<=(sind+5))
     pe_tim0 = try_pejdate0 (ncid);
     return;
  end;
  tim0str(1:(sind+4)) = [];
  tim0str = fliplr(deblank(fliplr(deblank(tim0str))));
  ind = findstr(tim0str,'-');
  if ~isempty(ind)
     tim0str(ind(1:2)) = ' ';
  end;
  ind = findstr(tim0str,':');
  if ~isempty(ind)
     tim0str(ind) = ' ';
  end;
  twk = str2num (tim0str');
  pe_tim0 = twk(1:6);
  gmtshft = twk(7:length(twk));
  repind = 2:length(gmtshft);
  gmtshft(repind) = abs(gmtshft(repind)).*sign(gmtshft(1));
  repind = 3 + (1:length(gmtshft));
  pe_tim0(repind) = pe_tim0(repind) + gmtshft;
endfunction
