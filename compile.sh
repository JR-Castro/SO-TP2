docker start SO
docker exec -it SO make clean -C /root/Toolchain
docker exec -it SO make clean -C /root/
if [[ "$1" != "clean" ]]
then
    docker exec -it SO make -C /root/Toolchain
    if [[ "$1" == "gdb" ]]
    then
      docker exec -it SO make gdb -C /root/
    else
      docker exec -it SO make -C /root/
    fi
fi
docker stop SO