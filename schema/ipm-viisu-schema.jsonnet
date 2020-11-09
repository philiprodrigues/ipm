// The schema used by classes in the ipm code tests.
//
// It is an example of the lowest layer schema below that of the "cmd"
// and "app" and which defines the final command object structure as
// consumed by instances of specific DAQModule implementations (ie,
// the test/plugins/VectorIntIPM* modules).

local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.ipm.viisu";
local s = moo.oschema.schema(ns);

// Object structure used by the test/fake producer module
local viisu = {
    size_t_attempt: s.number("Size_t", "u8",
                     doc="Same as a size_t in gcc v8.2.0"),

    int_attempt: s.number("Int", "i4",
                     doc="Same as an int in gcc v8.2.0"),

    conf: s.record("Conf", [
        s.field("nIntsPerVector", self.size_t_attempt, 10,
                doc="Number of numbers"),
        s.field("queue_timeout_ms", self.int_attempt, 100,
                doc="Milliseconds to wait on queue before timing out"),
        s.field("subscriber_type", string, "ZmqSubscriber", doc="Subscriber plugin type"),
        s.field("topic", string, "VectorIntTopic", doc="Topic for test"),
    ], doc="VectorIntIPMReceiverDAQModule Configuration"),

};

moo.oschema.sort_select(viisu, ns)

