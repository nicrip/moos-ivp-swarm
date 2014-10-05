function [retval_mat, retval_double, retval_str] = test_octave(a,b,c,d)
  disp('MATRIX:');
  disp(a);
  disp('DOUBLE:');
  disp(b);
  disp('STRING:');
  disp(c);
  disp('CELL ARRAY:');
  disp(d);
  retval_mat = [1 2; 3 4; 5 6];
  retval_double = 5.0;
  retval_str = 'TEST_STRING_OUT';
  disp('DONE')
endfunction
