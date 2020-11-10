local moo = import "moo.jsonnet";

local cmd = import "appfwk-cmd-make.jsonnet";
local fdc = import "appfwk-fdc-make.jsonnet";
local viir = import "ipm-viir-make.jsonnet";

local qnamec = "hose";            // the name of the single queue in this job

local connstr = "tcp://localhost:29870";

[

    cmd.init([cmd.qspec("hose", "StdDeQueue", 10)],
             [cmd.mspec("fdc",  "FakeDataConsumerDAQModule", cmd.qinfo(fdc.queue, qnamec, cmd.qdir.input)),
              cmd.mspec("viir", "VectorIntIPMReceiverDAQModule",      cmd.qinfo(viir.queue, qnamec, cmd.qdir.output))
              ]),


    cmd.conf([cmd.mcmd("fdc", fdc.conf(10,-4,14)),
              cmd.mcmd("viir", viir.conf(10,{connection_string: connstr}))
              ]),
    
    // send by match-all
    cmd.start(42),

    // send to modules in explicit order
    cmd.stop([cmd.mcmd("viir"), cmd.mcmd("fdc")]),

    // cmd.scrap(),

    // cmd.fini(),

]

