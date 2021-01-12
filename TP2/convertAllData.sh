#!/bin/sh
for i in data/*; do
  
  # converting triangulations
  for j in ${i}/*-triangulation; do
    echo "Converting ${j}/data.tri..."
    build/paraviewConversion -t ${j}/data.tri
    mkdir -p ${j/data/convertedData/} 
    mv output.vtu ${j/data/convertedData/}
  done

  # converting grids
  for j in ${i}/*-grid; do
    CONFIG=`ls ${j}/*txt`
    echo "Converting ${CONFIG}..."
    ATTRIBUTE_LIST=""
    ID=0
    for k in ${j}/*raw; do
      ATTRIBUTE_LIST="${ATTRIBUTE_LIST} -g${ID} $k"
      ID=`echo "$ID + 1" | bc`
    done
    build/paraviewConversion -g $CONFIG ${ATTRIBUTE_LIST}
    mkdir -p ${j/data/convertedData/}
    mv output.vti ${j/data/convertedData/}
  done
done
