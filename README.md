
# Mini BitTorrent

It is a miniBitTorrent file sharing system(peer to peer file sharing).

### Prerequisites

You need to install openssl package.
```
sudo apt-get update
sudo apt-get install openssl
```
code is written with respect to linux file system and path hierarchy.

### Installation 

run makefile in the respective directory where source file are present.


#### To run server

```
./tracker < my_tracker_ip >:< my_tracker_port > <other_tracker_ip>:<other_tracker_port> <seederlist_file> <log_file>

eg : ./tracker_2018201034 10.1.38.204:6565 10.1.38.204:6567 seed_file log_file  

```

To see activities of tracker, see the log_file. 


#### To run client 

```
./client_2018201034 < my__ip >:< my_upload_port > < tracker_1_ip >:< tracker_1_port > < tracker_2_ip >:< tracker_2_port > < log_file >

eg : ./client_2018201034 10.1.38.204:6540 10.1.38.204:6565 10.1.38.204:6567 log_file

```

#### Commands to run on client 

```
share <local_file_path> <filename>.<file_extension>.mtorrent
```
* it will share the data with tracker and put your client in seeding mode. So that other clients can access them by using the mtorrent file that you generated.

```
get <path_to_.mtorrent_file> <destination_path>
```
* it will get metadata from tracker and starts downloading file in the destination path.

```
remove <filename.mtorrent>
```
* it will remove the metadata entries from server and destroys mtorrent file from client.

```
show_downloads
```
* shows current status of download.

```
close 
```
* remove all metadata from tracker and shutdown.

#### Assumptions

* For files containing space in name, use ' \ ' as escape sequence.
* Absolute & relative path, both are supported.
* Only one tracker is implemented. 
* File is shared by only one peer now (First peer in the list).
* Whenever client starts, it scans current working directory ans update server about that files.
