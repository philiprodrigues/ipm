local moo = import "moo.jsonnet";

local cmd = import "appfwk-cmd-make.jsonnet";
local fdp = import "appfwk-fdp-make.jsonnet";
local fdc = import "appfwk-fdc-make.jsonnet";
local viir = import "ipm-viir-make.jsonnet";
local viis = import "ipm-viis-make.jsonnet";

local qnamep = "hose";            // the name of the single queue in this job
local qnamec = "spigot";

local connstr = "inproc://default";

[

    cmd.init([cmd.qspec("hose", "StdDeQueue", 10), cmd.qspec("spigot", "StdDeQueue", 10)],
             [cmd.mspec("fdp",  "FakeDataProducerDAQModule", cmd.qinfo(fdp.queue, qnamep, cmd.qdir.output)),
              cmd.mspec("fdc",  "FakeDataConsumerDAQModule", cmd.qinfo(fdc.queue, qnamec, cmd.qdir.input)),
              cmd.mspec("viis", "VectorIntIPMSenderDAQModule",        cmd.qinfo(viis.queue, qnamep, cmd.qdir.input)),
              cmd.mspec("viir", "VectorIntIPMSubscriberDAQModule",      cmd.qinfo(viir.queue, qnamec, cmd.qdir.output))
              ]),


    cmd.conf([cmd.mcmd("fdp", fdp.conf(10,-4,14)),
              cmd.mcmd("fdc", fdc.conf(10,-4,14)),
              cmd.mcmd("viis", viis.conf(10,{connection_string: connstr}, "ZmqPublisher", "VectorIntTopic")),
              cmd.mcmd("viir", viir.conf(10,{connection_string: connstr}, "ZmqSubscriber", "VectorIntTopic"))
              ]),
    
    // send by match-all
    cmd.start(42),

    // send to modules in explicit order
    cmd.stop([cmd.mcmd("fdp"), cmd.mcmd("viis"), cmd.mcmd("viir"), cmd.mcmd("fdc")]),

    // cmd.scrap(),

    // cmd.fini(),

]

