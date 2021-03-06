/*
Copyright 2017 VMware, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <v1model.p4>

header H { bit<32> f; }

struct my_packet {
    H h;
}

struct my_metadata {
}

parser MyParser(packet_in b, out my_packet p, inout my_metadata m, inout standard_metadata_t s) {
    bool bv = true;
    state start {
        transition select(bv) {
            false: next;
            true: accept;
        }
    }

    state next {
        b.extract(p.h);
        transition accept;
    }
}

control MyVerifyChecksum(inout my_packet hdr, inout my_metadata meta) {
  apply { }
}

control C() {
  apply {}
}

control MyIngress(inout my_packet p, inout my_metadata m, inout standard_metadata_t s) {
  apply { }
}

control MyEgress(inout my_packet p, inout my_metadata m, inout standard_metadata_t s) {
  apply { }
}

control MyComputeChecksum(inout my_packet p, inout my_metadata m) {
  apply { }
}

control MyDeparser(packet_out b, in my_packet p) {
  apply { }
}

V1Switch(MyParser(), MyVerifyChecksum(), MyIngress(), MyEgress(), MyComputeChecksum(), MyDeparser()) main;
