#!/usr/bin/env bash

# Really ugly and temporary glue to run moo code generator.
# This will simplify and move into CMake.

mydir=$(dirname $(realpath $BASH_SOURCE))
srcdir=$(dirname $mydir)

# The need for this detail will go away once moo is cleaned up a bit.
# This obviously John-specific directory is on mu2edaq01.fnal.gov
oschema=/home/jcfree/moo_work/moo/examples/oschema
runmoo () {
    moo -g '/lang:ocpp.jsonnet' \
        -M $oschema -T $oschema -M $mydir \
        "$@"
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

render viir Structs $srcdir/test/ipm/viir
render viir Nljs    $srcdir/test/ipm/viir

render viis Structs $srcdir/test/ipm/viis
render viis Nljs    $srcdir/test/ipm/viis

render viisu Structs $srcdir/test/ipm/viisu
render viisu Nljs    $srcdir/test/ipm/viisu