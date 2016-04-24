#/bin/bash
make
sleep 1
#make directories for server and client
mkdir -p client_d
mkdir -p server_d
#copy  executable files
cp client client_d
cp server server_d
#copy client_d files
cp text.txt client_d
cp fit.jpg client_d
#run server
cd server_d

./server -p 6666 &
SPID=$!
sleep 1
#client demonstrations
cd ../client_d
echo "Uploading textfile"
echo "./client -h localhost -p 6666 -u text.txt"
./client -h localhost -p 6666 -u text.txt
sleep 1
echo "Uploading jpg"
echo "./client -h localhost -p 6666 -u fit.jpg"
./client -h localhost -p 6666 -u fit.jpg
sleep 1
# remove uploaded files
rm text.txt
rm fit.jpg
# download the files
echo "Downloading textfile"
echo "./client -h localhost -p 6666 -d text.txt"
./client -h localhost -p 6666 -d fit.jpg
sleep 1
echo "Downloading jpg"
echo "./client -h localhost -p 6666 -d fit.jpg"
./client -h localhost -p 6666 -d text.txt
sleep 1

echo "Done: killing server"
#try to kill server proccess
kill -9 $SPID
