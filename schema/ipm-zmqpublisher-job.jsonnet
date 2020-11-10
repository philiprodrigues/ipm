local moo = import "moo.jsonnet";

local cmd = import "appfwk-cmd-make.jsonnet";
local fdp = import "appfwk-fdp-make.jsonnet";
local viis = import "ipm-viis-make.jsonnet";

local qnamep = "hose";            // the name of the single queue in this job

local connstr = "tcp://localhost:19870";

[

    cmd.init([cmd.qspec("hose", "StdDeQueue", 10)],
             [cmd.mspec("fdp",  "FakeDataProducerDAQModule", cmd.qinfo(fdp.queue, qnamep, cmd.qdir.output)),
              cmd.mspec("viis", "VectorIntIPMSender",        cmd.qinfo(viis.queue, qnamep, cmd.qdir.input))
              ]),


    cmd.conf([cmd.mcmd("fdp", fdp.conf(10,-4,14)),
              cmd.mcmd("viis", viis.conf(10,{connection_string: connstr}, "ZmqPublisher", "VectorIntTopic"))
              ]),
    
    // send by match-all
    cmd.start(42),

    // send to modules in explicit order
    cmd.stop([cmd.mcmd("fdp"), cmd.mcmd("viis")]),

    // cmd.scrap(),

    // cmd.fini(),

]

