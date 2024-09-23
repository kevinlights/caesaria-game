docker run -d --rm \
    --name build \
    -v $(pwd):/src \
    ubuntu:18.04 sleep 3600

docker exec build apt update
docker exec build apt install -y build-essential g++ ctags git cmake p7zip-full wget qtcreator mingw-w64-dev mingw-w64 gcc-mingw-w64-i686 g++-mingw-w64-i686 mingw-w64-tools

docker exec build bash -c '
cd /src/xcompile
ls
chmod a+x update-workspace-win.sh
./update-workspace-win.sh
'
