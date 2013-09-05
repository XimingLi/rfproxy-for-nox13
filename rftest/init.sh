#!/bin/bash

cd ~/openvswitch-1.10.0/
sudo insmod ./datapath/linux/openvswitch.ko

sudo kill `cd /usr/local/var/run/openvswitch && cat ovsdb-server.pid ovs-vswitchd.pid`

sudo ovsdb-server --remote=punix:/usr/local/var/run/openvswitch/db.sock \
                     --remote=db:Open_vSwitch,manager_options \
                     --private-key=db:SSL,private_key \
                     --certificate=db:SSL,certificate \
                     --bootstrap-ca-cert=db:SSL,ca_cert \
                     --pidfile --detach

sudo ovs-vsctl --no-wait init

sudo ovs-vswitchd --pidfile --detach

sudo ovs-vsctl del-br dp0
sudo ovs-vsctl del-br dp1


