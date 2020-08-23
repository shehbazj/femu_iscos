BASEDIR=cpulimit

rm -rf $BASEDIR
mkdir -p $BASEDIR
# generate NSC benchmark
FILESIZE=10

for CPULIMIT in `seq 50 10 100`
do
echo "FILESIZE=$FILESIZE
BMNAME=compression.$CPULIMIT.percent_nsc
BM=compression
CPU=4
MEMORY=10G
DEVSIZE=10240
COMPUTEMODE=1
FLASH_READ_LATENCY=0
FLASH_WRITE_LATENCY=10000
CPU_LIMIT=$CPULIMIT
PREP_SCRIPTS=()
PREP_ARG_START=()
PREP_ARG_END=()
PREP_ARGS=()
TESTSCRIPT=stream_compression
PARAMS=( $FILESIZE )" > $BASEDIR/compression_$CPULIMIT.percent_nsc
done


# generate Host benchmark

for CPULIMIT in `seq 50 10 100`
do
echo "FILESIZE=$FILESIZE
BMNAME=compression.$CPULIMIT.percent_host
BM=compression
CPU=4
MEMORY=10G
DEVSIZE=10240
COMPUTEMODE=0
FLASH_READ_LATENCY=0
FLASH_WRITE_LATENCY=10000
CPU_LIMIT=$CPULIMIT
PREP_SCRIPTS=()
PREP_ARG_START=()
PREP_ARG_END=()
PREP_ARGS=()
TESTSCRIPT=stream_compression
PARAMS=( $FILESIZE )" > $BASEDIR/compression_$CPULIMIT.percent_host
done