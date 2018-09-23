
 author - Rushitkumar Jasani    rollno - 2018201034


# Compilation steps 
 -> run makefile in the respective directory where source file are present.

******************************************************

# Run Server first

./tracker <my_tracker_ip>:<my_tracker_port> <other_tracker_ip>:<other_tracker_port> <seederlist_file> <log_file>

eg : ./tracker 10.1.38.204:6565 10.1.38.204:6567 seed_file log_file  

-> To see activities of tracker, see the log_file. 

******************************************************

# To run client 

./client <my__ip>:<my_upload_port> <tracker_1_ip>:<tracker_1_port> <tracker_2_ip>:<tracker_2_port> <log_file>

eg :> ./client 10.1.38.204:6540 10.1.38.204:6565 10.1.38.204:6567 log_file

******************************************************

# Client command supported. 

share <local_file_path> <filename>.<file_extension>.mtorrent

get <path_to_.mtorrent_file> <destination_path>

remove <filename.mtorrent>

show downloads

close 

******************************************************

# Assumptions
-> for files containing space in name, use '\' as escape seq.
-> absolute & relative path, both are supported.
-> Only one tracker is implemented. 
-> File is shared by only one peer now (First peer in the list).
