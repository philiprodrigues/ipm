#!/usr/bin/env bash

# Really ugly and temporary glue to run moo code generator.
# This will simplify and move into CMake.

mydir=$(dirname $(realpath $BASH_SOURCE))
srcdir=$(dirname $mydir)
appfwkdir=$(realpath $mydir/../../appfwk/schema)
echo "Mydir $mydir, Appfwkdir $appfwkdir"

runmoo () {
    moo -g '/lang:ocpp.jsonnet' -M $mydir "$@"
}

# Wrap up the render command.  This bakes in a mapping to file name
# which would be better somehow captured by the schema itself.
render () {
    local name="$1" ; shift
    local What="$1" ; shift
    local outdir="${1:-$srcdir/include/ipm/${name}}"
    local what="$(echo $What | tr '[:upper:]' '[:lower:]')"
    local tmpl="o${what}.hpp.j2"
    local outhpp="$outdir/${What}.hpp"
    mkdir -p $outdir
    set -x
    runmoo -A path="dunedaq.ipm.${name}" \
           -A ctxpath="dunedaq" \
           -A os="ipm-${name}-schema.jsonnet" \
           render omodel.jsonnet $tmpl \
           > $outhpp
    set +x
    echo $outhpp
}

compile () {
    local name="$1"; shift

    moo -M $mydir -T $mydir -M $appfwkdir -T $appfwkdir compile ipm-$name-job.jsonnet >ipm-$name-job.json
}

render viir Structs $srcdir/test/include/ipm/viir
render viir Nljs    $srcdir/test/include/ipm/viir

render viis Structs $srcdir/test/include/ipm/viis
render viis Nljs    $srcdir/test/include/ipm/viis


compile singleprocess
compile singleprocess-pubsub
compile zmqpublisher
compile zmqreceiver
compile zmqsender
compile zmqsubscriber
